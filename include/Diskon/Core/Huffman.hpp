///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Core/CoreTypes.hpp>

namespace dsk
{
	template<typename TSymbol>
	struct HuffmanTableEntry
	{
		uint8_t bitCount = 0;
		union
		{
			HuffmanTableEntry<TSymbol>* next = nullptr;
			TSymbol symbol;
		};

		static HuffmanTableEntry<TSymbol>* createTable();
		static bool addSymbol(HuffmanTableEntry<TSymbol>* table, const TSymbol& symbol, uint64_t codeLength, uint8_t* code = nullptr);
		static void swapEndianness(HuffmanTableEntry<TSymbol>* table);
		static void destroyTable(HuffmanTableEntry<TSymbol>* table);
	};

	// TODO: Handle different orders of construction of the tree (1 before 0) ? Is it necessary ?

	template<typename TSymbol, std::endian BitEndianness = std::endian::big>
	class HuffmanEncoder
	{
		public:

			static void symbolOccurencesToCodeLengths(const TSymbol* symbols, const uint64_t* symbolOccurences, uint64_t* codeLengths, uint64_t symbolCount);
			static void symbolBufferToSymbolOccurences(const TSymbol* symbolBuffer, uint64_t bufferLength, std::vector<TSymbol>& symbols, std::vector<uint64_t>& symbolOccurences);

			HuffmanEncoder(const TSymbol* symbols, const uint64_t* codeLengths, uint64_t symbolCount);
			HuffmanEncoder(const HuffmanEncoder& encoder) = delete;
			HuffmanEncoder(HuffmanEncoder&& encoder) = delete;

			HuffmanEncoder& operator=(const HuffmanEncoder& encoder) = delete;
			HuffmanEncoder& operator=(HuffmanEncoder&& encoder) = delete;

			template<uint8_t DstOffset = 0> void writeSymbol(const TSymbol& symbol, uint8_t* dst, uint64_t& bytesWritten, uint8_t& bitsWritten) const;
			void writeSymbol(const TSymbol& symbol, uint8_t* dst, uint8_t dstOffset, uint64_t& bytesWritten, uint8_t& bitsWritten);

			~HuffmanEncoder();

		private:

			struct Code
			{
				uint8_t* data;
				uint64_t byteCount;
				uint8_t bitCount;
			};

			LookupMultitable<TSymbol, Code> _table;
	};

	template<typename TSymbol, std::endian BitEndianness = std::endian::big>
	class HuffmanDecoder
	{
		public:

			HuffmanDecoder(const TSymbol* symbols, const uint64_t* codeLengths, uint64_t symbolCount);
			HuffmanDecoder(const HuffmanDecoder<TSymbol>& decoder) = delete;
			HuffmanDecoder(HuffmanDecoder<TSymbol>&& decoder) = delete;

			HuffmanDecoder<TSymbol>& operator=(const HuffmanDecoder<TSymbol>& decoder) = delete;
			HuffmanDecoder<TSymbol>& operator=(HuffmanDecoder<TSymbol>&& decoder) = delete;

			template<uint8_t SrcOffset = 0> bool readSymbol(TSymbol& symbol, const uint8_t* src, uint64_t& bytesRead, uint8_t& bitsRead) const;
			bool readSymbol(TSymbol& symbol, const uint8_t* src, uint8_t srcOffset, uint64_t& bytesRead, uint8_t& bitsRead) const;

			~HuffmanDecoder();

		private:

			HuffmanTableEntry<TSymbol>* _table;
	};
}
