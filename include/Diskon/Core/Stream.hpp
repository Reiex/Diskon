///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Core/CoreTypes.hpp>

namespace dsk
{
	class DSK_API IStream
	{
		public:

			using ReadFunc = uint64_t(*)(void*, uint8_t*, uint64_t);
			using EOFFunc = bool(*)(void*);

			IStream(void* handle, ReadFunc readFunc, EOFFunc eofFunc, uint64_t bufferSize=65536, uint64_t keepSize=1024);
			IStream(const IStream& stream) = delete;
			IStream(IStream&& stream) = delete;

			IStream& operator=(const IStream& stream) = delete;
			IStream& operator=(IStream&& stream) = delete;


			const ruc::Status& unread(uint64_t size);
			const ruc::Status& skip(uint64_t size);
			template<typename TValue> const ruc::Status& expect(const TValue& value);
			template<typename TValue> const ruc::Status& expect(const TValue* values, uint64_t count);
			template<typename TValue> const ruc::Status& read(TValue& value);
			template<typename TValue> const ruc::Status& read(TValue* values, uint64_t count);

			template<std::integral TValue> const ruc::Status& readAsciiNumber(TValue& value);
			template<std::floating_point TValue> const ruc::Status& readAsciiNumber(TValue& value);

			template<typename TConditionFunc> const ruc::Status& skipCharWhile(TConditionFunc conditionFunc, uint64_t& count);
			template<typename TConditionFunc> const ruc::Status& readCharWhile(TConditionFunc conditionFunc, char* dst, uint64_t dstSize, uint64_t& count);
			template<typename TConditionFunc> const ruc::Status& readCharWhile(TConditionFunc conditionFunc, std::string& dst);


			const ruc::Status& bitUnread(uint64_t bitCount);
			const ruc::Status& bitRead(bool& bit);
			const ruc::Status& bitRead(uint8_t* data, uint64_t bitCount, uint8_t bitOffset = 0);

			const ruc::Status& finishByte();


			bool eof() const { return _cursor == _bufferEnd && _eof(_handle); }

			void setByteEndianness(std::endian endianness)	{ _byteEndianness = endianness; }
			std::endian getByteEndianness() const			{ return _byteEndianness; }
			void setBitEndianness(std::endian endianness)	{ _bitEndianness = endianness; }
			std::endian getBitEndianness() const			{ return _bitEndianness; }
			ruc::Status& getStatus()						{ return _status; }
			const ruc::Status& getStatus() const			{ return _status; }

			~IStream();

		private:

			const ruc::Status& _refillBuffer(uint64_t size);

			ruc::Status _status;

			void* _handle;
			ReadFunc _read;
			EOFFunc _eof;

			uint64_t _bufferSize;
			uint64_t _keepSize;

			uint8_t* const _buffer;
			uint8_t* const _bufferBeginBuffer;
			uint8_t* const _bufferBeginRetrieve;
			uint8_t* _bufferEnd;

			uint8_t* _cursor;
			uint8_t _bitCursor;
			
			std::endian _byteEndianness;
			std::endian _bitEndianness;
	};

	class DSK_API OStream
	{
		public:

			using WriteFunc = uint64_t(*)(void*, const uint8_t*, uint64_t);

			OStream(void* handle, WriteFunc writeFunc, uint64_t bufferSize = 65536);
			OStream(const OStream& stream) = delete;
			OStream(OStream&& stream) = delete;

			OStream& operator=(const OStream& stream) = delete;
			OStream& operator=(OStream&& stream) = delete;


			template<typename TValue> const ruc::Status& write(const TValue& value);
			template<typename TValue> const ruc::Status& write(const TValue* values, uint64_t count);

			template<std::integral TValue> const ruc::Status& writeAsciiNumber(TValue value);
			template<std::floating_point TValue> const ruc::Status& writeAsciiNumber(TValue value);


			const ruc::Status& bitWrite(bool bit);
			const ruc::Status& bitWrite(const uint8_t* data, uint64_t bitCount, uint8_t bitOffset = 0);

			const ruc::Status& finishByte(uint8_t padBits = 0);


			const ruc::Status& flush();


			void setByteEndianness(std::endian endianness)	{ _byteEndianness = endianness; }
			std::endian getByteEndianness() const			{ return _byteEndianness; }
			void setBitEndianness(std::endian endianness)	{ _bitEndianness = endianness; }
			std::endian getBitEndianness() const			{ return _bitEndianness; }
			ruc::Status& getStatus()						{ return _status; }
			const ruc::Status& getStatus() const			{ return _status; }

			~OStream() = default;

		private:

			ruc::Status _status;

			void* _handle;
			WriteFunc _write;

			std::vector<uint8_t> _buffer;
			uint64_t _cursor;
			uint8_t _bitCursor;

			std::endian _byteEndianness;
			std::endian _bitEndianness;
	};
}
