#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace wave
		{
			enum class Format : uint16_t
			{
				Unknown		= 0x0000,
				PCM			= 0x0001,
				Float		= 0x0003,
				// ALaw			= 0x0006,
				// MuLaw		= 0x0007,
				// Extensible	= 0xFFFE
			};

			struct Header
			{
				Format formatTag;			///! Format type (usually PCM or Float).
				uint16_t channels;			///! Number of channels (1: mono, 2: stereo, 3: 2.1, etc...).
				uint32_t samplesPerSec;		///! Number of samples per second on one channel (usually 44100).
				uint32_t avgBytesPerSec;	///! Usually `blockAlign*samplesPerSec`.
				uint16_t blockAlign;		///! Offset between two blocks of samples (usually `bitsPerSample*channels/8`).
				uint16_t bitsPerSample;		///! Number of bits in each sample (usually 16 or 32).
				uint32_t blockCount;		///! Number of blocks of samples (time of the sound divided by sampling frequency).
				std::vector<uint8_t> extension;

				bool isValid() const;
			};

			struct File
			{
				public:

					Header header;
					std::vector<uint8_t> rawSamples;	///! Must be of size `sampleBlockCount * blockAlign`

					void getSamples(uint16_t channel, int8_t* samples) const;
					void getSamples(uint16_t channel, int16_t* samples) const;
					void getSamples(uint16_t channel, int32_t* samples) const;
					void getSamples(uint16_t channel, int64_t* samples) const;
					void getSamples(uint16_t channel, float* samples) const;
					void getSamples(uint16_t channel, double* samples) const;

					void setSamples(uint16_t channel, const int8_t* samples);
					void setSamples(uint16_t channel, const int16_t* samples);
					void setSamples(uint16_t channel, const int32_t* samples);
					void setSamples(uint16_t channel, const int64_t* samples);
					void setSamples(uint16_t channel, const float* samples);
					void setSamples(uint16_t channel, const double* samples);

					bool isValid() const;

				private:

					template<typename TTo> void _getSamples(uint16_t channel, TTo* samples) const;
					template<typename TFrom> void _setSamples(uint16_t channel, const TFrom* samples);

					template<typename TTo> static void castFromRawSamples(const Header& header, const void* raw, void* data, uint32_t offset, uint32_t stride, uint32_t count);
					template<typename TFrom> static void castToRawSamples(const Header& header, void* raw, const void* data, uint32_t offset, uint32_t stride, uint32_t count);

				friend class dsk::fmt::WaveStream;
			};
		}

		class WaveStream : public FormatStream
		{
			public:

				WaveStream() = default;
				WaveStream(const WaveStream& file) = default;
				WaveStream(WaveStream&& file) = default;

				WaveStream& operator=(const WaveStream& file) = default;
				WaveStream& operator=(WaveStream&& file) = default;

				const FormatError& readFile(wave::File& file);
				const FormatError& readHeader(wave::Header& header);
				const FormatError& readSampleBlock(const wave::Header& header, int8_t* sampleBlock);
				const FormatError& readSampleBlock(const wave::Header& header, int16_t* sampleBlock);
				const FormatError& readSampleBlock(const wave::Header& header, int32_t* sampleBlock);
				const FormatError& readSampleBlock(const wave::Header& header, int64_t* sampleBlock);
				const FormatError& readSampleBlock(const wave::Header& header, float* sampleBlock);
				const FormatError& readSampleBlock(const wave::Header& header, double* sampleBlock);

				const FormatError& writeFile(const wave::File& file);
				const FormatError& writeHeader(const wave::Header& header);
				const FormatError& writeSampleBlock(const wave::Header& header, const int8_t* sampleBlock);
				const FormatError& writeSampleBlock(const wave::Header& header, const int16_t* sampleBlock);
				const FormatError& writeSampleBlock(const wave::Header& header, const int32_t* sampleBlock);
				const FormatError& writeSampleBlock(const wave::Header& header, const int64_t* sampleBlock);
				const FormatError& writeSampleBlock(const wave::Header& header, const float* sampleBlock);
				const FormatError& writeSampleBlock(const wave::Header& header, const double* sampleBlock);

				~WaveStream() = default;

			private:

				template<typename TTo> const FormatError& _readSampleBlock(const wave::Header& header, TTo* sampleBlock);
				template<typename TFrom> const FormatError& _writeSampleBlock(const wave::Header& header, const TFrom* sampleBlock);
		};
	}
}

#include <Diskon/Format/templates/Wave.hpp>
