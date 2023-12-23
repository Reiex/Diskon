///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace
		{
			/*
			* Sort according codeLengths in ascending order, and copy each operation on symbols to keep the associations (symbol, codeLength)
			* 0 is the highest code length, not the lowest
			* symbolCount at the end of the function is the number of symbol that have non-zero code length
			*/
			template<typename TSymbol>
			void sortCodeLengths(TSymbol* symbols, uint8_t* codeLengths, uint16_t& symbolCount)
			{
				// We check "the real end" because deflate creates a lot of 0s at the end of the codeLengths array

				TSymbol* symbolsEnd = symbols + symbolCount;
				uint8_t* codeLengthsEnd = codeLengths + symbolCount;

				while (*codeLengthsEnd == 0)
				{
					--codeLengthsEnd;
					--symbolsEnd;
					--symbolCount;
				}

				// Move the 0s that are not already at the end to the end

				for (uint16_t i = 0; i < symbolCount; ++i)
				{
					if (codeLengths[i] == 0)
					{
						std::swap(codeLengths[i], *codeLengthsEnd);
						std::swap(symbols[i], *symbolsEnd);
						--codeLengthsEnd;
						--symbolsEnd;
						--symbolCount;
						--i;
					}
				}

				// Now we only have non-0 codeLengths, bubble sort

				bool changed = true;
				while (changed)
				{
					changed = false;
					for (uint16_t i = 1; i < symbolCount; ++i)
					{
						if (codeLengths[i] < codeLengths[i - 1])
						{
							std::swap(codeLengths[i], codeLengths[i - 1]);
							std::swap(symbols[i], symbols[i - 1]);
							changed = true;
						}
					}
				}
			}
		}

		DeflateIStream::DeflateIStream(IStream* stream) : FormatIStream(stream),
			_litlenDecoder(nullptr),
			_distDecoder(nullptr),
			_readingBlock(false),
			_readingLastBlock(false),
			_currentBlockCompressed(false),
			_currentBlockRemainingSize(0),
			_bytesRead(0),
			_window(),
			_windowIndex(0),
			_repeaterDistance(0),
			_remainingRepeats(0)
		{
			if (stream)
			{
				setStreamState();
			}
		}
		
		void DeflateIStream::readFile(deflate::File& file)
		{
			DSKFMT_BEGIN();
			
			assert(!_readingBlock);
			assert(!_bytesRead);
		
			file.blocks.clear();
		
			do {
				file.blocks.emplace_back();
				DSK_CALL(readBlock, file.blocks.back());
			} while (!file.blocks.back().header.isFinal);
		}
		
		void DeflateIStream::readBlock(deflate::Block& block)
		{
			DSKFMT_BEGIN();
		
			assert(!_readingBlock);
		
			block.data.clear();
		
			// Read block header
		
			DSK_CALL(readBlockHeader, block.header);
		
			// Read block data
		
			constexpr uint64_t bufferSize = _singleBufferSize;
			uint8_t buffer[bufferSize];
			uint64_t sizeRead;
			do {
				DSK_CALL(readBlockData, buffer, bufferSize, sizeRead);
				block.data.insert(block.data.end(), buffer, buffer + sizeRead);
			} while (sizeRead == bufferSize);
		
			// Read block end
		
			DSK_CALL(readBlockEnd);
		}
		
		void DeflateIStream::readBlockHeader(deflate::BlockHeader& header)
		{
			DSKFMT_BEGIN();
		
			assert(!_readingBlock);
		
			uint8_t buffer;
		
			// Read the header flags (BFINAL and BTYPE)
		
			_readingBlock = true;
		
			DSKFMT_STREAM_CALL(bitRead, _readingLastBlock);
			header.isFinal = _readingLastBlock;

			DSKFMT_STREAM_CALL(bitRead, &buffer, 2);
			DSK_CHECK(buffer != 3, "Compression type cannot be 0b11.");
			header.compressionType = static_cast<deflate::CompressionType>(buffer);
		
			// Read the rest of the header depending on the compression type
		
			uint16_t hlit = 288;
			uint16_t hdist = 32;
			uint64_t codeLengths[288];
			uint8_t codeLengths8bit[320];
			switch (header.compressionType)
			{
				case deflate::CompressionType::NoCompression:
				{
					_currentBlockCompressed = false;
		
					DSKFMT_STREAM_CALL(finishByte);
		
					uint16_t nlen;
					DSKFMT_STREAM_CALL(read, _currentBlockRemainingSize);
					DSKFMT_STREAM_CALL(read, nlen);
		
					DSK_CHECK(_currentBlockRemainingSize == static_cast<uint16_t>(~nlen), "Block length check for non-compressed block failed.");
		
					break;
				}
				case deflate::CompressionType::FixedHuffman:
				{
					_currentBlockCompressed = true;
					_currentBlockLastByteRead = false;
					
					std::fill_n(header.litlenCodeLengths, 144, 8);
					std::fill_n(header.litlenCodeLengths + 144, 256 - 144, 9);
					std::fill_n(header.litlenCodeLengths + 256, 280 - 256, 7);
					std::fill_n(header.litlenCodeLengths + 280, 288 - 280, 8);
					std::fill_n(header.distCodeLengths, 32, 5);

					break;
				}
				case deflate::CompressionType::DynamicHuffman:
				{
					_currentBlockCompressed = true;
					_currentBlockLastByteRead = false;
		
					// Read additional header info (number of literal codes, distance codes, and code length codes)
		
					DSKFMT_STREAM_CALL(bitRead, &buffer, 5);
					hlit = buffer + 257;
					DSKFMT_STREAM_CALL(bitRead, &buffer, 5);
					hdist = buffer + 1;
					DSKFMT_STREAM_CALL(bitRead, &buffer, 4);
					const uint8_t hclen = buffer + 4;
		
					// Read code lengths for the code length alphabet and create the associated huffman decoder
		
					uint8_t symbols[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
		
					for (uint8_t i = 0; i < hclen; ++i)
					{
						DSKFMT_STREAM_CALL(bitRead, codeLengths8bit + i, 3);
					}
					std::fill_n(codeLengths8bit + hclen, 19 - hclen, 0);

					uint16_t symbolCount = 19;
					sortCodeLengths(symbols, codeLengths8bit, symbolCount);
					std::copy_n(codeLengths8bit, symbolCount, codeLengths);

					HuffmanDecoder<uint8_t, std::endian::little> codeLengthsDecoder(symbols, codeLengths, symbolCount);
					
					// Read code lengths for the two alphabets (literal/length + distances)

					const uint16_t codeLengthCount = hlit + hdist;
		
					bool success;

					uint64_t bytesRead;
					uint8_t bitsRead;
					uint8_t nextByte;
					DSKFMT_STREAM_CALL(bitRead, &nextByte, 8, 0);

					uint8_t symbol;

					uint16_t i = 0;
					while (i != codeLengthCount)
					{
						success = codeLengthsDecoder.readSymbol(symbol, &nextByte, bytesRead, bitsRead);
						DSK_CHECK(success, "Error while reading code length symbol.");

						nextByte >>= bitsRead;
						DSKFMT_STREAM_CALL(bitRead, &nextByte, bitsRead, 8 - bitsRead);

						if (symbol < 16)
						{
							codeLengths8bit[i] = symbol;
							++i;
						}
						else
						{
							if (symbol == 16)
							{
								DSK_CHECK(i != 0, "The first symbol cannot be a repeat symbol.");

								bitsRead = 2;
								const uint8_t repeat = (nextByte & 3) + 3;
								std::fill_n(codeLengths8bit + i, repeat, codeLengths8bit[i - 1]);
								i += repeat;
							}
							else if (symbol == 17)
							{
								bitsRead = 3;
								const uint8_t repeat = (nextByte & 7) + 3;
								std::fill_n(codeLengths8bit + i, repeat, 0);
								i += repeat;
							}
							else if (symbol == 18)
							{
								bitsRead = 7;
								const uint8_t repeat = (nextByte & 127) + 11;
								std::fill_n(codeLengths8bit + i, repeat, 0);
								i += repeat;
							}

							nextByte >>= bitsRead;
							DSKFMT_STREAM_CALL(bitRead, &nextByte, bitsRead, 8 - bitsRead);
						}
		
						DSK_CHECK(i <= codeLengthCount, "Too many code lengths given for dynamic huffman tree.");
					}

					DSKFMT_STREAM_CALL(bitUnread, 8);
		
					std::copy_n(codeLengths8bit, hlit, header.litlenCodeLengths);
					std::fill_n(header.litlenCodeLengths + hlit, 288 - hlit, 0);
					std::copy_n(codeLengths8bit + hlit, hdist, header.distCodeLengths);
					std::fill_n(header.distCodeLengths + hdist, 32 - hdist, 0);
		
					// TODO: Check the code lengths are valid

					break;
				}
			}

			// Create huffman decoders if necessary

			if (_currentBlockCompressed)
			{
				{
					uint16_t symbolCount = 288;
					std::copy_n(header.litlenCodeLengths, 288, codeLengths8bit);

					uint16_t symbols[288];
					for (uint16_t i = 0; i < 288; ++i)
					{
						symbols[i] = i;
					}

					sortCodeLengths(symbols, codeLengths8bit, symbolCount);
					std::copy_n(codeLengths8bit, symbolCount, codeLengths);

					_litlenDecoder = new HuffmanDecoder<uint16_t, std::endian::little>(symbols, codeLengths, symbolCount);
				}

				{
					uint16_t symbolCount = 32;
					std::copy_n(header.distCodeLengths, 32, codeLengths8bit);

					uint8_t symbols[32];
					for (uint16_t i = 0; i < 32; ++i)
					{
						symbols[i] = i;
					}

					sortCodeLengths(symbols, codeLengths8bit, symbolCount);
					std::copy_n(codeLengths8bit, symbolCount, codeLengths);

					_distDecoder = new HuffmanDecoder<uint8_t, std::endian::little>(symbols, codeLengths, symbolCount);
				}
			}
		}
		
		void DeflateIStream::readBlockData(uint8_t* data, uint64_t size, uint64_t& sizeRead)
		{
			DSKFMT_BEGIN();
		
			assert(_readingBlock);

			// Compressed data

			if (_currentBlockCompressed)
			{
				bool success;

				uint64_t bytesRead;
				uint8_t bitsRead;
				uint8_t buffer[2];
				DSKFMT_STREAM_CALL(bitRead, buffer, 16, 0);

				uint16_t litlenSymbol;
				uint8_t distSymbol;

				sizeRead = 0;
				while (size && !_currentBlockLastByteRead)
				{
					// If repeating characters, extract nothing from the stream and just read in _window

					if (_remainingRepeats)
					{
						uint16_t iSrc = (_windowIndex + _windowSize - _repeaterDistance) & _windowIndexFilter;

						for (; size && _remainingRepeats; --_remainingRepeats, --size, ++data, ++sizeRead, ++_bytesRead)
						{
							_window[_windowIndex] = _window[iSrc];
							*data = _window[_windowIndex];

							_windowIndex = (_windowIndex + 1) & _windowIndexFilter;
							iSrc = (iSrc + 1) & _windowIndexFilter;
						}
					}

					// Parse a new symbol

					else
					{
						// Read a literal/length symbol

						success = _litlenDecoder->readSymbol(litlenSymbol, buffer, bytesRead, bitsRead);
						DSK_CHECK(success, "Error while reading literal/length symbol.");

						buffer[0] = (bytesRead ? 0 : (buffer[1] << (8 - bitsRead))) | (buffer[bytesRead] >> bitsRead);
						buffer[1] >>= bitsRead;
						DSKFMT_STREAM_CALL(bitRead, buffer + (bytesRead == 0 || bitsRead == 0), (bytesRead << 3) + bitsRead, (8 - bitsRead) & 7);

						// Raw byte

						if (litlenSymbol < 256)
						{
							*data = litlenSymbol;
							_window[_windowIndex] = *data;

							--size;
							++data;
							++sizeRead;
							++_bytesRead;
							_windowIndex = (_windowIndex + 1) & _windowIndexFilter;
						}

						// End of block

						else if (litlenSymbol == 256)
						{
							_currentBlockLastByteRead = true;
						}

						// Repeater

						else
						{
							DSK_CHECK(litlenSymbol < 286, "Codes 286-287 are reserved.");

							// Read repeating length

							static constexpr uint8_t lenExtraBits[] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0 };
							static constexpr uint8_t filterLenExtraBits[] = { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 3, 3, 3, 3, 7, 7, 7, 7, 15, 15, 15, 15, 31, 31, 31, 31, 0 };
							static constexpr uint16_t lenStart[] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258 };

							litlenSymbol -= 257;
							_remainingRepeats = lenStart[litlenSymbol];

							bitsRead = lenExtraBits[litlenSymbol];
							if (bitsRead)
							{
								_remainingRepeats += buffer[0] & filterLenExtraBits[litlenSymbol];
								buffer[0] = (bytesRead ? 0 : (buffer[1] << (8 - bitsRead))) | (buffer[bytesRead] >> bitsRead);
								buffer[1] >>= bitsRead;
								DSKFMT_STREAM_CALL(bitRead, buffer + 1, bitsRead, 8 - bitsRead);
							}

							// Read repeating distance

							static constexpr uint8_t distExtraBits[] = { 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };
							static constexpr uint16_t filterDistExtraBits[] = { 0, 0, 0, 0, 1, 1, 3, 3, 7, 7, 15, 15, 31, 31, 63, 63, 127, 127, 0, 0, 1, 1, 3, 3, 7, 7, 15, 15, 31, 31 };
							static constexpr uint16_t distStart[] = { 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };

							success = _distDecoder->readSymbol(distSymbol, buffer, bytesRead, bitsRead);
							DSK_CHECK(success, "Error while reading distance symbol.");
							DSK_CHECK(distSymbol < 30, "Distance codes 30-31 are reserved.");

							buffer[0] = (bytesRead ? 0 : (buffer[1] << (8 - bitsRead))) | (buffer[bytesRead] >> bitsRead);
							buffer[1] >>= bitsRead;
							DSKFMT_STREAM_CALL(bitRead, buffer + 1 - bytesRead, (bytesRead << 3) + bitsRead, (8 - bitsRead) & 7);

							_repeaterDistance = distStart[distSymbol];

							bytesRead = distExtraBits[distSymbol] >> 3;
							bitsRead = distExtraBits[distSymbol] & 7;
							if (bytesRead)
							{
								_repeaterDistance += buffer[0] | ((buffer[1] & filterDistExtraBits[distSymbol]) << 8);
								buffer[0] = buffer[1] >> bitsRead;
								DSKFMT_STREAM_CALL(bitRead, buffer + (bitsRead == 0), 8 + bitsRead, (8 - bitsRead) & 7);
							}
							else if (bitsRead)
							{
								_repeaterDistance += buffer[0] & filterDistExtraBits[distSymbol];
								buffer[0] = (buffer[1] << (8 - bitsRead)) | (buffer[0] >> bitsRead);
								buffer[1] >>= bitsRead;
								DSKFMT_STREAM_CALL(bitRead, buffer + 1, bitsRead, 8 - bitsRead);
							}

							DSK_CHECK(_repeaterDistance <= _bytesRead, "Distance code goes further than beginning of deflate stream window.");
						}
					}
				}

				DSKFMT_STREAM_CALL(bitUnread, 16);
			}

			// Uncompressed data

			else
			{
				sizeRead = std::min<uint64_t>(_currentBlockRemainingSize, size);
				DSKFMT_STREAM_CALL(read, data, sizeRead);
				_currentBlockRemainingSize -= sizeRead;
				_bytesRead += sizeRead;

				if (sizeRead < _windowSize)
				{
					if (_windowIndex + sizeRead > _windowSize)
					{
						uint16_t sizeBegin = _windowSize - _windowIndex;
						std::copy_n(data, sizeBegin, _window + _windowIndex);
						std::copy_n(data + sizeBegin, sizeRead - sizeBegin, _window);
					}
					else
					{
						std::copy_n(data, sizeRead, _window + _windowIndex);
					}

					_windowIndex = (_windowIndex + sizeRead) & _windowIndexFilter;
				}
				else
				{
					std::copy_n(data - _windowSize, _windowSize, _window);
					_windowIndex = 0;
				}
			}
		}
		
		void DeflateIStream::readBlockEnd()
		{
			DSKFMT_BEGIN();

			assert(_readingBlock);

			if (_currentBlockCompressed)
			{
				if (!_currentBlockLastByteRead)
				{
					assert(_remainingRepeats == 0);
					
					uint64_t bytesRead;
					uint8_t bitsRead;
					uint8_t buffer[2];
					DSKFMT_STREAM_CALL(bitRead, buffer, 16, 0);

					uint16_t symbol;
					_litlenDecoder->readSymbol(symbol, buffer, bytesRead, bitsRead);

					DSKFMT_STREAM_CALL(bitUnread, 16 - (bytesRead << 3) - bitsRead);
				}

				delete _litlenDecoder;
				_litlenDecoder = nullptr;

				delete _distDecoder;
				_distDecoder = nullptr;

				_currentBlockCompressed = false;
				_currentBlockRemainingSize = 0;	// More powerful than setting _currentBlockLastByteRead

				_repeaterDistance = 0;
			}
			else
			{
				assert(_currentBlockRemainingSize == 0);
			}

			_readingBlock = false;
			if (_readingLastBlock)
			{
				_readingLastBlock = false;
				_bytesRead = 0;
				_windowIndex = 0;
			}
		}

		DeflateIStream::~DeflateIStream()
		{
			if (_litlenDecoder)
			{
				delete _litlenDecoder;
			}

			if (_distDecoder)
			{
				delete _distDecoder;
			}
		}
		
		void DeflateIStream::setStreamState()
		{
			_stream->setBitEndianness(std::endian::little);
			_stream->setByteEndianness(std::endian::little);
		}

		void DeflateIStream::resetFormatState()
		{
			if (_litlenDecoder)
			{
				delete _litlenDecoder;
				_litlenDecoder = nullptr;
			}

			if (_distDecoder)
			{
				delete _distDecoder;
				_distDecoder = nullptr;
			}

			_readingBlock = false;
			_readingLastBlock = false;
			_currentBlockCompressed = false;
			_currentBlockRemainingSize = 0;
			_bytesRead = 0;
			_windowIndex = 0;
			_repeaterDistance = 0;
			_remainingRepeats = 0;
		}


		DeflateOStream::DeflateOStream(OStream* stream) : FormatOStream(stream),
			_litlenEncoder(nullptr),
			_distEncoder(nullptr),
			_writingBlock(false),
			_writingLastBlock(false),
			_currentBlockCompressed(false),
			_currentBlockRemainingSize(0),
			_bytesWritten(0),
			_window(),
			_windowIndex(0)
		{
			if (stream)
			{
				setStreamState();
			}
		}
		
		void DeflateOStream::writeFile(const deflate::File& file)
		{
			DSKFMT_BEGIN();
		
			assert(!_writingBlock);
			assert(!_bytesWritten);
		
			assert(!file.blocks.empty());
		
			for (uint64_t i = 0; i < file.blocks.size() - 1; ++i)
			{
				assert(!file.blocks[i].header.isFinal);
			}
			assert(file.blocks.back().header.isFinal);
		
		
			for (const deflate::Block& block : file.blocks)
			{
				DSK_CALL(writeBlock, block);
			}
		}
		
		void DeflateOStream::writeBlock(const deflate::Block& block)
		{
			DSKFMT_BEGIN();
		
			assert(!_writingBlock);
		
			DSK_CALL(writeBlockHeader, block.header, block.data.size());
			DSK_CALL(writeBlockData, block.data.data(), block.data.size());
			DSK_CALL(writeBlockEnd);
		}
		
		void DeflateOStream::writeBlockHeader(const deflate::BlockHeader& header, uint16_t size)
		{
			DSKFMT_BEGIN();
		
			assert(!_writingBlock);

			// Write the header flags (BFINAL and BTYPE)

			_writingBlock = true;
			_writingLastBlock = header.isFinal;
		
			DSKFMT_STREAM_CALL(bitWrite, header.isFinal);
			DSKFMT_STREAM_CALL(bitWrite, reinterpret_cast<const uint8_t*>(&header.compressionType), 2);
		
			// Write the rest of the header depending on the compression type

			uint16_t hlit = 288;
			uint16_t hdist = 32;
			uint64_t codeLengths[288];
			uint8_t codeLengths8bit[320];
			switch (header.compressionType)
			{
				case deflate::CompressionType::NoCompression:
				{
					_currentBlockCompressed = false;
					_currentBlockRemainingSize = size;
		
					DSKFMT_STREAM_CALL(finishByte);
					DSKFMT_STREAM_CALL(write, size);
					DSKFMT_STREAM_CALL(write, static_cast<uint16_t>(~size));

					break;
				}
				case deflate::CompressionType::FixedHuffman:
				{
					_currentBlockCompressed = true;

					for (uint16_t i = 0; i < 144; ++i)
					{
						codeLengths8bit[i] = 8;
					}
					for (uint16_t i = 144; i < 256; ++i)
					{
						codeLengths8bit[i] = 9;
					}
					for (uint16_t i = 256; i < 280; ++i)
					{
						codeLengths8bit[i] = 7;
					}
					for (uint16_t i = 280; i < 288; ++i)
					{
						codeLengths8bit[i] = 8;
					}
		
					std::fill_n(codeLengths8bit + 288, 32, 5);
		
					break;
				}
				case deflate::CompressionType::DynamicHuffman:
				{
					_currentBlockCompressed = true;
		
					for (const uint8_t& x : header.litlenCodeLengths)
					{
						assert(x < 16);
					}
					for (const uint8_t& x : header.distCodeLengths)
					{
						assert(x < 16);
					}
					assert(header.litlenCodeLengths[256] != 0);
		
					uint8_t buffer;

					// Compute and write HLIT
		
					for (; header.litlenCodeLengths[hlit - 1] == 0; --hlit);
		
					buffer = hlit - 257;
					DSKFMT_STREAM_CALL(bitWrite, &buffer, 5);
		
					// Compute and write HDIST
		
					for (; header.distCodeLengths[hdist - 1] == 0 && hdist; --hdist);
		
					buffer = hdist - 1;
					DSKFMT_STREAM_CALL(bitWrite, &buffer, 5);
		
					// Compute and write HCLEN
		
					// TODO: Compress code lengths
		
					uint8_t hclen = 19;
		
					buffer = hclen - 4;
					DSKFMT_STREAM_CALL(bitWrite, &buffer, 4);
		
					// Write code lengths for the code length alphabet and create the associated huffman encoder

					uint8_t symbols[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

					std::fill_n(codeLengths8bit, hclen, 5);

					for (uint8_t i = 0; i < hclen; ++i)
					{
						DSKFMT_STREAM_CALL(bitWrite, codeLengths8bit + i, 3);
					}
		
					uint16_t symbolCount = hclen;
					sortCodeLengths(symbols, codeLengths8bit, symbolCount);
					std::copy_n(codeLengths8bit, symbolCount, codeLengths);

					HuffmanEncoder<uint8_t, std::endian::little> codeLengthsEncoder(symbols, codeLengths, symbolCount);

					// Write code lengths for the literal/length alphabet to codeLengths8bit (used as a buffer to improve perfs)

					uint8_t bitCount = 0;
					uint64_t byteCount = 0;

					for (uint16_t i = 0; i < hlit; ++i)
					{
						uint8_t bitsWritten;
						uint64_t bytesWritten;
						codeLengthsEncoder.writeSymbol(header.litlenCodeLengths[i], codeLengths8bit + byteCount, bitCount, bytesWritten, bitsWritten);
						bitCount += bitsWritten;
						byteCount += bytesWritten + (bitCount >> 3);
						bitCount &= 7;
					}
		
					// Write code lengths for the distance alphabet to codeLengths8bit (used as a buffer to improve perfs)
		
					for (uint8_t i = 0; i < hdist; ++i)
					{
						uint8_t bitsWritten;
						uint64_t bytesWritten;
						codeLengthsEncoder.writeSymbol(header.distCodeLengths[i], codeLengths8bit + byteCount, bitCount, bytesWritten, bitsWritten);
						bitCount += bitsWritten;
						byteCount += bytesWritten + (bitCount >> 3);
						bitCount &= 7;
					}

					// Write all code lengths to the stream

					DSKFMT_STREAM_CALL(bitWrite, codeLengths8bit, (byteCount << 3) + bitCount);
		
					// Fill codeLengths8bit to create encoder later

					std::copy_n(header.litlenCodeLengths, 288, codeLengths8bit);
					std::copy_n(header.distCodeLengths, 32, codeLengths8bit + 288);

					break;
				}
			}
		
			// Create huffman encoders if necessary

			if (_currentBlockCompressed)
			{
				{
					uint16_t symbolCount = 288;

					uint16_t symbols[288];
					for (uint16_t i = 0; i < 288; ++i)
					{
						symbols[i] = i;
					}

					sortCodeLengths(symbols, codeLengths8bit, symbolCount);
					std::copy_n(codeLengths8bit, symbolCount, codeLengths);

					_litlenEncoder = new HuffmanEncoder<uint16_t, std::endian::little>(symbols, codeLengths, symbolCount);
				}

				{
					uint16_t symbolCount = 32;

					uint8_t symbols[32];
					for (uint16_t i = 0; i < 32; ++i)
					{
						symbols[i] = i;
					}

					sortCodeLengths(symbols, codeLengths8bit + 288, symbolCount);
					std::copy_n(codeLengths8bit + 288, symbolCount, codeLengths);

					_distEncoder = new HuffmanEncoder<uint8_t, std::endian::little>(symbols, codeLengths, symbolCount);
				}
			}
		}
		
		void DeflateOStream::writeBlockData(const uint8_t* data, uint64_t size)
		{
			DSKFMT_BEGIN();
		
			assert(_writingBlock);
		
			if (_currentBlockCompressed)
			{
				// TODO: LZ77 compression

				uint8_t buffer[_singleBufferSize];
				while (size)
				{
					uint8_t bitCount = 0;
					uint64_t byteCount = 0;
					uint64_t symbolCount = _singleBufferSize / 2;
					for (; symbolCount && size; --symbolCount, --size, ++data, ++_bytesWritten)
					{
						_window[_windowIndex] = *data;

						uint8_t bitsWritten;
						uint64_t bytesWritten;
						_litlenEncoder->writeSymbol(*data, buffer + byteCount, bitCount, bytesWritten, bitsWritten);
						bitCount += bitsWritten;
						byteCount += bytesWritten + (bitCount >> 3);
						bitCount &= 7;

						_windowIndex = (_windowIndex + 1) & _windowIndexFilter;
					}

					DSKFMT_STREAM_CALL(bitWrite, buffer, (byteCount << 3) + bitCount);
				}
			}
			else
			{
				assert(_currentBlockRemainingSize >= size);
		
				DSKFMT_STREAM_CALL(write, data, size);
				_currentBlockRemainingSize -= size;

				if (size < _windowSize)
				{
					if (_windowIndex + size > _windowSize)
					{
						uint16_t sizeBegin = _windowSize - _windowIndex;
						std::copy_n(data, sizeBegin, _window + _windowIndex);
						std::copy_n(data + sizeBegin, size - sizeBegin, _window);
					}
					else
					{
						std::copy_n(data, size, _window + _windowIndex);
					}

					_windowIndex = (_windowIndex + size) & _windowIndexFilter;
				}
				else
				{
					std::copy_n(data - _windowSize, _windowSize, _window);
					_windowIndex = 0;
				}
			}
		}
		
		void DeflateOStream::writeBlockEnd()
		{
			DSKFMT_BEGIN();
		
			assert(_writingBlock);
		
			if (_currentBlockCompressed)
			{
				uint8_t buffer[2];
				uint64_t bytesWritten;
				uint8_t bitsWritten;
				_litlenEncoder->writeSymbol(256, buffer, bytesWritten, bitsWritten);
				DSKFMT_STREAM_CALL(bitWrite, buffer, (bytesWritten << 3) + bitsWritten);

				delete _litlenEncoder;
				_litlenEncoder = nullptr;

				delete _distEncoder;
				_distEncoder = nullptr;

				_currentBlockCompressed = false;
				_currentBlockRemainingSize = 0;
			}
			else
			{
				assert(_currentBlockRemainingSize == 0);
			}

			_writingBlock = false;
			if (_writingLastBlock)
			{
				_writingLastBlock = false;
				_bytesWritten = 0;
				_windowIndex = 0;

				DSKFMT_STREAM_CALL(finishByte);
			}
		}
		
		DeflateOStream::~DeflateOStream()
		{
			if (_litlenEncoder)
			{
				delete _litlenEncoder;
			}

			if (_distEncoder)
			{
				delete _distEncoder;
			}
		}

		void DeflateOStream::setStreamState()
		{
			_stream->setBitEndianness(std::endian::little);
			_stream->setByteEndianness(std::endian::little);
		}

		void DeflateOStream::resetFormatState()
		{
			if (_litlenEncoder)
			{
				delete _litlenEncoder;
				_litlenEncoder = nullptr;
			}

			if (_distEncoder)
			{
				delete _distEncoder;
				_distEncoder = nullptr;
			}

			_writingBlock = false;
			_writingLastBlock = false;
			_currentBlockCompressed = false;
			_currentBlockRemainingSize = 0;
			_bytesWritten = 0;
			_windowIndex = 0;
		}
	}
}
