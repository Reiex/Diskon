///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Marius Pélégrin
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		FormatIStream::FormatIStream(IStream* stream) :
			_stream(stream),
			_subStreams()
		{
		}

		void FormatIStream::setStream(IStream* stream, bool resetState)
		{
			for (FormatIStream* subStream : _subStreams)
			{
				subStream->setStream(stream, resetState);
			}

			if (resetState)
			{
				resetFormatState();
			}

			_stream = stream;
			setStreamState();
		}
	
		void FormatIStream::setStreamState()
		{
			_stream->setByteEndianness(std::endian::native);
			_stream->setBitEndianness(std::endian::little);
		}

		FormatOStream::FormatOStream(OStream* stream) :
			_stream(stream),
			_subStreams()
		{
		}
	
		void FormatOStream::setStream(OStream* stream, bool resetState)
		{
			for (FormatOStream* subStream : _subStreams)
			{
				subStream->setStream(stream, resetState);
			}

			if (resetState)
			{
				resetFormatState();
			}

			_stream = stream;
			setStreamState();
		}

		void FormatOStream::setStreamState()
		{
			_stream->setByteEndianness(std::endian::native);
			_stream->setBitEndianness(std::endian::little);
		}
	}
}
