///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Format/FormatTypes.hpp>

namespace dsk
{
	namespace fmt
	{
		// namespace zlib
		// {
		// 	enum class CompressionMethod : uint8_t
		// 	{
		// 		Deflate = 0x8
		// 	};
		// 
		// 	enum class CompressionLevel : uint8_t
		// 	{
		// 		FastestAlgorithm	= 0b00,
		// 		FastAlgorithm		= 0b01,
		// 		DefaultAlgorithm	= 0b10,
		// 		MaximumCompression	= 0b11
		// 	};
		// 
		// 	struct Header
		// 	{
		// 		CompressionMethod compressionMethod;
		// 		uint8_t compressionInfo;
		// 		CompressionLevel compressionLevel;
		// 		std::optional<uint32_t> dictId;
		// 	};
		// 
		// 	struct File
		// 	{
		// 		Header header;
		// 		std::vector<uint8_t> data;
		// 	};
		// }
		// 
		// class ZlibIStream : public FormatIStream
		// {
		// 	public:
		// 
		// 		ZlibIStream();
		// 		ZlibIStream(const ZlibIStream& stream) = delete;
		// 		ZlibIStream(ZlibIStream&& stream) = delete;
		// 
		// 		ZlibIStream& operator=(const ZlibIStream& stream) = delete;
		// 		ZlibIStream& operator=(ZlibIStream&& stream) = delete;
		// 
		// 		const ruc::Status& readFile(zlib::File& file);
		// 		const ruc::Status& readHeader(zlib::Header& header);
		// 		const ruc::Status& readData(void* data, uint64_t size, uint64_t& sizeRead);
		// 		const ruc::Status& readEndFile();
		// 
		// 		~ZlibIStream() = default;
		// 
		// 	private:
		// 
		// 		void onSourceRemoved() override final;
		// 
		// 		DeflateIStream* _deflateStream;
		// 
		// 		bool _headerRead;
		// 		bool _readingLastBlock;
		// 
		// 		uint32_t _checksum;
		// };
		// 
		// class ZlibOStream : public FormatOStream
		// {
		// 	public:
		// 
		// 		ZlibOStream();
		// 		ZlibOStream(const ZlibOStream& stream) = delete;
		// 		ZlibOStream(ZlibOStream&& stream) = delete;
		// 
		// 		ZlibOStream& operator=(const ZlibOStream& stream) = delete;
		// 		ZlibOStream& operator=(ZlibOStream&& stream) = delete;
		// 
		// 		const ruc::Status& writeFile(const zlib::File& file);
		// 		const ruc::Status& writeHeader(const zlib::Header& header, const deflate::BlockHeader& firstBlockHeader);
		// 		const ruc::Status& writeData(const void* data, uint64_t size, deflate::BlockHeader* blockHeader = nullptr);
		// 		const ruc::Status& writeEndFile();
		// 
		// 		~ZlibOStream() = default;
		// 
		// 	private:
		// 
		// 		void onDestinationRemoved() override final;
		// 
		// 		DeflateOStream* _deflateStream;
		// 
		// 		bool _headerWritten;
		// 		deflate::BlockHeader _lastBlockHeader;
		// 		
		// 		uint32_t _checksum;
		// };
	}
}
