#pragma once

#include <Diskon/Core/Core.hpp>

namespace dsk
{
	namespace fmt
	{
		struct FormatError;
		class FormatStream;

		namespace png
		{

		}
		class PngStream;

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

		namespace zlib
		{

		}
		class ZlibStream;
	}
}


// Verify macros

#define FMTSTREAM_VERIFY(cond, code, message)				\
do {														\
	if (!(cond))											\
	{														\
		error.errorCode = FormatError::ErrorCode::code;		\
		error.errorMessage = funcName + ": " + message;		\
		return error;										\
	}														\
} while (0)


#define FMTSTREAM_VERIFY_CALL(call, ...)							\
do {																\
	if (!call(__VA_ARGS__))											\
	{																\
		error.errorMessage = funcName + "\n" + error.errorMessage;	\
		return error;												\
	}																\
} while (0)


#define FMTSTREAM_VERIFY_STREAM_CALL(stream, call, ...)				\
do {																\
	if (!(stream).call(__VA_ARGS__))								\
	{																\
		error = (stream).getLastError();							\
		error.errorMessage = funcName + "\n" + error.errorMessage;	\
		return error;												\
	}																\
} while (0)


// Read/Write macros

#define FMTSTREAM_READ(...)						\
FMTSTREAM_VERIFY_CALL(streamRead, __VA_ARGS__)


#define FMTSTREAM_WRITE(...)					\
FMTSTREAM_VERIFY_CALL(streamWrite, __VA_ARGS__)


// Function begining macros

#define FMTSTREAM_READ_FUNC(name)			\
static const std::string funcName = name;	\
FormatError& error = _error;				\
std::istream& stream = *_srcStream


#define FMTSTREAM_WRITE_FUNC(name)			\
static const std::string funcName = name;	\
FormatError& error = _error;				\
std::ostream& stream = *_dstStream


#define FMTSTREAM_BEGIN_READ_FUNC(name)															\
if (!_srcStream)																				\
{																								\
	_error.errorCode = FormatError::ErrorCode::NoStream;										\
	_error.errorMessage = "Start of read function with no source stream specified.";			\
	return _error;																				\
}																								\
FMTSTREAM_READ_FUNC(name);																		\
FMTSTREAM_VERIFY(stream, InvalidStream, "Start of read function with already invalid stream.");	\
error.clear()


#define FMTSTREAM_BEGIN_WRITE_FUNC(name)															\
if (!_dstStream)																					\
{																									\
	_error.errorCode = FormatError::ErrorCode::NoStream;											\
	_error.errorMessage = "Start of write function with no destination stream specified.";			\
	return _error;																					\
}																									\
FMTSTREAM_WRITE_FUNC(name);																			\
FMTSTREAM_VERIFY(stream, InvalidStream, "Start of write function with already invalid stream.");	\
error.clear()
