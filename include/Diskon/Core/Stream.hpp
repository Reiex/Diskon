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


			void unread(uint64_t size);
			void skip(uint64_t size);
			template<typename TValue> void expect(const TValue& value);
			template<typename TValue> void expect(const TValue* values, uint64_t count);
			template<typename TValue> void read(TValue& value);
			template<typename TValue> void read(TValue* values, uint64_t count);

			template<std::integral TValue> void readAsciiNumber(TValue& value);
			template<std::floating_point TValue> void readAsciiNumber(TValue& value);

			template<typename TConditionFunc> void skipCharWhile(TConditionFunc conditionFunc, uint64_t& count);
			template<typename TConditionFunc> void readCharWhile(TConditionFunc conditionFunc, char* dst, uint64_t dstSize, uint64_t& count);
			template<typename TConditionFunc> void readCharWhile(TConditionFunc conditionFunc, std::string& dst);


			void bitUnread(uint64_t bitCount);
			void bitRead(bool& bit);
			void bitRead(uint8_t* data, uint64_t bitCount, uint8_t bitOffset = 0);

			void finishByte();


			inline bool eof() const;

			constexpr void setByteEndianness(std::endian endianness);
			constexpr std::endian getByteEndianness() const;
			constexpr void setBitEndianness(std::endian endianness);
			constexpr std::endian getBitEndianness() const;
			constexpr const ruc::Status& getStatus() const;

			~IStream();

		private:

			void _refillBuffer(uint64_t size);

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


			template<typename TValue> void write(const TValue& value);
			template<typename TValue> void write(const TValue* values, uint64_t count);

			template<std::integral TValue> void writeAsciiNumber(TValue value);
			template<std::floating_point TValue> void writeAsciiNumber(TValue value);


			void bitWrite(bool bit);
			void bitWrite(const uint8_t* data, uint64_t bitCount, uint8_t bitOffset = 0);

			void finishByte(uint8_t padBits = 0);


			void flush();


			constexpr void setByteEndianness(std::endian endianness);
			constexpr std::endian getByteEndianness() const;
			constexpr void setBitEndianness(std::endian endianness);
			constexpr std::endian getBitEndianness() const;
			constexpr const ruc::Status& getStatus() const;

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

#define DSK_CHECK(condition, message)	RUC_CHECK(_status, RUC_VOID, condition, message)
#define DSK_CALL(func, ...)				func(__VA_ARGS__); RUC_RELAY(_status, RUC_VOID)