///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Marius Pélégrin
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		// ZlibIStream::ZlibIStream() : FormatIStream(std::endian::big),
		// 	_deflateStream(nullptr),
		// 	_headerRead(false),
		// 	_readingLastBlock(false),
		// 	_checksum(1)
		// {
		// 	FMT_CREATE_SUBSTREAM(_deflateStream, DeflateIStream);
		// 
		// 	finishSubStreamTree();
		// }
		// 
		// const ruc::Status& ZlibIStream::readFile(zlib::File& file)
		// {
		// 	FMT_BEGIN("ZlibIStream::readFile(zlib::File& file)");
		// 
		// 	file.data.clear();
		// 
		// 	// Read header
		// 
		// 	FMT_CALL(readHeader, file.header);
		// 
		// 	// Read data
		// 
		// 	constexpr uint64_t bufferSize = 1024;
		// 	uint8_t buffer[bufferSize];
		// 	uint64_t sizeRead;
		// 	do {
		// 		FMT_CALL(readData, buffer, bufferSize, sizeRead);
		// 		file.data.insert(file.data.end(), buffer, buffer + sizeRead);
		// 	} while (sizeRead == bufferSize);
		// 
		// 	// Read end
		// 
		// 	FMT_CALL(readEndFile);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& ZlibIStream::readHeader(zlib::Header& header)
		// {
		// 	FMT_BEGIN("ZlibIStream::readHeader(zlib::Header& header)");
		// 
		// 	assert(!_headerRead);
		// 
		// 	header.dictId.reset();
		// 
		// 	// Read CMF & FLG bytes
		// 
		// 	uint16_t buffer;
		// 	FMT_READ(buffer);
		// 
		// 	FMT_VERIFY(buffer % 31 == 0, ZlibInvalidFlagCheck, "Expected 2 first bytes of header to be a multiple of 31.");
		// 	buffer >>= 5;
		// 
		// 	if (buffer & 1)
		// 	{
		// 		header.dictId.emplace();
		// 		FMT_READ(header.dictId.value());
		// 	}
		// 	buffer >>= 1;
		// 
		// 	header.compressionLevel = static_cast<zlib::CompressionLevel>(buffer & 3);
		// 	buffer >>= 2;
		// 
		// 	FMT_VERIFY((buffer & 15) == 8, ZlibInvalidCompressionMethod, "Expected compression method to be 8 (deflate). Instead, got " + std::to_string(buffer & 15) + "'.");
		// 	header.compressionMethod = static_cast<zlib::CompressionMethod>(buffer & 15);
		// 	buffer >>= 4;
		// 
		// 	header.compressionInfo = buffer;
		// 	// TODO: Set deflate window size to 2^(header.compressionInfo + 8)
		// 	// TODO: Verify it is less than 8
		// 	
		// 	// Read deflate block header
		// 
		// 	deflate::BlockHeader blockHeader;
		// 	FMT_SUB_CALL(_deflateStream, readBlockHeader, blockHeader);
		// 
		// 	_headerRead = true;
		// 	_readingLastBlock = blockHeader.isFinal;
		// 	_checksum = 1;
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& ZlibIStream::readData(void* data, uint64_t size, uint64_t& sizeRead)
		// {
		// 	FMT_BEGIN("ZlibIStream::readData(void* data, uint64_t size, uint64_t& sizeRead)");
		// 
		// 	assert(_headerRead);
		// 
		// 	deflate::BlockHeader blockHeader;
		// 	uint64_t blockSizeRead;
		// 	sizeRead = 0;
		// 
		// 	uint8_t* it = reinterpret_cast<uint8_t*>(data);
		// 	const uint8_t* const itEnd = it + size;
		// 	do {
		// 		FMT_SUB_CALL(_deflateStream, readBlockData, it, size, blockSizeRead);
		// 		
		// 		if (blockSizeRead == 0 && !_readingLastBlock)
		// 		{
		// 			FMT_SUB_CALL(_deflateStream, readBlockEnd);
		// 			FMT_SUB_CALL(_deflateStream, readBlockHeader, blockHeader);
		// 			FMT_SUB_CALL(_deflateStream, readBlockData, it, size, blockSizeRead);
		// 			_readingLastBlock = blockHeader.isFinal;
		// 		}
		// 
		// 		sizeRead += blockSizeRead;
		// 		it += blockSizeRead;
		// 		size -= blockSizeRead;
		// 
		// 	} while (it != itEnd && !(_readingLastBlock && blockSizeRead == 0));
		// 
		// 	_checksum = cksm::adler32(data, sizeRead, _checksum);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& ZlibIStream::readEndFile()
		// {
		// 	FMT_BEGIN("ZlibIStream::readEndFile()");
		// 
		// 	assert(_readingLastBlock);
		// 
		// 	FMT_SUB_CALL(_deflateStream, readBlockEnd);
		// 
		// 	uint32_t checksum;
		// 	FMT_READ(checksum);
		// 	FMT_VERIFY(checksum == _checksum, ZlibInvalidChecksum, "Checksum computed and read are not equal.");
		// 
		// 	_headerRead = false;
		// 	_readingLastBlock = false;
		// 	_checksum = 1;
		// 
		// 	return *_error;
		// }
		// 
		// void ZlibIStream::onSourceRemoved()
		// {
		// 	_headerRead = false;
		// 	_readingLastBlock = false;
		// 	_checksum = 1;
		// }
		// 
		// 
		// ZlibOStream::ZlibOStream() : FormatOStream(std::endian::big),
		// 	_deflateStream(nullptr),
		// 	_headerWritten(false),
		// 	_lastBlockHeader(),
		// 	_checksum(1)
		// {
		// 	FMT_CREATE_SUBSTREAM(_deflateStream, DeflateOStream);
		// 
		// 	finishSubStreamTree();
		// }
		// 
		// const ruc::Status& ZlibOStream::writeFile(const zlib::File& file)
		// {
		// 	FMT_BEGIN("ZlibOStream::writeFile(const zlib::File& file)");
		// 
		// 	assert(!_headerWritten);
		// 
		// 	// TODO: better compression
		// 	deflate::BlockHeader blockHeader;
		// 	blockHeader.isFinal = true;
		// 	blockHeader.compressionType = deflate::CompressionType::FixedHuffman;
		// 
		// 	FMT_CALL(writeHeader, file.header, blockHeader);
		// 	FMT_CALL(writeData, file.data.data(), file.data.size());
		// 	FMT_CALL(writeEndFile);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& ZlibOStream::writeHeader(const zlib::Header& header, const deflate::BlockHeader& firstBlockHeader)
		// {
		// 	FMT_BEGIN("ZlibOStream::writeHeader(const zlib::Header& header, const deflate::BlockHeader& firstBlockHeader)");
		// 
		// 	assert(!_headerWritten);
		// 
		// 	uint16_t buffer = 0;
		// 
		// 	assert(header.compressionInfo < 16);
		// 	buffer |= header.compressionInfo;
		// 
		// 	buffer <<= 4;
		// 	assert(header.compressionMethod == zlib::CompressionMethod::Deflate);
		// 	buffer |= static_cast<uint8_t>(header.compressionMethod);
		// 
		// 	buffer <<= 2;
		// 	buffer |= static_cast<uint8_t>(header.compressionLevel);
		// 
		// 	buffer <<= 1;
		// 	buffer |= header.dictId.has_value() ? 1 : 0;
		// 
		// 	buffer <<= 5;
		// 	buffer += 31 - (buffer % 31);
		// 
		// 	FMT_WRITE(buffer);
		// 
		// 	if (header.dictId.has_value())
		// 	{
		// 		FMT_WRITE(header.dictId.value());
		// 	}
		// 
		// 	FMT_SUB_CALL(_deflateStream, writeBlockHeader, firstBlockHeader);
		// 
		// 	_headerWritten = true;
		// 	_lastBlockHeader = firstBlockHeader;
		// 	_checksum = 1;
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& ZlibOStream::writeData(const void* data, uint64_t size, deflate::BlockHeader* blockHeader)
		// {
		// 	FMT_BEGIN("ZlibOStream::writeData(const void* data, uint64_t size, deflate::BlockHeader* blockHeader)");
		// 
		// 	assert(_headerWritten);
		// 	assert(!(blockHeader && _lastBlockHeader.isFinal));
		// 
		// 	if (blockHeader)
		// 	{
		// 		FMT_SUB_CALL(_deflateStream, writeBlockEnd);
		// 		FMT_SUB_CALL(_deflateStream, writeBlockHeader, *blockHeader);
		// 		_lastBlockHeader = *blockHeader;
		// 	}
		// 
		// 	FMT_SUB_CALL(_deflateStream, writeBlockData, data, size);
		// 
		// 	_checksum = cksm::adler32(data, size, _checksum);
		// 
		// 	return *_error;
		// }
		// 
		// const ruc::Status& ZlibOStream::writeEndFile()
		// {
		// 	FMT_BEGIN("ZlibOStream::writeEndFile()");
		// 
		// 	assert(_lastBlockHeader.isFinal);
		// 
		// 	FMT_SUB_CALL(_deflateStream, writeBlockEnd);
		// 	FMT_WRITE(_checksum);
		// 
		// 	return *_error;
		// }
		// 
		// void ZlibOStream::onDestinationRemoved()
		// {
		// 	_headerWritten = false;
		// 	_lastBlockHeader = deflate::BlockHeader();
		// 	_checksum = 1;
		// }
	}
}
