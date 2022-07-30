#include <Diskon/Format/Format.hpp>


namespace dsk
{
	namespace fmt
	{
		namespace
		{
			struct ChunkHeader
			{
				uint32_t length;
				char type[4];
			};
		}

		PngStream::PngStream() : FormatStream(std::endian::big)
		{
		}

		const FormatError& PngStream::readFile(png::File& file)
		{
			FMTSTREAM_BEGIN_READ_FUNC("PngStream::readFile(png::File& file)");

			FMTSTREAM_VERIFY_CALL(readHeader, file.header);

			return error;
		}

		const FormatError& PngStream::readHeader(png::Header& header)
		{
			FMTSTREAM_BEGIN_READ_FUNC("PngStream::readHeader(png::Header& header)");

			char buffer[8];

			ChunkHeader chunkHeader;
			void* chunk = nullptr;

			// Read PNG signature

			FMTSTREAM_READ(buffer, 8);
			FMTSTREAM_VERIFY(std::equal(buffer, buffer + 8, "\x89PNG\r\n\x1A\n"), PngInvalidSignature, "Invalid PNG signature.");

			// Load IHDR chunk

			FMTSTREAM_VERIFY_CALL(readChunkHeader, &chunkHeader);
			FMTSTREAM_VERIFY(std::equal(chunkHeader.type, chunkHeader.type + 4, "IHDR"), PngInvalidChunkType, "Expected first chunk to be 'IHDR'. Instead, got '" + std::string(chunkHeader.type, 4) + "'.");
			FMTSTREAM_VERIFY_CALL(readChunk_IHDR, header, &chunkHeader);
			FMTSTREAM_VERIFY_CALL(readAndCheckChunkCrc, &chunkHeader);

			// Load all chunks (except IDAT) until IEND

			std::vector<uint32_t> chunksFound = { 0x49484452 };	// IHDR
			std::streampos firstIdatChunk = -1;

			FMTSTREAM_VERIFY_CALL(readChunkHeader, &chunkHeader);
			while (!std::equal(chunkHeader.type, chunkHeader.type + 4, "IEND"))
			{
				uint32_t chunkType = *reinterpret_cast<const uint32_t*>(chunkHeader.type);
				if (std::endian::native == std::endian::little)
				{
					chunkType = std::byteswap(chunkType);
				}

				switch (chunkType)
				{
					case 0x504C5445:	// PLTE
					{
						FMTSTREAM_VERIFY_CALL(readChunk_PLTE, header, &chunkHeader);
						break;
					}
					case 0x74524E53:	// tRNS
					{
						FMTSTREAM_VERIFY_CALL(readChunk_tRNS, header, &chunkHeader);
						break;
					}
					case 0x6348524D:	// cHRM
					{
						FMTSTREAM_VERIFY_CALL(readChunk_cHRM, header, &chunkHeader);
						break;
					}
					case 0x67414D41:	// gAMA
					{
						FMTSTREAM_VERIFY_CALL(readChunk_gAMA, header, &chunkHeader);
						break;
					}
					case 0x73524742:	// sRGB
					{
						FMTSTREAM_VERIFY_CALL(readChunk_sRGB, header, &chunkHeader);
						break;
					}
					default:	// Unrecognized or ignored chunks
					{
						FMTSTREAM_VERIFY_CALL(setSourcePos, getSourcePos() + std::streamoff(chunkHeader.length));
						break;
					}
				}

				FMTSTREAM_VERIFY_CALL(readAndCheckChunkCrc, &chunkHeader);

				chunksFound.push_back(chunkType);

				FMTSTREAM_VERIFY_CALL(readChunkHeader, &chunkHeader);
			}

			chunksFound.push_back(0x49454E44);	// IEND

			// TODO: Check there is a PLTE if color type == 3
			// TODO: Check IDAT chunks appear consecutively
			// TODO: Check cHRM and gAMA if there is sRGB

			// Load IEND

			// Go to first IDAT chunk

			return error;
		}

		const FormatError& PngStream::writeFile(const png::File& file)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("PngStream::writeFile(const png::File& file)");

			return error;
		}

		const FormatError& PngStream::writeHeader(const png::Header& header)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("PngStream::writeHeader(const png::Header& header)");

