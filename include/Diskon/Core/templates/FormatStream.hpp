#pragma once

#include <Diskon/Core/FormatStream.hpp>

namespace dsk
{
	template<typename TValue>
	const FormatError& FormatStream::streamRead(TValue& value)
	{
		if (!_srcStream->read(reinterpret_cast<char*>(&value), sizeof(TValue)))
		{
			_error.errorCode = FormatError::ErrorCode::InvalidStream;
			_error.errorMessage = "FormatStream: Error while reading a single value of size " + std::to_string(sizeof(TValue)) + ".";
		}

		if constexpr (sizeof(TValue) > 1)
		{
			if (_endianness != std::endian::native)
			{
				value = std::byteswap(value);
			}
		}

		return _error;
	}

	template<typename TValue>
	const FormatError& FormatStream::streamRead(TValue* values, uint64_t count)
	{
		if (!_srcStream->read(reinterpret_cast<char*>(values), sizeof(TValue) * count))
		{
			_error.errorCode = FormatError::ErrorCode::InvalidStream;
			_error.errorMessage = "FormatStream: Error while reading an array of " + std::to_string(count) + " values of size " + std::to_string(sizeof(TValue)) + ".";
		}

		if constexpr (sizeof(TValue) > 1)
		{
			if (_endianness != std::endian::native)
			{
				for (TValue* it = values, *itEnd = values + count; it != itEnd; ++it)
				{
					*it = std::byteswap(*it);
				}
			}
		}

		return _error;
	}

	template<typename TValue>
	const FormatError& FormatStream::streamWrite(TValue value)
	{
		if constexpr (sizeof(TValue) > 1)
		{
			if (_endianness != std::endian::native)
			{
				value = std::byteswap(value);
			}
		}

		if (!_dstStream->write(reinterpret_cast<const char*>(&value), sizeof(TValue)))
		{
			_error.errorCode = FormatError::ErrorCode::InvalidStream;
			_error.errorMessage = "FormatStream: Error while writing a single value of size " + std::to_string(sizeof(TValue)) + ".";
		}

		return _error;
	}

	template<typename TValue>
	const FormatError& FormatStream::streamWrite(const TValue* values, uint64_t count)
	{
		const TValue* savedValues = values;

		if constexpr (sizeof(TValue) > 1)
		{
			if (_endianness != std::endian::native)
			{
				TValue* copy = new TValue[count];
				savedValues = copy;

				const TValue* it = values;
				const TValue* const itEnd = values + count;
				for (; it != itEnd; ++it, ++copy)
				{
					*copy = std::byteswap(*it);
				}
			}
		}

		if (!_dstStream->write(reinterpret_cast<const char*>(savedValues), sizeof(TValue) * count))
		{
			_error.errorCode = FormatError::ErrorCode::InvalidStream;
			_error.errorMessage = "FormatStream: Error while writing an array of " + std::to_string(count) + " values of size " + std::to_string(sizeof(TValue)) + ".";
		}

		if (savedValues != values)
		{
			delete savedValues;
		}

		return _error;
	}
}
