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
		namespace riff
		{
			enum class ChunkType
			{
				StandardChunk,
				RiffChunk,
				ListChunk
			};

			struct ChunkHeader
			{
				ChunkType type;
				char id[4];
				uint32_t size;	// Result of chunk.computeInnerSize()
			};

			struct Chunk
			{
				ChunkType type;
				char id[4];
				std::vector<Chunk> subChunks;
				std::vector<uint8_t> data;

				constexpr uint32_t computeInnerSize() const;
				constexpr uint32_t computeTotalSize() const;
			};
		}

		class DSK_API RiffIStream : public FormatIStream
		{
			public:

				RiffIStream(IStream* stream);
				RiffIStream(const RiffIStream& stream) = delete;
				RiffIStream(RiffIStream&& stream) = delete;

				RiffIStream& operator=(const RiffIStream& stream) = delete;
				RiffIStream& operator=(RiffIStream&& stream) = delete;

				void readFile(riff::File& file);
				void readFileHeader(riff::FileHeader& header);
				void readChunk(riff::Chunk& chunk);
				void readChunkHeader(riff::ChunkHeader& header);
				template<typename TValue> void readChunkData(TValue& value);
				template<typename TValue> void readChunkData(TValue* values, uint32_t count);
				void skipChunkData(uint32_t size);
				void finishCurrentChunk();

				uint32_t computeRemainingSize() const;	// TODO ? Remove or rewrite ?

				~RiffIStream() = default;

			private:

				void setStreamState() override final;
				void resetFormatState() override final;

				void _readChunk(riff::Chunk& chunk);
				void _readChunkHeader(riff::ChunkHeader& header);
				void _readChunkEnd();

				std::vector<std::array<uint32_t, 2>> _remainingSizes;
				bool _readingData;
		};

		class DSK_API RiffOStream : public FormatOStream
		{
			public:

				RiffOStream(OStream* stream);
				RiffOStream(const RiffOStream& stream) = delete;
				RiffOStream(RiffOStream&& stream) = delete;

				RiffOStream& operator=(const RiffOStream& stream) = delete;
				RiffOStream& operator=(RiffOStream&& stream) = delete;

				void writeFile(const riff::File& file);
				void writeFileHeader(const riff::FileHeader& header);
				void writeChunk(const riff::Chunk& chunk);
				void writeChunkHeader(const riff::ChunkHeader& header);
				template<typename TValue> void writeChunkData(const TValue& value);
				template<typename TValue> void writeChunkData(const TValue* values, uint32_t count);

				uint32_t computeRemainingSize() const;	// TODO ? Remove or rewrite ?

				~RiffOStream() = default;

			private:

				void setStreamState() override final;
				void resetFormatState() override final;

				void _writeChunk(const riff::Chunk& chunk);
				void _writeChunkHeader(const riff::ChunkHeader& header);
				void _writeChunkEnd();

				std::vector<std::array<uint32_t, 2>> _remainingSizes;
				bool _writingData;
		};
	}
}
