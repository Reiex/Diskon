///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Marius Pélégrin
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Diskon/Core/CoreDecl.hpp>

namespace dsk
{
	template<typename TSymbol>
	HuffmanTableEntry<TSymbol>* HuffmanTableEntry<TSymbol>::createTable()
	{
		return new HuffmanTableEntry<TSymbol>[256];
	}

	template<typename TSymbol>
	bool HuffmanTableEntry<TSymbol>::addSymbol(HuffmanTableEntry<TSymbol>* table, const TSymbol& symbol, uint64_t codeLength, uint8_t* code)
	{
		assert(table);
		assert(codeLength != 0);

		if (codeLength <= 8)
		{
			const uint8_t filter = (0xFF >> codeLength);
			for (uint16_t i = 0; i < 256; ++table, ++i)
			{
				if (table->bitCount == 0 && !table->next && (i & filter) == 0)
				{
					const uint8_t span = (1 << (8 - codeLength));

					if (i + span > 256)
					{
						return false;
					}

					const HuffmanTableEntry<TSymbol>* const tableEnd = table + span;

					for (; table != tableEnd; ++table)
					{
						table->bitCount = codeLength;
						table->symbol = symbol;
					}

					if (code)
					{
						*code = i;
					}

					return true;
				}

				assert(table->bitCount == 0 || table->symbol != symbol);
			}
		}
		else
		{
			for (uint16_t i = 0; i < 256; ++table, ++i)
			{
				if (table->bitCount == 0)
				{
					if (code)
					{
						*code = i;
					}

					if (table->next)
					{
						if (addSymbol(table->next, symbol, codeLength - 8, code + (code != nullptr)))
						{
							return true;
						}
						else
						{
							continue;
						}
					}
					else
					{
						table->next = createTable();
						return addSymbol(table->next, symbol, codeLength - 8, code + (code != nullptr));
					}
				}
				else
				{
					assert(table->symbol != symbol);
				}
			}
		}

		return false;
	}

	template<typename TSymbol>
	void HuffmanTableEntry<TSymbol>::swapEndianness(HuffmanTableEntry<TSymbol>* table)
	{
		// 0 and 255 can be skipped because they're invariant by bitswap
		for (uint8_t i = 1; i < 255; ++i)
		{
			uint8_t j = bitswap(i);
			if (j > i)
			{
				std::swap(table[i], table[j]);
			}
		}

		const HuffmanTableEntry<TSymbol>* const tableEnd = table + 256;
		for (; table != tableEnd; ++table)
		{
			if (table->bitCount == 0 && table->next)
			{
				swapEndianness(table->next);
			}
		}
	}

	template<typename TSymbol>
	void HuffmanTableEntry<TSymbol>::destroyTable(HuffmanTableEntry<TSymbol>* table)
	{
		const HuffmanTableEntry<TSymbol>* const tableEnd = table + 256;
		for (; table != tableEnd; ++table)
		{
			if (table->bitCount == 0 && table->next)
			{
				destroyTable(table->next);
			}
		}

		delete[](table - 256);
	}


	namespace _dsk
	{
		template<typename TSymbol>
		struct Node
		{
			bool isLeaf;
			uint64_t occurences;

			union
			{
				uint64_t index;

				struct
				{
					Node* childA;
					Node* childB;
				};
			};

			void destroy()
			{
				if (!isLeaf)
				{
					childA->destroy();
					childB->destroy();

					delete childA;
					delete childB;
				}
			}

			static bool compare(const Node<TSymbol>* a, const Node<TSymbol>* b)
			{
				return a->occurences > b->occurences;
			}
		};

		template<typename TSymbol>
		void extractCodeLengths(const Node<TSymbol>* node, uint64_t* codeLengths, uint64_t currentLength)
		{
			if (node->isLeaf)
			{
				codeLengths[node->index] = currentLength;
			}
			else
			{
				++currentLength;
				extractCodeLengths(node->childA, codeLengths, currentLength);
				extractCodeLengths(node->childB, codeLengths, currentLength);
			}
		}
	}
	

