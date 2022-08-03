#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace zlib
		{
			struct HuffmanTree
			{
				std::array<uint8_t, 285> codeLengths;
			};

			struct DeflateBlockHeader
			{
				bool isFinal;
				bool isCompressed;
				std::optional<HuffmanTree> huffmanTree;
			};

			struct DeflateBlock
			{
				DeflateBlockHeader header;
				std::vector<uint8_t> data;
			};

			enum class CompressionMethod : uint8_t
			{
				Deflate = 0x8
			};

			enum class CompressionLevel : uint8_t
			{
				FastestAlgorithm	= 0b00,
				FastAlgorithm		= 0b01,
				DefaultAlgorithm	= 0b10,
				MaximumCompression	= 0b11
			};

			struct Header
			{
				CompressionMethod compressionMethod;
				uint8_t compressionInfo;
				CompressionLevel compressionLevel;
				std::optional<uint32_t> dictId;
			};

			struct File
			{
				Header header;
				std::vector<uint8_t> data;
			};
		}

		class ZlibStream : public FormatStream
		{
			public:

				ZlibStream();
				ZlibStream(const ZlibStream& stream) = default;
				ZlibStream(ZlibStream&& stream) = default;

				ZlibStream& operator=(const ZlibStream& stream) = default;
				ZlibStream& operator=(ZlibStream&& stream) = default;

				const FormatError& readFile(zlib::File& file);
				const FormatError& readHeader(zlib::Header& header);

				const FormatError& writeFile(const zlib::File& file);
				const FormatError& writeHeader(const zlib::Header& header);
				const FormatError& writeDeflateBlock(const zlib::DeflateBlock& block);

				~ZlibStream() = default;

			private:

				const FormatError& readDeflateBlock(zlib::DeflateBlock& block);

				uint8_t _byte;
				uint8_t _bitCount;
		};
	}
}