			return error;
		}

		const FormatError& PngStream::writeImageStructure(const png::ImageStructure& imageStructure)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("PngStream::writeImageStructure(const png::ImageStructure& imageStructure)");

			return error;
		}

		const FormatError& PngStream::writeColorSpace(const png::ColorSpace& colorSpace)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("PngStream::writeColorSpace(const png::ColorSpace& colorSpace)");

			return error;
		}

		const FormatError& PngStream::writeGamma(float gamma)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("PngStream::writeGamma(float gamma)");

			return error;
		}

		const FormatError& PngStream::writeSRBGIntent(const png::SRGBIntent& sRGBIntent)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("PngStream::writeSRBGIntent(const png::SRGBIntent& sRGBIntent)");

			return error;
		}

		const FormatError& PngStream::readChunkHeader(void* header)
		{
			FMTSTREAM_READ_FUNC("PngStream::readChunkHeader(void* header)");

			ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(header);

			FMTSTREAM_READ(chunkHeader->length);
			FMTSTREAM_READ(chunkHeader->type, 4);

			FMTSTREAM_VERIFY(chunkHeader->length < (1ULL << 31) - 1, PngInvalidChunkSize, "Expected chunk size to be less than 2**31 - 1. Instead, got " + std::to_string(chunkHeader->length) + ".");

			return error;
		}

		namespace
		{
			bool isBitDepthColorTypeAllowed(uint8_t bitDepth, uint8_t colorType)
			{
				switch (colorType)
				{
					case 0:
						return (bitDepth == 1) || (bitDepth == 2) || (bitDepth == 4) || (bitDepth == 8) || (bitDepth == 16);
					case 3:
						return (bitDepth == 1) || (bitDepth == 2) || (bitDepth == 4) || (bitDepth == 8);
					case 2:
					case 4:
					case 6:
						return (bitDepth == 8) || (bitDepth == 16);
					default:
						return false;
				}
			}
		}

		const FormatError& PngStream::readChunk_IHDR(png::Header& header, void* ptr)
		{
			FMTSTREAM_READ_FUNC("PngStream::readChunk_IHDR(png::Header& header, void* ptr)");

			// Check chunk size

			ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(ptr);
			FMTSTREAM_VERIFY(chunkHeader->length == 13, PngInvalidChunkSize, "Expected IHDR chunk size to be 13. Instead, got " + std::to_string(chunkHeader->length) + ".");

			// Load chunk

			png::ImageStructure& imgStruct = header.imageStructure;
			uint8_t buffer;

			FMTSTREAM_READ(imgStruct.width);
			FMTSTREAM_VERIFY(imgStruct.width != 0, PngInvalidChunkContent, "Image width cannot be 0.");

			FMTSTREAM_READ(imgStruct.height);
			FMTSTREAM_VERIFY(imgStruct.height != 0, PngInvalidChunkContent, "Image height cannot be 0.");

			FMTSTREAM_READ(imgStruct.bitDepth);
			FMTSTREAM_READ(buffer);
			FMTSTREAM_VERIFY(isBitDepthColorTypeAllowed(imgStruct.bitDepth, buffer), PngInvalidChunkContent, "Invalid combination of bit depth and color type: (" + std::to_string(imgStruct.bitDepth) + ", " + std::to_string(static_cast<uint8_t>(imgStruct.colorType)) + ").");
			imgStruct.colorType = static_cast<png::ColorType>(buffer);

			FMTSTREAM_READ(buffer);
			FMTSTREAM_VERIFY(buffer == 0, PngInvalidChunkContent, "Expected compression method to be 0. Instead, got " + std::to_string(buffer) + ".");

			FMTSTREAM_READ(buffer);
			FMTSTREAM_VERIFY(buffer == 0, PngInvalidChunkContent, "Expected filter method to be 0. Instead, got " + std::to_string(buffer) + ".");

			FMTSTREAM_READ(buffer);
			FMTSTREAM_VERIFY(buffer < 2, PngInvalidChunkContent, "Expected interlace method to be less than 2. Instead, got " + std::to_string(buffer) + ".");
			imgStruct.interlaceMethod = static_cast<png::InterlaceMethod>(buffer);

			return error;
		}

		const FormatError& PngStream::readChunk_PLTE(png::Header& header, void* ptr)
		{
			FMTSTREAM_READ_FUNC("PngStream::readChunk_PLTE(png::Header& header, void* ptr)");

			// Check chunk size and color type

			ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(ptr);
			FMTSTREAM_VERIFY(chunkHeader->length % 3 == 0, PngInvalidChunkSize, "Expected PLTE chunk to have a size multiple of 3. Instead, got " + std::to_string(chunkHeader->length) + ".");

			uint8_t colorType = static_cast<uint8_t>(header.imageStructure.colorType);
			FMTSTREAM_VERIFY(colorType != 0 && colorType != 4, PngInvalidChunkType, "PLTE chunk cannot appear for images of color type " + std::to_string(colorType) + ".");

			// Load chunk

			header.palette.resize(chunkHeader->length / 3);
			for (std::array<uint8_t, 3>& color : header.palette)
			{
				FMTSTREAM_READ(color.data(), 3);
			}

			return error;
		}

		const FormatError& PngStream::readChunk_IDAT(std::vector<uint8_t>& data, void* ptr)
		{
			FMTSTREAM_READ_FUNC("PngStream::readChunk_IDAT(std::vector<uint8_t>& data, void* ptr)");

			ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(ptr);
			const uint64_t oldSize = data.size();
			data.resize(oldSize + chunkHeader->length);
			FMTSTREAM_READ(data.data() + oldSize, chunkHeader->length);

			return error;
		}

		const FormatError& PngStream::readChunk_IEND(png::Header& header, void* ptr)
		{
			FMTSTREAM_READ_FUNC("PngStream::readChunk_IEND(png::Header& header, void* ptr)");

			ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(ptr);
			FMTSTREAM_VERIFY(chunkHeader->length == 0, PngInvalidChunkSize, "Expected IEND chunk size to be 0. Instead, got " + std::to_string(chunkHeader->length) + ".");

			return error;
		}

		const FormatError& PngStream::readChunk_tRNS(png::Header& header, void* ptr)
		{
			FMTSTREAM_READ_FUNC("PngStream::readChunk_tRNS(png::Header& header, void* ptr)");

			ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(ptr);
			const uint32_t maxMask = uint32_t(1) << header.imageStructure.bitDepth;
			header.transparency = png::TransparencyData();

			switch (header.imageStructure.colorType)
			{
				case png::ColorType::GreyScale:
				{
					FMTSTREAM_VERIFY(chunkHeader->length == 2, PngInvalidChunkSize, "Expected tRNS chunk with color type 0 to have a size of 2. Instead, got " + std::to_string(chunkHeader->length) + ".");
					
					FMTSTREAM_READ(header.transparency->greyMask);
					FMTSTREAM_VERIFY(header.transparency->greyMask < maxMask, PngInvalidChunkContent, "Expected grey mask to be less that 2**bitDepth (" + std::to_string(maxMask) + "). Instead, got " + std::to_string(header.transparency->greyMask) + ".");
					
					break;
				}
				case png::ColorType::TrueColor:
				{
					FMTSTREAM_VERIFY(chunkHeader->length == 6, PngInvalidChunkSize, "Expected tRNS chunk with color type 2 to have a size of 6. Instead, got " + std::to_string(chunkHeader->length) + ".");
					
					FMTSTREAM_READ(header.transparency->colorMask.data(), 3);
					FMTSTREAM_VERIFY(header.transparency->colorMask[0] < maxMask, PngInvalidChunkContent, "Expected color mask red channel to be less that 2**bitDepth (" + std::to_string(maxMask) + "). Instead, got " + std::to_string(header.transparency->colorMask[0]) + ".");
					FMTSTREAM_VERIFY(header.transparency->colorMask[1] < maxMask, PngInvalidChunkContent, "Expected color mask green channel to be less that 2**bitDepth (" + std::to_string(maxMask) + "). Instead, got " + std::to_string(header.transparency->colorMask[1]) + ".");
					FMTSTREAM_VERIFY(header.transparency->colorMask[2] < maxMask, PngInvalidChunkContent, "Expected color mask blue channel to be less that 2**bitDepth (" + std::to_string(maxMask) + "). Instead, got " + std::to_string(header.transparency->colorMask[2]) + ".");
					
					break;
				}
				case png::ColorType::IndexedColor:
				{
					FMTSTREAM_VERIFY(chunkHeader->length == header.palette.size(), PngInvalidChunkSize, "Expected tRNS chunk with color type 3 to have the same size as the palette (" + std::to_string(header.palette.size()) + "). Instead, got " + std::to_string(chunkHeader->length) + ".");
					
					header.transparency->palette.resize(header.palette.size());
					FMTSTREAM_READ(header.transparency->palette.data(), header.palette.size());
					
					break;
				}
				default:
				{
					FMTSTREAM_VERIFY(false, PngInvalidChunkType, "tRNS chunk cannot appear for images of color type " + std::to_string(static_cast<uint8_t>(header.imageStructure.colorType)) + ".");
				}
			}

			return error;
		}

		const FormatError& PngStream::readChunk_cHRM(png::Header& header, void* ptr)
		{
			FMTSTREAM_READ_FUNC("PngStream::readChunk_cHRM(png::Header& header, void* ptr)");

			// Check chunk size

			ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(ptr);
			FMTSTREAM_VERIFY(chunkHeader->length == 32, PngInvalidChunkSize, "Expected cHRM chunk size to be 32. Instead, got " + std::to_string(chunkHeader->length) + ".");

			// Load chunk

			uint32_t points[8];
			FMTSTREAM_READ(points, 8);

			header.colorSpace = png::ColorSpace();
			header.colorSpace->whitePoint[0] = points[0] / 100000.f;
			header.colorSpace->whitePoint[1] = points[1] / 100000.f;
			header.colorSpace->redPoint[0] = points[2] / 100000.f;
			header.colorSpace->redPoint[1] = points[3] / 100000.f;
			header.colorSpace->greenPoint[0] = points[4] / 100000.f;
			header.colorSpace->greenPoint[1] = points[5] / 100000.f;
			header.colorSpace->bluePoint[0] = points[6] / 100000.f;
			header.colorSpace->bluePoint[1] = points[7] / 100000.f;

			return error;
		}

		const FormatError& PngStream::readChunk_gAMA(png::Header& header, void* ptr)
		{
			FMTSTREAM_READ_FUNC("PngStream::readChunk_gAMA(png::Header& header, void* ptr)");

			// Check chunk size

			ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(ptr);
			FMTSTREAM_VERIFY(chunkHeader->length == 4, PngInvalidChunkSize, "Expected gAMA chunk size to be 4. Instead, got " + std::to_string(chunkHeader->length) + ".");

			// Load chunk

			uint32_t gamma;
			FMTSTREAM_READ(gamma);

			header.gamma = gamma / 100000.f;

			return error;
		}

		const FormatError& PngStream::readChunk_sRGB(png::Header& header, void* ptr)
		{
			FMTSTREAM_READ_FUNC("PngStream::readChunk_sRGB(png::Header& header, void* ptr)");

			// Check chunk size

			ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(ptr);
			FMTSTREAM_VERIFY(chunkHeader->length == 1, PngInvalidChunkSize, "Expected sRGB chunk size to be 1. Instead, got " + std::to_string(chunkHeader->length) + ".");

			// Load chunk

			uint8_t sRGBIntent;
			FMTSTREAM_READ(sRGBIntent);
			FMTSTREAM_VERIFY(sRGBIntent < 4, PngInvalidChunkContent, "Expected sRGB intent to be less than 4. Instead, got " + std::to_string(sRGBIntent) + ".");

			header.sRGBIntent = static_cast<png::SRGBIntent>(sRGBIntent);

			return error;
		}

		namespace
		{
			class CrcTable
			{
				public:

					constexpr CrcTable()
					{
						for (uint32_t n = 0; n < 256; ++n)
						{
							values[n] = n;
							for (uint8_t k = 0; k < 8; ++k)
							{
								if (values[n] & 1)
								{
									values[n] = 0xEDB88320 ^ (values[n] >> 1);
								}
								else
								{
									values[n] >>= 1;
								}
							}
						}
					}

					uint32_t computeCrc(const uint8_t* buffer, uint64_t size) const
					{
						uint32_t crc = 0xFFFFFFFF;

						const uint8_t* const bufferEnd = buffer + size;
						for (; buffer != bufferEnd; ++buffer)
						{
							crc = values[(crc ^ *buffer) & 0xFF] ^ (crc >> 8);
						}

						return ~crc;
					}

				private:

					uint32_t values[256];
			};

			constexpr CrcTable crcTable;
		}

		const FormatError& PngStream::readAndCheckChunkCrc(void* ptr)
		{
			FMTSTREAM_READ_FUNC("PngStream::readAndCheckChunkCrc(void* ptr)");

			ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(ptr);
			const uint32_t crcLength = chunkHeader->length + 4;
			uint8_t* buffer = new uint8_t[crcLength];
			FMTSTREAM_VERIFY_CALL(setSourcePos, getSourcePos() - std::streamoff(crcLength));
			FMTSTREAM_READ(buffer, crcLength);

			uint32_t computedCrc = crcTable.computeCrc(buffer, crcLength);
			delete[] buffer;

			uint32_t storedCrc;
			FMTSTREAM_READ(storedCrc);
			FMTSTREAM_VERIFY(computedCrc == storedCrc, PngInvalidChunkCRC, "Bad CRC for chunk '" + std::string(chunkHeader->type, 4) + "'.");

			return error;
		}
	
		const FormatError& PngStream::checkChunksFound(const std::vector<uint32_t>& chunksFound)
		{
			FMTSTREAM_READ_FUNC("PngStream::checkChunksFound(const std::vector<uint32_t>& chunksFound)");

			return error;
		}
	}
}
