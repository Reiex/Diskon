#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		void RiffFile::clear()
		{
			formType.fill(0);
			chunks.clear();
		}

		void RiffFile::read(std::istream& stream, IOResult& result)
		{
			char riffId[4];
			stream.read(riffId, 4);
			if (std::strcmp(riffId, "RIFF"))
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "'RIFF' identifier not found";
				return;
			}

			unsigned long size;
			stream.read((char*) &size, 4);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while reading RIFF size";
				return;
			}
			else if (size < 4)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "RIFF size less than 4 bytes";
				return;
			}

			stream.read(formType.data(), 4);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while reading RIFF form-type";
				return;
			}

			unsigned long remainingSize = size - 4;
			while (remainingSize)
			{
				chunks.emplace_back();
				readChunk(stream, result, chunks.back(), remainingSize);

				if (result.failedStep != IOResult::FailedStep::None)
				{
					return;
				}
			}
		}

		void RiffFile::write(std::ostream& stream, IOResult& result)
		{
			stream.write("RIFF", 4);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while writing 'RIFF'";
				return;
			}

			unsigned long size = 4;
			for (const riff::Chunk& chunk : chunks)
			{
				size += 8 + chunk.data.size();
			}

			stream.write((char*) &size, 4);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while writing RIFF size";
				return;
			}

			stream.write(formType.data(), 4);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while writing RIFF form-type";
				return;
			}

			for (const riff::Chunk& chunk : chunks)
			{
				writeChunk(stream, result, chunk);

				if (result.failedStep != IOResult::FailedStep::None)
				{
					return;
				}
			}
		}

		void RiffFile::readChunk(std::istream& stream, IOResult& result, riff::Chunk& chunk, unsigned long& remainingSize)
		{
			if (remainingSize < 8)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "RIFF chunk size less than 8 bytes";
				return;
			}

			stream.read(chunk.id.data(), 4);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while reading RIFF chunk ID";
				return;
			}

			unsigned long size;
			stream.read((char*)& size, 4);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while reading RIFF chunk size";
				return;
			}

			if (size > remainingSize)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "RIFF chunk size bigger than remaining size";
				return;
			}

			chunk.data.resize(size);

			stream.read(chunk.data.data(), size);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while read RIFF chunk data";
				return;
			}
		}

		void RiffFile::writeChunk(std::ostream& stream, IOResult& result, const riff::Chunk& chunk)
		{
			stream.write(chunk.id.data(), 4);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while writing RIFF chunk ID";
				return;
			}

			unsigned long chunkSize = chunk.data.size();
			stream.write((char*) &chunkSize, 4);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while writing RIFF chunk size";
				return;
			}

			stream.write(chunk.data.data(), chunkSize);
			if (!stream)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Error while writing RIFF chunk data";
				return;
			}
		}
	}
}
