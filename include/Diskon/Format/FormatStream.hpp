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
		class DSK_API FormatIStream
		{
			public:

				FormatIStream(const FormatIStream& stream) = delete;
				FormatIStream(FormatIStream&& stream) = delete;

				FormatIStream& operator=(const FormatIStream& stream) = delete;
				FormatIStream& operator=(FormatIStream&& stream) = delete;

				void setStream(IStream* stream, bool resetState = true);

				~FormatIStream() = default;

			protected:

				FormatIStream(IStream* stream);

				virtual void setStreamState();
				virtual void resetFormatState() = 0;

				static constexpr uint64_t _singleBufferSize = 4096;

				IStream* _stream;
				std::vector<FormatIStream*> _subStreams;
		};

		class DSK_API FormatOStream
		{
			public:

				FormatOStream(const FormatOStream& stream) = delete;
				FormatOStream(FormatOStream&& stream) = delete;

				FormatOStream& operator=(const FormatOStream& stream) = delete;
				FormatOStream& operator=(FormatOStream&& stream) = delete;

				void setStream(OStream* stream, bool resetState = true);

				~FormatOStream() = default;

			protected:

				FormatOStream(OStream* stream);

				virtual void setStreamState();
				virtual void resetFormatState() = 0;

				static constexpr uint64_t _singleBufferSize = 4096;

				OStream* _stream;
				std::vector<FormatOStream*> _subStreams;
		};
	}
}

#define DSKFMT_BEGIN()						assert(_stream); assert(_stream->getStatus())
#define DSKFMT_CHECK(condition, message)	RUC_CHECK(_stream->getStatus(), _stream->getStatus(), condition, message)
#define DSKFMT_CALL(func, ...)				func(__VA_ARGS__); RUC_RELAY(_stream->getStatus(), _stream->getStatus())
