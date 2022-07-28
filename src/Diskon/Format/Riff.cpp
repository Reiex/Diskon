#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		RiffStream::RiffStream() : FormatStream(std::endian::little)
		{
		}

		const FormatError& RiffStream::readFile(riff::File& file)
		{
			FMTSTREAM_BEGIN_READ_FUNC("RiffStream::readFile(riff::File& file)");

			riff::FileHeader fileHeader;
			FMTSTREAM_VERIFY_CALL(readFileHeader, fileHeader);

			std::copy_n(fileHeader.formType, 4, file.formType);
			file.chunks.clear();

			fileHeader.size -= 4;

			riff::Chunk chunk;
			while (fileHeader.size)
			{
				FMTSTREAM_VERIFY(fileHeader.size >= 8, RiffInvalidFileSize, "Error while reading chunk. Expected remaining file size to be more than 8, got " + std::to_string(fileHeader.size));
				fileHeader.size -= 8;

				FMTSTREAM_VERIFY_CALL(readChunk, chunk);

				FMTSTREAM_VERIFY(fileHeader.size >= chunk.data.size(), RiffInvalidFileSize, "Error while reading chunk. Expected remaining file size to be more than chunk size (" + std::to_string(chunk.data.size()) + "), got " + std::to_string(fileHeader.size));
				fileHeader.size -= chunk.data.size();

				file.chunks.push_back(chunk);
			}

			return error;
		}

		const FormatError& RiffStream::readFileHeader(riff::FileHeader& fileHeader)
		{
			FMTSTREAM_BEGIN_READ_FUNC("RiffStream::readFileHeader(riff::FileHeader& fileHeader)");

			char riffId[4];
			FMTSTREAM_READ(riffId, 4);
			FMTSTREAM_VERIFY(std::equal(riffId, riffId + 4, "RIFF"), RiffIdentifierNotFound, "'RIFF' identifier not found.");

			FMTSTREAM_READ(fileHeader.size);
			FMTSTREAM_VERIFY(fileHeader.size >= 4, RiffInvalidFileSize, "Invalid file size. Expected more than 4, got " + std::to_string(fileHeader.size));

			FMTSTREAM_VERIFY_CALL(streamRead, fileHeader.formType, 4);

			return error;
		}

		const FormatError& RiffStream::readChunk(riff::Chunk& chunk)
		{
			FMTSTREAM_BEGIN_READ_FUNC("RiffStream::readChunk(riff::Chunk& chunk)");

			riff::ChunkHeader chunkHeader;
			FMTSTREAM_VERIFY_CALL(readChunkHeader, chunkHeader);

			std::copy_n(chunkHeader.id, 4, chunk.id);
			chunk.data.resize(chunkHeader.size);

			FMTSTREAM_VERIFY_CALL(readChunkData, chunk.data.data(), chunkHeader.size);

			if (chunkHeader.size % 2)
			{
				uint8_t padByte;
				FMTSTREAM_VERIFY_CALL(readChunkData, &padByte, 1);

				FMTSTREAM_VERIFY(padByte == 0, RiffInvalidPadByte, "Bad pad byte value. Expected 0, got " + std::to_string(padByte));
			}

			return error;
		}

		const FormatError& RiffStream::readChunkHeader(riff::ChunkHeader& chunkHeader)
		{
			FMTSTREAM_BEGIN_READ_FUNC("RiffStream::readChunkHeader(riff::ChunkHeader& chunkHeader)");

			FMTSTREAM_READ(chunkHeader.id, 4);
			FMTSTREAM_READ(chunkHeader.size);

			return error;
		}

		const FormatError& RiffStream::readChunkData(uint8_t* data, uint32_t size)
		{
			FMTSTREAM_BEGIN_READ_FUNC("RiffStream::readChunkData(uint8_t* data, uint32_t size)");

			FMTSTREAM_READ(data, size);

			return error;
		}

		const FormatError& RiffStream::writeFile(const riff::File& file)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("RiffStream::writeFile(const riff::File& file)");

			riff::FileHeader fileHeader;
			std::copy_n(file.formType, 4, fileHeader.formType);
			fileHeader.size = 4;
			for (const riff::Chunk& chunk : file.chunks)
			{
				fileHeader.size += 8 + chunk.data.size();
			}
			FMTSTREAM_VERIFY_CALL(writeFileHeader, fileHeader);

			for (const riff::Chunk& chunk : file.chunks)
			{
				FMTSTREAM_VERIFY_CALL(writeChunk, chunk);
			}

			return error;
		}

		const FormatError& RiffStream::writeFileHeader(const riff::FileHeader& fileHeader)
		{
			assert(fileHeader.size >= 4);

			FMTSTREAM_BEGIN_WRITE_FUNC("RiffStream::writeFileHeader(const riff::FileHeader& fileHeader)");

			FMTSTREAM_WRITE("RIFF", 4);
			FMTSTREAM_WRITE(fileHeader.size);
			FMTSTREAM_WRITE(fileHeader.formType, 4);

			return error;
		}

		const FormatError& RiffStream::writeChunk(const riff::Chunk& chunk)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("RiffStream::writeChunk(const riff::Chunk& chunk)");

			riff::ChunkHeader chunkHeader;
			std::copy_n(chunk.id, 4, chunkHeader.id);
			chunkHeader.size = chunk.data.size();
			FMTSTREAM_VERIFY_CALL(writeChunkHeader, chunkHeader);

			FMTSTREAM_VERIFY_CALL(writeChunkData, chunk.data.data(), chunk.data.size());
			if (chunkHeader.size % 2)
			{
				uint8_t padByte = 0;
				FMTSTREAM_VERIFY_CALL(writeChunkData, &padByte, 1);
			}

			return error;
		}

		const FormatError& RiffStream::writeChunkHeader(const riff::ChunkHeader& chunkHeader)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("RiffStream::writeChunkHeader(const riff::ChunkHeader& chunkHeader)");

			FMTSTREAM_WRITE(chunkHeader.id, 4);
			FMTSTREAM_WRITE(chunkHeader.size);

			return error;
		}

		const FormatError& RiffStream::writeChunkData(const uint8_t* data, uint32_t size)
		{
			FMTSTREAM_BEGIN_WRITE_FUNC("RiffStream::writeChunkData(const uint8_t* data, uint32_t size)");

			FMTSTREAM_WRITE(data, size);

			return error;
		}
	}
}