	template<typename TSymbol, std::endian BitEndianness>
	void HuffmanEncoder<TSymbol, BitEndianness>::symbolOccurencesToCodeLengths(const TSymbol* symbols, const uint64_t* symbolOccurences, uint64_t* codeLengths, uint64_t symbolCount)
	{
		if (symbolCount == 0)
		{
			return;
		}
		else if (symbolCount == 1)
		{
			*codeLengths = 1;
			return;
		}

		std::vector<_dsk::Node<TSymbol>*> nodes(symbolCount);
		for (uint64_t i = 0; i < symbolCount; ++i)
		{
			nodes[i] = new _dsk::Node<TSymbol>;
			nodes[i]->isLeaf = true;
			nodes[i]->occurences = symbolOccurences[i];
			nodes[i]->index = i;
		}

		_dsk::Node<TSymbol>** nodesBegin = nodes.data();
		_dsk::Node<TSymbol>** nodesEnd = nodesBegin + symbolCount;

		std::make_heap(nodesBegin, nodesEnd, _dsk::Node<TSymbol>::compare);

		for (uint64_t i = 0; i < symbolCount - 1; ++i)
		{
			std::pop_heap(nodesBegin, nodesEnd, _dsk::Node<TSymbol>::compare);
			--nodesEnd;
			std::pop_heap(nodesBegin, nodesEnd, _dsk::Node<TSymbol>::compare);
			--nodesEnd;

			_dsk::Node<TSymbol>* node = new _dsk::Node<TSymbol>;
			node->isLeaf = false;
			node->childA = *nodesEnd;
			node->childB = *(nodesEnd + 1);
			node->occurences = node->childA->occurences + node->childB->occurences;

			*nodesEnd = node;

			++nodesEnd;
			std::push_heap(nodesBegin, nodesEnd, _dsk::Node<TSymbol>::compare);
		}

		_dsk::extractCodeLengths(*nodesBegin, codeLengths, 0);

		(*nodesBegin)->destroy();
	}

	template<typename TSymbol, std::endian BitEndianness>
	void HuffmanEncoder<TSymbol, BitEndianness>::symbolBufferToSymbolOccurences(const TSymbol* symbolBuffer, uint64_t bufferLength, std::vector<TSymbol>& symbols, std::vector<uint64_t>& symbolOccurences)
	{
		constexpr uint64_t symbolSize = sizeof(TSymbol);

		symbols.clear();
		symbolOccurences.clear();

		LookupMultitable<TSymbol, uint64_t> symbolToIndex;
		const auto symbolToIndexEnd = symbolToIndex.end();

		const TSymbol* const symbolBufferEnd = symbolBuffer + bufferLength;
		for (; symbolBuffer != symbolBufferEnd; ++symbolBuffer)
		{
			const auto it = symbolToIndex.find(*symbolBuffer);
			if (it == symbolToIndexEnd)
			{
				symbolToIndex.emplace(*symbolBuffer, symbols.size());
				symbols.push_back(*symbolBuffer);
				symbolOccurences.push_back(1);
			}
			else
			{
				++symbolOccurences[*(it->second)];
			}
		}
	}

	template<typename TSymbol, std::endian BitEndianness>
	HuffmanEncoder<TSymbol, BitEndianness>::HuffmanEncoder(const TSymbol* symbols, const uint64_t* codeLengths, uint64_t symbolCount) :
		_table()
	{
		HuffmanTableEntry<TSymbol>* table = HuffmanTableEntry<TSymbol>::createTable();

		const TSymbol* const symbolsEnd = symbols + symbolCount;
		for (; symbols != symbolsEnd; ++symbols, ++codeLengths)
		{
			Code code;
			code.byteCount = (*codeLengths >> 3);
			code.bitCount = *codeLengths & 7;
			code.data = new uint8_t[code.byteCount + (code.bitCount != 0)];

			bool success = HuffmanTableEntry<TSymbol>::addSymbol(table, *symbols, *codeLengths, code.data);
			assert(success);

			success = _table.emplace(*symbols, code);
			assert(success);
		}

		if constexpr (BitEndianness == std::endian::little)
		{
			for (const std::pair<TSymbol, Code*>& elt : _table)
			{
				uint8_t* data = elt.second->data;
				const uint8_t* const dataEnd = data + elt.second->byteCount + (elt.second->bitCount != 0);
				for (; data != dataEnd; ++data)
				{
					*data = bitswap(*data);
				}
			}
		}

		HuffmanTableEntry<TSymbol>::destroyTable(table);
	}

	template<typename TSymbol, std::endian BitEndianness>
	template<uint8_t DstOffset>
	void HuffmanEncoder<TSymbol, BitEndianness>::writeSymbol(const TSymbol& symbol, uint8_t* dst, uint64_t& bytesWritten, uint8_t& bitsWritten) const
	{
		auto it = _table.find(symbol);
		assert(it != _table.end());

		const Code& code = *(it->second);

		bytesWritten = code.byteCount;
		bitsWritten = code.bitCount;

		bitcpy<BitEndianness, 0, DstOffset>(code.data, dst, bytesWritten, bitsWritten);
	}

