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
		namespace wave
		{
			enum class Format : uint16_t
			{
				PCM			= 0x0001,
				Float		= 0x0003,
				// ALaw			= 0x0006,
				// MuLaw		= 0x0007,
				// Extensible	= 0xFFFE
			};
		
			struct Metadata
			{
				Format format;
				uint16_t channelCount;
				uint32_t frequency;
				uint16_t bitsPerSample;
			};
		
			struct Header
			{
				Metadata metadata;
				uint32_t blockCount;	// A block is a set of `channelCount` samples
			};
		
			template<typename TSample>
			struct File
			{
				Metadata metadata;
				std::vector<TSample> samples;	// Size is a multiple of `blockCount`
			};
		}
		
		class DSK_API WaveIStream : public FormatIStream
		{
			public:
		
				WaveIStream(IStream* stream);
				WaveIStream(const WaveIStream& stream) = delete;
				WaveIStream(WaveIStream&& stream) = delete;
		
				WaveIStream& operator=(const WaveIStream& stream) = delete;
				WaveIStream& operator=(WaveIStream&& stream) = delete;
		
		
				template<typename TSample> void readFile(wave::File<TSample>& file);
				void readHeader(wave::Header& header);
				template<typename TSample> void readSampleBlocks(TSample* samples, uint32_t blockCount);
				void skipBlocks(uint32_t blockCount);
				void readEndFile();
		
				~WaveIStream();
		
			private:
		
				void setStreamState() override final;
				void resetFormatState() override final;
		
				void _readFormatChunk(const riff::ChunkHeader& chunkHeader);
				// TODO: Split _readFormatChunk into sub functions for each format
		
				void _readFactChunk(const riff::ChunkHeader& chunkHeader);
				// TODO: Read other optionnal chunks
		
				template<typename TRaw, typename TSample> void _readRawSampleBlocks(TSample* samples, uint32_t blockCount);
		
				bool _headerRead;
				wave::Header _header;
				uint32_t _remainingBlocks;

				RiffIStream* _riffStream;
		};
		
		class DSK_API WaveOStream : public FormatOStream
		{
			public:
		
				WaveOStream(OStream* stream);
				WaveOStream(const WaveOStream& stream) = delete;
				WaveOStream(WaveOStream&& stream) = delete;
		
				WaveOStream& operator=(const WaveOStream& stream) = delete;
				WaveOStream& operator=(WaveOStream&& stream) = delete;
		
		
				template<typename TSample> void writeFile(const wave::File<TSample>& file);
				void writeHeader(const wave::Header& header);
				template<typename TSample> void writeSampleBlocks(const TSample* samples, uint32_t blockCount);
		
				~WaveOStream();
		
			private:
		
				void setStreamState() override final;
				void resetFormatState() override final;
		
				template<typename TRaw, typename TSample> void _writeRawSampleBlocks(const TSample* samples, uint32_t blockCount);
		
				bool _headerWritten;
				wave::Header _header;
				uint32_t _remainingBlocks;

				RiffOStream* _riffStream;
		};
	}
}
