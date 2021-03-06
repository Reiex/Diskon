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

		WaveStream::WaveStream() : FormatStream(std::endian::native)
		{
		}

		const FormatError& WaveStream::readFile(wave::File& file)
		{
			FMTSTREAM_BEGIN_READ_FUNC("WaveStream::readFile(wave::File& file)");

			FMTSTREAM_VERIFY_CALL(readHeader, file.header);

			file.rawSamples.resize(file.header.blockAlign * file.header.blockCount);
			FMTSTREAM_READ(file.rawSamples.data(), file.rawSamples.size());

			return error;
		}

		const FormatError& WaveStream::readHeader(wave::Header& header)
		{
			FMTSTREAM_BEGIN_READ_FUNC("WaveStream::readHeader(wave::Header& header)");

			RiffStream riffStream;
			FMTSTREAM_VERIFY_STREAM_CALL(riffStream, setSource, stream);

			riff::FileHeader fileHeader;
			FMTSTREAM_VERIFY_STREAM_CALL(riffStream, readFileHeader, fileHeader);
			FMTSTREAM_VERIFY(std::equal(fileHeader.formType, fileHeader.formType + 4, "WAVE"), WaveBadFormType, "Bad form-type for RIFF file of WAVE.");
			
			// Create index of all chunks in the file

			riff::ChunkHeader chunkHeader;
			riff::Chunk chunk;

			std::unordered_map<std::string, std::streampos> index;

			fileHeader.size -= 4;
			while (fileHeader.size)
			{
				FMTSTREAM_VERIFY(fileHeader.size >= 8, RiffInvalidFileSize, "Error while reading RIFF chunk. Expected remaining file size to be more than 8, got " + std::to_string(fileHeader.size));
				fileHeader.size -= 8;

				std::streampos savedPos = riffStream.getSourcePos();

				FMTSTREAM_VERIFY_STREAM_CALL(riffStream, readChunkHeader, chunkHeader);

				FMTSTREAM_VERIFY(fileHeader.size >= chunkHeader.size, RiffInvalidFileSize, "Error while reading RIFF chunk. Expected remaining file size to be more than chunk size (" + std::to_string(chunkHeader.size) + "), got " + std::to_string(fileHeader.size));
				fileHeader.size -= chunkHeader.size;

				index[{chunkHeader.id, 4}] = savedPos;
				FMTSTREAM_VERIFY_STREAM_CALL(riffStream, setSourcePos, riffStream.getSourcePos() + std::streamoff(chunkHeader.size));
			}

			FMTSTREAM_VERIFY(index.find("fmt ") != index.end(), WaveNoFmtChunk, "'fmt ' RIFF chunk not found.");
			FMTSTREAM_VERIFY(index.find("data") != index.end(), WaveNoDataChunk, "'data' RIFF chunk not found.");

			// Load "fmt " chunk

			FMTSTREAM_VERIFY_STREAM_CALL(riffStream, setSourcePos, index["fmt "]);
			FMTSTREAM_VERIFY_STREAM_CALL(riffStream, readChunk, chunk);
			wave::_wave::RawFormatChunk* rawFormatChunk = (wave::_wave::RawFormatChunk*) chunk.data.data();
			FMTSTREAM_VERIFY((rawFormatChunk->formatTag == wave::Format::PCM && chunk.data.size() == 16) || (chunk.data.size() == 18 + rawFormatChunk->extSize), WaveBadFmtChunk, "Bad fmt chunk size: " + std::to_string(chunk.data.size()));

			header.formatTag = rawFormatChunk->formatTag;
			header.channels = rawFormatChunk->channels;
			header.samplesPerSec = rawFormatChunk->samplesPerSec;
			header.avgBytesPerSec = rawFormatChunk->avgBytesPerSec;
			header.blockAlign = rawFormatChunk->blockAlign;
			header.bitsPerSample = rawFormatChunk->bitsPerSample;
			header.blockCount = 0;

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

			// Load "fact" chunk

			if (index.find("fact") != index.end())
			{
				FMTSTREAM_VERIFY_STREAM_CALL(riffStream, setSourcePos, index["fact"]);
				FMTSTREAM_VERIFY_STREAM_CALL(riffStream, readChunk, chunk);
				FMTSTREAM_VERIFY(chunk.data.size() == 4, WaveBadFactChunk, "Bad fact chunk size. Expected 4, got " + std::to_string(chunk.data.size()));
				header.blockCount = *reinterpret_cast<uint32_t*>(chunk.data.data());
			}

			// TODO: Load Cue Points, Playlist and Associated Data chunks.

			// Go to "data" chunk and read size to compute sample block count

			FMTSTREAM_VERIFY(header.isValid(), WaveInvalidHeader, "Parse of header succeeded but for some reason the header is invalid.");

			FMTSTREAM_VERIFY_STREAM_CALL(riffStream, setSourcePos, index["data"]);
			FMTSTREAM_VERIFY_STREAM_CALL(riffStream, readChunkHeader, chunkHeader);
			if (header.blockCount == 0)
			{
				header.blockCount = chunkHeader.size / header.blockAlign;
			}

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

			FMTSTREAM_BEGIN_WRITE_FUNC("WaveStream::writeFile(const wave::File& file)");

			FMTSTREAM_VERIFY_CALL(writeHeader, file.header);
			FMTSTREAM_WRITE(file.rawSamples.data(), file.rawSamples.size());

			return error;
		}

		const FormatError& WaveStream::writeHeader(const wave::Header& header)
		{
			assert(header.isValid());

			FMTSTREAM_BEGIN_WRITE_FUNC("WaveStream::writeHeader(const wave::Header& header)");

			RiffStream riffStream;
			FMTSTREAM_VERIFY_STREAM_CALL(riffStream, setDestination, stream);

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

			// Create "fact" chunk

			riff::Chunk factChunk;
			if (header.formatTag != wave::Format::PCM)
			{
				std::copy_n("fact", 4, factChunk.id);
				factChunk.data.resize(4);
				*reinterpret_cast<uint32_t*>(factChunk.data.data()) = header.blockCount;

				fileHeader.size += factChunk.data.size() + 8;
			}

			// TODO: Create Cue Points, Playlist and Associated Data chunks.

			// Create "data" chunk header

			riff::ChunkHeader dataChunkHeader;
			std::copy_n("data", 4, dataChunkHeader.id);
			dataChunkHeader.size = header.blockAlign * header.blockCount;

			fileHeader.size += dataChunkHeader.size + 8;

			// Write everything

			FMTSTREAM_VERIFY_STREAM_CALL(riffStream, writeFileHeader, fileHeader);
			FMTSTREAM_VERIFY_STREAM_CALL(riffStream, writeChunk, formatChunk);

			if (header.formatTag != wave::Format::PCM)
			{
				FMTSTREAM_VERIFY_STREAM_CALL(riffStream, writeChunk, factChunk);
			}

			FMTSTREAM_VERIFY_STREAM_CALL(riffStream, writeChunkHeader, dataChunkHeader);

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
