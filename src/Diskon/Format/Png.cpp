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

		namespace png
		{
			FormatError File::getSamples(std::vector<uint8_t>& samples) const
			{
				FormatError error;
				samples.clear();

				// TODO...

				return error;
			}

			FormatError File::setSamples(const std::vector<uint8_t>& samples)
			{
				FormatError error;
				rawData.clear();

				// TODO: Interlacing
				// TODO: Filtering

				zlib::File zlibFile;
				zlibFile.header.compressionMethod = zlib::CompressionMethod::Deflate;
				zlibFile.header.compressionInfo = 7;
				zlibFile.header.compressionLevel = zlib::CompressionLevel::DefaultAlgorithm;
				zlibFile.data.resize(header.imageStructure.height * (header.imageStructure.width * header.imageStructure.bitDepth * 4 /* colorType */ + 1));

				std::stringstream stream(std::ios::binary);

				ZlibStream zlibStream;
				zlibStream.setDestination(stream);
				zlibStream.writeFile(zlibFile);

				stream.flush();

				rawData.resize(stream.tellp());
				stream.read((char*) rawData.data(), rawData.size());

				return error;
			}
		}

		PngStream::PngStream() : FormatStream(std::endian::big)
		{
		}

		const FormatError& PngStream::readFile(png::File& file)
		{
			FMTSTREAM_BEGIN_READ_FUNC("PngStream::readFile(png::File& file)");

			FMTSTREAM_VERIFY_CALL(readHeader, file.header);
			FMTSTREAM_VERIFY_CALL(readRawData, file.rawData);

			return error;
		}

		const FormatError& PngStream::readHeader(png::Header& header)
		{
			FMTSTREAM_BEGIN_READ_FUNC("PngStream::readHeader(png::Header& header)");

			char buffer[8];

			ChunkHeader chunkHeader;
			std::vector<uint32_t> chunksFound;

			// Read PNG signature

			FMTSTREAM_READ(buffer, 8);
			FMTSTREAM_VERIFY(std::equal(buffer, buffer + 8, "\x89PNG\r\n\x1A\n"), PngInvalidSignature, "Invalid PNG signature.");

			// Load IHDR chunk

			FMTSTREAM_VERIFY_CALL(readChunkHeader, &chunkHeader);
			FMTSTREAM_VERIFY(std::equal(chunkHeader.type, chunkHeader.type + 4, "IHDR"), PngInvalidChunkType, "Expected first chunk to be 'IHDR'. Instead, got '" + std::string(chunkHeader.type, 4) + "'.");
			FMTSTREAM_VERIFY_CALL(readChunk_IHDR, header, &chunkHeader);
			FMTSTREAM_VERIFY_CALL(readAndCheckChunkCrc, &chunkHeader);
			chunksFound.push_back(0x49484452);

			// Load all chunks (except IDAT) until IEND

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
						if (chunkType == 0x49444154 && firstIdatChunk == std::streampos(-1))
						{
							firstIdatChunk = getSourcePos() - std::streamoff(8);
						}

						FMTSTREAM_VERIFY_CALL(setSourcePos, getSourcePos() + std::streamoff(chunkHeader.length));
						break;
					}
				}

				FMTSTREAM_VERIFY_CALL(readAndCheckChunkCrc, &chunkHeader);

				chunksFound.push_back(chunkType);

				FMTSTREAM_VERIFY_CALL(readChunkHeader, &chunkHeader);
			}

			// Load IEND chunk

			FMTSTREAM_VERIFY_CALL(readChunk_IEND, header, &chunkHeader);
			FMTSTREAM_VERIFY_CALL(readAndCheckChunkCrc, &chunkHeader);
			chunksFound.push_back(0x49454E44);

			// Check everything is valid

			FMTSTREAM_VERIFY_CALL(checkChunksFound, header, chunksFound);
			// TODO: Check cHRM and gAMA if there is sRGB

			// Go to first IDAT chunk

			FMTSTREAM_VERIFY_CALL(setSourcePos, firstIdatChunk);

			return error;
		}

		const FormatError& PngStream::readRawData(std::vector<uint8_t>& rawData)
		{
			FMTSTREAM_BEGIN_READ_FUNC("PngStream::readRawData(std::vector<uint8_t>& rawData)");

			rawData.clear();
			ChunkHeader chunkHeader;

			FMTSTREAM_VERIFY_CALL(readChunkHeader, &chunkHeader);
			FMTSTREAM_VERIFY(std::equal(chunkHeader.type, chunkHeader.type + 4, "IDAT"), PngInvalidChunkType, "Expected IDAT chunk. Instead, got '" + std::string(chunkHeader.type, 4) + "'.");
			
			do
			{
				FMTSTREAM_VERIFY_CALL(readChunk_IDAT, rawData, &chunkHeader);
				FMTSTREAM_VERIFY_CALL(readAndCheckChunkCrc, &chunkHeader);
				FMTSTREAM_VERIFY_CALL(readChunkHeader, &chunkHeader);

			} while (std::equal(chunkHeader.type, chunkHeader.type + 4, "IDAT"));

			return error;
		}

		const FormatError& PngStream::writeFile(const png::File& file)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("PngStream::writeFile(const png::File& file)");

			// PNG Signature

			FMTSTREAM_WRITE("\x89PNG\r\n\x1A\n", 8);

			// IHDR

			FMTSTREAM_WRITE(uint32_t(13));
			FMTSTREAM_WRITE("IHDR", 4);
			FMTSTREAM_WRITE(file.header.imageStructure.width);
			FMTSTREAM_WRITE(file.header.imageStructure.height);
			FMTSTREAM_WRITE(file.header.imageStructure.bitDepth);
			FMTSTREAM_WRITE(file.header.imageStructure.colorType);
			FMTSTREAM_WRITE(uint8_t(0));
			FMTSTREAM_WRITE(uint8_t(0));
			FMTSTREAM_WRITE(file.header.imageStructure.interlaceMethod);
			FMTSTREAM_WRITE(uint32_t(0));

			// IDAT

			FMTSTREAM_WRITE(file.rawData.size());
			FMTSTREAM_WRITE("IDAT", 4);
			FMTSTREAM_WRITE(file.rawData.data(), file.rawData.size());
			FMTSTREAM_WRITE(uint32_t(0));

			// IEND

			FMTSTREAM_WRITE(uint32_t(0));
			FMTSTREAM_WRITE("IEND", 4);
			FMTSTREAM_WRITE(uint32_t(0));

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
	
		const FormatError& PngStream::checkChunksFound(const png::Header& header, const std::vector<uint32_t>& chunksFound)
		{
			FMTSTREAM_READ_FUNC("PngStream::checkChunksFound(const png::Header& header, const std::vector<uint32_t>& chunksFound)");

			// First chunk and last chunk

			FMTSTREAM_VERIFY(chunksFound.size() >= 2, PngInvalidChunkOrdering, "PNG Files cannot have less than 2 chunks.");
			FMTSTREAM_VERIFY(chunksFound.front() == 0x49484452, PngInvalidChunkOrdering, "Expected first chunk to be IHDR. Instead, got '" + std::string(reinterpret_cast<const char*>(&chunksFound.front()), 4) + "'.");
			FMTSTREAM_VERIFY(chunksFound.back() == 0x49454E44, PngInvalidChunkOrdering, "Expected last chunk to be IEND. Instead, got '" + std::string(reinterpret_cast<const char*>(&chunksFound.front()), 4) + "'.");

			// Chunk presence and count

			std::unordered_map<uint32_t, uint32_t> chunkCount = {
				{0x49484452, 0},	// IHDR
				{0x504C5445, 0},	// PLTE
				{0x49444154, 0},	// IDAT
				{0x49454E44, 0},	// IEND
				{0x74524E53, 0},	// tRNS
				{0x6348524D, 0},	// cHRM
				{0x67414D41, 0},	// gAMA
				{0x69434350, 0},	// iCCP
				{0x73424954, 0},	// sBIT
				{0x73524742, 0},	// sRGB
				{0x74455854, 0},	// tEXT
				{0x7A545874, 0},	// zTXt
				{0x69545874, 0},	// iTXt
				{0x624B4744, 0},	// bKGD
				{0x68495354, 0},	// hIST
				{0x70485973, 0},	// pHYs
				{0x73504C54, 0},	// sPLT
				{0x74494D45, 0},	// tIME
			};

			for (const uint32_t& chunkType : chunksFound)
			{
				if (chunkCount.find(chunkType) != chunkCount.end())
				{
					++chunkCount[chunkType];
				}
			}

			FMTSTREAM_VERIFY(chunkCount[0x49484452] == 1, PngInvalidChunkOrdering, "There should be exactly 1 chunk IHDR. Found " + std::to_string(chunkCount[0x49484452]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x504C5445] <= 1, PngInvalidChunkOrdering, "There should be at most 1 chunk PLTE. Found " + std::to_string(chunkCount[0x504C5445]) + ".");
			if (header.imageStructure.colorType == png::ColorType::IndexedColor)
			{
				FMTSTREAM_VERIFY(chunkCount[0x504C5445] == 1, PngInvalidChunkOrdering, "If color type is 3, a 'PLTE' chunk is expected. Found none.");
			}
			FMTSTREAM_VERIFY(chunkCount[0x49444154] >= 1, PngInvalidChunkOrdering, "There should be at least 1 chunk IDAT. Found " + std::to_string(chunkCount[0x49444154]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x49454E44] == 1, PngInvalidChunkOrdering, "There should be exactly 1 chunk IEND. Found " + std::to_string(chunkCount[0x49454E44]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x74524E53] <= 1, PngInvalidChunkOrdering, "There should be at most 1 chunk tRNS. Found " + std::to_string(chunkCount[0x74524E53]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x6348524D] <= 1, PngInvalidChunkOrdering, "There should be at most 1 chunk cHRM. Found " + std::to_string(chunkCount[0x6348524D]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x67414D41] <= 1, PngInvalidChunkOrdering, "There should be at most 1 chunk gAMA. Found " + std::to_string(chunkCount[0x67414D41]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x69434350] + chunkCount[0x73524742] <= 1, PngInvalidChunkOrdering, "There should be at most 1 chunk iCCP or sRGB. Found " + std::to_string(chunkCount[0x69434350] + chunkCount[0x73524742]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x73424954] <= 1, PngInvalidChunkOrdering, "There should be at most 1 chunk sBIT. Found " + std::to_string(chunkCount[0x73424954]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x624B4744] <= 1, PngInvalidChunkOrdering, "There should be at most 1 chunk bKGD. Found " + std::to_string(chunkCount[0x624B4744]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x68495354] <= 1, PngInvalidChunkOrdering, "There should be at most 1 chunk hIST. Found " + std::to_string(chunkCount[0x68495354]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x70485973] <= 1, PngInvalidChunkOrdering, "There should be at most 1 chunk pHYs. Found " + std::to_string(chunkCount[0x70485973]) + ".");
			FMTSTREAM_VERIFY(chunkCount[0x74494D45] <= 1, PngInvalidChunkOrdering, "There should be at most 1 chunk tIME. Found " + std::to_string(chunkCount[0x74494D45]) + ".");

			// Chunk ordering

			bool has_PLTE = (chunkCount[0x504C5445] != 0);
			bool found_PLTE = false;
			bool found_IDAT = false;
			bool in_IDAT = false;


			uint64_t n = chunksFound.size() - 1;
			for (uint64_t i = 1; i < n; ++i)
			{
				bool chunkValid = false;

				if (in_IDAT)
				{
					if (chunksFound[i] == 0x49444154)
					{
						continue;
					}
					else
					{
						in_IDAT = false;
					}
				}

				chunkValid = chunkValid || (chunksFound[i] == 0x74494D45);	// tIME
				chunkValid = chunkValid || (chunksFound[i] == 0x7A545874);	// zTXT
				chunkValid = chunkValid || (chunksFound[i] == 0x74455854);	// tEXt
				chunkValid = chunkValid || (chunksFound[i] == 0x69545874);	// iTXt

				if (!found_PLTE && !found_IDAT)
				{
					chunkValid = chunkValid || (chunksFound[i] == 0x69434350);	// iCCP
					chunkValid = chunkValid || (chunksFound[i] == 0x73524742);	// sRGB
					chunkValid = chunkValid || (chunksFound[i] == 0x73424954);	// sBIT
					chunkValid = chunkValid || (chunksFound[i] == 0x67414D41);	// gAMA
					chunkValid = chunkValid || (chunksFound[i] == 0x6348524D);	// cHRM

					if (chunksFound[i] == 0x504C5445)
					{
						found_PLTE = true;
						continue;
					}
				}

				if (!found_IDAT)
				{
					chunkValid = chunkValid || (chunksFound[i] == 0x70485973);	// pHYs
					chunkValid = chunkValid || (chunksFound[i] == 0x73504C54);	// sPLT
				}

				if ((!has_PLTE || found_PLTE) && !found_IDAT)
				{
					chunkValid = chunkValid || (chunksFound[i] == 0x74524E53);	// tRNS
					chunkValid = chunkValid || (chunksFound[i] == 0x68495354);	// hIST
					chunkValid = chunkValid || (chunksFound[i] == 0x624B4744);	// bKGD

					if (chunksFound[i] == 0x49444154)
					{
						found_IDAT = true;
						in_IDAT = true;
						continue;
					}
				}

				FMTSTREAM_VERIFY(chunkValid, PngInvalidChunkOrdering, "Chunk '" + std::string(reinterpret_cast<const char*>(&chunksFound[i]), 4) + "' unrecognized or at bad position.");
			}

			// Chunk presence (PLTE mandatory if color type, etc...)

			// Chunk values (cHRM and gAMA if sRGB, etc...)

			return error;
		}
	}
}