	template<typename TSymbol, std::endian BitEndianness>
	void HuffmanEncoder<TSymbol, BitEndianness>::writeSymbol(const TSymbol& symbol, uint8_t* dst, uint8_t dstOffset, uint64_t& bytesWritten, uint8_t& bitsWritten)
	{
		assert(dstOffset < 8);

		switch (dstOffset)
		{
			case 0:
				return writeSymbol<0>(symbol, dst, bytesWritten, bitsWritten);
			case 1:
				return writeSymbol<1>(symbol, dst, bytesWritten, bitsWritten);
			case 2:
				return writeSymbol<2>(symbol, dst, bytesWritten, bitsWritten);
			case 3:
				return writeSymbol<3>(symbol, dst, bytesWritten, bitsWritten);
			case 4:
				return writeSymbol<4>(symbol, dst, bytesWritten, bitsWritten);
			case 5:
				return writeSymbol<5>(symbol, dst, bytesWritten, bitsWritten);
			case 6:
				return writeSymbol<6>(symbol, dst, bytesWritten, bitsWritten);
			case 7:
				return writeSymbol<7>(symbol, dst, bytesWritten, bitsWritten);
		}
	}

	template<typename TSymbol, std::endian BitEndianness>
	HuffmanEncoder<TSymbol, BitEndianness>::~HuffmanEncoder()
	{
		for (const std::pair<TSymbol, Code*> elt : _table)
		{
			delete[] elt.second->data;
		}
	}


	template<typename TSymbol, std::endian BitEndianness>
	HuffmanDecoder<TSymbol, BitEndianness>::HuffmanDecoder(const TSymbol* symbols, const uint64_t* codeLengths, uint64_t symbolCount) :
		_table(HuffmanTableEntry<TSymbol>::createTable())
	{
		const TSymbol* const symbolsEnd = symbols + symbolCount;
		for (; symbols != symbolsEnd; ++symbols, ++codeLengths)
		{
			bool success = HuffmanTableEntry<TSymbol>::addSymbol(_table, *symbols, *codeLengths);
			assert(success);
		}

		if constexpr (BitEndianness == std::endian::little)
		{
			HuffmanTableEntry<TSymbol>::swapEndianness(_table);
		}
	}

	template<typename TSymbol, std::endian BitEndianness>
	template<uint8_t SrcOffset>
	bool HuffmanDecoder<TSymbol, BitEndianness>::readSymbol(TSymbol& symbol, const uint8_t* src, uint64_t& bytesRead, uint8_t& bitsRead) const
	{
		static constexpr uint8_t revSrcOffset = 8 - SrcOffset;

		const HuffmanTableEntry<TSymbol>* entry;
		if constexpr (SrcOffset == 0)
		{
			entry = _table + *src;
		}
		else
		{
			if constexpr (BitEndianness == std::endian::big)
			{
				entry = _table + (static_cast<uint8_t>(*src << SrcOffset) | (*(src + 1) >> revSrcOffset));
			}
			else
			{
				entry = _table + ((*src >> SrcOffset) | static_cast<uint8_t>(*(src + 1) << revSrcOffset));
			}
		}

		bytesRead = 0;
		while (entry->bitCount == 0)
		{
			if (!entry->next)
			{
				return false;
			}

			if constexpr (SrcOffset == 0)
			{
				entry = entry->next + *(++src);
			}
			else
			{
				++src;
				if constexpr (BitEndianness == std::endian::big)
				{
					entry = entry->next + (static_cast<uint8_t>(*src << SrcOffset) | (*(src + 1) >> revSrcOffset));
				}
				else
				{
					entry = entry->next + ((*src >> SrcOffset) | static_cast<uint8_t>(*(src + 1) << revSrcOffset));
				}

			}

			++bytesRead;
		}

		bitsRead = entry->bitCount & 7;
		bytesRead += entry->bitCount >> 3;
		symbol = entry->symbol;

		return true;
	}

	template<typename TSymbol, std::endian BitEndianness>
	bool HuffmanDecoder<TSymbol, BitEndianness>::readSymbol(TSymbol& symbol, const uint8_t* src, uint8_t srcOffset, uint64_t& bytesRead, uint8_t& bitsRead) const
	{
		assert(srcOffset < 8);

		switch (srcOffset)
		{
			case 0:
				return readSymbol<0>(symbol, src, bytesRead, bitsRead);
			case 1:
				return readSymbol<1>(symbol, src, bytesRead, bitsRead);
			case 2:
				return readSymbol<2>(symbol, src, bytesRead, bitsRead);
			case 3:
				return readSymbol<3>(symbol, src, bytesRead, bitsRead);
			case 4:
				return readSymbol<4>(symbol, src, bytesRead, bitsRead);
			case 5:
				return readSymbol<5>(symbol, src, bytesRead, bitsRead);
			case 6:
				return readSymbol<6>(symbol, src, bytesRead, bitsRead);
			case 7:
				return readSymbol<7>(symbol, src, bytesRead, bitsRead);
		}
	}

	template<typename TSymbol, std::endian BitEndianness>
	HuffmanDecoder<TSymbol, BitEndianness>::~HuffmanDecoder()
	{
		HuffmanTableEntry<TSymbol>::destroyTable(_table);
	}
}
