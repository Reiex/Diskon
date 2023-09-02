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

				const ruc::Status& readFile(pnm::File& file);
				const ruc::Status& readHeader(pnm::Header& header);
				const ruc::Status& readPixels(uint16_t* samples, uint64_t pixelCount);

				~PnmIStream() = default;

			private:

				void setStreamState() override final;
				void resetFormatState() override final;

				const ruc::Status& _readHeaderChar(char& value);
				const ruc::Status& _readHeaderSpaces();


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

				const ruc::Status& writeFile(const pnm::File& file);
				const ruc::Status& writeHeader(const pnm::Header& header);
				const ruc::Status& writePixels(const uint16_t* samples, uint64_t pixelCount);

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
