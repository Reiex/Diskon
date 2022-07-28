#include <Diskon/Format/Format.hpp>

/*
namespace dsk
{
	namespace fmt
	{
		namespace
		{
			#pragma pack(push, 1)

			struct ChunkHeader
			{
				uint32_t length;
				char type[4];
			};

			struct ChunkFooter
			{
				uint32_t crc;
			};

			// Critical chunks

			struct Chunk_IHDR
			{
				ChunkHeader header;

				uint32_t width;
				uint32_t height;
				uint8_t bitDepth;
				uint8_t colorType;
				uint8_t compressionMethod;
				uint8_t filterMethod;
				uint8_t interlaceMethod;

				ChunkFooter footer;
			};

			struct Chunk_PLTE
			{
				ChunkHeader header;

				std::vector<std::array<uint8_t, 3>> palette;

				ChunkFooter footer;
			};

			struct Chunk_IDAT
			{
				ChunkHeader header;

				std::vector<uint8_t> data;

				ChunkFooter footer;
			};

			struct Chunk_IEND
			{
				ChunkHeader header;
				ChunkFooter footer;
			};

			// Ancillary chunks

			struct Chunk_tRNS
			{
				ChunkHeader header;

				union
				{
					uint16_t grayMask;
					struct { uint16_t r, g, b; } rgbMask;
					uint8_t paletteAlpha[256];
				};

				ChunkFooter footer;
			};

			struct Chunk_cHRM
			{
				ChunkHeader header;

				uint32_t xWhitePoint;
				uint32_t yWhitePoint;
				uint32_t xRedPoint;
				uint32_t yRedPoint;
				uint32_t xGreenPoint;
				uint32_t yGreenPoint;
				uint32_t xBluePoint;
				uint32_t yBluePoint;

				ChunkFooter footer;
			};

			struct Chunk_gAMA
			{
				ChunkHeader header;

				uint32_t gamma;

				ChunkFooter footer;
			};

			// TODO: Chunk_iCCP

			// Chunk sBIT is ignored.

			struct Chunk_sRGB
			{
				ChunkHeader header;

				uint8_t renderingIntent;

				ChunkFooter footer;
			};

			#pragma pack(pop)

			// Functions

			const FormatError& readChunkHeader(std::istream& stream, FormatError& error, ChunkHeader& chunkHeader)
			{
				FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(&chunkHeader), sizeof(ChunkHeader)), InvalidStream, "PngStream: Error while reading chunk header.");

				if constexpr (std::endian::native == std::endian::little)
				{
					chunkHeader.length = std::byteswap(chunkHeader.length);
				}

				FMTSTREAM_VERIFY(chunkHeader.length < (1ULL << 31) - 1, PngInvalidChunkSize, "PngStream: Expected chunk size to be less than 2**31 - 1. Instead, got " + std::to_string(chunkHeader.length) + ".");

				return error;
			}

			bool isColorTypeBitDepthAllowed(uint8_t colorType, uint8_t bitDepth)
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

			const FormatError& readChunkData(std::istream& stream, FormatError& error, const ChunkHeader& chunkHeader, void*& chunk)
			{
				if (chunk)
				{
					delete chunk;
					chunk = nullptr;
				}

				constexpr uint32_t offset = sizeof(ChunkHeader);

				const uint8_t* crcStart;

				uint32_t chunkType = *reinterpret_cast<const uint32_t*>(chunkHeader.type);
				if constexpr (std::endian::native == std::endian::little)
				{
					chunkType = std::byteswap(chunkType);
				}

				switch (chunkType)
				{
					// IHDR

					case 0x49484452:
					{
						FMTSTREAM_VERIFY(chunkHeader.length == 13, PngInvalidChunkSize, "PngStream: Expected IHDR chunk size to be 13. Instead, got " + std::to_string(chunkHeader.length) + ".");

						chunk = new Chunk_IHDR;
						Chunk_IHDR* chunk_IHDR = reinterpret_cast<Chunk_IHDR*>(chunk);
						chunk_IHDR->header = chunkHeader;
						crcStart = reinterpret_cast<const uint8_t*>(&chunk_IHDR->header.type);
						constexpr uint32_t size = sizeof(Chunk_IHDR) - offset;
						FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(chunk) + offset, size), InvalidStream, "PngStream: Error while reading IHDR chunk.");

						if constexpr (std::endian::native == std::endian::little)
						{
							chunk_IHDR->width = std::byteswap(chunk_IHDR->width);
							chunk_IHDR->height = std::byteswap(chunk_IHDR->height);
						}

						FMTSTREAM_VERIFY(chunk_IHDR->width != 0, PngInvalidChunkContent, "PngStream: Width of image cannot be 0.");
						FMTSTREAM_VERIFY(chunk_IHDR->height != 0, PngInvalidChunkContent, "PngStream: Height of image cannot be 0.");
						FMTSTREAM_VERIFY(isColorTypeBitDepthAllowed(chunk_IHDR->colorType, chunk_IHDR->bitDepth), PngInvalidChunkContent, "PngStream: Combination of color type and bit depth not allowed. Combination: {" + std::to_string(chunk_IHDR->colorType) + ", " + std::to_string(chunk_IHDR->bitDepth) + "}.");
						FMTSTREAM_VERIFY(chunk_IHDR->compressionMethod == 0, PngInvalidChunkContent, "PngStream: Expected compression method to be 0. Instead, got " + std::to_string(chunk_IHDR->compressionMethod) + ".");
						FMTSTREAM_VERIFY(chunk_IHDR->filterMethod == 0, PngInvalidChunkContent, "PngStream: Expected filter method to be 0. Instead, got " + std::to_string(chunk_IHDR->filterMethod) + ".");
						FMTSTREAM_VERIFY(chunk_IHDR->interlaceMethod <= 1, PngInvalidChunkContent, "PngStream: Expected interlace method to be 0 or 1. Instead, got " + std::to_string(chunk_IHDR->interlaceMethod) + ".");

						break;
					}

					// PLTE

					case 0x504C5445:
					{
						FMTSTREAM_VERIFY(chunkHeader.length % 3 == 0, PngInvalidChunkSize, "PngStream: Expected PLTE chunk size to be a multiple of 3. Instead, got " + std::to_string(chunkHeader.length) + ".");

						chunk = new Chunk_PLTE;
						Chunk_PLTE* chunk_PLTE = reinterpret_cast<Chunk_PLTE*>(chunk);
						chunk_PLTE->header = chunkHeader;
						crcStart = reinterpret_cast<const uint8_t*>(&chunk_PLTE->header.type);

						uint8_t buffer[3];
						for (uint32_t i = 0; i < chunkHeader.length; i += 3)
						{
							FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(buffer), 3), InvalidStream, "PngStream: Error while reading PLTE entry.");
							chunk_PLTE->palette.push_back({ buffer[0], buffer[1], buffer[2] });
						}

						FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(&chunk_PLTE->footer), sizeof(ChunkFooter)), InvalidStream, "PngStream: Error while reading PLTE footer.");

						break;
					}

					// IDAT

					case 0x49444154:
					{
						chunk = new Chunk_IDAT;
						Chunk_IDAT* chunk_IDAT = reinterpret_cast<Chunk_IDAT*>(chunk);
						chunk_IDAT->header = chunkHeader;
						crcStart = reinterpret_cast<const uint8_t*>(&chunk_IDAT->header.type);

						chunk_IDAT->data.resize(chunkHeader.length);
						FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(chunk_IDAT->data.data()), chunkHeader.length), InvalidStream, "PngStream: Error while reading IDAT chunk data.");
						FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(&chunk_IDAT->footer), sizeof(ChunkFooter)), InvalidStream, "PngStream: Error while reading IDAT footer.");

						break;
					}

					// IEND

					case 0x49454E44:
					{
						FMTSTREAM_VERIFY(chunkHeader.length == 0, PngInvalidChunkSize, "PngStream: Expected IEND chunk size to be 0. Instead, got " + std::to_string(chunkHeader.length) + ".");
						
						chunk = new Chunk_IEND;
						Chunk_IEND* chunk_IEND = reinterpret_cast<Chunk_IEND*>(chunk);
						chunk_IEND->header = chunkHeader;
						crcStart = reinterpret_cast<const uint8_t*>(&chunk_IEND->header.type);
						constexpr uint32_t size = sizeof(Chunk_IEND) - offset;
						FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(chunk) + offset, size), InvalidStream, "PngStream: Error while reading IEND chunk.");
						
						break;
					}

					// tRNS

					case 0x74524E53:
					{
						chunk = new Chunk_tRNS;
						Chunk_tRNS* chunk_tRNS = reinterpret_cast<Chunk_tRNS*>(chunk);
						chunk_tRNS->header = chunkHeader;
						crcStart = reinterpret_cast<const uint8_t*>(&chunk_tRNS->header.type);
						const uint32_t size = chunkHeader.length + sizeof(ChunkFooter);
						FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(chunk) + offset, size), InvalidStream, "PngStream: Error while reading tRNS chunk.");

						// TODO: If type 0/2, byteswap

						break;
					}

					// cHRM

					case 0x6348524D:
					{
						FMTSTREAM_VERIFY(chunkHeader.length == 32, PngInvalidChunkSize, "PngStream: Expected cHRM chunk size to be 32. Instead, got " + std::to_string(chunkHeader.length) + ".");

						chunk = new Chunk_cHRM;
						Chunk_cHRM* chunk_cHRM = reinterpret_cast<Chunk_cHRM*>(chunk);
						chunk_cHRM->header = chunkHeader;
						crcStart = reinterpret_cast<const uint8_t*>(&chunk_cHRM->header.type);
						constexpr uint32_t size = sizeof(Chunk_cHRM) - offset;
						FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(chunk) + offset, size), InvalidStream, "PngStream: Error while reading cHRM chunk.");

						if constexpr (std::endian::native == std::endian::little)
						{
							chunk_cHRM->xWhitePoint = std::byteswap(chunk_cHRM->xWhitePoint);
							chunk_cHRM->yWhitePoint = std::byteswap(chunk_cHRM->yWhitePoint);
							chunk_cHRM->xRedPoint = std::byteswap(chunk_cHRM->xRedPoint);
							chunk_cHRM->yRedPoint = std::byteswap(chunk_cHRM->yRedPoint);
							chunk_cHRM->xGreenPoint = std::byteswap(chunk_cHRM->xGreenPoint);
							chunk_cHRM->yGreenPoint = std::byteswap(chunk_cHRM->yGreenPoint);
							chunk_cHRM->xBluePoint = std::byteswap(chunk_cHRM->xBluePoint);
							chunk_cHRM->yBluePoint = std::byteswap(chunk_cHRM->yBluePoint);
						}

						break;
					}

					// gAMA

					case 0x67414D41:
					{
						FMTSTREAM_VERIFY(chunkHeader.length == 4, PngInvalidChunkSize, "PngStream: Expected gAMA chunk size to be 4. Instead, got " + std::to_string(chunkHeader.length) + ".");

						chunk = new Chunk_gAMA;
						Chunk_gAMA* chunk_gAMA = reinterpret_cast<Chunk_gAMA*>(chunk);
						chunk_gAMA->header = chunkHeader;
						crcStart = reinterpret_cast<const uint8_t*>(&chunk_gAMA->header.type);
						constexpr uint32_t size = sizeof(Chunk_gAMA) - offset;
						FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(chunk) + offset, size), InvalidStream, "PngStream: Error while reading gAMA chunk.");

						if constexpr (std::endian::native == std::endian::little)
						{
							chunk_gAMA->gamma = std::byteswap(chunk_gAMA->gamma);
						}

						break;
					}

					// sRGB

					case 0x73524742:
					{
						FMTSTREAM_VERIFY(chunkHeader.length == 1, PngInvalidChunkSize, "PngStream: Expected sRGB chunk size to be 1. Instead, got " + std::to_string(chunkHeader.length) + ".");

						chunk = new Chunk_sRGB;
						Chunk_sRGB* chunk_sRGB = reinterpret_cast<Chunk_sRGB*>(chunk);
						chunk_sRGB->header = chunkHeader;
						crcStart = reinterpret_cast<const uint8_t*>(&chunk_sRGB->header.type);
						constexpr uint32_t size = sizeof(Chunk_sRGB) - offset;
						FMTSTREAM_VERIFY(stream.read(reinterpret_cast<char*>(chunk) + offset, size), InvalidStream, "PngStream: Error while reading sRGB chunk.");

						FMTSTREAM_VERIFY(chunk_sRGB->renderingIntent <= 3, PngInvalidChunkContent, "PngStream: Error while reading sRGB rendering intent. Expected value in [0, 3]. Instead, got " + std::to_string(chunk_sRGB->renderingIntent) + ".");

						break;
					}

					// Unrecognized header

					default:
					{
						return error;
					}
				}

				const uint32_t crcLength = chunkHeader.length + 4;
				const uint32_t crcStored = *reinterpret_cast<const uint32_t*>(crcStart + crcLength);
				// byteswap

				// TODO: Check CRC

				return error;
			}
		}

		PngStream::PngStream() : FormatStream(std::endian::big)
		{
		}

		const FormatError& PngStream::readFile(png::File& file)
		{
			FMTSTREAM_BEGIN_READ();

			FMTSTREAM_VERIFY_CALL(readHeader, file.header);

			return error;
		}

		const FormatError& PngStream::readHeader(png::Header& header)
		{
			FMTSTREAM_BEGIN_READ();

			char buffer[8];
			std::streampos firstIdatChunk = -1;
			
			ChunkHeader chunkHeader;
			void* chunk = nullptr;

			// Read PNG signature

			FMTSTREAM_VERIFY(stream.read(buffer, 8), InvalidStream, "PngStream: Error while reading PNG signature.");
			FMTSTREAM_VERIFY(std::equal(buffer, buffer + 8, "\x89PNG\r\n\x1A\n"), PngInvalidSignature, "PngStream: Invalid PNG signature.");

			// Load IHDR chunk

			FMTSTREAM_VERIFY_CALL(readChunkHeader, stream, error, chunkHeader);
			FMTSTREAM_VERIFY(std::equal(chunkHeader.type, chunkHeader.type + 4, "IHDR"), PngInvalidChunkType, "PngStream: Expected first chunk to be 'IHDR'. Instead, got '" + std::string(chunkHeader.type, 4) + "'.");
			if (!readChunkData(stream, error, chunkHeader, chunk))
			{
				delete chunk;
				return error;
			}
			Chunk_IHDR* chunkIHDR = reinterpret_cast<Chunk_IHDR*>(chunk);

			header.imageStructure.width = chunkIHDR->width;
			header.imageStructure.height = chunkIHDR->height;
			header.imageStructure.bitDepth = chunkIHDR->bitDepth;
			header.imageStructure.colorType = static_cast<png::ColorType>(chunkIHDR->colorType);
			header.imageStructure.interlaceMethod = static_cast<png::InterlaceMethod>(chunkIHDR->interlaceMethod);

			// Load all chunks (except IDAT) until IEND

			FMTSTREAM_VERIFY_CALL(readChunkHeader, stream, error, chunkHeader);
			while (!std::equal(chunkHeader.type, chunkHeader.type + 4, "IEND"))
			{
				/*if (!readChunkData(stream, error, chunkHeader, chunk))
				{
					delete chunk;
					return error;
				}

				FMTSTREAM_VERIFY_CALL(readChunkHeader, stream, error, chunkHeader);
			}

			// Load IEND

			// Go to first IDAT chunk

			return error;
		}

		const FormatError& PngStream::writeFile(const png::File& file)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& PngStream::writeHeader(const png::Header& header)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& PngStream::writeImageStructure(const png::ImageStructure& imageStructure)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& PngStream::writeColorSpace(const png::ColorSpace& colorSpace)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& PngStream::writeGamma(float gamma)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& PngStream::writeSRBGIntent(const png::SRGBIntent& sRGBIntent)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}
	}
}
*/
