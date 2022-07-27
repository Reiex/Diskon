#pragma once

#include <Diskon/Core/Core.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace riff
		{
			struct ChunkHeader;
			struct Chunk;
			struct FileHeader;
			struct File;
		}
		class RifStream;
		
		namespace wave
		{
			enum class Format : uint16_t;
			struct Header;
			struct File;
		}
		class WaveStream;
		
		namespace xml
		{
			struct Declaration;
			struct DocType;
			struct ProcessingInstruction;
			struct ElementData;
			struct Element;
			struct ElementTree;
			struct Prolog;
			struct File;
		}
		class XmlStream;

		namespace png
		{

		}
		class PngStream;
	}
}


#define FMTSTREAM_VERIFY(cond, code, message)				\
do {														\
	if (!(cond))											\
	{														\
		error.errorCode = FormatError::ErrorCode::code;		\
		error.errorMessage = message;						\
		return error;										\
	}														\
} while (0)


#define FMTSTREAM_VERIFY_CALL(call, ...)	\
do {											\
	if (!call(__VA_ARGS__))						\
	{											\
		return error;							\
	}											\
} while (0)


#define FMTSTREAM_VERIFY_STREAM_CALL(stream, call, ...)	\
do {												\
	if (!(stream).call(__VA_ARGS__))				\
	{												\
		error = (stream).getLastError();			\
		return error;								\
	}												\
} while (0)


#define FMTSTREAM_BEGIN_FUNC(streamptr, streamtype)																			\
FormatError& error = _error;																								\
error.clear();																												\
																															\
FMTSTREAM_VERIFY(streamptr, NoStream, "Start of FormatStream function with " #streamptr " required but not set.");			\
FMTSTREAM_VERIFY(*streamptr, InvalidStream, "Start of FormatStream function with " #streamptr " required but invalid.");	\
																															\
streamtype& stream = *streamptr;


#define FMTSTREAM_BEGIN_READ()	FMTSTREAM_BEGIN_FUNC(_srcStream, std::istream)
#define FMTSTREAM_BEGIN_WRITE()	FMTSTREAM_BEGIN_FUNC(_dstStream, std::ostream)
