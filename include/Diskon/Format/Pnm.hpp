///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author P�l�grin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Format/FormatTypes.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace pnm
		{
			enum class Format
			{
				PlainPBM	= 1,
				PlainPGM	= 2,
				PlainPPM	= 3,
				RawPBM		= 4,
				RawPGM		= 5,
				RawPPM		= 6
			};

			struct Header
			{
				Format format;
				uint64_t width;
				uint64_t height;
				std::optional<uint16_t> maxSampleVal;
			};

			struct File
			{
				Header header;
				std::vector<uint16_t> samples;
			};
		}

		class DSK_API PnmIStream : public FormatIStream
		{
			public:

				PnmIStream(IStream* stream);
				PnmIStream(const PnmIStream& stream) = delete;
				PnmIStream(PnmIStream&& stream) = delete;

				PnmIStream& operator=(const PnmIStream& stream) = delete;
				PnmIStream& operator=(PnmIStream&& stream) = delete;

				void readFile(pnm::File& file);
				void readHeader(pnm::Header& header);
				void readPixels(uint16_t* samples, uint64_t pixelCount);

				~PnmIStream() = default;

			private:

				void setStreamState() override final;
				void resetFormatState() override final;

				void _readHeaderChar(char& value);
				void _readHeaderSpaces();


				pnm::Header _header;

				bool _headerRead;
				uint8_t _samplesPerPixel;
				uint64_t _remainingSamples;
		};

		class DSK_API PnmOStream : public FormatOStream
		{
			public:

				PnmOStream(OStream* stream);
				PnmOStream(const PnmOStream& stream) = delete;
				PnmOStream(PnmOStream&& stream) = delete;

				PnmOStream& operator=(const PnmOStream& stream) = delete;
				PnmOStream& operator=(PnmOStream&& stream) = delete;

				void writeFile(const pnm::File& file);
				void writeHeader(const pnm::Header& header);
				void writePixels(const uint16_t* samples, uint64_t pixelCount);

				~PnmOStream() = default;

			private:

				void setStreamState() override final;
				void resetFormatState() override final;

				pnm::Header _header;

				bool _headerWritten;
				uint8_t _samplesPerPixel;
				uint64_t _remainingSamples;
		};
	}
}
