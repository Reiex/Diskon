#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace
		{
			uint32_t adler32(const uint8_t* buf, uint64_t len)
			{
				uint32_t s1 = 1;
				uint32_t s2 = 0;

				for (uint64_t n = 0; n < len; ++n) {
					s1 = (s1 + buf[n]) % 65521;
					s2 = (s2 + s1) % 65521;
				}
				return (s2 << 16) + s1;
			}
		}

		ZlibStream::ZlibStream() : FormatStream(std::endian::big)
		{
		}

		const FormatError& ZlibStream::readFile(zlib::File& file)
		{
			FMTSTREAM_BEGIN_READ_FUNC("ZlibStream::readFile(zlib::File& file)");

			FMTSTREAM_VERIFY_CALL(readHeader, file.header);

			switch (file.header.compressionMethod)
			{
				case zlib::CompressionMethod::Deflate:
				{
					_endianness = std::endian::little;
					_byte = 0;
					_bitCount = 0;

					zlib::DeflateBlock block;
					FMTSTREAM_VERIFY_CALL(readDeflateBlock, block);
					file.data = block.data;

					while (!block.header.isFinal)
					{
						FMTSTREAM_VERIFY_CALL(readDeflateBlock, block);
						
						file.data.resize(file.data.size() + block.data.size());
						std::copy(block.data.begin(), block.data.end(), file.data.end() - block.data.size());
					}

					_endianness = std::endian::big;

					break;
				}
			}

			uint32_t adlerStored;
			FMTSTREAM_READ(adlerStored);

			uint32_t adlerComputed = adler32(file.data.data(), file.data.size());

			FMTSTREAM_VERIFY(adlerComputed == adlerStored, ZlibInvalidCheck, "Failed ZLIB Adler-32 check on the uncompressed data.");

			return error;
		}

		const FormatError& ZlibStream::readHeader(zlib::Header& header)
		{
			FMTSTREAM_BEGIN_READ_FUNC("ZlibStream::readHeader(zlib::Header& header)");

			uint16_t cmfFlg;
			FMTSTREAM_READ(cmfFlg);

			header.compressionMethod = static_cast<zlib::CompressionMethod>((cmfFlg & 0x0f00) >> 8);
			header.compressionInfo = cmfFlg >> 12;
			header.compressionLevel = static_cast<zlib::CompressionLevel>((cmfFlg & 0x00C0) >> 6);
			bool hasDict = (cmfFlg & 0x0020);

			FMTSTREAM_VERIFY(cmfFlg % 31 == 0, ZlibInvalidCheck, "");

			switch (header.compressionMethod)
			{
				case zlib::CompressionMethod::Deflate:
				{
					FMTSTREAM_VERIFY(header.compressionInfo < 8, ZlibInvalidCompressionInfo, "Expected ZLIB compression info with DEFLATE compression method to be less than 8. Instead, got " + std::to_string(header.compressionInfo) + ".");
					break;
				}
				default:
				{
					FMTSTREAM_VERIFY(false, ZlibInvalidCompressionMethod, "ZLIB compression method invalid or not recognized. Compression method: " + std::to_string(static_cast<uint8_t>(header.compressionMethod)));
				}
			}

			if (hasDict)
			{
				uint32_t dictId;
				FMTSTREAM_READ(dictId);
				header.dictId = dictId;
			}

			return error;
		}

		const FormatError& ZlibStream::writeFile(const zlib::File& file)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("ZlibStream::writeFile(const zlib::File& file)");

			FMTSTREAM_VERIFY_CALL(writeHeader, file.header);

			// TODO: Compression

			_byte = 0;
			_bitCount = 0;

			constexpr uint64_t blockMaxSize = 65535;

			zlib::DeflateBlock block;
			block.header.isCompressed = false;
			block.header.isFinal = false;
			block.data.resize(blockMaxSize);

			auto it = file.data.begin();
			while (std::distance(it, file.data.end()) > blockMaxSize)
			{
				std::copy(it, it + blockMaxSize, block.data.begin());
				FMTSTREAM_VERIFY_CALL(writeDeflateBlock, block);
			}

			block.header.isFinal = true;
			block.data.resize(std::distance(it, file.data.end()));
			std::copy(it, file.data.end(), block.data.begin());
			FMTSTREAM_VERIFY_CALL(writeDeflateBlock, block);

			FMTSTREAM_WRITE(adler32(file.data.data(), file.data.size()));

			return error;
		}

		const FormatError& ZlibStream::writeHeader(const zlib::Header& header)
		{
			assert(header.compressionMethod == zlib::CompressionMethod::Deflate);
			assert(header.compressionInfo < 16);

			FMTSTREAM_BEGIN_WRITE_FUNC("ZlibStream::writeHeader(const zlib::Header& header)");

			uint16_t cmfFlg = (static_cast<uint16_t>(header.compressionInfo) << 12);
			cmfFlg |= (static_cast<uint16_t>(header.compressionMethod) & 0x000f) << 8;
			cmfFlg |= (static_cast<uint16_t>(header.compressionLevel) & 0x0003) << 6;

			if (header.dictId.has_value())
			{
				cmfFlg |= 0x0020;
			}

			cmfFlg += 31 - (cmfFlg % 31);

			FMTSTREAM_WRITE(cmfFlg);
			if (header.dictId.has_value())
			{
				FMTSTREAM_WRITE(header.dictId.value());
			}

			return error;
		}

		const FormatError& ZlibStream::writeDeflateBlock(const zlib::DeflateBlock& block)
		{
			assert(block.data.size() <= UINT16_MAX);

			FMTSTREAM_BEGIN_WRITE_FUNC("ZlibStream::writeDeflateBlock(const zlib::DeflateBlock& block)");

			uint16_t buffer = (_byte << 8);

			buffer >>= 1;

			if (block.header.isFinal)
			{
				buffer |= 0x8000;
			}

			buffer >>= 2;

			uint8_t compression = 0b00;
			if (block.header.isCompressed && block.header.huffmanTree.has_value())
			{
				buffer |= 0x8000;
				compression = 0b10;
			}
			else if (block.header.isCompressed)
			{
				buffer |= 0x4000;
				compression = 0b01;
			}

			_bitCount += 3;

			if (_bitCount >= 8)
			{
				buffer >>= (16 - _bitCount);
				FMTSTREAM_WRITE(static_cast<uint8_t>(buffer & 0x00ff));
				buffer = ((buffer & 0xff00) << (16 - _bitCount));
				_bitCount -= 8;
			}

			switch (compression)
			{
				case 0b00:	// Uncompressed
				{
					buffer >>= (8 - _bitCount);
					FMTSTREAM_WRITE(static_cast<uint8_t>(buffer & 0x00ff));

					_byte = 0;
					_bitCount = 0;

					uint16_t blockSize = block.data.size();
					FMTSTREAM_WRITE(blockSize);
					FMTSTREAM_WRITE(~blockSize);
					FMTSTREAM_WRITE(block.data.data(), blockSize);

					break;
				}
				case 0b01:	// Fixed huffman codes
				{
					// TODO
					break;
				}
				case 0b10:	// Dynamic huffman codes
				{
					// TODO
					break;
				}
			}

			return error;
		}

		const FormatError& ZlibStream::readDeflateBlock(zlib::DeflateBlock& block)
		{
			FMTSTREAM_BEGIN_READ_FUNC("ZlibStream::readDeflateBlock(zlib::DeflateBlock& block)");

			uint16_t buffer = _byte;
			uint8_t byte;

			if (_bitCount < 3)
			{
				FMTSTREAM_READ(byte);
				buffer |= static_cast<uint16_t>(byte) << _bitCount;
				_bitCount += 8;
			}

			block.header.isFinal = (buffer & 0x0001);
			buffer >>= 1;

			uint8_t compression = (buffer & 0x0003);
			buffer >>= 2;
			_bitCount -= 3;

			switch (compression)
			{
				case 0b00:	// Uncompressed
				{
					_byte = 0;
					_bitCount = 0;

					uint16_t blockSize;
					FMTSTREAM_READ(blockSize);
					FMTSTREAM_READ(buffer);
					FMTSTREAM_VERIFY(blockSize == ~buffer, ZlibInvalidCheck, "Failed length check in non-compressed deflate block.");

					block.data.resize(blockSize);
					FMTSTREAM_READ(block.data.data(), blockSize);

					break;
				}
				case 0b01:	// Fixed huffman codes
				{
					// TODO
					_byte = static_cast<uint8_t>(buffer & 0x00ff);
					break;
				}
				case 0b10:	// Dynamic huffman codes
				{
					// TODO
					_byte = static_cast<uint8_t>(buffer & 0x00ff);
					break;
				}
				default:	// Reserved
				{
					FMTSTREAM_VERIFY(false, ZlibDeflateInvalidCompression, "Expected compression flag to be in [0, 3]. Instead, got " + std::to_string(compression) + ".");
				}
			}

			return error;
		}
	}
}
