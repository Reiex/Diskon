#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace wave
		{
			enum class WaveFormat : uint16_t
			{
				Unknown		= 0x0000,
				PCM			= 0x0001,
				Float		= 0x0003,
				// ALaw			= 0x0006,
				// MuLaw		= 0x0007,
				// Extensible	= 0xFFFE
			};

			struct FormatChunk
			{
				FormatChunk();
				FormatChunk(const FormatChunk& formatChunk) = default;
				FormatChunk(FormatChunk&& formatChunk) = default;

				FormatChunk& operator=(const FormatChunk& formatChunk) = default;
				FormatChunk& operator=(FormatChunk&& formatChunk) = default;

				WaveFormat formatTag;		///< Format type (usually PCM or Float).
				uint16_t channels;			///< Number of channels (1: mono, 2: stereo, 3: 2.1, etc...).
				uint32_t samplesPerSec;		///< Number of samples per second on one channel (usually 44100).
				uint32_t avgBytesPerSec;	///< Usually `blockAlign*samplesPerSec`.
				uint16_t blockAlign;		///< Offset between two blocks of samples (usually `bitsPerSample*samplesPerSec/8`).
				uint16_t bitsPerSample;		///< Number of bits in each sample (usually 16 or 32).
				uint16_t extSize;			///< Size of the extension chunk (usually 0).

				bool isValid() const;
				void clear();

				~FormatChunk() = default;
			};
		}

		class WaveFile : public FormatHandler
		{
			public:

				WaveFile();
				WaveFile(const WaveFile& file) = default;
				WaveFile(WaveFile&& file) = default;

				WaveFile& operator=(const WaveFile& file) = default;
				WaveFile& operator=(WaveFile&& file) = default;

				const wave::FormatChunk& getFormatChunk() const;
				uint32_t getSampleCount() const;
				void getSamples(uint16_t channel, std::vector<int8_t>& samples) const;
				void getSamples(uint16_t channel, std::vector<int16_t>& samples) const;
				void getSamples(uint16_t channel, std::vector<int32_t>& samples) const;
				void getSamples(uint16_t channel, std::vector<int64_t>& samples) const;
				void getSamples(uint16_t channel, std::vector<float>& samples) const;
				void getSamples(uint16_t channel, std::vector<double>& samples) const;

				void setFormatChunk(const wave::FormatChunk& formatChunk, uint32_t sampleCount);
				void setSamples(uint16_t channel, const int8_t* samples);
				void setSamples(uint16_t channel, const int16_t* samples);
				void setSamples(uint16_t channel, const int32_t* samples);
				void setSamples(uint16_t channel, const int64_t* samples);
				void setSamples(uint16_t channel, const float* samples);
				void setSamples(uint16_t channel, const double* samples);

				void clear() override;

				~WaveFile() = default;

			private:

				void read(std::istream& stream, IOResult& result) override;
				void write(std::ostream& stream, IOResult& result) override;

				template<typename TTo> void getRawSamples(uint16_t channel, void* data) const;
				template<typename TFrom> void setRawSamples(uint16_t channel, const void* data);

				wave::FormatChunk _formatChunk;
				uint32_t _sampleCount;
				std::vector<char> _rawSamples;
		};
	}
}

#include <Diskon/Format/templates/Wave.hpp>
