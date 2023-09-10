///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Marius Pélégrin
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

				const ruc::Status& readFile(riff::File& file);
				const ruc::Status& readFileHeader(riff::FileHeader& header);
				const ruc::Status& readChunk(riff::Chunk& chunk);
				const ruc::Status& readChunkHeader(riff::ChunkHeader& header);
				template<typename TValue> const ruc::Status& readChunkData(TValue& value);
				template<typename TValue> const ruc::Status& readChunkData(TValue* values, uint32_t count);
				const ruc::Status& skipChunkData(uint32_t size);
				const ruc::Status& finishCurrentChunk();

				uint32_t computeRemainingSize() const;	// TODO ? Remove or rewrite ?

				~RiffIStream() = default;

			private:

				void setStreamState() override final;
				void resetFormatState() override final;

				const ruc::Status& _readChunk(riff::Chunk& chunk);
				const ruc::Status& _readChunkHeader(riff::ChunkHeader& header);
				const ruc::Status& _readChunkEnd();

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

				const ruc::Status& writeFile(const riff::File& file);
				const ruc::Status& writeFileHeader(const riff::FileHeader& header);
				const ruc::Status& writeChunk(const riff::Chunk& chunk);
				const ruc::Status& writeChunkHeader(const riff::ChunkHeader& header);
				template<typename TValue> const ruc::Status& writeChunkData(const TValue& value);
				template<typename TValue> const ruc::Status& writeChunkData(const TValue* values, uint32_t count);

				uint32_t computeRemainingSize() const;	// TODO ? Remove or rewrite ?

				~RiffOStream() = default;

			private:

				void setStreamState() override final;
				void resetFormatState() override final;

				const ruc::Status& _writeChunk(const riff::Chunk& chunk);
				const ruc::Status& _writeChunkHeader(const riff::ChunkHeader& header);
				const ruc::Status& _writeChunkEnd();

				std::vector<std::array<uint32_t, 2>> _remainingSizes;
				bool _writingData;
		};
	}
}
