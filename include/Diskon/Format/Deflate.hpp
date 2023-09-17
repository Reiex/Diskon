///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Format/FormatTypes.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace deflate
		{
			enum class CompressionType : uint8_t
			{
				NoCompression	= 0b00,
				FixedHuffman	= 0b10,
				DynamicHuffman	= 0b01
			};
		
			struct BlockHeader
			{
				bool isFinal;
				CompressionType compressionType;
				uint8_t litlenCodeLengths[288];
				uint8_t distCodeLengths[32];
			};
		
			struct Block
			{
				BlockHeader header;
				std::vector<uint8_t> data;
			};
		
			struct File
			{
				std::vector<Block> blocks;
			};
		}
		
		class DSK_API DeflateIStream : public FormatIStream
		{
			public:
		
				DeflateIStream(IStream* stream);
				DeflateIStream(const DeflateIStream& stream) = delete;
				DeflateIStream(DeflateIStream&& stream) = delete;
		
				DeflateIStream& operator=(const DeflateIStream& stream) = delete;
				DeflateIStream& operator=(DeflateIStream&& stream) = delete;
		
				void readFile(deflate::File& file);
				void readBlock(deflate::Block& block);
				void readBlockHeader(deflate::BlockHeader& header);
				void readBlockData(uint8_t* data, uint64_t size, uint64_t& sizeRead);
				void readBlockEnd();
		
				~DeflateIStream();
		
			private:
		
				void setStreamState() override final;
				void resetFormatState() override final;

				static constexpr uint16_t _windowSize = 32768;
				static constexpr uint16_t _windowIndexFilter = _windowSize - 1;
		
				HuffmanDecoder<uint16_t, std::endian::little>* _litlenDecoder;
				HuffmanDecoder<uint8_t, std::endian::little>* _distDecoder;

				bool _readingBlock;

				bool _readingLastBlock;
				bool _currentBlockCompressed;

				union
				{
					uint16_t _currentBlockRemainingSize;
					bool _currentBlockLastByteRead;
				};

				uint64_t _bytesRead;
				uint8_t _window[_windowSize];
				uint16_t _windowIndex;

				uint16_t _repeaterDistance;
				uint16_t _remainingRepeats;
		};
		
		class DSK_API DeflateOStream : public FormatOStream
		{
			public:
		
				DeflateOStream(OStream* stream);
				DeflateOStream(const DeflateOStream& stream) = delete;
				DeflateOStream(DeflateOStream&& stream) = delete;
		
				DeflateOStream& operator=(const DeflateOStream& stream) = delete;
				DeflateOStream& operator=(DeflateOStream&& stream) = delete;
		
				void writeFile(const deflate::File& file);
				void writeBlock(const deflate::Block& block);
				void writeBlockHeader(const deflate::BlockHeader& header, uint16_t size = 0);
				void writeBlockData(const uint8_t* data, uint64_t size);
				void writeBlockEnd();
		
				~DeflateOStream();
		
			private:
		
				void setStreamState() override final;
				void resetFormatState() override final;

				static constexpr uint16_t _windowSize = 32768;
				static constexpr uint16_t _windowIndexFilter = _windowSize - 1;

				HuffmanEncoder<uint16_t, std::endian::little>* _litlenEncoder;
				HuffmanEncoder<uint8_t, std::endian::little>* _distEncoder;

				bool _writingBlock;

				bool _writingLastBlock;
				bool _currentBlockCompressed;

				uint16_t _currentBlockRemainingSize;

				uint64_t _bytesWritten;
				uint8_t _window[_windowSize];
				uint16_t _windowIndex;
		};
	}
}
