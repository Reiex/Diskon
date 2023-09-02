///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Reiex
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Format/FormatTypes.hpp>

namespace dsk
{
	namespace fmt
	{
		// namespace png
		// {
		// 	enum class ColorType : uint8_t
		// 	{
		// 		GreyScale		= 0x00,
		// 		TrueColor		= 0x02,
		// 		IndexedColor	= 0x03,
		// 		GreyScaleAlpha	= 0x04,
		// 		TrueColorAlpha	= 0x06,
		// 	};
		// 
		// 	enum class CompressionMethod : uint8_t
		// 	{
		// 		Deflate	= 0x00
		// 	};
		// 
		// 	enum class FilterMethod : uint8_t
		// 	{
		// 		Default = 0x00
		// 	};
		// 
		// 	enum class InterlaceMethod : uint8_t
		// 	{
		// 		NoInterlace	= 0x00,
		// 		Adam7		= 0x01
		// 	};
		// 
		// 	struct ImageStructure
		// 	{
		// 		uint32_t width;
		// 		uint32_t height;
		// 		uint8_t bitDepth;
		// 		ColorType colorType;
		// 		CompressionMethod compressionMethod;
		// 		FilterMethod filterMethod;
		// 		InterlaceMethod interlaceMethod;
		// 	};
		// 
		// 	struct Chromaticity
		// 	{
		// 		std::array<float, 2> whitePoint;
		// 		std::array<float, 2> redPoint;
		// 		std::array<float, 2> greenPoint;
		// 		std::array<float, 2> bluePoint;
		// 	};
		// 
		// 	struct ICCProfile
		// 	{
		// 		std::string name;
		// 		CompressionMethod compressionMethod;
		// 		std::vector<uint8_t> data;	// TODO: Handle ICC file format and replace this by "icc::File"
		// 	};
		// 
		// 	struct SignificantBits
		// 	{
		// 		uint8_t grayscaleBits;
		// 		uint8_t redBits;
		// 		uint8_t greenBits;
		// 		uint8_t blueBits;
		// 		uint8_t alphaBits;
		// 	};
		// 
		// 	enum class SRGBIntent : uint8_t
		// 	{
		// 		Perceptual				= 0x00,
		// 		RelativeColorimetric	= 0x01,
		// 		Saturation				= 0x02,
		// 		AbsoluteColorimetric	= 0x03
		// 	};
		// 
		// 	struct BackgroundColor
		// 	{
		// 		uint16_t r, g, b;
		// 	};
		// 
		// 	union TransparencyMask
		// 	{
		// 		uint16_t r, g, b;
		// 	};
		// 
		// 	enum class PixelDimensionUnit : uint8_t
		// 	{
		// 		Unknown	= 0x00,
		// 		Meter	= 0x01
		// 	};
		// 
		// 	struct PhysicalPixelDimensions
		// 	{
		// 		uint32_t xPixelsPerUnit;
		// 		uint32_t yPixelsPerUnit;
		// 		PixelDimensionUnit unit;
		// 	};
		// 
		// 	struct PaletteEntry
		// 	{
		// 		uint8_t r, g, b, a;
		// 		uint16_t frequency;
		// 	};
		// 
		// 	struct Palette
		// 	{
		// 		std::string name;
		// 		std::vector<PaletteEntry> entries;
		// 	};
		// 
		// 	struct LastModification
		// 	{
		// 		uint16_t year;
		// 		uint8_t month;
		// 		uint8_t day;
		// 		uint8_t hour;
		// 		uint8_t minute;
		// 		uint8_t second;
		// 	};
		// 
		// 	struct TextualData
		// 	{
		// 		std::string keyword;
		// 		std::string languageTag;
		// 		std::string translatedKeyword;
		// 		std::string text;
		// 		bool hasCompression;
		// 		CompressionMethod compressionMethod;
		// 	};
		// 	
		// 	struct Header
		// 	{
		// 		ImageStructure imageStructure;
		// 
		// 		std::optional<Chromaticity> chromaticity;
		// 		std::optional<float> gamma;
		// 		std::optional<ICCProfile> iccProfile;
		// 		std::optional<SignificantBits> significantBits;
		// 		std::optional<SRGBIntent> srgbIntent;
		// 		std::optional<BackgroundColor> backgroundColor;
		// 		std::optional<TransparencyMask> transparencyMask;
		// 		std::optional<PhysicalPixelDimensions> pixelDimensions;
		// 		std::vector<Palette> suggestedPalettes;
		// 		std::optional<LastModification> lastModification;
		// 		std::vector<TextualData> textualDatas;
		// 	};
		// 
		// 	struct Ending
		// 	{
		// 		std::optional<LastModification> lastModification;
		// 		std::vector<TextualData> textualDatas;
		// 	};
		// 
		// 	template<typename TSample>
		// 	struct File
		// 	{
		// 		Header header;
		// 		std::vector<TSample> pixels;	// 4 samples per pixel (no matter the image structure), but order of pixels depend on interlace method
		// 		Ending ending;
		// 		
		// 		// TODO: methods about interlacing
		// 		// TODO: methods to find the best header from pixels
		// 		// TODO: methods to compute palette from pixels
		// 		// TODO: method to apply transparency mask
		// 		// TODO: method to apply background color
		// 	};
		// }
		// 
		// class PngIStream : public FormatIStream
		// {
		// 	public:
		// 
		// 		PngIStream();
		// 		PngIStream(const PngIStream& stream) = delete;
		// 		PngIStream(PngIStream&& stream) = delete;
		// 
		// 		PngIStream& operator=(const PngIStream& stream) = delete;
		// 		PngIStream& operator=(PngIStream&& stream) = delete;
		// 
		// 		template<typename TSample> const ruc::Status& readFile(png::File<TSample>& file);
		// 		const ruc::Status& readHeader(png::Header& header);
		// 		template<typename TSample> const ruc::Status& readPixels(TSample* samples, uint64_t pixelCount);
		// 		const ruc::Status& readEnding(png::Ending& ending);
		// 
		// 		~PngIStream() = default;
		// 
		// 	private:
		// 
		// 		void onSourceRemoved() override final;
		// 
		// 		const ruc::Status& _readChunkHeader(void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_IHDR(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_PLTE(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_IDAT(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_IEND(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_cHRM(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_gAMA(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_iCCP(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_sBIT(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_sRGB(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_bKGD(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_hIST(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_tRNS(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_pHYs(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_sPLT(const void* ptrChunkHeader);
		// 		const ruc::Status& _readChunk_tIME(const void* ptrChunkHeader, png::LastModification& lastModification);
		// 		const ruc::Status& _readChunk_iTXt(const void* ptrChunkHeader, png::TextualData& textualData);
		// 		const ruc::Status& _readChunk_tEXt(const void* ptrChunkHeader, png::TextualData& textualData);
		// 		const ruc::Status& _readChunk_zTXt(const void* ptrChunkHeader, png::TextualData& textualData);
		// 		const ruc::Status& _readChunkEnding(const void* ptrChunkHeader);
		// 
		// 		const ruc::Status& _readRawPixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel);
		// 		const ruc::Status& _readSameScanlinePixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel);
		// 		const ruc::Status& _readRawData(void* data, uint64_t size);
		// 
		// 		ZlibIStream* _zlibStream;
		// 		std::stringstream _dataStream;
		// 
		// 		png::Header _header;
		// 		png::Palette _palette;
		// 
		// 		bool _headerRead;
		// 
		// 		uint8_t _currentPass;
		// 		uint32_t _currentScanline;
		// 		uint32_t _currentPixel;
		// 
		// 		std::vector<uint8_t> _scanline;
		// 		uint32_t _scanlineIndex;
		// 		uint8_t _scanlineFilter;
		// };
		// 
		// class PngOStream : public FormatOStream
		// {
		// 	public:
		// 
		// 		PngOStream();
		// 		PngOStream(const PngOStream& stream) = delete;
		// 		PngOStream(PngOStream&& stream) = delete;
		// 
		// 		PngOStream& operator=(const PngOStream& stream) = delete;
		// 		PngOStream& operator=(PngOStream&& stream) = delete;
		// 
		// 		template<typename TSample> const ruc::Status& writeFile(const png::File<TSample>& file, const png::Palette* palette = nullptr);
		// 		const ruc::Status& writeHeader(const png::Header& header, const png::Palette* palette = nullptr);
		// 		template<typename TSample> const ruc::Status& writePixels(const TSample* samples, uint64_t pixelCount);
		// 		const ruc::Status& writeEnding(const png::Ending& ending);
		// 
		// 		~PngOStream() = default;
		// 
		// 	private:
		// 
		// 		void onDestinationRemoved() override final;
		// 
		// 		const ruc::Status& _writeChunk_IHDR();
		// 		const ruc::Status& _writeChunk_PLTE();
		// 		const ruc::Status& _writeChunk_IDAT();
		// 		const ruc::Status& _writeChunk_IEND();
		// 		const ruc::Status& _writeChunk_cHRM();
		// 		const ruc::Status& _writeChunk_gAMA();
		// 		const ruc::Status& _writeChunk_iCCP();
		// 		const ruc::Status& _writeChunk_sBIT();
		// 		const ruc::Status& _writeChunk_sRGB();
		// 		const ruc::Status& _writeChunk_bKGD();
		// 		const ruc::Status& _writeChunk_hIST();
		// 		const ruc::Status& _writeChunk_tRNS();
		// 		const ruc::Status& _writeChunk_pHYs();
		// 		const ruc::Status& _writeChunk_sPLT(const png::Palette& palette);
		// 		const ruc::Status& _writeChunk_tIME(const png::LastModification& lastModification);
		// 		const ruc::Status& _writeChunk_iTXt(const png::TextualData& textualData);
		// 		template<typename TValue> const ruc::Status& _writeRawMetadata(const TValue& value, uint32_t& crc);
		// 		template<typename TValue> const ruc::Status& _writeRawMetadata(const TValue* values, uint32_t count, uint32_t& crc);
		// 
		// 		const ruc::Status& _writeRawPixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel);
		// 		const ruc::Status& _writeSameScanlinePixels(void* pixels, uint64_t pixelCount, uint8_t bytesPerPixel);
		// 		const ruc::Status& _writeRawData(void* data, uint64_t size);
		// 
		// 		ZlibOStream* _zlibStream;
		// 		std::stringstream _dataStream;
		// 
		// 		png::Header _header;
		// 		png::Palette _palette;
		// 
		// 		bool _headerWritten;
		// 
		// 		uint8_t _currentPass;
		// 		uint32_t _currentScanline;
		// 		uint32_t _currentPixel;
		// 
		// 		std::vector<uint8_t> _scanline;
		// 		uint32_t _scanlineIndex;
		// 		uint8_t _scanlineFilter;
		// };
	}
}
