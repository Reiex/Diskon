#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace wave
		{
			namespace _wave
			{
				struct RawFormatChunk
				{
					Format formatTag;
					uint16_t channels;
					uint32_t samplesPerSec;
					uint32_t avgBytesPerSec;
					uint16_t blockAlign;
					uint16_t bitsPerSample;
					uint16_t extSize;
				};
			}


			bool Header::isValid() const
			{
				bool bValid = true;

				switch (formatTag)
				{
					case Format::PCM:
					{
						bValid = bValid && (channels > 0);
						bValid = bValid && (samplesPerSec > 0);
						bValid = bValid && (bitsPerSample > 0);

						const uint16_t bytesPerSample = (bitsPerSample + 7) / 8;

						bValid = bValid && (blockAlign == channels * bytesPerSample);
						bValid = bValid && (avgBytesPerSec == blockAlign * samplesPerSec);

						bValid = bValid && (extension.size() == 0);

						break;
					}
					case Format::Float:
					{
						break;
					}
				}

				return bValid;
			}


			void File::getSamples(uint16_t channel, int8_t* samples) const
			{
				_getSamples(channel, samples);
			}

			void File::getSamples(uint16_t channel, int16_t* samples) const
			{
				_getSamples(channel, samples);
			}

			void File::getSamples(uint16_t channel, int32_t* samples) const
			{
				_getSamples(channel, samples);
			}

			void File::getSamples(uint16_t channel, int64_t* samples) const
			{
				_getSamples(channel, samples);
			}

			void File::getSamples(uint16_t channel, float* samples) const
			{
				_getSamples(channel, samples);
			}

			void File::getSamples(uint16_t channel, double* samples) const
			{
				_getSamples(channel, samples);
			}

			void File::setSamples(uint16_t channel, const int8_t* samples)
			{
				_setSamples(channel, samples);
			}

			void File::setSamples(uint16_t channel, const int16_t* samples)
			{
				_setSamples(channel, samples);
			}

			void File::setSamples(uint16_t channel, const int32_t* samples)
			{
				_setSamples(channel, samples);
			}

			void File::setSamples(uint16_t channel, const int64_t* samples)
			{
				_setSamples(channel, samples);
			}

			void File::setSamples(uint16_t channel, const float* samples)
			{
				_setSamples(channel, samples);
			}

			void File::setSamples(uint16_t channel, const double* samples)
			{
				_setSamples(channel, samples);
			}
		

			bool File::isValid() const
			{
				return header.isValid() && rawSamples.size() == header.blockAlign * header.blockCount;
			}
		}


		const FormatError& WaveStream::readFile(wave::File& file)
		{
			FMTSTREAM_BEGIN_READ();

			FMTSTREAM_VERIFY_SELF_CALL(readHeader, file.header);

			file.rawSamples.resize(file.header.blockAlign * file.header.blockCount);
			FMTSTREAM_VERIFY(stream.read((char*) file.rawSamples.data(), file.rawSamples.size()), InvalidStream, "WaveStream: Error while reading samples.");

			return error;
		}

		const FormatError& WaveStream::readHeader(wave::Header& header)
		{
			FMTSTREAM_BEGIN_READ();

			RiffStream riffStream;
			FMTSTREAM_VERIFY_CALL(riffStream, setSource, stream);

			riff::FileHeader fileHeader;
			FMTSTREAM_VERIFY_CALL(riffStream, readFileHeader, fileHeader);
			FMTSTREAM_VERIFY(std::equal(fileHeader.formType, fileHeader.formType + 4, "WAVE"), WaveBadFormType, "WaveStream: Bad form-type for RIFF file of WAVE.");
			
			// Create index of all chunks in the file

			riff::ChunkHeader chunkHeader;
			riff::Chunk chunk;

			std::unordered_map<std::string, std::streampos> index;

			fileHeader.size -= 4;
			while (fileHeader.size)
			{
				FMTSTREAM_VERIFY(fileHeader.size >= 8, RiffInvalidFileSize, "WaveStream: Error while reading RIFF chunk. Expected remaining file size to be more than 8, got " + std::to_string(fileHeader.size));
				fileHeader.size -= 8;

				std::streampos savedPos = riffStream.getSourcePos();

				FMTSTREAM_VERIFY_CALL(riffStream, readChunkHeader, chunkHeader);

				FMTSTREAM_VERIFY(fileHeader.size >= chunkHeader.size, RiffInvalidFileSize, "WaveStream: Error while reading RIFF chunk. Expected remaining file size to be more than chunk size (" + std::to_string(chunkHeader.size) + "), got " + std::to_string(fileHeader.size));
				fileHeader.size -= chunkHeader.size;

				index[{chunkHeader.id, 4}] = savedPos;
				FMTSTREAM_VERIFY_CALL(riffStream, setSourcePos, riffStream.getSourcePos() + std::streamoff(chunkHeader.size));
			}

			FMTSTREAM_VERIFY(index.find("fmt ") != index.end(), WaveNoFmtChunk, "WaveStream: 'fmt ' RIFF chunk not found.");
			FMTSTREAM_VERIFY(index.find("data") != index.end(), WaveNoDataChunk, "WaveStream: 'data' RIFF chunk not found.");

			// Load "fmt " chunk

			FMTSTREAM_VERIFY_CALL(riffStream, setSourcePos, index["fmt "]);
			FMTSTREAM_VERIFY_CALL(riffStream, readChunk, chunk);
			wave::_wave::RawFormatChunk* rawFormatChunk = (wave::_wave::RawFormatChunk*) chunk.data.data();

			header.formatTag = rawFormatChunk->formatTag;
			header.channels = rawFormatChunk->channels;
			header.samplesPerSec = rawFormatChunk->samplesPerSec;
			header.avgBytesPerSec = rawFormatChunk->avgBytesPerSec;
			header.blockAlign = rawFormatChunk->blockAlign;
			header.bitsPerSample = rawFormatChunk->bitsPerSample;
			
			if (header.formatTag != wave::Format::PCM)
			{
				header.extension.resize(rawFormatChunk->extSize);
			}
			else
			{
				header.extension.clear();
			}

			if (header.extension.size() > 0)
			{
				uint8_t* it = (uint8_t*) (rawFormatChunk + 1);
				std::copy(it, it + header.extension.size(), header.extension.data());
			}

			// TODO: Load all metadata chunks

			FMTSTREAM_VERIFY(header.isValid(), WaveInvalidHeader, "WaveStream: Parse of header succeeded but for some reason the header is invalid.");

			// Go to "data" chunk and read size to compute sample block count

			FMTSTREAM_VERIFY_CALL(riffStream, setSourcePos, index["data"]);
			FMTSTREAM_VERIFY_CALL(riffStream, readChunkHeader, chunkHeader);
			header.blockCount = chunkHeader.size / header.blockAlign;

			return error;
		}

		const FormatError& WaveStream::readSampleBlock(const wave::Header& header, int8_t* sampleBlock)
		{
			return _readSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::readSampleBlock(const wave::Header& header, int16_t* sampleBlock)
		{
			return _readSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::readSampleBlock(const wave::Header& header, int32_t* sampleBlock)
		{
			return _readSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::readSampleBlock(const wave::Header& header, int64_t* sampleBlock)
		{
			return _readSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::readSampleBlock(const wave::Header& header, float* sampleBlock)
		{
			return _readSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::readSampleBlock(const wave::Header& header, double* sampleBlock)
		{
			return _readSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::writeFile(const wave::File& file)
		{
			assert(file.isValid());

			FMTSTREAM_BEGIN_WRITE();

			FMTSTREAM_VERIFY_SELF_CALL(writeHeader, file.header);
			FMTSTREAM_VERIFY(stream.write((char*) file.rawSamples.data(), file.rawSamples.size()), InvalidStream, "WaveStream: Error while writing samples.");

			return error;
		}

		const FormatError& WaveStream::writeHeader(const wave::Header& header)
		{
			assert(header.isValid());

			FMTSTREAM_BEGIN_WRITE();

			RiffStream riffStream;
			FMTSTREAM_VERIFY_CALL(riffStream, setDestination, stream);

			riff::FileHeader fileHeader;
			std::copy_n("WAVE", 4, fileHeader.formType);
			fileHeader.size = 4;
			
			// Create "fmt " chunk

			riff::Chunk formatChunk;
			wave::_wave::RawFormatChunk rawFormatChunk;
			std::copy_n("fmt ", 4, formatChunk.id);
			rawFormatChunk.formatTag = header.formatTag;
			rawFormatChunk.channels = header.channels;
			rawFormatChunk.samplesPerSec = header.samplesPerSec;
			rawFormatChunk.avgBytesPerSec = header.avgBytesPerSec;
			rawFormatChunk.blockAlign = header.blockAlign;
			rawFormatChunk.bitsPerSample = header.bitsPerSample;
			rawFormatChunk.extSize = header.extension.size();

			if (rawFormatChunk.formatTag == wave::Format::PCM)
			{
				formatChunk.data.resize(16);
				std::copy_n((uint8_t*) &rawFormatChunk, 16, formatChunk.data.data());
			}
			else
			{
				formatChunk.data.resize(18 + rawFormatChunk.extSize);
				std::copy_n((uint8_t*) &rawFormatChunk, 18, formatChunk.data.data());
				std::copy(header.extension.begin(), header.extension.end(), formatChunk.data.data() + 18);
			}

			fileHeader.size += formatChunk.data.size() + 8;

			// Create metadata chunks

			// Create "data" chunk header

			riff::ChunkHeader dataChunkHeader;
			std::copy_n("data", 4, dataChunkHeader.id);
			dataChunkHeader.size = header.blockAlign * header.blockCount;

			fileHeader.size += dataChunkHeader.size + 8;

			// Write everything

			FMTSTREAM_VERIFY_CALL(riffStream, writeFileHeader, fileHeader);
			FMTSTREAM_VERIFY_CALL(riffStream, writeChunk, formatChunk);
			FMTSTREAM_VERIFY_CALL(riffStream, writeChunkHeader, dataChunkHeader);

			return error;
		}

		const FormatError& WaveStream::writeSampleBlock(const wave::Header& header, const int8_t* sampleBlock)
		{
			return _writeSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::writeSampleBlock(const wave::Header& header, const int16_t* sampleBlock)
		{
			return _writeSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::writeSampleBlock(const wave::Header& header, const int32_t* sampleBlock)
		{
			return _writeSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::writeSampleBlock(const wave::Header& header, const int64_t* sampleBlock)
		{
			return _writeSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::writeSampleBlock(const wave::Header& header, const float* sampleBlock)
		{
			return _writeSampleBlock(header, sampleBlock);
		}

		const FormatError& WaveStream::writeSampleBlock(const wave::Header& header, const double* sampleBlock)
		{
			return _writeSampleBlock(header, sampleBlock);
		}
	}
}
