///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Diskon/Core/Core.hpp>

namespace dsk
{
	IStream::IStream(void* handle, ReadFunc readFunc, EOFFunc eofFunc, uint64_t bufferSize, uint64_t keepSize) :
		_status(),
		_handle(handle),
		_read(readFunc),
		_eof(eofFunc),
		_bufferSize(bufferSize),
		_keepSize(keepSize),
		_buffer(new uint8_t[_bufferSize + _keepSize]),
		_bufferBeginBuffer(_buffer + _keepSize),
		_bufferBeginRetrieve(_buffer + _bufferSize),
		_bufferEnd(_buffer + _keepSize + _bufferSize),
		_cursor(_bufferEnd),
		_bitCursor(0),
		_byteEndianness(std::endian::native),
		_bitEndianness(std::endian::little)
	{
		assert(readFunc != nullptr);
		assert(eofFunc != nullptr);
		assert(bufferSize != 0);
		assert(keepSize != 0);
	}

	const ruc::Status& IStream::unread(uint64_t size)
	{
		assert(_status);
		assert(_bitCursor == 0);

		if (std::distance(_buffer, _cursor) < size)
		{
			return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Tried to unread too many bytes.");
		}

		_cursor -= size;

		return _status;
	}

	const ruc::Status& IStream::skip(uint64_t size)
	{
		assert(_status);
		assert(_bitCursor == 0);

		if (size <= std::distance(_cursor, _bufferEnd))
		{
			_cursor += size;
		}
		else
		{
			size -= std::distance(_cursor, _bufferEnd);
			while (size > _bufferSize && _refillBuffer(_bufferSize))
			{
				size -= _bufferSize;
			}

			if (size > _bufferSize)
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}

			if (!_refillBuffer(size))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}

