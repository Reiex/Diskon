#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace dsk
{
	struct FormatError;
	class FormatStream;
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


#define FMTSTREAM_VERIFY_SELF_CALL(call, ...)	\
do {											\
	if (!call(__VA_ARGS__))						\
	{											\
		return error;							\
	}											\
} while (0)


#define FMTSTREAM_VERIFY_CALL(stream, call, ...)	\
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
