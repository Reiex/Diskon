///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
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

				const ruc::Status& getStatus() const { return _status; }

				~FormatIStream() = default;

			protected:

				FormatIStream(IStream* stream);

				virtual void setStreamState();
				virtual void resetFormatState() = 0;

				static constexpr uint64_t _singleBufferSize = 4096;

				ruc::Status _status;

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

				const ruc::Status& getStatus() const { return _status; }

				~FormatOStream() = default;

			protected:

				FormatOStream(OStream* stream);

				virtual void setStreamState();
				virtual void resetFormatState() = 0;

				static constexpr uint64_t _singleBufferSize = 4096;

				ruc::Status _status;

				OStream* _stream;
				std::vector<FormatOStream*> _subStreams;
		};
	}
}

#define DSKFMT_BEGIN()					assert(_stream); assert(_stream->getStatus()); assert(_status)
#define DSKFMT_STREAM_CALL(func, ...)	_stream->func(__VA_ARGS__); RUC_RELAYCOPY(_stream->getStatus(), _status, RUC_VOID)
