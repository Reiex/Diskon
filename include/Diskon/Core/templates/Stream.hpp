///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Marius Pélégrin
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Core/CoreDecl.hpp>

namespace dsk
{
	template<typename TValue>
	const ruc::Status& IStream::expect(const TValue& value)
	{
		assert(_status);
		assert(_bitCursor == 0);

		if constexpr (sizeof(TValue) == 1)
		{
			if (_cursor == _bufferEnd)
			{
				if (!_refillBuffer(1))
				{
					return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
				}
				_cursor = _bufferBeginBuffer;
			}

			if (*_cursor != value)
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, std::format("Expected '{}' but instead got '{}'.", value, *reinterpret_cast<TValue*>(_cursor)));
			}

			++_cursor;
		}
		else if (!expect(&value, 1))
		{
			return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
		}

		return _status;
	}

	template<typename TValue>
	const ruc::Status& IStream::expect(const TValue* values, uint64_t count)
	{
		assert(_status);
		assert(_bitCursor == 0);

		uint64_t size = count * sizeof(TValue);
		const uint64_t availableSize = std::distance(_cursor, _bufferEnd);

		// If a byteswap is needed

		if constexpr (std::integral<TValue> && sizeof(TValue) != 1)
		{
			if (_byteEndianness != std::endian::native)
			{
				// If data is already available

				if (size <= availableSize)
				{
					const TValue* it = reinterpret_cast<const TValue*>(_cursor);
					const TValue* const itEnd = it + count;
					for (; it != itEnd && std::byteswap(*it) == *values; ++it, ++values);

					if (it != itEnd)
					{
						return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Expect failed.");
					}

					_cursor += size;
				}

				// If data must be read from handle

				else
				{
					TValue value;
					const TValue* const valuesEnd = values + count;
					for (; values != valuesEnd && read(&value, 1) && value == *values; ++values);

					if (values != valuesEnd)
					{
						if (_status)
						{
							return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Expect failed.");
						}
						else
						{
							return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
						}
					}
				}

				return _status;
			}
		}
		
		// If byteswap is not needed

		// If data is already available

		if (size <= availableSize)
		{
			const TValue* const it = reinterpret_cast<const TValue*>(_cursor);
			
			if (!std::equal(it, it + count, values))
			{
				if constexpr (std::same_as<TValue, char>)
				{
					return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, std::format("Expected '{}' but instead got '{}'.", std::string_view(values, count), std::string_view(reinterpret_cast<const char*>(_cursor), count)));
				}
				else
				{
					return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Expect failed.");
				}
			}

			_cursor += size;
		}

		// If data must be read from handle

		else
		{
			// Compare data already available with begining of values

			if (std::memcmp(_cursor, values, availableSize))
			{
				if constexpr (std::same_as<TValue, char>)
				{
					const std::string message = std::format(
						"Expected '{}' but instead got '{}'.",
						std::string_view(values, availableSize),
						std::string_view(reinterpret_cast<const char*>(_cursor), availableSize)
					);
					return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, message);
				}
				else
				{
					return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Expect failed.");
				}
			}

			size -= availableSize;
			values += availableSize;

			// Compare "full buffers" of data with values

			while (size > _bufferSize)
			{
				if (!_refillBuffer(_bufferSize))
				{
					return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
				}

				if (std::memcmp(_bufferBeginBuffer, values, _bufferSize))
				{
					if constexpr (std::same_as<TValue, char>)
					{
						const std::string message = std::format(
							"Expected '{}' but instead got '{}'.",
							std::string_view(values, _bufferSize),
							std::string_view(reinterpret_cast<const char*>(_bufferBeginBuffer), _bufferSize)
						);
						return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, message);
					}
					else
					{
						return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Expect failed.");
					}
				}

				size -= _bufferSize;
				values += _bufferSize;
			}

			// Compare begining of last buffer read with end of values

			if (!_refillBuffer(size))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}

			if (std::memcmp(_bufferBeginBuffer, values, size))
			{
				if constexpr (std::same_as<TValue, char>)
				{
					const std::string message = std::format(
						"Expected '{}' but instead got '{}'.",
						std::string_view(values, size),
						std::string_view(reinterpret_cast<const char*>(_bufferBeginBuffer), size)
					);
					return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, message);
				}
				else
				{
					return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Expect failed.");
				}
			}

			_cursor = _bufferBeginBuffer + size;
		}

		return _status;
	}

	template<typename TValue>
	const ruc::Status& IStream::read(TValue& value)
	{
		assert(_status);
		assert(_bitCursor == 0);

		if constexpr (sizeof(TValue) == 1)
		{
			if (_cursor == _bufferEnd)
			{
				if (!_refillBuffer(1))
				{
					return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
				}
				_cursor = _bufferBeginBuffer;
			}

			*reinterpret_cast<uint8_t*>(&value) = *_cursor;
			++_cursor;
		}
		else if (!read(&value, 1))
		{
			return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
		}

		return _status;
	}

	template<typename TValue>
	const ruc::Status& IStream::read(TValue* values, uint64_t count)
	{
		assert(_status);
		assert(_bitCursor == 0);

		const uint64_t size = count * sizeof(TValue);
		const uint64_t availableSize = std::distance(_cursor, _bufferEnd);

		// If data is already available

		if (size <= availableSize)
		{
			const TValue* const it = reinterpret_cast<const TValue*>(_cursor);

			if constexpr (std::integral<TValue> && sizeof(TValue) != 1)
			{
				if (_byteEndianness != std::endian::native)
				{
					std::transform(it, it + count, values, std::byteswap<TValue>);
				}
				else
				{
					std::copy_n(it, count, values);
				}
			}
			else
			{
				std::copy_n(it, count, values);
			}

			_cursor += size;
		}

		// If data must be read from handle

		else
		{
			std::memcpy(values, _cursor, availableSize);

			uint8_t* const it = reinterpret_cast<uint8_t*>(values) + availableSize;
			const uint64_t remainingSize = size - availableSize;

			// If remaining data to be read is not big enough to be worth two _read calls

			if (remainingSize < (_bufferSize >> 1))
			{
				if (!_refillBuffer(remainingSize))
				{
					return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
				}

				std::memcpy(it, _bufferBeginBuffer, remainingSize);

				_cursor = _bufferBeginBuffer + remainingSize;
			}

			// If two _read calls is worth it

			else
			{
				// Read directly into result data

				uint64_t readSize = _read(_handle, it, remainingSize);
				if (readSize != remainingSize)
				{
					if (_eof(_handle))
					{
						return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Tried to read more than what can be read from handle.");
					}
					else
					{
						return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while reading from handle.");
					}
				}

				// Retrieve "keep data"

				const int64_t diffSize = _keepSize - remainingSize;
				if (diffSize <= 0)
				{
					std::copy_n(it + remainingSize - _keepSize, _keepSize, _buffer);
				}
				else
				{
					// Use std::copy instead of std::copy_n because it handles the possible overlap
					std::copy(_bufferEnd - diffSize, _bufferEnd, _buffer);
					std::copy_n(it, remainingSize, _buffer + diffSize);
				}

				// Read a new buffer from handle

				readSize = _read(_handle, _bufferBeginBuffer, _bufferSize);
				if (readSize != _bufferSize)
				{
					if (!_eof(_handle))
					{
						return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while reading from handle.");
					}

					_bufferEnd = _bufferBeginBuffer + readSize;
				}

				_cursor = _bufferBeginBuffer;
			}

			// Byteswap if necessary

			if constexpr (std::integral<TValue> && sizeof(TValue) != 1)
			{
				if (_byteEndianness != std::endian::native)
				{
					std::for_each_n(values, count, std::byteswap<TValue>);
				}
			}
		}

		return _status;
	}

	template<std::integral TValue>
	const ruc::Status& IStream::readAsciiNumber(TValue& value)
	{
		assert(_status);
		assert(_bitCursor == 0);

		// Make sure cursor points to a valid character

		if (_cursor == _bufferEnd)
		{
			if (!_refillBuffer(1))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}
			_cursor = _bufferBeginBuffer;
		}

		// Parse +/- sign

		bool neg = false;
		if constexpr (std::signed_integral<TValue>)
		{
			neg = *_cursor == '-';
			if ((neg || *_cursor == '+') && ++_cursor == _bufferEnd)
			{
				if (!_refillBuffer(1))
				{
					return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
				}
				_cursor = _bufferBeginBuffer;
			}
		}
		else
		{
			if (*_cursor == '+' && ++_cursor == _bufferEnd)
			{
				if (!_refillBuffer(1))
				{
					return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
				}
				_cursor = _bufferBeginBuffer;
			}
		}

		// Check first character of the number

		uint8_t digit = *_cursor - '0';
		if (digit >= 10)
		{
			return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, std::format("Tried to read an integer starting with '{}'", *_cursor));
		}

		// Parsing the integer itself

		value = 0;
		TValue valueNext = 0;
		while (digit < 10)
		{
			valueNext = (value << 3) + (value << 1) + digit;

			if (valueNext < value)
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Unsigned integer too big to be read in the specified type.");
			}

			value = valueNext;

			if (++_cursor == _bufferEnd)
			{
				if (_eof(_handle))
				{
					break;
				}

				if (!_refillBuffer(1))
				{
					return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
				}
				_cursor = _bufferBeginBuffer;
			}

			digit = *_cursor - '0';
		}

		// Negate the result if necessary

		if constexpr (std::signed_integral<TValue>)
		{
			if (neg)
			{
				value = -value;
			}
		}

		return _status;
	}

	template<std::floating_point TValue>
	const ruc::Status& IStream::readAsciiNumber(TValue& value)
	{
		assert(_status);
		assert(_bitCursor == 0);

		// Make sure cursor points to a valid character

		if (_cursor == _bufferEnd)
		{
			if (!_refillBuffer(1))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}
			_cursor = _bufferBeginBuffer;
		}

		// Parse +/- sign

		bool neg = *_cursor == '-';
		if ((neg || *_cursor == '+') && ++_cursor == _bufferEnd)
		{
			if (!_refillBuffer(1))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}
			_cursor = _bufferBeginBuffer;
		}

		// Parse integer part (optional)

		bool integerPartRead = static_cast<uint8_t>(*_cursor - '0') < 10;
		uint64_t intPart = 0;
		uint64_t intPartNext = 0;
		while (static_cast<uint8_t>(*_cursor - '0') < 10)
		{
			intPartNext = (intPart << 3) + (intPart << 1) + *_cursor - '0';

			if (intPartNext < intPart)
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Integer part too big to be read (and efficiently stored in a floating point value).");
			}

			intPart = intPartNext;

			if (++_cursor == _bufferEnd)
			{
				if (_eof(_handle))
				{
					value = intPart;
					if (neg)
					{
						value = -value;
					}
					return _status;
				}

				if (!_refillBuffer(1))
				{
					return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
				}
				_cursor = _bufferBeginBuffer;
			}
		}

		// Parse decimal part starting with a point (mandatory if no integer part, optional otherwise)

		value = intPart;
		if (*_cursor == '.')
		{
			if (++_cursor == _bufferEnd)
			{
				if (_eof(_handle))
				{
					if (integerPartRead)
					{
						if (neg)
						{
							value = -value;
						}
						return _status;
					}
					else
					{
						return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "'.' is not a valid floating point value.");
					}
				}

				if (!_refillBuffer(1))
				{
					return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
				}
				_cursor = _bufferBeginBuffer;
			}

			if (!integerPartRead && static_cast<uint8_t>(*_cursor - '0') >= 10)
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "'.' is not a valid floating point value.");
			}

			uint64_t divisor = 10;
			uint64_t divisorNext = 0;
			while (static_cast<uint8_t>(*_cursor - '0') < 10)
			{
				value += static_cast<TValue>(*_cursor - '0') / divisor;
				divisorNext = (divisor << 3) + (divisor << 1);

				if (divisorNext < divisor)
				{
					return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Decimal part too long to be read (and efficiently stored in a floating point value).");
				}

				divisor = divisorNext;

				if (++_cursor == _bufferEnd)
				{
					if (_eof(_handle))
					{
						if (neg)
						{
							value = -value;
						}
						return _status;
					}

					if (!_refillBuffer(1))
					{
						return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
					}
					_cursor = _bufferBeginBuffer;
				}
			}
		}
		else if (!integerPartRead)
		{
			return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Tried to read a floating point value without integer part nor decimal part.");
		}

		// Parse exponent (optional)

		if (*_cursor == 'e' || *_cursor == 'E')
		{
			++_cursor;

			int16_t exponent;
			readAsciiNumber(exponent);

			value *= std::pow(TValue(10), exponent);
		}

		if (neg)
		{
			value = -value;
		}

		return _status;
	}

	template<typename TConditionFunc>
	const ruc::Status& IStream::skipCharWhile(TConditionFunc conditionFunc, uint64_t& count)
	{
		assert(_status);
		assert(_bitCursor == 0);

		const uint8_t* const oldCursor = _cursor;

		--_cursor;
		while (++_cursor != _bufferEnd && conditionFunc(static_cast<char>(*_cursor)));

		count = std::distance<const uint8_t*>(oldCursor, _cursor);

		uint8_t* const cursorStart = _bufferBeginBuffer - 1;
		while (_cursor == _bufferEnd)
		{
			if (_eof(_handle))
			{
				return _status;
			}
			else if (!_refillBuffer(1))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}

			_cursor = cursorStart;
			while (++_cursor != _bufferEnd && conditionFunc(static_cast<char>(*_cursor)));

			count += std::distance(_bufferBeginBuffer, _cursor);
		}

		return _status;
	}

	template<typename TConditionFunc>
	const ruc::Status& IStream::readCharWhile(TConditionFunc conditionFunc, char* dst, uint64_t dstSize, uint64_t& count)
	{
		assert(_status);
		assert(_bitCursor == 0);

		const char* const dstEnd = dst + dstSize;

		const uint8_t* const oldCursor = _cursor;

		--_cursor;
		--dst;
		while (((++dst != dstEnd) & (++_cursor != _bufferEnd)) && conditionFunc(static_cast<char>(*_cursor)))
		{
			*dst = static_cast<char>(*_cursor);
		}

		count = std::distance<const uint8_t*>(oldCursor, _cursor);

		uint8_t* const cursorStart = _bufferBeginBuffer - 1;
		while (dst != dstEnd && _cursor == _bufferEnd)
		{
			if (_eof(_handle))
			{
				return _status;
			}
			else if (!_refillBuffer(1))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}

			_cursor = cursorStart;
			while (((++dst != dstEnd) & (++_cursor != _bufferEnd)) && conditionFunc(static_cast<char>(*_cursor)))
			{
				*dst = static_cast<char>(*_cursor);
			}

			count += std::distance(_bufferBeginBuffer, _cursor);
		}

		return _status;
	}

	template<typename TConditionFunc>
	const ruc::Status& IStream::readCharWhile(TConditionFunc conditionFunc, std::string& dst)
	{
		assert(_status);
		assert(_bitCursor == 0);

		const char* const oldCursor = reinterpret_cast<const char*>(_cursor);

		--_cursor;
		while (++_cursor != _bufferEnd && conditionFunc(static_cast<char>(*_cursor)));

		dst.append(oldCursor, reinterpret_cast<const char*>(_cursor));

		uint8_t* const cursorStart = _bufferBeginBuffer - 1;
		while (_cursor == _bufferEnd)
		{
			if (_eof(_handle))
			{
				return _status;
			}
			else if (!_refillBuffer(1))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}

			_cursor = cursorStart;
			while (++_cursor != _bufferEnd && conditionFunc(static_cast<char>(*_cursor)));

			dst.append(reinterpret_cast<const char*>(_bufferBeginBuffer), reinterpret_cast<const char*>(_cursor));
		}

		return _status;
	}


	template<typename TValue>
	const ruc::Status& OStream::write(const TValue& value)
	{
		assert(_status);
		assert(_bitCursor == 0);

		if constexpr (sizeof(TValue) == 1)
		{
			if (_cursor < _buffer.size())
			{
				_buffer[_cursor] = *reinterpret_cast<const uint8_t*>(&value);
				++_cursor;
			}
			else
			{
				const uint64_t writeSize = _write(_handle, _buffer.data(), _cursor);
				if (writeSize != _cursor)
				{
					return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while writing to handle.");
				}

				_buffer.front() = *reinterpret_cast<const uint8_t*>(&value);
				_cursor = 1;
			}
		}
		else
		{
			if (!write(&value, 1))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}
		}

		return _status;
	}

	template<typename TValue>
	const ruc::Status& OStream::write(const TValue* values, uint64_t count)
	{
		assert(_status);
		assert(_bitCursor == 0);

		const uint64_t size = count * sizeof(TValue);
		const uint64_t availableSize = _buffer.size() - _cursor;

		// If data fits in buffer, just copy it and move cursor

		if (size <= availableSize)
		{
			TValue* it = reinterpret_cast<TValue*>(_buffer.data() + _cursor);

			if constexpr (std::integral<TValue> && sizeof(TValue) != 1)
			{
				if (_byteEndianness != std::endian::native)
				{
					std::transform(values, values + count, it, std::byteswap<TValue>);
				}
				else
				{
					std::copy_n(values, count, it);
				}
			}
			else
			{
				std::copy_n(values, count, it);
			}

			_cursor += size;
		}

		// If data must be written to handle in addition to what is in the buffer

		else
		{
			// If we must byteswap the values

			if constexpr (std::integral<TValue> && sizeof(TValue) != 1)
			{
				if (_byteEndianness != std::endian::native)
				{
					assert(_buffer.size() >= sizeof(TValue));

					// "Fill" the buffer with byteswapped values

					TValue* it = reinterpret_cast<TValue*>(_buffer.data() + _cursor);
					const uint64_t availableCount = availableSize / sizeof(TValue);

					std::transform(values, values + availableCount, it, std::byteswap<TValue>);
					values += availableCount;
					count -= availableCount;
					_cursor += availableCount * sizeof(TValue);

					it = reinterpret_cast<TValue*>(_buffer.data());
					const uint64_t countPerBuffer = _buffer.size() / sizeof(TValue);
					const uint64_t sizePerBuffer = countPerBuffer * sizeof(TValue);

					// While there are values to be written, write the buffer to the handle and refill it
					
					while (count)
					{
						const uint64_t writeSize = _write(_handle, _buffer.data(), _cursor);
						if (writeSize != _cursor)
						{
							return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while writing to handle.");
						}

						if (count >= countPerBuffer)
						{
							std::transform(values, values + countPerBuffer, it, std::byteswap<TValue>);
							_cursor = sizePerBuffer;
							count -= countPerBuffer;
							values += countPerBuffer;
						}
						else
						{
							std::transform(values, values + count, it, std::byteswap<TValue>);
							_cursor = count * sizeof(TValue);
							count = 0;
						}
					}

					return _status;
				}
			}

			// If the values can directly be written without byteswap

			// Fill the buffer with the data and write it to handle
			
			std::memcpy(_buffer.data() + _cursor, values, availableSize);

			uint64_t writeSize = _write(_handle, _buffer.data(), _buffer.size());
			if (writeSize != _buffer.size())
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while writing to handle.");
			}
			
			// If the remaining data to be written is not big enough to be worth a second _write call
			
			const uint64_t remainingSize = size - availableSize;
			if (remainingSize < (_buffer.size() >> 1))
			{
				std::memcpy(_buffer.data(), reinterpret_cast<const uint8_t*>(values) + availableSize, remainingSize);
				_cursor = remainingSize;
			}
			
			// If the remaining data to be written is big enough to be worth a second _write
			
			else
			{
				writeSize = _write(_handle, reinterpret_cast<const uint8_t*>(values) + availableSize, remainingSize);
				if (writeSize != remainingSize)
				{
					return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while writing to handle.");
				}

				_cursor = 0;
			}
		}

		return _status;
	}

	template<std::integral TValue>
	const ruc::Status& OStream::writeAsciiNumber(TValue value)
	{
		assert(_status);
		assert(_bitCursor == 0);

		constexpr uint8_t tabMaxSize[9] = { 0, 4, 6, 8, 11, 13, 16, 18, 20 };
		constexpr uint64_t maxSize = tabMaxSize[sizeof(TValue)];

		if (_buffer.size() - _cursor < maxSize)
		{
			const uint64_t writeSize = _write(_handle, _buffer.data(), _cursor);
			if (writeSize != _cursor)
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while writing to handle.");
			}

			_cursor = 0;
		}

		if (value == 0)
		{
			_buffer[_cursor] = '0';
			++_cursor;
		
			return _status;
		}
		
		bool neg;
		if constexpr (std::signed_integral<TValue>)
		{
			neg = value < 0;
		}
		
		uint8_t* it = _buffer.data() + _cursor - 1;
		while (value)
		{
			*(++it) = (value % 10) + '0';
			value /= 10;
		}
		
		if constexpr (std::signed_integral<TValue>)
		{
			if (neg)
			{
				*(++it) = '-';
			}
		}
		
		std::reverse(_buffer.data() + _cursor, ++it);
		_cursor = std::distance(_buffer.data(), it);

		return _status;
	}

	template<std::floating_point TValue>
	const ruc::Status& OStream::writeAsciiNumber(TValue value)
	{
		assert(_status);
		assert(_bitCursor == 0);

		// TODO: Compute real max size
		constexpr uint64_t maxSize = 32;

		if (_buffer.size() - _cursor < maxSize)
		{
			const uint64_t writeSize = _write(_handle, _buffer.data(), _cursor);
			if (writeSize != _cursor)
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while writing to handle.");
			}

			_cursor = 0;
		}

		char* it = reinterpret_cast<char*>(_buffer.data() + _cursor);
		std::to_chars_result result = std::to_chars(it, it + maxSize, value);

		_cursor = std::distance(reinterpret_cast<char*>(_buffer.data()), result.ptr);

		return _status;
	}
}
