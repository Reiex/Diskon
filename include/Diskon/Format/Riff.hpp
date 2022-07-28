#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace riff
		{
			struct ChunkHeader
			{
				char id[4];
				uint32_t size;	// Number of bytes in chunk data. Total chunk size - 8.
			};

			struct Chunk
			{
				char id[4];
				std::vector<uint8_t> data;
			};

			struct FileHeader
			{
				uint32_t size;	// Number of bytes in each Chunk (including id) + size of formType (4 bytes). File total size - 8.
				char formType[4];
			};

			struct File
			{
				char formType[4];
				std::vector<Chunk> chunks;
			};
		}

		class RiffStream : public FormatStream
		{
			public:

				RiffStream();
				RiffStream(const RiffStream& stream) = default;
				RiffStream(RiffStream&& stream) = default;

				RiffStream& operator=(const RiffStream& stream) = default;
				RiffStream& operator=(RiffStream&& stream) = default;

				const FormatError& readFile(riff::File& file);
				const FormatError& readFileHeader(riff::FileHeader& fileHeader);
				const FormatError& readChunk(riff::Chunk& chunk);
				const FormatError& readChunkHeader(riff::ChunkHeader& chunkHeader);
				const FormatError& readChunkData(uint8_t* data, uint32_t size);

				const FormatError& writeFile(const riff::File& file);
				const FormatError& writeFileHeader(const riff::FileHeader& fileHeader);
				const FormatError& writeChunk(const riff::Chunk& chunk);
				const FormatError& writeChunkHeader(const riff::ChunkHeader& chunkHeader);
				const FormatError& writeChunkData(const uint8_t* data, uint32_t size);

				~RiffStream() = default;
		};
	}
}
