#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace wave
		{
			FormatChunk::FormatChunk()
			{
				clear();
			}

			bool FormatChunk::isValid() const
			{
				bool success = true;

				switch (formatTag)
				{
					case WaveFormat::PCM:
					{
						success = success && (channels > 0 && channels <= 6);
						success = success && (samplesPerSec != 0);
						success = success && (avgBytesPerSec == blockAlign * samplesPerSec);
						success = success && (blockAlign == channels * bitsPerSample / 8);
						success = success && (bitsPerSample == 8 || bitsPerSample == 16 || bitsPerSample == 32 || bitsPerSample == 64);
						success = success && (extSize == 0);

						break;
					}
					case WaveFormat::Float:
					{
						success = success && (channels > 0 && channels <= 6);
						success = success && (samplesPerSec != 0);
						success = success && (avgBytesPerSec == blockAlign * samplesPerSec);
						success = success && (blockAlign == channels * bitsPerSample / 8);
						success = success && (bitsPerSample == 32 || bitsPerSample == 64);
						success = success && (extSize == 0);

						break;
					}
					default:
					{
						success = false;

						break;
					}
				}

				return success;
			}

			void FormatChunk::clear()
			{
				std::memset(this, 0, sizeof(wave::FormatChunk));
			}
		}

		WaveFile::WaveFile() :
			_formatChunk(),
			_sampleCount(0),
			_rawSamples()
		{
		}

		const wave::FormatChunk& WaveFile::getFormatChunk() const
		{
			return _formatChunk;
		}

		uint32_t WaveFile::getSampleCount() const
		{
			return _sampleCount;
		}

		void WaveFile::getSamples(uint16_t channel, std::vector<int8_t>& samples) const
		{
			samples.resize(_sampleCount);
			getRawSamples<int8_t>(channel, samples.data());
		}

		void WaveFile::getSamples(uint16_t channel, std::vector<int16_t>& samples) const
		{
			samples.resize(_sampleCount);
			getRawSamples<int16_t>(channel, samples.data());
		}

		void WaveFile::getSamples(uint16_t channel, std::vector<int32_t>& samples) const
		{
			samples.resize(_sampleCount);
			getRawSamples<int32_t>(channel, samples.data());
		}

		void WaveFile::getSamples(uint16_t channel, std::vector<int64_t>& samples) const
		{
			samples.resize(_sampleCount);
			getRawSamples<int64_t>(channel, samples.data());
		}

		void WaveFile::getSamples(uint16_t channel, std::vector<float>& samples) const
		{
			samples.resize(_sampleCount);
			getRawSamples<float>(channel, samples.data());
		}

		void WaveFile::getSamples(uint16_t channel, std::vector<double>& samples) const
		{
			samples.resize(_sampleCount);
			getRawSamples<double>(channel, samples.data());
		}

		void WaveFile::setFormatChunk(const wave::FormatChunk& formatChunk, uint32_t sampleCount)
		{
			assert(formatChunk.isValid());

			clear();

			_formatChunk = formatChunk;
			_sampleCount = sampleCount;
			_rawSamples.resize(_sampleCount * _formatChunk.blockAlign);
		}

		void WaveFile::setSamples(uint16_t channel, const int8_t* samples)
		{
			setRawSamples<int8_t>(channel, samples);
		}

		void WaveFile::setSamples(uint16_t channel, const int16_t* samples)
		{
			setRawSamples<int16_t>(channel, samples);
		}

		void WaveFile::setSamples(uint16_t channel, const int32_t* samples)
		{
			setRawSamples<int32_t>(channel, samples);
		}

		void WaveFile::setSamples(uint16_t channel, const int64_t* samples)
		{
			setRawSamples<int64_t>(channel, samples);
		}

		void WaveFile::setSamples(uint16_t channel, const float* samples)
		{
			setRawSamples<float>(channel, samples);
		}

		void WaveFile::setSamples(uint16_t channel, const double* samples)
		{
			setRawSamples<double>(channel, samples);
		}

		void WaveFile::clear()
		{
			_formatChunk.clear();
			_rawSamples.clear();
		}

		void WaveFile::read(std::istream& stream, IOResult& result)
		{
			// Load RIFF File

			RiffFile file;
			result = file.readFromStream(stream);
			if (!result)
			{
				return;
			}

			// Check if it is a WAVE file

			if (!std::equal(file.formType.begin(), file.formType.end(), "WAVE"))
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Bad form-type for WAVE RIFF file: " + std::string(file.formType.begin(), file.formType.end());
				return;
			}

			// Check if format and data chunk are present

			riff::Chunk* rawFormatChunk = nullptr;
			riff::Chunk* dataChunk = nullptr;
			for (riff::Chunk& chunk : file.chunks)
			{
				if (std::equal(chunk.id.begin(), chunk.id.end(), "fmt ") && !rawFormatChunk)
				{
					rawFormatChunk = &chunk;
				}
				else if (std::equal(chunk.id.begin(), chunk.id.end(), "data") && !dataChunk)
				{
					dataChunk = &chunk;
				}
			}

			if (!rawFormatChunk)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Could not find format chunk for WAVE file";
				return;
			}
			if (!dataChunk)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Could not find data chunk for WAVE file";
				return;
			}

			// Load format chunk

			if (rawFormatChunk->data.size() < 14)
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Format chunk of insufficient size for WAVE file";
				return;
			}

			_formatChunk = *reinterpret_cast<wave::FormatChunk*>(rawFormatChunk->data.data());

			if (rawFormatChunk->data.size() < 18)
			{
				_formatChunk.extSize = 0;
			}

			if (rawFormatChunk->data.size() < 16)
			{
				_formatChunk.bitsPerSample = 8 * (_formatChunk.blockAlign / _formatChunk.channels);
			}

			if (!_formatChunk.isValid())
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Invalid format chunk for WAVE file.";
				return;
			}

			// Load data chunk

			_sampleCount = dataChunk->data.size() / _formatChunk.blockAlign;
			_rawSamples = dataChunk->data;
		}

		void WaveFile::write(std::ostream& stream, IOResult& result)
		{
			RiffFile file;

			file.formType = { 'W', 'A', 'V', 'E' };

			file.chunks.resize(2);

			file.chunks[0].id = { 'f', 'm', 't', ' ' };
			file.chunks[0].data.resize(sizeof(wave::FormatChunk));
			std::memcpy(file.chunks[0].data.data(), &_formatChunk, sizeof(wave::FormatChunk));

			file.chunks[1].id = { 'd', 'a', 't', 'a' };
			file.chunks[1].data = _rawSamples;

			result = file.writeToStream(stream);
		}
	}
}