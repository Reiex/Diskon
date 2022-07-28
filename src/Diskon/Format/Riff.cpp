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
			FMTSTREAM_BEGIN_READ();

			riff::FileHeader fileHeader;
			FMTSTREAM_VERIFY_CALL(readFileHeader, fileHeader);

			std::copy_n(fileHeader.formType, 4, file.formType);
			file.chunks.clear();

			fileHeader.size -= 4;

			riff::Chunk chunk;
			while (fileHeader.size)
			{
				FMTSTREAM_VERIFY(fileHeader.size >= 8, RiffInvalidFileSize, "RiffStream: Error while reading chunk. Expected remaining file size to be more than 8, got " + std::to_string(fileHeader.size));
				fileHeader.size -= 8;

				FMTSTREAM_VERIFY_CALL(readChunk, chunk);

				FMTSTREAM_VERIFY(fileHeader.size >= chunk.data.size(), RiffInvalidFileSize, "RiffStream: Error while reading chunk. Expected remaining file size to be more than chunk size (" + std::to_string(chunk.data.size()) + "), got " + std::to_string(fileHeader.size));
				fileHeader.size -= chunk.data.size();

				file.chunks.push_back(chunk);
			}

			return error;
		}

		const FormatError& RiffStream::readFileHeader(riff::FileHeader& fileHeader)
		{
			FMTSTREAM_BEGIN_READ();

			char riffId[4];
			FMTSTREAM_VERIFY_CALL(streamRead, riffId, 4);
			FMTSTREAM_VERIFY(std::equal(riffId, riffId + 4, "RIFF"), RiffIdentifierNotFound, "RiffStream: 'RIFF' identifier not found.");

			FMTSTREAM_VERIFY_CALL(streamRead, fileHeader.size);
			FMTSTREAM_VERIFY(fileHeader.size >= 4, RiffInvalidFileSize, "RiffStream: Invalid file size. Expected more than 4, got " + std::to_string(fileHeader.size));

			FMTSTREAM_VERIFY_CALL(streamRead, fileHeader.formType, 4);

			return error;
		}

		const FormatError& RiffStream::readChunk(riff::Chunk& chunk)
		{
			FMTSTREAM_BEGIN_READ();

			riff::ChunkHeader chunkHeader;
			FMTSTREAM_VERIFY_CALL(readChunkHeader, chunkHeader);

			std::copy_n(chunkHeader.id, 4, chunk.id);
			chunk.data.resize(chunkHeader.size);

			FMTSTREAM_VERIFY_CALL(readChunkData, chunk.data.data(), chunkHeader.size);

			if (chunkHeader.size % 2)
			{
				uint8_t padByte;
				FMTSTREAM_VERIFY_CALL(readChunkData, &padByte, 1);

				FMTSTREAM_VERIFY(padByte == 0, RiffInvalidPadByte, "RiffStream: Bad pad byte value. Expected 0, got " + std::to_string(padByte));
			}

			return error;
		}

		const FormatError& RiffStream::readChunkHeader(riff::ChunkHeader& chunkHeader)
		{
			FMTSTREAM_BEGIN_READ();

			FMTSTREAM_VERIFY_CALL(streamRead, chunkHeader.id, 4);
			FMTSTREAM_VERIFY_CALL(streamRead, chunkHeader.size);

			return error;
		}

		const FormatError& RiffStream::readChunkData(uint8_t* data, uint32_t size)
		{
			FMTSTREAM_BEGIN_READ();

			FMTSTREAM_VERIFY_CALL(streamRead, data, size);

			return error;
		}

		const FormatError& RiffStream::writeFile(const riff::File& file)
		{
			FMTSTREAM_BEGIN_WRITE();

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

			FMTSTREAM_BEGIN_WRITE();

			FMTSTREAM_VERIFY_CALL(streamWrite, "RIFF", 4);
			FMTSTREAM_VERIFY_CALL(streamWrite, fileHeader.size);
			FMTSTREAM_VERIFY_CALL(streamWrite, fileHeader.formType, 4);

			return error;
		}

		const FormatError& RiffStream::writeChunk(const riff::Chunk& chunk)
		{
			FMTSTREAM_BEGIN_WRITE();

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
			FMTSTREAM_BEGIN_WRITE();

			FMTSTREAM_VERIFY_CALL(streamWrite, chunkHeader.id, 4);
			FMTSTREAM_VERIFY_CALL(streamWrite, chunkHeader.size);

			return error;
		}

		const FormatError& RiffStream::writeChunkData(const uint8_t* data, uint32_t size)
		{
			FMTSTREAM_BEGIN_WRITE();

			FMTSTREAM_VERIFY_CALL(streamWrite, data, size);

			return error;
		}
	}
}
