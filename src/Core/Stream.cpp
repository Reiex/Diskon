///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author P�l�grin Marius
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

	void IStream::unread(uint64_t size)
	{
		assert(_status);
		assert(_bitCursor == 0);

		DSK_CHECK(std::distance(_buffer, _cursor) >= size, "Tried to unread too many bytes.");

		_cursor -= size;
	}

	void IStream::skip(uint64_t size)
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
			while (size > _bufferSize && (_refillBuffer(_bufferSize), _status))
			{
				size -= _bufferSize;
			}

			if (size > _bufferSize)
			{
				return _status.relayErrorMessage(__PRETTY_FUNCTION__, __LINE__);
			}

			DSK_CALL(_refillBuffer, size);

			_cursor = _bufferBeginBuffer + size;
		}
	}

	void IStream::bitUnread(uint64_t bitCount)
	{
		assert(_status);

		const uint64_t size = bitCount >> 3;
		const uint8_t remainingBits = bitCount & 7;

		std::ptrdiff_t distance = std::distance(_buffer, _cursor);
		DSK_CHECK(distance > size || (distance == size && _bitCursor >= remainingBits), "Tried to unread too many bits.");

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
	}

	void IStream::bitRead(bool& bit)
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
			DSK_CALL(_refillBuffer, 1);

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
	}

	void IStream::bitRead(uint8_t* data, uint64_t bitCount, uint8_t bitOffset)
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
				DSK_CALL(_refillBuffer, _bufferSize);

				bitcpy(_bufferBeginBuffer, 0, data, bitOffset, _bufferSize, 0, _bitEndianness);

				data += _bufferSize;
				remainingSize -= _bufferSize;
			}

			DSK_CALL(_refillBuffer, remainingSize + (remainingTrailingBits != 0));

			bitcpy(_bufferBeginBuffer, 0, data, bitOffset, remainingSize, remainingTrailingBits, _bitEndianness);

			_cursor = _bufferBeginBuffer + remainingSize;
			_bitCursor = remainingTrailingBits;
		}
	}

	void IStream::finishByte()
	{
		assert(_status);
		
		if (_bitCursor)
		{
			_bitCursor = 0;
			++_cursor;
		}
	}

	IStream::~IStream()
	{
		delete[] _buffer;
	}

	void IStream::_refillBuffer(uint64_t size)
	{
		assert(_status);
		assert(size != 0);

		// Check EOF was not already reached - Because this function will ALWAYS need to read more

		DSK_CHECK(_bufferEnd == _bufferBeginBuffer + _bufferSize, "Tried to read more than what can be read from handle.");

		// Retrieve "keep data" - Use std::copy instead of std::copy_n because it handles the possible overlap

		std::copy(_bufferBeginRetrieve, _bufferEnd, _buffer);

		// Read the new buffer from handle

		const uint64_t readSize = _read(_handle, _bufferBeginBuffer, _bufferSize);
		if (readSize != _bufferSize)
		{
			DSK_CHECK(_eof(_handle), "Error while reading from handle.");
			DSK_CHECK(readSize >= size, "Tried to read more than what can be read from handle.");

			_bufferEnd = _bufferBeginBuffer + readSize;
		}
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

	void OStream::bitWrite(bool bit)
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
			DSK_CHECK(writeSize == _cursor, "Error while writing to handle.");

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
	}

	void OStream::bitWrite(const uint8_t* data, uint64_t bitCount, uint8_t bitOffset)
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
				DSK_CHECK(writeSize == _buffer.size(), "Error while writing to handle.");
				
				bitcpy(data, bitOffset, _buffer.data(), 0, _buffer.size(), 0, _bitEndianness);

				data += _buffer.size();
				remainingSize -= _buffer.size();
			}

			const uint64_t writeSize = _write(_handle, _buffer.data(), _buffer.size());
			DSK_CHECK(writeSize == _buffer.size(), "Error while writing to handle.");

			bitcpy(data, bitOffset, _buffer.data(), 0, remainingSize, remainingTrailingBits, _bitEndianness);

			_cursor = remainingSize;
			_bitCursor = remainingTrailingBits;
		}
	}

	void OStream::finishByte(uint8_t padBits)
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
	}

	void OStream::flush()
	{
		assert(_status);
		assert(_bitCursor == 0);

		if (_cursor != 0)
		{
			const uint64_t writeSize = _write(_handle, _buffer.data(), _cursor);
			DSK_CHECK(writeSize == _cursor, "Error while writing to handle.");

			_cursor = 0;
		}
	}
}
