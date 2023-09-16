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
		
		
				template<typename TSample> const ruc::Status& readFile(wave::File<TSample>& file);
				const ruc::Status& readHeader(wave::Header& header);
				template<typename TSample> const ruc::Status& readSampleBlocks(TSample* samples, uint32_t blockCount);
				const ruc::Status& skipBlocks(uint32_t blockCount);
				const ruc::Status& readEndFile();
		
				~WaveIStream();
		
			private:
		
				void setStreamState() override final;
				void resetFormatState() override final;
		
				const ruc::Status& _readFormatChunk(const riff::ChunkHeader& chunkHeader);
				// TODO: Split _readFormatChunk into sub functions for each format
		
				const ruc::Status& _readFactChunk(const riff::ChunkHeader& chunkHeader);
				// TODO: Read other optionnal chunks
		
				template<typename TRaw, typename TSample> const ruc::Status& _readRawSampleBlocks(TSample* samples, uint32_t blockCount);
		
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
		
		
				template<typename TSample> const ruc::Status& writeFile(const wave::File<TSample>& file);
				const ruc::Status& writeHeader(const wave::Header& header);
				template<typename TSample> const ruc::Status& writeSampleBlocks(const TSample* samples, uint32_t blockCount);
		
				~WaveOStream();
		
			private:
		
				void setStreamState() override final;
				void resetFormatState() override final;
		
				template<typename TRaw, typename TSample> const ruc::Status& _writeRawSampleBlocks(const TSample* samples, uint32_t blockCount);
		
				bool _headerWritten;
				wave::Header _header;
				uint32_t _remainingBlocks;

				RiffOStream* _riffStream;
		};
	}
}
