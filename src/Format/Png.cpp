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
		// namespace
		// {
		// 	struct ChunkHeader
		// 	{
		// 		uint32_t size;
		// 		char type[4];
		// 	}; 
		// 
		// 	bool isBitDepthColorTypeAllowed(uint8_t bitDepth, uint8_t colorType)
		// 	{
		// 		switch (colorType)
		// 		{
		// 			case 0:
		// 				return (bitDepth == 1) || (bitDepth == 2) || (bitDepth == 4) || (bitDepth == 8) || (bitDepth == 16);
		// 			case 3:
		// 				return (bitDepth == 1) || (bitDepth == 2) || (bitDepth == 4) || (bitDepth == 8);
		// 			case 2:
		// 			case 4:
		// 			case 6:
		// 				return (bitDepth == 8) || (bitDepth == 16);
		// 			default:
		// 				return false;
		// 		}
		// 	}
		// 
		// 	bool isLastInterlacePass(const png::ImageStructure& imageStructure, uint8_t currentPass)
		// 	{
		// 		return (imageStructure.interlaceMethod == png::InterlaceMethod::NoInterlace && currentPass == 0)
		// 			|| (imageStructure.interlaceMethod == png::InterlaceMethod::Adam7 && currentPass == 6);
		// 	}
		// 
		// 	uint32_t computeScanlineLength(const png::ImageStructure& imageStructure, uint8_t currentPass)
		// 	{
		// 		const uint32_t& width = imageStructure.width;
		// 		const uint32_t& height = imageStructure.height;
		// 
		// 		switch (imageStructure.interlaceMethod)
		// 		{
		// 			case png::InterlaceMethod::NoInterlace:
		// 			{
		// 				return width;
		// 			}
		// 			case png::InterlaceMethod::Adam7:
		// 			{
		// 				switch (currentPass)
		// 				{
		// 					case 0:
		// 					{
		// 						return (width + 7) / 8;
		// 					}
		// 					case 1:
		// 					{
		// 						return ((width + 3) / 8);
		// 					}
		// 					case 2:
		// 					{
		// 						return ((width + 3) / 4);
		// 					}
		// 					case 3:
		// 					{
		// 						return ((width + 1) / 4);
		// 					}
		// 					case 4:
		// 					{
		// 						return ((width + 1) / 2);
		// 					}
		// 					case 5:
		// 					{
		// 						return width / 2;
		// 					}
		// 					case 6:
		// 					{
		// 						return width;
		// 					}
		// 				}
		// 			}
		// 		}
		// 	}
		// 	
		// 	uint32_t computeScanlineCount(const png::ImageStructure& imageStructure, uint8_t currentPass)
		// 	{
		// 		const uint32_t& width = imageStructure.width;
		// 		const uint32_t& height = imageStructure.height;
		// 
		// 		switch (imageStructure.interlaceMethod)
		// 		{
		// 			case png::InterlaceMethod::NoInterlace:
		// 			{
		// 				return height;
		// 			}
		// 			case png::InterlaceMethod::Adam7:
		// 			{
		// 				switch (currentPass)
		// 				{
		// 					case 0:
		// 					{
		// 						return (height + 7) / 8;
		// 					}
		// 					case 1:
		// 					{
		// 						return (width > 4) * ((height + 7) / 8);
		// 					}
		// 					case 2:
		// 					{
		// 						return (height + 3) / 8;
		// 					}
		// 					case 3:
		// 					{
		// 						return (width > 2) * ((height + 3) / 4);
		// 					}
		// 					case 4:
		// 					{
		// 						return (height + 1) / 4;
		// 					}
		// 					case 5:
		// 					{
		// 						return (width > 1) * ((height + 1) / 2);
		// 					}
		// 					case 6:
		// 					{
		// 						return height / 2;
		// 					}
		// 				}
		// 			}
		// 		}
		// 	}
		// 
		// 	bool isLastPixel(const png::ImageStructure& imageStructure, uint8_t currentPass, uint32_t currentScanline, uint32_t currentPixel)
		// 	{
		// 		return isLastInterlacePass(imageStructure, currentPass)
		// 			&& currentScanline == computeScanlineCount(imageStructure, currentPass) - 1
		// 			&& currentPixel == computeScanlineLength(imageStructure, currentPass);
		// 	}
		// 
		// 	uint8_t paethPredictor(uint8_t a, uint8_t b, uint8_t c)
		// 	{
		// 		int16_t p = a + b - c;
		// 		int16_t pa = std::abs(p - a);
		// 		int16_t pb = std::abs(p - b);
		// 		int16_t pc = std::abs(p - c);
		// 
		// 		if (pa <= pb && pa <= pc)
		// 		{
		// 			return a;
		// 		}
		// 		else if (pb <= pc)
		// 		{
		// 			return b;
		// 		}
		// 		else
		// 		{
		// 			return c;
		// 		}
		// 	}
		// }
		// 
		// PngIStream::PngIStream() : FormatIStream(std::endian::big),
		// 	_zlibStream(nullptr),
		// 	_dataStream(),
		// 	_header(),
		// 	_palette(),
		// 	_headerRead(false),
		// 	_currentPass(0),
		// 	_currentScanline(0),
		// 	_currentPixel(0),
		// 	_scanline(),
		// 	_scanlineIndex(0),
		// 	_scanlineFilter(0)
		// {
		// 	FMT_CREATE_SUBSTREAM(_zlibStream, ZlibIStream);
		// 
		// 	finishSubStreamTree();
		// }
		// 
		// const ruc::Status& PngIStream::readHeader(png::Header& header)
		// {
		// 	FMT_BEGIN("PngIStream::readHeader(png::Header& header)");
		// 
		// 	assert(!_headerRead);
		// 
		// 	char buffer[8];
		// 	ChunkHeader chunkHeader;
		// 
		// 	// Read PNG signature
		// 
		// 	FMT_READ(buffer, 8);
		// 	FMT_VERIFY(std::equal(buffer, buffer + 8, "\x89PNG\r\n\x1A\n"), PngInvalidSignature, "Invalid PNG signature.");
		// 
		// 	// Read IHDR chunk
		// 
		// 	FMT_CALL(_readChunkHeader, &chunkHeader);
		// 	FMT_VERIFY(fourcc(chunkHeader.type) == fourcc("IHDR"), PngInvalidChunkLayout, "Expected first chunk to be 'IHDR'. Instead, got '" + std::string(chunkHeader.type, 4) + "'.");
		// 	FMT_CALL(_readChunk_IHDR, &chunkHeader);
		// 	FMT_CALL(_readChunkEnding, &chunkHeader);
		// 
		// 	// Read all chunks before IDAT
		// 
		// 	std::unordered_set<uint32_t> chunksRead = { fourcc("IHDR") };
		// 
		// 	FMT_CALL(_readChunkHeader, &chunkHeader);
		// 	while (fourcc(chunkHeader.type) != fourcc("IDAT"))
		// 	{
		// 		switch (fourcc(chunkHeader.type))
		// 		{
		// 			case fourcc("PLTE"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("PLTE")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'PLTE' chunks.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("tRNS")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have 'PLTE' chunk after 'tRNS' chunk.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("hIST")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have 'PLTE' chunk after 'hIST' chunk.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("bKGD")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have 'PLTE' chunk after 'bKGD' chunk.");
		// 
		// 				FMT_CALL(_readChunk_PLTE, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("cHRM"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("cHRM")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'cHRM' chunks.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("PLTE")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have 'cHRM' chunk after 'PLTE' chunk.");
		// 
		// 				FMT_CALL(_readChunk_cHRM, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("gAMA"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("gAMA")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'gAMA' chunks.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("PLTE")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have 'gAMA' chunk after 'PLTE' chunk.");
		// 
		// 				FMT_CALL(_readChunk_gAMA, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("iCCP"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("iCCP")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'iCCP' chunks.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("PLTE")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have 'iCCP' chunk after 'PLTE' chunk.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("sRGB")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have 'iCCP' chunk after 'sRGB' chunk.");
		// 
		// 				FMT_CALL(_readChunk_iCCP, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("sBIT"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("sBIT")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'sBIT' chunks.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("PLTE")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have 'sBIT' chunk after 'PLTE' chunk.");
		// 
		// 				FMT_CALL(_readChunk_sBIT, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("sRGB"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("sRGB")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'sRGB' chunks.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("PLTE")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have 'sRGB' chunk after 'PLTE' chunk.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("iCCP")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have 'sRGB' chunk after 'iCCP' chunk.");
		// 
		// 				FMT_CALL(_readChunk_sRGB, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("bKGD"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("bKGD")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'bKGD' chunks.");
		// 
		// 				FMT_CALL(_readChunk_bKGD, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("hIST"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("PLTE")) != chunksRead.end(), PngInvalidChunkLayout, "'hIST' chunk must appears after 'PLTE' chunk.");
		// 				FMT_VERIFY(chunksRead.find(fourcc("hIST")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'hIST' chunks.");
		// 
		// 				FMT_CALL(_readChunk_hIST, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("tRNS"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("tRNS")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'tRNS' chunks.");
		// 
		// 				FMT_CALL(_readChunk_tRNS, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("pHYs"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("pHYs")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'pHYs' chunks.");
		// 
		// 				FMT_CALL(_readChunk_pHYs, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("sPLT"):
		// 			{
		// 				FMT_CALL(_readChunk_sPLT, &chunkHeader);
		// 
		// 				break;
		// 			}
		// 			case fourcc("tIME"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("tIME")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'tIME' chunks.");
		// 
		// 				_header.lastModification.emplace();
		// 				FMT_CALL(_readChunk_tIME, &chunkHeader, _header.lastModification.value());
		// 
		// 				break;
		// 			}
		// 			case fourcc("iTXt"):
		// 			{
		// 				_header.textualDatas.emplace_back();
		// 				FMT_CALL(_readChunk_iTXt, &chunkHeader, _header.textualDatas.back());
		// 
		// 				break;
		// 			}
		// 			case fourcc("tEXt"):
		// 			{
		// 				_header.textualDatas.emplace_back();
		// 				FMT_CALL(_readChunk_tEXt, &chunkHeader, _header.textualDatas.back());
		// 
		// 				break;
		// 			}
		// 			case fourcc("zTXt"):
		// 			{
		// 				_header.textualDatas.emplace_back();
		// 				FMT_CALL(_readChunk_zTXt, &chunkHeader, _header.textualDatas.back());
		// 
		// 				break;
		// 			}
		// 			default:
		// 			{
		// 				FMT_VERIFY(false, PngInvalidChunkLayout, "Unknown chunk type: '" + std::string(chunkHeader.type, 4) + "'.");
		// 			}
		// 		}
		// 
		// 		FMT_CALL(_readChunkEnding, &chunkHeader);
		// 
		// 		chunksRead.insert(fourcc(chunkHeader.type));
		// 		FMT_CALL(_readChunkHeader, &chunkHeader);
		// 	}
		// 
		// 	FMT_CALL(unget, 8);	// Unget IDAT header
		// 
		// 	// Check a palette was read if one is needed and move it to suggested palettes if one is optional
		// 
		// 	if (_header.imageStructure.colorType == png::ColorType::IndexedColor)
		// 	{
		// 		FMT_VERIFY(!_palette.entries.empty(), PngInvalidChunkLayout, "Missing 'PLTE' chunk for color type 3 (IndexedColor).");
		// 	}
		// 	else if (!_palette.entries.empty())
		// 	{
		// 		_header.suggestedPalettes.push_back(_palette);
		// 	}
		// 
		// 	// Check gamma and chromaticity if 'sRGB' chunk was found
		// 
		// 	if (chunksRead.find(fourcc("sRGB")) != chunksRead.end())
		// 	{
		// 		if (chunksRead.find(fourcc("cHRM")) != chunksRead.end())
		// 		{
		// 			FMT_VERIFY(_header.chromaticity->whitePoint[0] == 0.3127f, PngInvalidChunkContent, "Bad x coordinate for white point of chromaticity for sRGB color space.");
		// 			FMT_VERIFY(_header.chromaticity->whitePoint[1] == 0.329f, PngInvalidChunkContent, "Bad y coordinate for white point of chromaticity for sRGB color space.");
		// 			FMT_VERIFY(_header.chromaticity->redPoint[0] == 0.64f, PngInvalidChunkContent, "Bad x coordinate for red point of chromaticity for sRGB color space.");
		// 			FMT_VERIFY(_header.chromaticity->redPoint[1] == 0.33f, PngInvalidChunkContent, "Bad y coordinate for red point of chromaticity for sRGB color space.");
		// 			FMT_VERIFY(_header.chromaticity->greenPoint[0] == 0.3f, PngInvalidChunkContent, "Bad x coordinate for green point of chromaticity for sRGB color space.");
		// 			FMT_VERIFY(_header.chromaticity->greenPoint[1] == 0.6f, PngInvalidChunkContent, "Bad y coordinate for green point of chromaticity for sRGB color space.");
		// 			FMT_VERIFY(_header.chromaticity->bluePoint[0] == 0.15f, PngInvalidChunkContent, "Bad x coordinate for blue point of chromaticity for sRGB color space.");
		// 			FMT_VERIFY(_header.chromaticity->bluePoint[1] == 0.06f, PngInvalidChunkContent, "Bad y coordinate for blue point of chromaticity for sRGB color space.");
		// 		}
		// 		if (chunksRead.find(fourcc("gAMA")) != chunksRead.end())
		// 		{
		// 			FMT_VERIFY(_header.gamma == 0.45455f, PngInvalidChunkContent, "Bad y coordinate for blue point of chromaticity for sRGB color space.");
		// 		}
		// 	}
		// 
		// 	// Assign header read to the one returned
		// 
		// 	_headerRead = true;
		// 	header = _header;
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::readEnding(png::Ending& ending)
		// {
		// 	FMT_BEGIN("PngIStream::readEnding(png::Ending& ending)");
		// 
		// 	assert(_headerRead);
		// 	assert(isLastPixel(_header.imageStructure, _currentPass, _currentScanline, _currentPixel));
		// 
		// 	ending.textualDatas.clear();
		// 	ending.lastModification.reset();
		// 
		// 	ChunkHeader chunkHeader;
		// 	std::unordered_set<uint32_t> chunksRead;
		// 
		// 	FMT_SUB_CALL(_zlibStream, readEndFile);
		// 
		// 	FMT_CALL(_readChunkHeader, &chunkHeader);
		// 	while (fourcc(chunkHeader.type) != fourcc("IEND"))
		// 	{
		// 		switch (fourcc(chunkHeader.type))
		// 		{
		// 			case fourcc("tIME"):
		// 			{
		// 				FMT_VERIFY(chunksRead.find(fourcc("tIME")) == chunksRead.end(), PngInvalidChunkLayout, "Cannot have multiple 'tIME' chunks.");
		// 				FMT_VERIFY(!_header.lastModification.has_value(), PngInvalidChunkLayout, "Cannot have multiple 'tIME' chunks.");
		// 
		// 				ending.lastModification.emplace();
		// 				FMT_CALL(_readChunk_tIME, &chunkHeader, _header.lastModification.value());
		// 
		// 				break;
		// 			}
		// 			case fourcc("iTXt"):
		// 			{
		// 				_header.textualDatas.emplace_back();
		// 				FMT_CALL(_readChunk_iTXt, &chunkHeader, _header.textualDatas.back());
		// 
		// 				break;
		// 			}
		// 			case fourcc("tEXt"):
		// 			{
		// 				_header.textualDatas.emplace_back();
		// 				FMT_CALL(_readChunk_tEXt, &chunkHeader, _header.textualDatas.back());
		// 
		// 				break;
		// 			}
		// 			case fourcc("zTXt"):
		// 			{
		// 				_header.textualDatas.emplace_back();
		// 				FMT_CALL(_readChunk_zTXt, &chunkHeader, _header.textualDatas.back());
		// 
		// 				break;
		// 			}
		// 		}
		// 
		// 		FMT_CALL(_readChunkEnding, &chunkHeader);
		// 
		// 		chunksRead.insert(fourcc(chunkHeader.type));
		// 		FMT_CALL(_readChunkHeader, &chunkHeader);
		// 	}
		// 
		// 	FMT_CALL(_readChunk_IEND, &chunkHeader);
		// 	FMT_CALL(_readChunkEnding, &chunkHeader);
		// 
		// 	_dataStream = std::stringstream();
		// 
		// 	_header = png::Header();
		// 	_palette.entries.clear();
		// 
		// 	_headerRead = false;
		// 
		// 	_currentPass = 0;
		// 	_currentScanline = 0;
		// 	_currentPixel = 0;
		// 
		// 	_scanline.clear();
		// 	_scanlineIndex = 0;
		// 	_scanlineFilter = 0;
		// 
		// 	return *_error;
		// }
		// 
		// void PngIStream::onSourceRemoved()
		// {
		// 	_dataStream = std::stringstream();
		// 
		// 	_header = png::Header();
		// 	_palette.entries.clear();
		// 
		// 	_headerRead = false;
		// 
		// 	_currentPass = 0;
		// 	_currentScanline = 0;
		// 	_currentPixel = 0;
		// 
		// 	_scanline.clear();
		// 	_scanlineIndex = 0;
		// 	_scanlineFilter = 0;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunkHeader(void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunkHeader(void* ptrChunkHeader)");
		// 
		// 	ChunkHeader& chunkHeader = *reinterpret_cast<ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_READ(chunkHeader.size);
		// 	FMT_READ(chunkHeader.type, 4);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_IHDR(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_IHDR(const void* ptrChunkHeader)");
		// 
		// 	uint8_t buffer;
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size == 13, PngInvalidChunkSize, "Expected IHDR chunk size to be 13. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	FMT_READ(_header.imageStructure.width);
		// 	FMT_VERIFY(_header.imageStructure.width != 0, PngInvalidChunkContent, "Image width cannot be 0.");
		// 
		// 	FMT_READ(_header.imageStructure.height);
		// 	FMT_VERIFY(_header.imageStructure.height != 0, PngInvalidChunkContent, "Image height cannot be 0.");
		// 
		// 	FMT_READ(_header.imageStructure.bitDepth);
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(isBitDepthColorTypeAllowed(_header.imageStructure.bitDepth, buffer), PngInvalidChunkContent, "Invalid combination of bit depth and color type: (" + std::to_string(_header.imageStructure.bitDepth) + ", " + std::to_string(buffer) + ").");
		// 	_header.imageStructure.colorType = static_cast<png::ColorType>(buffer);
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer == 0, PngInvalidChunkContent, "Compression method must be 0. Instead, got " + std::to_string(buffer) + ".");
		// 	_header.imageStructure.compressionMethod = static_cast<png::CompressionMethod>(buffer);
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer == 0, PngInvalidChunkContent, "Filter method must be 0. Instead, got " + std::to_string(buffer) + ".");
		// 	_header.imageStructure.filterMethod = static_cast<png::FilterMethod>(buffer);
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer < 2, PngInvalidChunkContent, "Interlace method must be 0 or 1. Instead, got " + std::to_string(buffer) + ".");
		// 	_header.imageStructure.interlaceMethod = static_cast<png::InterlaceMethod>(buffer);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_PLTE(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_PLTE(const void* ptrChunkHeader)");
		// 
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(_header.imageStructure.colorType != png::ColorType::GreyScale, PngInvalidChunkLayout, "Cannot have 'PLTE' chunk with color type of 0 (GreyScale).");
		// 	FMT_VERIFY(_header.imageStructure.colorType != png::ColorType::GreyScaleAlpha, PngInvalidChunkLayout, "Cannot have 'PLTE' chunk with color type of 4 (GreyScaleAlpha).");
		// 
		// 	FMT_VERIFY(chunkHeader.size % 3 == 0, PngInvalidChunkSize, "'PLTE' chunk size should be a multiple of 3. Instead, got '" + std::to_string(chunkHeader.size) + "'.");
		// 
		// 	_palette.entries.resize(chunkHeader.size / 3);
		// 	for (png::PaletteEntry& entry : _palette.entries)
		// 	{
		// 		entry.frequency = 1;
		// 		FMT_READ(entry.r);
		// 		FMT_READ(entry.g);
		// 		FMT_READ(entry.b);
		// 		entry.a = 255;
		// 	}
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_IDAT(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_IDAT(const void* ptrChunkHeader)");
		// 
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	// Initialize crc with the chunk type
		// 
		// 	uint32_t computedCrc = cksm::crc32(chunkHeader.type, 4);
		// 
		// 	// Read the whole chunk, put it into _dataStream and compute the crc
		// 
		// 	constexpr uint64_t bufferSize = 1024;
		// 	char buffer[bufferSize];
		// 
		// 	uint64_t size = chunkHeader.size;
		// 	while (size > bufferSize)
		// 	{
		// 		FMT_READ(buffer, bufferSize);
		// 		_dataStream.write(buffer, bufferSize);
		// 		computedCrc = cksm::crc32(buffer, bufferSize, computedCrc);
		// 		size -= bufferSize;
		// 	}
		// 	FMT_READ(buffer, size);
		// 	_dataStream.write(buffer, size);
		// 	computedCrc = cksm::crc32(buffer, size, computedCrc);
		// 
		// 	// Check the crc is equal to the one read
		// 
		// 	uint32_t readCrc;
		// 	FMT_READ(readCrc);
		// 	FMT_VERIFY(readCrc == computedCrc, PngInvalidChunkCRC, "Computed CRC and read CRC are different for chunk '" + std::string(chunkHeader.type, 4) + "'.");
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_IEND(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_IEND(const void* ptrChunkHeader)");
		// 
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size == 0, PngInvalidChunkSize, "Expected 'IEND' chunk size to be 0. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_cHRM(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_cHRM(const void* ptrChunkHeader)");
		// 
		// 	uint32_t buffer[8];
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size == 32, PngInvalidChunkSize, "Expected 'cHRM' chunk size to be 32. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	_header.chromaticity.emplace();
		// 
		// 	FMT_READ(buffer, 8);
		// 
		// 	_header.chromaticity->whitePoint[0] = buffer[0] / 100000.f;
		// 	_header.chromaticity->whitePoint[1] = buffer[1] / 100000.f;
		// 	_header.chromaticity->redPoint[0] = buffer[2] / 100000.f;
		// 	_header.chromaticity->redPoint[1] = buffer[3] / 100000.f;
		// 	_header.chromaticity->greenPoint[0] = buffer[4] / 100000.f;
		// 	_header.chromaticity->greenPoint[1] = buffer[5] / 100000.f;
		// 	_header.chromaticity->bluePoint[0] = buffer[6] / 100000.f;
		// 	_header.chromaticity->bluePoint[1] = buffer[7] / 100000.f;
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_gAMA(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_gAMA(const void* ptrChunkHeader)");
		// 
		// 	uint32_t buffer;
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size == 4, PngInvalidChunkSize, "Expected 'gAMA' chunk size to be 4. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	_header.gamma.emplace();
		// 
		// 	FMT_READ(buffer);
		// 
		// 	_header.gamma = buffer / 100000.f;
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_iCCP(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_iCCP(const void* ptrChunkHeader)");
		// 
		// 	uint8_t buffer;
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size >= 4, PngInvalidChunkSize, "Expected 'iCCP' chunk size to be at least 4. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	_header.iccProfile.emplace();
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer != 0, PngInvalidChunkContent, "ICC Profile must have a name.");
		// 
		// 	while (buffer != 0)
		// 	{
		// 		_header.iccProfile->name.push_back(buffer);
		// 		FMT_READ(buffer);
		// 	}
		// 
		// 	// TODO: check the name (latin-1 characters, no leading/trailing spaces...)
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer == 0, PngInvalidChunkContent, "Compression method must be 0. Instead, got " + std::to_string(buffer) + ".");
		// 	_header.iccProfile->compressionMethod = static_cast<png::CompressionMethod>(buffer);
		// 
		// 	zlib::File zlibFile;
		// 	FMT_SUB_CALL(_zlibStream, readFile, zlibFile);
		// 	_header.iccProfile->data = zlibFile.data;
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_sBIT(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_sBIT(const void* ptrChunkHeader)");
		// 
		// 	uint8_t buffer[4];
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	_header.significantBits.emplace(png::SignificantBits{0, 0, 0, 0, 0});
		// 
		// 	switch (_header.imageStructure.colorType)
		// 	{
		// 		case png::ColorType::GreyScale:
		// 		{
		// 			FMT_VERIFY(chunkHeader.size == 1, PngInvalidChunkSize, "Expected 'sBIT' chunk size to be 1 for color type 0 (GreyScale). Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 			FMT_READ(_header.significantBits->grayscaleBits);
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColor:
		// 		case png::ColorType::IndexedColor:
		// 		{
		// 			FMT_VERIFY(chunkHeader.size == 3, PngInvalidChunkSize, "Expected 'sBIT' chunk size to be 3 for color types 2 and 3 (TrueColor and IndexedColor). Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 			FMT_READ(_header.significantBits->redBits);
		// 			FMT_READ(_header.significantBits->greenBits);
		// 			FMT_READ(_header.significantBits->blueBits);
		// 			break;
		// 		}
		// 		case png::ColorType::GreyScaleAlpha:
		// 		{
		// 			FMT_VERIFY(chunkHeader.size == 2, PngInvalidChunkSize, "Expected 'sBIT' chunk size to be 2 for color type 4 (GreyScaleAlpha). Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 			FMT_READ(_header.significantBits->grayscaleBits);
		// 			FMT_READ(_header.significantBits->alphaBits);
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColorAlpha:
		// 		{
		// 			FMT_VERIFY(chunkHeader.size == 4, PngInvalidChunkSize, "Expected 'sBIT' chunk size to be 4 for color type 6 (TrueColorAlpha). Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 			FMT_READ(_header.significantBits->redBits);
		// 			FMT_READ(_header.significantBits->greenBits);
		// 			FMT_READ(_header.significantBits->blueBits);
		// 			FMT_READ(_header.significantBits->alphaBits);
		// 			break;
		// 		}
		// 	}
		// 
		// 	FMT_VERIFY(_header.significantBits->redBits <= _header.imageStructure.bitDepth, PngInvalidChunkContent, "There cannot be more significant bits than in bit depth.");
		// 	FMT_VERIFY(_header.significantBits->greenBits <= _header.imageStructure.bitDepth, PngInvalidChunkContent, "There cannot be more significant bits than in bit depth.");
		// 	FMT_VERIFY(_header.significantBits->blueBits <= _header.imageStructure.bitDepth, PngInvalidChunkContent, "There cannot be more significant bits than in bit depth.");
		// 	FMT_VERIFY(_header.significantBits->alphaBits <= _header.imageStructure.bitDepth, PngInvalidChunkContent, "There cannot be more significant bits than in bit depth.");
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_sRGB(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_sRGB(const void* ptrChunkHeader)");
		// 
		// 	uint8_t buffer;
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size == 1, PngInvalidChunkSize, "Expected 'sRGB' chunk size to be 1. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	_header.srgbIntent.emplace();
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer < 4, PngInvalidChunkContent, "Expected sRGB intent to be less than 4. Instead, got " + std::to_string(buffer) + ".");
		// 	_header.srgbIntent = static_cast<png::SRGBIntent>(buffer);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_bKGD(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_bKGD(const void* ptrChunkHeader)");
		// 
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	_header.backgroundColor.emplace();
		// 
		// 	switch (_header.imageStructure.colorType)
		// 	{
		// 		case png::ColorType::GreyScale:
		// 		case png::ColorType::GreyScaleAlpha:
		// 		{
		// 			FMT_VERIFY(chunkHeader.size == 2, PngInvalidChunkSize, "Expected 'bKGD' chunk size to be 2 for color types 0 and 4 (GreyScale and GreyScaleAlpha). Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 			
		// 			FMT_READ(_header.backgroundColor->r);
		// 			_header.backgroundColor->g = _header.backgroundColor->r;
		// 			_header.backgroundColor->b = _header.backgroundColor->r;
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColor:
		// 		case png::ColorType::TrueColorAlpha:
		// 		{
		// 			FMT_VERIFY(chunkHeader.size == 6, PngInvalidChunkSize, "Expected 'bKGD' chunk size to be 6 for color types 2 and 6 (TrueColor and TrueColorAlpha). Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 			
		// 			FMT_READ(_header.backgroundColor->r);
		// 			FMT_READ(_header.backgroundColor->g);
		// 			FMT_READ(_header.backgroundColor->b);
		// 			
		// 			break;
		// 		}
		// 		case png::ColorType::IndexedColor:
		// 		{
		// 			uint8_t buffer;
		// 
		// 			FMT_VERIFY(chunkHeader.size == 1, PngInvalidChunkSize, "Expected 'bKGD' chunk size to be 1 for color type 3 (IndexedColor). Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 			
		// 			FMT_READ(buffer);
		// 
		// 			_header.backgroundColor->r = _palette.entries[buffer].r;
		// 			_header.backgroundColor->g = _palette.entries[buffer].g;
		// 			_header.backgroundColor->b = _palette.entries[buffer].b;
		// 			
		// 			break;
		// 		}
		// 	}
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_hIST(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_hIST(const void* ptrChunkHeader)");
		// 
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size == _palette.entries.size()*2, PngInvalidChunkSize, "Expected 'hIST' chunk to have same number of entries as in the palette.");
		// 
		// 	for (png::PaletteEntry& entry : _palette.entries)
		// 	{
		// 		FMT_READ(entry.frequency);
		// 	}
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_tRNS(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_tRNS(const void* ptrChunkHeader)");
		// 
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	switch (_header.imageStructure.colorType)
		// 	{
		// 		case png::ColorType::GreyScale:
		// 		{
		// 			_header.transparencyMask.emplace();
		// 			FMT_VERIFY(chunkHeader.size == 2, PngInvalidChunkSize, "Expected 'tRNS' chunk size to be 2 for color type 0 (GreyScale). Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 			FMT_READ(_header.transparencyMask->r);
		// 			_header.transparencyMask->g = _header.transparencyMask->r;
		// 			_header.transparencyMask->b = _header.transparencyMask->r;
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColor:
		// 		{
		// 			_header.transparencyMask.emplace();
		// 			FMT_VERIFY(chunkHeader.size == 6, PngInvalidChunkSize, "Expected 'tRNS' chunk size to be 6 for color type 2 (TrueColor). Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 			FMT_READ(_header.transparencyMask->r);
		// 			FMT_READ(_header.transparencyMask->g);
		// 			FMT_READ(_header.transparencyMask->b);
		// 			break;
		// 		}
		// 		case png::ColorType::IndexedColor:
		// 		{
		// 			FMT_VERIFY(chunkHeader.size == _palette.entries.size(), PngInvalidChunkSize, "Expected 'tRNS' chunk to be the size of the palette for color types 3 (IndexedColor). Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 			for (png::PaletteEntry& entry : _palette.entries)
		// 			{
		// 				FMT_READ(entry.a);
		// 			}
		// 			break;
		// 		}
		// 		case png::ColorType::GreyScaleAlpha:
		// 		{
		// 			FMT_VERIFY(false, PngInvalidChunkLayout, "Cannot have 'tRNS' chunk for color type 4 (GreyScaleAlpha).");
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColorAlpha:
		// 		{
		// 			FMT_VERIFY(false, PngInvalidChunkLayout, "Cannot have 'tRNS' chunk for color type 6 (TrueColorAlpha).");
		// 			break;
		// 		}
		// 	}
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_pHYs(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_pHYs(const void* ptrChunkHeader)");
		// 
		// 	uint8_t buffer;
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size == 9, PngInvalidChunkSize, "Expected 'pHYs' chunk size to be 9. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	_header.pixelDimensions.emplace();
		// 
		// 	FMT_READ(_header.pixelDimensions->xPixelsPerUnit);
		// 	FMT_READ(_header.pixelDimensions->yPixelsPerUnit);
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer < 2, PngInvalidChunkContent, "Expected pixel dimension unit to be less than 2. Instead, got " + std::to_string(buffer) + ".");
		// 	_header.pixelDimensions->unit = static_cast<png::PixelDimensionUnit>(buffer);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_sPLT(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_sPLT(const void* ptrChunkHeader)");
		// 
		// 	uint8_t buffer;
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size >= 3, PngInvalidChunkSize, "Expected 'sPLT' chunk size to be at least 3. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	_header.suggestedPalettes.emplace_back();
		// 	png::Palette& palette = _header.suggestedPalettes.back();
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer != 0, PngInvalidChunkContent, "Palette must have a name.");
		// 
		// 	while (buffer != 0)
		// 	{
		// 		palette.name.push_back(buffer);
		// 		FMT_READ(buffer);
		// 	}
		// 
		// 	for (png::Palette& otherPalette : _header.suggestedPalettes)
		// 	{
		// 		FMT_VERIFY(palette.name != otherPalette.name || &palette == &otherPalette, PngInvalidChunkContent, "Two palettes cannot have the same name. Name: '" + palette.name + "'.");
		// 	}
		// 
		// 	// TODO: Check palette name
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer == 8 || buffer == 16, PngInvalidChunkContent, "Sample depth of palette should be 8 or 16. Instead, got " + std::to_string(buffer) + ".");
		// 
		// 	uint32_t remainingSize = chunkHeader.size - 2 - palette.name.size();
		// 	FMT_VERIFY(remainingSize % (6 + buffer / 2) == 0, PngInvalidChunkSize, "Invalid 'sPLT' chunk size given the sample depth and the size of an entry.");
		// 
		// 	palette.entries.resize(remainingSize / (6 + buffer / 2));
		// 	if (buffer == 8)
		// 	{
		// 		for (png::PaletteEntry& entry : palette.entries)
		// 		{
		// 			FMT_READ(entry.r);
		// 			FMT_READ(entry.g);
		// 			FMT_READ(entry.b);
		// 			FMT_READ(entry.a);
		// 			FMT_READ(entry.frequency);
		// 		}
		// 	}
		// 	else
		// 	{
		// 		for (png::PaletteEntry& entry : palette.entries)
		// 		{
		// 			FMT_READ(entry.frequency);
		// 			entry.r = entry.frequency >> 8;
		// 			FMT_READ(entry.frequency);
		// 			entry.g = entry.frequency >> 8;
		// 			FMT_READ(entry.frequency);
		// 			entry.b = entry.frequency >> 8;
		// 			FMT_READ(entry.frequency);
		// 		}
		// 	}
		// 
		// 	// TODO: Check descending order of frequency for entries, std::is_sorted
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_tIME(const void* ptrChunkHeader, png::LastModification& lastModification)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_tIME(const void* ptrChunkHeader, png::LastModification& lastModification)");
		// 
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size == 7, PngInvalidChunkSize, "Expected 'tIME' chunk size to be 7. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	FMT_READ(lastModification.year);
		// 	FMT_READ(lastModification.month);
		// 	FMT_READ(lastModification.day);
		// 	FMT_READ(lastModification.hour);
		// 	FMT_READ(lastModification.minute);
		// 	FMT_READ(lastModification.second);
		// 
		// 	FMT_VERIFY(lastModification.month != 0 && lastModification.month < 13, PngInvalidChunkContent, "Invalid month : " + std::to_string(lastModification.month) + ".");
		// 	FMT_VERIFY(lastModification.day != 0 && lastModification.day < 32, PngInvalidChunkContent, "Invalid day : " + std::to_string(lastModification.day) + ".");
		// 	FMT_VERIFY(lastModification.hour < 24, PngInvalidChunkContent, "Invalid hour : " + std::to_string(lastModification.hour) + ".");
		// 	FMT_VERIFY(lastModification.minute < 60, PngInvalidChunkContent, "Invalid minute : " + std::to_string(lastModification.minute) + ".");
		// 	FMT_VERIFY(lastModification.second < 61, PngInvalidChunkContent, "Invalid second : " + std::to_string(lastModification.second) + ".");
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_iTXt(const void* ptrChunkHeader, png::TextualData& textualData)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_iTXt(const void* ptrChunkHeader, png::TextualData& textualData)");
		// 
		// 	uint8_t buffer;
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size >= 6, PngInvalidChunkSize, "Expected 'iTXt' chunk size to be at least 6. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer != 0, PngInvalidChunkContent, "Textual data must have a keyword.");
		// 
		// 	while (buffer != 0)
		// 	{
		// 		textualData.keyword.push_back(buffer);
		// 		FMT_READ(buffer);
		// 	}
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer < 2, PngInvalidChunkContent, "Compression flag should be 0 or 1. Instead, got '" + std::to_string(buffer) + "'.");
		// 	textualData.hasCompression = buffer;
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer == 0 || !textualData.hasCompression, PngInvalidChunkContent, "Compression method must be 0. Instead, got " + std::to_string(buffer) + ".");
		// 	textualData.compressionMethod = static_cast<png::CompressionMethod>(buffer);
		// 
		// 	FMT_READ(buffer);
		// 	while (buffer != 0)
		// 	{
		// 		textualData.languageTag.push_back(buffer);
		// 		FMT_READ(buffer);
		// 	}
		// 
		// 	FMT_READ(buffer);
		// 	while (buffer != 0)
		// 	{
		// 		textualData.translatedKeyword.push_back(buffer);
		// 		FMT_READ(buffer);
		// 	}
		// 
		// 	if (textualData.hasCompression)
		// 	{
		// 		zlib::File zlibFile;
		// 		FMT_SUB_CALL(_zlibStream, readFile, zlibFile);
		// 		textualData.text.assign(reinterpret_cast<char*>(zlibFile.data.data()), zlibFile.data.size());
		// 	}
		// 	else
		// 	{
		// 		textualData.text.resize(chunkHeader.size - 5 - textualData.translatedKeyword.size() - textualData.languageTag.size() - textualData.keyword.size());
		// 		FMT_READ(textualData.text.data(), textualData.text.size());
		// 	}
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_tEXt(const void* ptrChunkHeader, png::TextualData& textualData)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_tEXt(const void* ptrChunkHeader, png::TextualData& textualData)");
		// 
		// 	uint8_t buffer;
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size >= 2, PngInvalidChunkSize, "Expected 'tEXt' chunk size to be at least 2. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer != 0, PngInvalidChunkContent, "Textual data must have a keyword.");
		// 
		// 	while (buffer != 0)
		// 	{
		// 		textualData.keyword.push_back(buffer);
		// 		FMT_READ(buffer);
		// 	}
		// 
		// 	textualData.text.resize(chunkHeader.size - textualData.keyword.size() - 1);
		// 	FMT_READ(textualData.text.data(), textualData.text.size());
		// 
		// 	textualData.hasCompression = false;
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunk_zTXt(const void* ptrChunkHeader, png::TextualData& textualData)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunk_zTXt(const void* ptrChunkHeader, png::TextualData& textualData)");
		// 
		// 	uint8_t buffer;
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 
		// 	FMT_VERIFY(chunkHeader.size >= 4, PngInvalidChunkSize, "Expected 'zTXt' chunk size to be at least 4. Instead, got " + std::to_string(chunkHeader.size) + ".");
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer != 0, PngInvalidChunkContent, "Textual data must have a keyword.");
		// 
		// 	while (buffer != 0)
		// 	{
		// 		textualData.keyword.push_back(buffer);
		// 		FMT_READ(buffer);
		// 	}
		// 
		// 	FMT_READ(buffer);
		// 	FMT_VERIFY(buffer == 0 || !textualData.hasCompression, PngInvalidChunkContent, "Compression method must be 0. Instead, got " + std::to_string(buffer) + ".");
		// 	textualData.hasCompression = true;
		// 	textualData.compressionMethod = static_cast<png::CompressionMethod>(buffer);
		// 
		// 	zlib::File zlibFile;
		// 	FMT_SUB_CALL(_zlibStream, readFile, zlibFile);
		// 	textualData.text.assign(reinterpret_cast<char*>(zlibFile.data.data()), zlibFile.data.size());
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readChunkEnding(const void* ptrChunkHeader)
		// {
		// 	FMT_BEGIN("PngIStream::_readChunkEnding(const void* ptrChunkHeader)");
		// 
		// 	constexpr uint64_t bufferSize = 128;
		// 	uint8_t buffer[bufferSize];
		// 	const ChunkHeader& chunkHeader = *reinterpret_cast<const ChunkHeader*>(ptrChunkHeader);
		// 	
		// 	uint64_t size = chunkHeader.size + 4;
		// 	FMT_CALL(setPos, getPos() - std::streamoff(size));
		// 	
		// 	uint32_t computedCrc = 0;
		// 	while (size >= bufferSize)
		// 	{
		// 		FMT_READ(buffer, bufferSize);
		// 		computedCrc = cksm::crc32(buffer, bufferSize, computedCrc);
		// 		size -= bufferSize;
		// 	}
		// 	FMT_READ(buffer, size);
		// 	computedCrc = cksm::crc32(buffer, size, computedCrc);
		// 
		// 	uint32_t readCrc;
		// 	FMT_READ(readCrc);
		// 
		// 	FMT_VERIFY(readCrc == computedCrc, PngInvalidChunkCRC, "Computed CRC and read CRC are different for chunk '" + std::string(chunkHeader.type, 4) + "'.");
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readRawPixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel)
		// {
		// 	FMT_BEGIN("PngIStream::_readRawPixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel)");
		// 
		// 	// Compute "scanline context" (lengths, count, etc...) and check there are pixels left to read
		// 
		// 	bool isLastPass = isLastInterlacePass(_header.imageStructure, _currentPass);
		// 	uint32_t scanlineLength = computeScanlineLength(_header.imageStructure, _currentPass);
		// 	uint32_t scanlineCount = computeScanlineCount(_header.imageStructure, _currentPass);
		// 
		// 	assert(!isLastPixel(_header.imageStructure, _currentPass, _currentScanline, _currentPixel));
		// 
		// 	// Compute if pixels will be read out of the current scanline
		// 
		// 	uint32_t readablePixels = scanlineLength - _currentPixel;
		// 	if (readablePixels < pixelCount)
		// 	{
		// 		_currentPixel = 0;
		// 	}
		// 
		// 	// Read whole scanlines (or, for the first, whole remaining scanline)
		// 
		// 	uint8_t* it = reinterpret_cast<uint8_t*>(pixels);
		// 	while (readablePixels < pixelCount)
		// 	{
		// 		FMT_CALL(_readSameScanlinePixels, it, readablePixels, bytesPerPixel);
		// 
		// 		++_currentScanline;
		// 		if (_currentScanline == scanlineCount)	// If last scanline, search next pass with non-empty scanlines in it
		// 		{
		// 			_currentScanline = 0;
		// 
		// 			do {
		// 				isLastPass = isLastInterlacePass(_header.imageStructure, _currentPass);
		// 				FMT_VERIFY(!isLastPass, PngInvalidChunkContent, "Trying to read more pixels than there are in the picture.");
		// 				++_currentPass;
		// 				scanlineCount = computeScanlineCount(_header.imageStructure, _currentPass);
		// 			} while (scanlineCount == 0);
		// 
		// 			scanlineLength = computeScanlineLength(_header.imageStructure, _currentPass);
		// 			_scanline.resize((scanlineLength + 2) * bytesPerPixel);
		// 			std::fill(_scanline.begin(), _scanline.end(), 0);
		// 			_scanlineIndex = 0;
		// 		}
		// 		else	// If just next scanline in same pass, read a fake "black" pixel for filtering
		// 		{
		// 			for (uint8_t i = 0; i < bytesPerPixel; ++i)
		// 			{
		// 				_scanline[_scanlineIndex] = 0;
		// 				_scanlineIndex = (_scanlineIndex + 1) % _scanline.size();
		// 			}
		// 		}
		// 
		// 		it += readablePixels * bytesPerPixel;
		// 		pixelCount -= readablePixels;
		// 		readablePixels = scanlineLength;
		// 
		// 		// Read next scanline filter
		// 
		// 		FMT_CALL(_readRawData, &_scanlineFilter, 1);
		// 		FMT_VERIFY(_scanlineFilter < 5, PngInvalidChunkContent, "Scanline filter type should be in [0, 4]. Instead, got " + std::to_string(_scanlineFilter) + ".");
		// 	}
		// 
		// 	// Read the last pixels (not finishing a scanline)
		// 
		// 	FMT_CALL(_readSameScanlinePixels, it, pixelCount, bytesPerPixel);
		// 	_currentPixel += pixelCount;
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readSameScanlinePixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel)
		// {
		// 	FMT_BEGIN("PngIStream::_readSameScanlinePixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel)");
		// 
		// 	if (_header.imageStructure.bitDepth < 8)
		// 	{
		// 		FMT_VERIFY(false, NotImplementedYet, "Bit depth less than 8 (GreyScale and IndexedColor) is not implemented yet.");
		// 	}
		// 	else
		// 	{
		// 		const uint64_t byteCount = pixelCount * bytesPerPixel;
		// 
		// 		FMT_CALL(_readRawData, pixels, byteCount);
		// 
		// 		uint8_t* it = reinterpret_cast<uint8_t*>(pixels);
		// 		const uint8_t* const itEnd = it + byteCount;
		// 		for (; it != itEnd; ++it)
		// 		{
		// 			const uint8_t& a = _scanline[(_scanlineIndex + _scanline.size() - bytesPerPixel) % _scanline.size()];
		// 			const uint8_t& b = _scanline[(_scanlineIndex + bytesPerPixel) % _scanline.size()];
		// 			const uint8_t& c = _scanline[_scanlineIndex];
		// 
		// 			switch (_scanlineFilter)
		// 			{
		// 				case 0:
		// 				{
		// 					break;	// Nothing.
		// 				}
		// 				case 1:
		// 				{
		// 					*it += a;
		// 					break;
		// 				}
		// 				case 2:
		// 				{
		// 					*it += b;
		// 					break;
		// 				}
		// 				case 3:
		// 				{
		// 					*it += (a + b) / 2;
		// 					break;
		// 				}
		// 				case 4:
		// 				{
		// 					*it += paethPredictor(a, b, c);
		// 					break;
		// 				}
		// 			}
		// 
		// 			_scanline[_scanlineIndex] = *it;
		// 			_scanlineIndex = (_scanlineIndex + 1) % _scanline.size();
		// 		}
		// 	}
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngIStream::_readRawData(void* data, uint64_t size)
		// {
		// 	FMT_BEGIN("PngIStream::_readRawData(void* data, uint64_t size)");
		// 
		// 	uint64_t sizeRead;
		// 
		// 	// If first pixel read, prepare data stream and read first scanline filter
		// 
		// 	if (_currentPass == 0 && _currentScanline == 0 && _currentPixel == 0)
		// 	{
		// 		ChunkHeader chunkHeader;
		// 		FMT_CALL(_readChunkHeader, &chunkHeader);
		// 		while (fourcc(chunkHeader.type) == fourcc("IDAT"))
		// 		{
		// 			FMT_CALL(_readChunk_IDAT, &chunkHeader);	// The ending is read directly here for obvious performances reasons...
		// 			FMT_CALL(_readChunkHeader, &chunkHeader);
		// 		}
		// 		FMT_CALL(unget, 8);
		// 
		// 		FMT_SUB_CALL(_zlibStream, setSource, _dataStream);
		// 		_dataStream.flush();
		// 
		// 		zlib::Header zlibHeader;
		// 		FMT_SUB_CALL(_zlibStream, readHeader, zlibHeader);
		// 		FMT_VERIFY(!zlibHeader.dictId.has_value(), PngInvalidChunkContent, "ZLIB DICTID for PNG is not supported.");
		// 
		// 		FMT_SUB_CALL(_zlibStream, readData, &_scanlineFilter, 1, sizeRead);
		// 		FMT_VERIFY(sizeRead == 1, PngInvalidChunkContent, "Insufficient data in zlib stream.");
		// 		FMT_VERIFY(_scanlineFilter < 5, PngInvalidChunkContent, "Scanline filter type should be in [0, 4]. Instead, got " + std::to_string(_scanlineFilter) + ".");
		// 
		// 		const uint8_t colorType = static_cast<uint8_t>(_header.imageStructure.colorType);
		// 		const uint32_t bytesPerPixel = 1 + (colorType & 2) + (colorType & 4) / 4;
		// 		const uint32_t scanlineLength = computeScanlineLength(_header.imageStructure, _currentPass);
		// 
		// 		_scanline.resize((scanlineLength + 2) * bytesPerPixel, 0);
		// 	}
		// 
		// 	// Read data
		// 
		// 	FMT_SUB_CALL(_zlibStream, readData, data, size, sizeRead);
		// 	FMT_VERIFY(sizeRead == size, PngInvalidChunkContent, "Insufficient data in zlib stream.");
		// 
		// 	return *_error;
		// }
		// 
		// 
		// PngOStream::PngOStream() : FormatOStream(std::endian::big),
		// 	_zlibStream(nullptr),
		// 	_dataStream(),
		// 	_header(),
		// 	_palette(),
		// 	_headerWritten(false),
		// 	_currentPass(0),
		// 	_currentScanline(0),
		// 	_currentPixel(0),
		// 	_scanline(),
		// 	_scanlineIndex(0),
		// 	_scanlineFilter(0)
		// {
		// 	FMT_CREATE_SUBSTREAM(_zlibStream, ZlibOStream);
		// 
		// 	finishSubStreamTree();
		// }
		// 
		// const ruc::Status& PngOStream::writeHeader(const png::Header& header, const png::Palette* palette)
		// {
		// 	FMT_BEGIN("PngOStream::writeHeader(const png::Header& header, const png::Palette* palette)");
		// 
		// 	assert(!_headerWritten);
		// 
		// 	// Write PNG signature and copy header
		// 
		// 	FMT_WRITE("\x89PNG\r\n\x1A\n", 8);
		// 	_header = header;
		// 
		// 	// Write IHDR chunk
		// 
		// 	FMT_CALL(_writeChunk_IHDR);
		// 
		// 	// Write iCCP, sRGB, cHRM and gAMA chunks
		// 
		// 	if (_header.iccProfile.has_value())
		// 	{
		// 		assert(!_header.srgbIntent.has_value());
		// 		FMT_CALL(_writeChunk_iCCP);
		// 	}
		// 	else if (_header.srgbIntent.has_value())
		// 	{
		// 		FMT_CALL(_writeChunk_sRGB);
		// 
		// 		if (_header.gamma.has_value())
		// 		{
		// 			assert(_header.gamma.value() == 0.45455f);
		// 		}
		// 		else
		// 		{
		// 			_header.gamma.emplace(0.45455f);
		// 		}
		// 
		// 		if (_header.chromaticity.has_value())
		// 		{
		// 			assert(_header.chromaticity->whitePoint[0] == 0.3127f);
		// 			assert(_header.chromaticity->whitePoint[1] == 0.329f);
		// 			assert(_header.chromaticity->redPoint[0] == 0.64f);
		// 			assert(_header.chromaticity->redPoint[1] == 0.33f);
		// 			assert(_header.chromaticity->greenPoint[0] == 0.3f);
		// 			assert(_header.chromaticity->greenPoint[1] == 0.6f);
		// 			assert(_header.chromaticity->bluePoint[0] == 0.15f);
		// 			assert(_header.chromaticity->bluePoint[1] == 0.06f);
		// 		}
		// 		else
		// 		{
		// 			_header.chromaticity.emplace();
		// 
		// 			_header.chromaticity->whitePoint[0] = 0.3127f;
		// 			_header.chromaticity->whitePoint[1] = 0.329f;
		// 			_header.chromaticity->redPoint[0] = 0.64f;
		// 			_header.chromaticity->redPoint[1] = 0.33f;
		// 			_header.chromaticity->greenPoint[0] = 0.3f;
		// 			_header.chromaticity->greenPoint[1] = 0.6f;
		// 			_header.chromaticity->bluePoint[0] = 0.15f;
		// 			_header.chromaticity->bluePoint[1] = 0.06f;
		// 		}
		// 	}
		// 
		// 	if (_header.gamma.has_value())
		// 	{
		// 		FMT_CALL(_writeChunk_gAMA);
		// 	}
		// 
		// 	if (_header.chromaticity.has_value())
		// 	{
		// 		FMT_CALL(_writeChunk_cHRM);
		// 	}
		// 
		// 	// Write sBIT chunk
		// 
		// 	if (_header.significantBits.has_value())
		// 	{
		// 		FMT_CALL(_writeChunk_sBIT);
		// 	}
		// 
		// 	// Write PLTE, hIST and tRNS chunks
		// 
		// 	if (_header.imageStructure.colorType == png::ColorType::IndexedColor)
		// 	{
		// 		assert(palette);
		// 		_palette = *palette;
		// 
		// 		assert(!_header.transparencyMask.has_value());
		// 
		// 		FMT_CALL(_writeChunk_PLTE);
		// 		FMT_CALL(_writeChunk_hIST);
		// 		FMT_CALL(_writeChunk_tRNS);
		// 	}
		// 
		// 	if (_header.transparencyMask.has_value())
		// 	{
		// 		FMT_CALL(_writeChunk_tRNS);
		// 	}
		// 
		// 	// Write bKGD chunk
		// 
		// 	if (_header.backgroundColor.has_value())
		// 	{
		// 		FMT_CALL(_writeChunk_bKGD);
		// 	}
		// 
		// 	// Write pHYs chunk
		// 
		// 	if (_header.pixelDimensions.has_value())
		// 	{
		// 		FMT_CALL(_writeChunk_pHYs);
		// 	}
		// 
		// 	// Write sPLT chunks
		// 
		// 	for (const png::Palette& suggestedPalette : _header.suggestedPalettes)
		// 	{
		// 		FMT_CALL(_writeChunk_sPLT, suggestedPalette);
		// 	}
		// 
		// 	// Write tIME chunk
		// 
		// 	if (_header.lastModification.has_value())
		// 	{
		// 		FMT_CALL(_writeChunk_tIME, _header.lastModification.value());
		// 	}
		// 
		// 	// Write iTXt chunks
		// 
		// 	for (const png::TextualData& textualData : _header.textualDatas)
		// 	{
		// 		FMT_CALL(_writeChunk_iTXt, textualData);
		// 	}
		// 
		// 	// Prepare for pixels writing
		// 
		// 	_headerWritten = true;
		// 
		// 	_scanlineFilter = 1;
		// 	const uint8_t colorType = static_cast<uint8_t>(_header.imageStructure.colorType);
		// 	const uint32_t bytesPerPixel = 1 + (colorType & 2) + (colorType & 4) / 4;
		// 	const uint32_t scanlineLength = computeScanlineLength(_header.imageStructure, _currentPass);
		// 
		// 	_scanline.resize((scanlineLength + 2) * bytesPerPixel, 0);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::writeEnding(const png::Ending& ending)
		// {
		// 	FMT_BEGIN("PngOStream::writeEnding(const png::Ending& ending)");
		// 
		// 	assert(_headerWritten);
		// 	assert(isLastPixel(_header.imageStructure, _currentPass, _currentScanline, _currentPixel));
		// 
		// 	// Write last IDAT chunk
		// 
		// 	FMT_SUB_CALL(_zlibStream, writeEndFile);
		// 	FMT_CALL(_writeChunk_IDAT);
		// 
		// 	// Write tIME chunk if not written yet
		// 
		// 	if (ending.lastModification.has_value())
		// 	{
		// 		assert(!_header.lastModification.has_value());
		// 		FMT_CALL(_writeChunk_tIME, ending.lastModification.value());
		// 	}
		// 
		// 	// Write iTXt chunks
		// 
		// 	for (const png::TextualData& textualData : ending.textualDatas)
		// 	{
		// 		FMT_CALL(_writeChunk_iTXt, textualData);
		// 	}
		// 
		// 	// Write IEND chunk
		// 
		// 	FMT_CALL(_writeChunk_IEND);
		// 
		// 	// Reset for next file
		// 
		// 	_dataStream = std::stringstream();
		// 
		// 	_header = png::Header();
		// 	_palette.entries.clear();
		// 
		// 	_headerWritten = false;
		// 
		// 	_currentPass = 0;
		// 	_currentScanline = 0;
		// 	_currentPixel = 0;
		// 
		// 	_scanline.clear();
		// 	_scanlineIndex = 0;
		// 	_scanlineFilter = 0;
		// 
		// 	return *_error;
		// }
		// 
		// void PngOStream::onDestinationRemoved()
		// {
		// 	_dataStream = std::stringstream();
		// 
		// 	_header = png::Header();
		// 	_palette.entries.clear();
		// 
		// 	_headerWritten = false;
		// 
		// 	_currentPass = 0;
		// 	_currentScanline = 0;
		// 	_currentPixel = 0;
		// 
		// 	_scanline.clear();
		// 	_scanlineIndex = 0;
		// 	_scanlineFilter = 0;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_IHDR()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_IHDR()");
		// 	
		// 	assert(_header.imageStructure.width != 0);
		// 	assert(_header.imageStructure.height != 0);
		// 	assert(isBitDepthColorTypeAllowed(_header.imageStructure.bitDepth, static_cast<uint8_t>(_header.imageStructure.colorType)));
		// 	assert(_header.imageStructure.compressionMethod == png::CompressionMethod::Deflate);
		// 	assert(_header.imageStructure.filterMethod == png::FilterMethod::Default);
		// 
		// 	uint32_t crc = 0;
		// 	constexpr uint32_t chunkSize = 13;
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "IHDR", 4, crc);
		// 
		// 	FMT_CALL(_writeRawMetadata, _header.imageStructure.width, crc);
		// 	FMT_CALL(_writeRawMetadata, _header.imageStructure.height, crc);
		// 	FMT_CALL(_writeRawMetadata, _header.imageStructure.bitDepth, crc);
		// 	FMT_CALL(_writeRawMetadata, static_cast<uint8_t>(_header.imageStructure.colorType), crc);
		// 	FMT_CALL(_writeRawMetadata, static_cast<uint8_t>(_header.imageStructure.compressionMethod), crc);
		// 	FMT_CALL(_writeRawMetadata, static_cast<uint8_t>(_header.imageStructure.filterMethod), crc);
		// 	FMT_CALL(_writeRawMetadata, static_cast<uint8_t>(_header.imageStructure.interlaceMethod), crc);
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_PLTE()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_PLTE()");
		// 
		// 	assert(_palette.entries.size() <= (1 << _header.imageStructure.bitDepth));
		// 
		// 	uint32_t crc = 0;
		// 	const uint32_t chunkSize = 3 * _palette.entries.size();
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "PLTE", 4, crc);
		// 
		// 	for (const png::PaletteEntry& entry : _palette.entries)
		// 	{
		// 		FMT_CALL(_writeRawMetadata, entry.r, crc);
		// 		FMT_CALL(_writeRawMetadata, entry.g, crc);
		// 		FMT_CALL(_writeRawMetadata, entry.b, crc);
		// 	}
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_IDAT()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_IDAT()");
		// 
		// 	_dataStream.flush();
		// 
		// 	uint32_t crc = 0;
		// 	const uint32_t chunkSize = _dataStream.view().size();
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "IDAT", 4, crc);
		// 	FMT_CALL(_writeRawMetadata, _dataStream.view().data(), _dataStream.view().size(), crc);
		// 	FMT_WRITE(crc);
		// 	
		// 	_dataStream = std::stringstream();
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_IEND()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_IEND()");
		// 
		// 	uint32_t crc = 0;
		// 	constexpr uint32_t chunkSize = 0;
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "IEND", 4, crc);
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_cHRM()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_cHRM()");
		// 
		// 	uint32_t crc = 0;
		// 	constexpr uint32_t chunkSize = 32;
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "cHRM", 4, crc);
		// 
		// 	uint32_t buffer[8];
		// 	buffer[0] = _header.chromaticity->whitePoint[0] * 100000;
		// 	buffer[1] = _header.chromaticity->whitePoint[1] * 100000;
		// 	buffer[2] = _header.chromaticity->redPoint[0] * 100000;
		// 	buffer[3] = _header.chromaticity->redPoint[1] * 100000;
		// 	buffer[4] = _header.chromaticity->greenPoint[0] * 100000;
		// 	buffer[5] = _header.chromaticity->greenPoint[1] * 100000;
		// 	buffer[6] = _header.chromaticity->bluePoint[0] * 100000;
		// 	buffer[7] = _header.chromaticity->bluePoint[1] * 100000;
		// 	FMT_CALL(_writeRawMetadata, buffer, 8, crc);
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_gAMA()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_gAMA()");
		// 
		// 	uint32_t crc = 0;
		// 	constexpr uint32_t chunkSize = 4;
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "gAMA", 4, crc);
		// 
		// 	uint32_t gamma = _header.gamma.value() * 100000;
		// 	FMT_CALL(_writeRawMetadata, gamma, crc);
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_iCCP()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_iCCP()");
		// 
		// 	// TODO: check the name (latin-1 characters, no leading/trailing spaces...)
		// 	assert(!_header.iccProfile->name.empty());
		// 	assert(_header.iccProfile->compressionMethod == png::CompressionMethod::Deflate);
		// 
		// 	FMT_SUB_CALL(_zlibStream, setDestination, _dataStream);
		// 	
		// 	zlib::File zlibFile;
		// 	zlibFile.header.compressionMethod = zlib::CompressionMethod::Deflate;
		// 	zlibFile.header.compressionInfo = 15;	// TODO: Compute appropriate window size
		// 	zlibFile.header.compressionLevel = zlib::CompressionLevel::DefaultAlgorithm;
		// 	zlibFile.data = _header.iccProfile->data;
		// 	FMT_SUB_CALL(_zlibStream, writeFile, zlibFile);
		// 
		// 	uint32_t crc = 0;
		// 	const uint32_t chunkSize = _header.iccProfile->name.size() + 2 + _dataStream.view().size();
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "iCCP", 4, crc);
		// 
		// 	FMT_CALL(_writeRawMetadata, _header.iccProfile->name.data(), _header.iccProfile->name.size(), crc);
		// 	FMT_CALL(_writeRawMetadata, '\x00', crc);
		// 	FMT_CALL(_writeRawMetadata, static_cast<uint8_t>(_header.iccProfile->compressionMethod), crc);
		// 	FMT_CALL(_writeRawMetadata, _dataStream.view().data(), _dataStream.view().size(), crc);
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	_dataStream = std::stringstream();
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_sBIT()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_sBIT()");
		// 
		// 	assert(_header.significantBits->grayscaleBits <= _header.imageStructure.bitDepth);
		// 	assert(_header.significantBits->redBits <= _header.imageStructure.bitDepth);
		// 	assert(_header.significantBits->greenBits <= _header.imageStructure.bitDepth);
		// 	assert(_header.significantBits->blueBits <= _header.imageStructure.bitDepth);
		// 	assert(_header.significantBits->alphaBits <= _header.imageStructure.bitDepth);
		// 
		// 	uint32_t crc = 0;
		// 	const uint8_t colorType = static_cast<uint8_t>(_header.imageStructure.colorType);
		// 	const uint32_t chunkSize = 1 + (colorType & 2) + (colorType & 4) / 4;
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "sBIT", 4, crc);
		// 
		// 	switch (_header.imageStructure.colorType)
		// 	{
		// 		case png::ColorType::GreyScale:
		// 		{
		// 			FMT_CALL(_writeRawMetadata, _header.significantBits->grayscaleBits, crc);
		// 			break;
		// 		}
		// 		case png::ColorType::GreyScaleAlpha:
		// 		{
		// 			FMT_CALL(_writeRawMetadata, _header.significantBits->grayscaleBits, crc);
		// 			FMT_CALL(_writeRawMetadata, _header.significantBits->alphaBits, crc);
		// 			break;
		// 		}
		// 		case png::ColorType::IndexedColor:
		// 		case png::ColorType::TrueColor:
		// 		{
		// 			FMT_CALL(_writeRawMetadata, _header.significantBits->redBits, crc);
		// 			FMT_CALL(_writeRawMetadata, _header.significantBits->greenBits, crc);
		// 			FMT_CALL(_writeRawMetadata, _header.significantBits->blueBits, crc);
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColorAlpha:
		// 		{
		// 			FMT_CALL(_writeRawMetadata, _header.significantBits->redBits, crc);
		// 			FMT_CALL(_writeRawMetadata, _header.significantBits->greenBits, crc);
		// 			FMT_CALL(_writeRawMetadata, _header.significantBits->blueBits, crc);
		// 			FMT_CALL(_writeRawMetadata, _header.significantBits->alphaBits, crc);
		// 			break;
		// 		}
		// 	}
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_sRGB()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_sRGB()");
		// 
		// 	uint32_t crc = 0;
		// 	constexpr uint32_t chunkSize = 1;
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "sRGB", 4, crc);
		// 
		// 	FMT_CALL(_writeRawMetadata, static_cast<uint8_t>(_header.srgbIntent.value()), crc);
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_bKGD()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_bKGD()");
		// 	
		// 	uint32_t crc = 0;
		// 
		// 	switch (_header.imageStructure.colorType)
		// 	{
		// 		case png::ColorType::GreyScale:
		// 		case png::ColorType::GreyScaleAlpha:
		// 		{
		// 			assert(_header.backgroundColor->r == _header.backgroundColor->g);
		// 			assert(_header.backgroundColor->r == _header.backgroundColor->b);
		// 
		// 			constexpr uint32_t chunkSize = 2;
		// 			FMT_WRITE(chunkSize);
		// 			FMT_CALL(_writeRawMetadata, "bKGD", 4, crc);
		// 			
		// 			FMT_CALL(_writeRawMetadata, _header.backgroundColor->r, crc);
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColor:
		// 		case png::ColorType::TrueColorAlpha:
		// 		{
		// 			constexpr uint32_t chunkSize = 6;
		// 			FMT_WRITE(chunkSize);
		// 			FMT_CALL(_writeRawMetadata, "bKGD", 4, crc);
		// 
		// 			FMT_CALL(_writeRawMetadata, _header.backgroundColor->r, crc);
		// 			FMT_CALL(_writeRawMetadata, _header.backgroundColor->g, crc);
		// 			FMT_CALL(_writeRawMetadata, _header.backgroundColor->b, crc);
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::IndexedColor:
		// 		{
		// 			constexpr uint32_t chunkSize = 1;
		// 			FMT_WRITE(chunkSize);
		// 			FMT_CALL(_writeRawMetadata, "bKGD", 4, crc);
		// 
		// 			auto it = _palette.entries.cbegin();
		// 			const auto itEnd = _palette.entries.cend();
		// 			for (uint8_t i = 0; it != itEnd; ++i, ++it)
		// 			{
		// 				if (it->r == _header.backgroundColor->r
		// 					&& it->g == _header.backgroundColor->g
		// 					&& it->b == _header.backgroundColor->b)
		// 				{
		// 					FMT_CALL(_writeRawMetadata, i, crc);
		// 					break;
		// 				}
		// 			}
		// 
		// 			assert(it != itEnd);
		// 
		// 			break;
		// 		}
		// 	}
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_hIST()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_hIST()");
		// 
		// 	uint32_t crc = 0;
		// 	const uint32_t chunkSize = 2 * _palette.entries.size();
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "hIST", 4, crc);
		// 
		// 	for (const png::PaletteEntry& entry : _palette.entries)
		// 	{
		// 		FMT_CALL(_writeRawMetadata, entry.frequency, crc);
		// 	}
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_tRNS()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_tRNS()");
		// 	
		// 	uint32_t crc = 0;
		// 
		// 	switch (_header.imageStructure.colorType)
		// 	{
		// 		case png::ColorType::GreyScale:
		// 		{
		// 			assert(_header.transparencyMask->r == _header.transparencyMask->g);
		// 			assert(_header.transparencyMask->r == _header.transparencyMask->b);
		// 
		// 			constexpr uint32_t chunkSize = 2;
		// 			FMT_WRITE(chunkSize);
		// 			FMT_CALL(_writeRawMetadata, "tRNS", 4, crc);
		// 			
		// 			FMT_CALL(_writeRawMetadata, _header.transparencyMask->r, crc);
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColor:
		// 		{
		// 			constexpr uint32_t chunkSize = 6;
		// 			FMT_WRITE(chunkSize);
		// 			FMT_CALL(_writeRawMetadata, "tRNS", 4, crc);
		// 
		// 			FMT_CALL(_writeRawMetadata, _header.transparencyMask->r, crc);
		// 			FMT_CALL(_writeRawMetadata, _header.transparencyMask->g, crc);
		// 			FMT_CALL(_writeRawMetadata, _header.transparencyMask->b, crc);
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::IndexedColor:
		// 		{
		// 			const uint32_t chunkSize = _palette.entries.size();
		// 			FMT_WRITE(chunkSize);
		// 			FMT_CALL(_writeRawMetadata, "tRNS", 4, crc);
		// 
		// 			for (const png::PaletteEntry& entry : _palette.entries)
		// 			{
		// 				FMT_CALL(_writeRawMetadata, entry.a, crc);
		// 			}
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::GreyScaleAlpha:
		// 		case png::ColorType::TrueColorAlpha:
		// 		{
		// 			assert(false);
		// 			break;
		// 		}
		// 	}
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_pHYs()
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_pHYs()");
		// 
		// 	assert(_header.pixelDimensions->xPixelsPerUnit != 0);
		// 	assert(_header.pixelDimensions->yPixelsPerUnit != 0);
		// 
		// 	uint32_t crc = 0;
		// 	constexpr uint32_t chunkSize = 9;
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "pHYs", 4, crc);
		// 
		// 	FMT_CALL(_writeRawMetadata, _header.pixelDimensions->xPixelsPerUnit, crc);
		// 	FMT_CALL(_writeRawMetadata, _header.pixelDimensions->yPixelsPerUnit, crc);
		// 	FMT_CALL(_writeRawMetadata, static_cast<uint8_t>(_header.pixelDimensions->unit), crc);
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_sPLT(const png::Palette& palette)
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_sPLT(const png::Palette& palette)");
		// 
		// 	// TODO: Checks on name, entries, etc...
		// 	assert(!palette.name.empty());
		// 
		// 	uint32_t crc = 0;
		// 	const uint32_t chunkSize = palette.name.size() + 2 + 6 * palette.entries.size();
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "sPLT", 4, crc);
		// 
		// 	FMT_CALL(_writeRawMetadata, palette.name.data(), palette.name.size(), crc);
		// 	FMT_CALL(_writeRawMetadata, '\x00', crc);
		// 	FMT_CALL(_writeRawMetadata, '\x08', crc);
		// 
		// 	for (const png::PaletteEntry& entry : palette.entries)
		// 	{
		// 		FMT_CALL(_writeRawMetadata, entry.r, crc);
		// 		FMT_CALL(_writeRawMetadata, entry.g, crc);
		// 		FMT_CALL(_writeRawMetadata, entry.b, crc);
		// 		FMT_CALL(_writeRawMetadata, entry.a, crc);
		// 		FMT_CALL(_writeRawMetadata, entry.frequency, crc);
		// 	}
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_tIME(const png::LastModification& lastModification)
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_tIME(const png::LastModification& lastModification)");
		// 
		// 	assert(lastModification.month != 0 && lastModification.month < 13);
		// 	assert(lastModification.day != 0 && lastModification.day < 32);
		// 	assert(lastModification.hour < 24);
		// 	assert(lastModification.minute < 60);
		// 	assert(lastModification.second < 61);
		// 
		// 	uint32_t crc = 0;
		// 	constexpr uint32_t chunkSize = 9;
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "tIME", 4, crc);
		// 
		// 	FMT_CALL(_writeRawMetadata, lastModification.year, crc);
		// 	FMT_CALL(_writeRawMetadata, lastModification.month, crc);
		// 	FMT_CALL(_writeRawMetadata, lastModification.day, crc);
		// 	FMT_CALL(_writeRawMetadata, lastModification.hour, crc);
		// 	FMT_CALL(_writeRawMetadata, lastModification.minute, crc);
		// 	FMT_CALL(_writeRawMetadata, lastModification.second, crc);
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeChunk_iTXt(const png::TextualData& textualData)
		// {
		// 	FMT_BEGIN("PngOStream::_writeChunk_iTXt(const png::TextualData& textualData)");
		// 
		// 	assert(!textualData.keyword.empty());
		// 
		// 	if (textualData.hasCompression)
		// 	{
		// 		zlib::File zlibFile;
		// 		zlibFile.header.compressionMethod = zlib::CompressionMethod::Deflate;
		// 		zlibFile.header.compressionInfo = 15;	// TODO: Compute appropriate window size
		// 		zlibFile.header.compressionLevel = zlib::CompressionLevel::DefaultAlgorithm;
		// 		zlibFile.data = std::vector<uint8_t>(textualData.text.begin(), textualData.text.end());
		// 
		// 		FMT_SUB_CALL(_zlibStream, setDestination, _dataStream);
		// 		FMT_SUB_CALL(_zlibStream, writeFile, zlibFile);
		// 	}
		// 	else
		// 	{
		// 		_dataStream.write(textualData.text.data(), textualData.text.size());
		// 	}
		// 
		// 	uint32_t crc = 0;
		// 	const uint32_t chunkSize = 5 + textualData.keyword.size() + textualData.languageTag.size() + textualData.translatedKeyword.size() + _dataStream.view().size();
		// 	FMT_WRITE(chunkSize);
		// 	FMT_CALL(_writeRawMetadata, "iTXt", 4, crc);
		// 
		// 	FMT_CALL(_writeRawMetadata, textualData.keyword.data(), textualData.keyword.size(), crc);
		// 	FMT_CALL(_writeRawMetadata, '\x00', crc);
		// 	FMT_CALL(_writeRawMetadata, static_cast<uint8_t>(textualData.hasCompression), crc);
		// 	FMT_CALL(_writeRawMetadata, static_cast<uint8_t>(textualData.compressionMethod), crc);
		// 	FMT_CALL(_writeRawMetadata, textualData.languageTag.data(), textualData.languageTag.size(), crc);
		// 	FMT_CALL(_writeRawMetadata, '\x00', crc);
		// 	FMT_CALL(_writeRawMetadata, textualData.translatedKeyword.data(), textualData.translatedKeyword.size(), crc);
		// 	FMT_CALL(_writeRawMetadata, '\x00', crc);
		// 	FMT_CALL(_writeRawMetadata, _dataStream.view().data(), _dataStream.view().size(), crc);
		// 
		// 	FMT_WRITE(crc);
		// 
		// 	_dataStream = std::stringstream();
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeRawPixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel)
		// {
		// 	FMT_BEGIN("PngOStream::_writeRawPixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel)");
		// 	
		// 	// Compute "scanline context" (lengths, count, etc...) and check there are pixels left to write
		// 
		// 	bool isLastPass = isLastInterlacePass(_header.imageStructure, _currentPass);
		// 	uint32_t scanlineLength = computeScanlineLength(_header.imageStructure, _currentPass);
		// 	uint32_t scanlineCount = computeScanlineCount(_header.imageStructure, _currentPass);
		// 
		// 	assert(!isLastPixel(_header.imageStructure, _currentPass, _currentScanline, _currentPixel));
		// 
		// 	// Compute if pixels will be written out of the current scanline
		// 
		// 	uint32_t writablePixels = scanlineLength - _currentPixel;
		// 	if (writablePixels < pixelCount)
		// 	{
		// 		_currentPixel = 0;
		// 	}
		// 
		// 	// Write whole scanlines (or, for the first, whole remaining scanline)
		// 
		// 	uint8_t* it = reinterpret_cast<uint8_t*>(pixels);
		// 	while (writablePixels < pixelCount)
		// 	{
		// 		FMT_CALL(_writeSameScanlinePixels, it, writablePixels, bytesPerPixel);
		// 
		// 		++_currentScanline;
		// 		if (_currentScanline == scanlineCount)	// If last scanline, search next pass with non-empty scanlines in it
		// 		{
		// 			_currentScanline = 0;
		// 
		// 			do {
		// 				isLastPass = isLastInterlacePass(_header.imageStructure, _currentPass);
		// 				FMT_VERIFY(!isLastPass, PngInvalidChunkContent, "Trying to read more pixels than there are in the picture.");
		// 				++_currentPass;
		// 				scanlineCount = computeScanlineCount(_header.imageStructure, _currentPass);
		// 			} while (scanlineCount == 0);
		// 
		// 			scanlineLength = computeScanlineLength(_header.imageStructure, _currentPass);
		// 			_scanline.resize((scanlineLength + 2) * bytesPerPixel);
		// 			std::fill(_scanline.begin(), _scanline.end(), 0);
		// 			_scanlineIndex = 0;
		// 		}
		// 		else	// If just next scanline in same pass, write a fake "black" pixel for filtering
		// 		{
		// 			for (uint8_t i = 0; i < bytesPerPixel; ++i)
		// 			{
		// 				_scanline[_scanlineIndex] = 0;
		// 				_scanlineIndex = (_scanlineIndex + 1) % _scanline.size();
		// 			}
		// 		}
		// 
		// 		it += writablePixels * bytesPerPixel;
		// 		pixelCount -= writablePixels;
		// 		writablePixels = scanlineLength;
		// 
		// 		// Write next scanline filter
		// 
		// 		_scanlineFilter = 4;
		// 		FMT_CALL(_writeRawData, &_scanlineFilter, 1);
		// 	}
		// 
		// 	// Write the last pixels (not finishing a scanline)
		// 
		// 	FMT_CALL(_writeSameScanlinePixels, it, pixelCount, bytesPerPixel);
		// 	_currentPixel += pixelCount;
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeSameScanlinePixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel)
		// {
		// 	FMT_BEGIN("PngOStream::_writeSameScanlinePixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel)");
		// 
		// 	if (_header.imageStructure.bitDepth < 8)
		// 	{
		// 		FMT_VERIFY(false, NotImplementedYet, "Bit depth less than 8 (GreyScale and IndexedColor) is not implemented yet.");
		// 	}
		// 	else
		// 	{
		// 		const uint64_t byteCount = pixelCount * bytesPerPixel;
		// 
		// 		uint8_t* it = reinterpret_cast<uint8_t*>(pixels);
		// 		const uint8_t* const itEnd = it + byteCount;
		// 		for (; it != itEnd; ++it)
		// 		{
		// 			const uint8_t& a = _scanline[(_scanlineIndex + _scanline.size() - bytesPerPixel) % _scanline.size()];
		// 			const uint8_t& b = _scanline[(_scanlineIndex + bytesPerPixel) % _scanline.size()];
		// 			const uint8_t c = _scanline[_scanlineIndex];
		// 
		// 			_scanline[_scanlineIndex] = *it;
		// 			_scanlineIndex = (_scanlineIndex + 1) % _scanline.size();
		// 
		// 			switch (_scanlineFilter)
		// 			{
		// 				case 0:
		// 				{
		// 					break;	// Nothing.
		// 				}
		// 				case 1:
		// 				{
		// 					*it -= a;
		// 					break;
		// 				}
		// 				case 2:
		// 				{
		// 					*it -= b;
		// 					break;
		// 				}
		// 				case 3:
		// 				{
		// 					*it -= (a + b) / 2;
		// 					break;
		// 				}
		// 				case 4:
		// 				{
		// 					*it -= paethPredictor(a, b, c);
		// 					break;
		// 				}
		// 			}
		// 
		// 		}
		// 
		// 		FMT_CALL(_writeRawData, pixels, byteCount);
		// 	}
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& PngOStream::_writeRawData(void* data, uint64_t size)
		// {
		// 	FMT_BEGIN("PngOStream::_writeRawData(void* data, uint64_t size)");
		// 
		// 	// If first pixel written, prepare data stream and write first scanline filter
		// 
		// 	if (_currentPass == 0 && _currentScanline == 0 && _currentPixel == 0)
		// 	{
		// 		FMT_SUB_CALL(_zlibStream, setDestination, _dataStream);
		// 
		// 		zlib::Header zlibHeader;
		// 		zlibHeader.compressionMethod = zlib::CompressionMethod::Deflate;
		// 		zlibHeader.compressionInfo = 7;
		// 		zlibHeader.compressionLevel = zlib::CompressionLevel::FastAlgorithm;
		// 
		// 		// TODO: Better compression
		// 		deflate::BlockHeader deflateHeader;
		// 		deflateHeader.isFinal = true;
		// 		deflateHeader.compressionType = deflate::CompressionType::FixedHuffman;	// TODO: Dynamic
		// 		deflateHeader.litlenCodeLengths.fill(0);
		// 		deflateHeader.litlenCodeLengths[0] = 1;
		// 		deflateHeader.litlenCodeLengths[1] = 4;
		// 		for (uint16_t i = 2; i < 255; ++i)
		// 		{
		// 			deflateHeader.litlenCodeLengths[i] = 10;
		// 		}
		// 		deflateHeader.litlenCodeLengths[255] = 4;
		// 		deflateHeader.litlenCodeLengths[256] = 15;
		// 		// for (uint16_t i = 0; i < 144; ++i)
		// 		// {
		// 		// 	deflateHeader.litlenCodeLengths[i] = 8;
		// 		// }
		// 		// for (uint16_t i = 144; i < 256; ++i)
		// 		// {
		// 		// 	deflateHeader.litlenCodeLengths[i] = 9;
		// 		// }
		// 		// for (uint16_t i = 256; i < 280; ++i)
		// 		// {
		// 		// 	deflateHeader.litlenCodeLengths[i] = 7;
		// 		// }
		// 		// for (uint16_t i = 280; i < 288; ++i)
		// 		// {
		// 		// 	deflateHeader.litlenCodeLengths[i] = 8;
		// 		// }
		// 		deflateHeader.distCodeLengths.fill(5);
		// 
		// 		FMT_SUB_CALL(_zlibStream, writeHeader, zlibHeader, deflateHeader);
		// 
		// 		FMT_SUB_CALL(_zlibStream, writeData, &_scanlineFilter, 1);
		// 	}
		// 
		// 	// Write data
		// 
		// 	FMT_SUB_CALL(_zlibStream, writeData, data, size);
		// 
		// 	// Create IDAT chunk if enough data written
		// 
		// 	if (_dataStream.view().size() >= 65536)
		// 	{
		// 		FMT_CALL(_writeChunk_IDAT);
		// 	}
		// 
		// 	return *_error;
		// }
	}
}
