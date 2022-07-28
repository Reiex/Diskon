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
				std::optional<ColorSpace> colorSpace;
				std::optional<float> gamma;
				// TODO: ICC Profile
				std::optional<SRGBIntent> sRGBIntent;
			};

			struct File
			{
				Header header;
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
		};
	}
}
