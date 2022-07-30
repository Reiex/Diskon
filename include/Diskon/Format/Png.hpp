#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace png
		{
			enum class ColorType : uint8_t
			{
				GreyScale		= 0,
				TrueColor		= 2,
				IndexedColor	= 3,
				GreyScaleAlpha	= 4,
				TrueColorAlpha	= 6,
			};

			enum class InterlaceMethod : uint8_t
			{
				NoInterlace	= 0,
				Adam7		= 1
			};

			struct ImageStructure
			{
				uint32_t width;
				uint32_t height;
				uint8_t bitDepth;
				ColorType colorType;
				InterlaceMethod interlaceMethod;
			};

			struct TransparencyData
			{
				std::vector<uint8_t> palette;
				std::array<uint16_t, 3> colorMask;
				uint16_t greyMask;
			};

			struct ColorSpace
			{
				std::array<float, 2> whitePoint;
				std::array<float, 2> redPoint;
				std::array<float, 2> greenPoint;
				std::array<float, 2> bluePoint;
			};

			enum class SRGBIntent : uint8_t
			{
				Perceptual,
				RelativeColorimetric,
				Saturation,
				AbsoluteColorimetric
			};

			struct Header
			{
				ImageStructure imageStructure;
				std::vector<std::array<uint8_t, 3>> palette;
				std::optional<TransparencyData> transparency;
				std::optional<ColorSpace> colorSpace;
				std::optional<float> gamma;
				// TODO: ICC Profile
				std::optional<SRGBIntent> sRGBIntent;
			};

			struct File
			{
				Header header;

				std::vector<uint8_t> rawData;
			};
		}

		class PngStream : public FormatStream
		{
			public:

				PngStream();
				PngStream(const PngStream& stream) = default;
				PngStream(PngStream&& stream) = default;

				PngStream& operator=(const PngStream& stream) = default;
				PngStream& operator=(PngStream&& stream) = default;

				const FormatError& readFile(png::File& file);
				const FormatError& readHeader(png::Header& header);

				const FormatError& writeFile(const png::File& file);
				const FormatError& writeHeader(const png::Header& header);
				const FormatError& writeImageStructure(const png::ImageStructure& imageStructure);
				const FormatError& writeColorSpace(const png::ColorSpace& colorSpace);
				const FormatError& writeGamma(float gamma);
				const FormatError& writeSRBGIntent(const png::SRGBIntent& sRGBIntent);

				~PngStream() = default;

			private:

				const FormatError& readChunkHeader(void* header);

				const FormatError& readChunk_IHDR(png::Header& header, void* ptr);
				const FormatError& readChunk_PLTE(png::Header& header, void* ptr);
				const FormatError& readChunk_IDAT(std::vector<uint8_t>& data, void* ptr);
				const FormatError& readChunk_IEND(png::Header& header, void* ptr);
				const FormatError& readChunk_tRNS(png::Header& header, void* ptr);
				const FormatError& readChunk_cHRM(png::Header& header, void* ptr);
				const FormatError& readChunk_gAMA(png::Header& header, void* ptr);
				// const FormatError& readChunk_iCCP()
				// const FormatError& readChunk_sBIT()
				const FormatError& readChunk_sRGB(png::Header& header, void* ptr);
				// const FormatError& readChunk_tEXT()
				// const FormatError& readChunk_zTXt()
				// const FormatError& readChunk_iTXt()
				// const FormatError& readChunk_bKGD()
				// const FormatError& readChunk_hIST()
				// const FormatError& readChunk_pHYs()
				// const FormatError& readChunk_sPLT()
				// const FormatError& readChunk_tIME()

				const FormatError& readAndCheckChunkCrc(void* ptr);

				const FormatError& checkChunksFound(const std::vector<uint32_t>& chunksFound);
		};
	}
}