			_cursor = _bufferBeginBuffer + size;
		}

		return _status;
	}

	const ruc::Status& IStream::bitUnread(uint64_t bitCount)
	{
		assert(_status);

		const uint64_t size = bitCount >> 3;
		const uint8_t remainingBits = bitCount & 7;

		std::ptrdiff_t distance = std::distance(_buffer, _cursor);
		if (distance < size || (distance == size && _bitCursor < remainingBits))
		{
			return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Tried to unread too many bits.");
		}

		_cursor -= size;
		if (_bitCursor >= remainingBits)
		{
			_bitCursor -= remainingBits;
		}
		else
		{
			_bitCursor = _bitCursor + 8 - remainingBits;
			--_cursor;
		}

		return _status;
	}

	const ruc::Status& IStream::bitRead(bool& bit)
	{
		assert(_status);

		static constexpr uint8_t filterLsb[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };
		static constexpr uint8_t filterMsb[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

		if (_cursor != _bufferEnd)
		{
			if (_bitEndianness == std::endian::little)
			{
				bit = (*_cursor & filterLsb[_bitCursor]);
			}
			else
			{
				bit = (*_cursor & filterMsb[_bitCursor]);
			}

			++_bitCursor;
			_cursor += (_bitCursor >> 3);
			_bitCursor &= 7;
		}
		else
		{
			if (!_refillBuffer(1))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}

			_cursor = _bufferBeginBuffer;

			if (_bitEndianness == std::endian::little)
			{
				bit = (*_cursor & filterLsb[0]);
			}
			else
			{
				bit = (*_cursor & filterMsb[0]);
			}

			_bitCursor = 1;
		}

		return _status;
	}

	const ruc::Status& IStream::bitRead(uint8_t* data, uint64_t bitCount, uint8_t bitOffset)
	{
		assert(_status);

		const uint64_t size = (bitCount >> 3);
		const uint8_t trailingBits = (bitCount & 7);

		const uint64_t availableBitCount = (std::distance(_cursor, _bufferEnd) << 3) - _bitCursor;

		if (bitCount <= availableBitCount)
		{
			bitcpy(_cursor, _bitCursor, data, bitOffset, size, trailingBits, _bitEndianness);

			_cursor += size + ((_bitCursor + trailingBits) >> 3);
			_bitCursor = ((_bitCursor + trailingBits) & 7);
		}
		else
		{
			const uint64_t availableSize = (availableBitCount >> 3);
			const uint64_t availableTrailingBits = (availableBitCount & 7);

			bitcpy(_cursor, _bitCursor, data, bitOffset, availableSize, availableTrailingBits, _bitEndianness);

			data += availableSize + ((availableTrailingBits + bitOffset) >> 3);
			bitOffset = ((availableTrailingBits + bitOffset) & 7);

			bitCount -= availableBitCount;

			uint64_t remainingSize = (bitCount >> 3);
			uint8_t remainingTrailingBits = (bitCount & 7);

			while (remainingSize >= _bufferSize)
			{
				if (!_refillBuffer(_bufferSize))
				{
					return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
				}

				bitcpy(_bufferBeginBuffer, 0, data, bitOffset, _bufferSize, 0, _bitEndianness);

				data += _bufferSize;
				remainingSize -= _bufferSize;
			}

			if (!_refillBuffer(remainingSize + (remainingTrailingBits != 0)))
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}

			bitcpy(_bufferBeginBuffer, 0, data, bitOffset, remainingSize, remainingTrailingBits, _bitEndianness);

			_cursor = _bufferBeginBuffer + remainingSize;
			_bitCursor = remainingTrailingBits;
		}

		return _status;
	}

	const ruc::Status& IStream::finishByte()
	{
		assert(_status);
		
		if (_bitCursor)
		{
			_bitCursor = 0;
			++_cursor;
		}

		return _status;
	}

	IStream::~IStream()
	{
		delete[] _buffer;
	}

	const ruc::Status& IStream::_refillBuffer(uint64_t size)
	{
		assert(_status);
		assert(size != 0);

		// Check EOF was not already reached - Because this function will ALWAYS need to read more

		if (_bufferEnd != _bufferBeginBuffer + _bufferSize)
		{
			return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Tried to read more than what can be read from handle.");
		}

		// Retrieve "keep data" - Use std::copy instead of std::copy_n because it handles the possible overlap

		std::copy(_bufferBeginRetrieve, _bufferEnd, _buffer);

		// Read the new buffer from handle

		const uint64_t readSize = _read(_handle, _bufferBeginBuffer, _bufferSize);
		if (readSize != _bufferSize)
		{
			if (!_eof(_handle))
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while reading from handle.");
			}
			else if (readSize < size)
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Tried to read more than what can be read from handle.");
			}

			_bufferEnd = _bufferBeginBuffer + readSize;
		}

		return _status;
	}


	OStream::OStream(void* handle, WriteFunc writeFunc, uint64_t bufferSize) :
		_status(),
		_handle(handle),
		_write(writeFunc),
		_buffer(bufferSize),
		_cursor(0),
		_bitCursor(0),
		_byteEndianness(std::endian::native),
		_bitEndianness(std::endian::little)
	{
		assert(writeFunc != nullptr);
		assert(bufferSize != 0);
	}

	const ruc::Status& OStream::bitWrite(bool bit)
	{
		assert(_status);

		static constexpr uint8_t filterLsb[8] = { ~1, ~2, ~4, ~8, ~16, ~32, ~64, ~128 };
		static constexpr uint8_t filterMsb[8] = { ~128, ~64, ~32, ~16, ~8, ~4, ~2, ~1 };

		if (_cursor < _buffer.size())
		{
			if (_bitEndianness == std::endian::little)
			{
				_buffer[_cursor] = (_buffer[_cursor] & filterLsb[_bitCursor]) | (bit << _bitCursor);
			}
			else
			{
				_buffer[_cursor] = (_buffer[_cursor] & filterMsb[_bitCursor]) | (bit << (7 - _bitCursor));
			}

			++_bitCursor;
			_cursor += (_bitCursor >> 3);
			_bitCursor &= 7;
		}
		else
		{
			const uint64_t writeSize = _write(_handle, _buffer.data(), _cursor);
			if (writeSize != _cursor)
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while writing to handle.");
			}


			if (_bitEndianness == std::endian::little)
			{
				_buffer.front() = bit;
			}
			else
			{
				_buffer.front() = (bit << 7);
			}

			_cursor = 0;
			_bitCursor = 1;
		}

		return _status;
	}

	const ruc::Status& OStream::bitWrite(const uint8_t* data, uint64_t bitCount, uint8_t bitOffset)
	{
		assert(_status);

		const uint64_t size = (bitCount >> 3);
		const uint8_t trailingBits = (bitCount & 7);

		const uint64_t availableBitCount = ((_buffer.size() - _cursor) << 3) - _bitCursor;

		if (bitCount <= availableBitCount)
		{
			bitcpy(data, bitOffset, _buffer.data() + _cursor, _bitCursor, size, trailingBits, _bitEndianness);

			_cursor += size + ((_bitCursor + trailingBits) >> 3);
			_bitCursor = ((_bitCursor + trailingBits) & 7);
		}
		else
		{
			const uint64_t availableSize = (availableBitCount >> 3);
			const uint64_t availableTrailingBits = (availableBitCount & 7);

			bitcpy(data, bitOffset, _buffer.data() + _cursor, _bitCursor, availableSize, availableTrailingBits, _bitEndianness);

			data += availableSize + ((availableTrailingBits + bitOffset) >> 3);
			bitOffset = ((availableTrailingBits + bitOffset) & 7);

			bitCount -= availableBitCount;

			uint64_t remainingSize = (bitCount >> 3);
			uint8_t remainingTrailingBits = (bitCount & 7);

			while (remainingSize >= _buffer.size())
			{
				const uint64_t writeSize = _write(_handle, _buffer.data(), _buffer.size());
				if (writeSize != _buffer.size())
				{
					return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while writing to handle.");
				}

				bitcpy(data, bitOffset, _buffer.data(), 0, _buffer.size(), 0, _bitEndianness);

				data += _buffer.size();
				remainingSize -= _buffer.size();
			}

			const uint64_t writeSize = _write(_handle, _buffer.data(), _buffer.size());
			if (writeSize != _buffer.size())
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while writing to handle.");
			}

			bitcpy(data, bitOffset, _buffer.data(), 0, remainingSize, remainingTrailingBits, _bitEndianness);

			_cursor = remainingSize;
			_bitCursor = remainingTrailingBits;
		}

		return _status;
	}

	const ruc::Status& OStream::finishByte(uint8_t padBits)
	{
		assert(_status);

		static constexpr uint8_t filterLsb[8] = { 0, 1, 3, 7, 15, 31, 63, 127 };
		static constexpr uint8_t nFilterLsb[8] = { ~0, ~1, ~3, ~7, ~15, ~31, ~63, ~127 };
		static constexpr uint8_t filterMsb[8] = { 0, 128, 192, 224, 240, 248, 252, 254 };
		static constexpr uint8_t nFilterMsb[8] = { ~0, ~128, ~192, ~224, ~240, ~248, ~252, ~254 };

		if (_bitCursor)
		{
			if (_bitEndianness == std::endian::little)
			{
				_buffer[_cursor] = (_buffer[_cursor] & filterLsb[_bitCursor]) | (padBits & nFilterLsb[_bitCursor]);
			}
			else
			{
				_buffer[_cursor] = (_buffer[_cursor] & filterMsb[_bitCursor]) | (padBits & nFilterMsb[_bitCursor]);
			}

			_bitCursor = 0;
			++_cursor;
		}

		return _status;
	}

	const ruc::Status& OStream::flush()
	{
		assert(_status);
		assert(_bitCursor == 0);

		if (_cursor != 0)
		{
			const uint64_t writeSize = _write(_handle, _buffer.data(), _cursor);
			if (writeSize != _cursor)
			{
				return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, "Error while writing to handle.");
			}

			_cursor = 0;
		}

		return _status;
	}
}
