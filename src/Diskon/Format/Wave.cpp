#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		void WaveFile::clear()
		{
			channelCount = 0;
			sampleRate = 0;
			bitsPerSample = 0;
			samples.clear();
		}

		void WaveFile::read(std::istream& stream, IOResult& result)
		{
		}

		void WaveFile::write(std::ostream& stream, IOResult& result)
		{
			assert(channelCount > 0);
			assert(channelCount == samples.size());
			assert(bitsPerSample == 8
				|| bitsPerSample == 16
				|| bitsPerSample == 32); // Better handling of bitsPerSample (not only the assert but all)

			const uint32_t blockCount = samples[0].size();
			const uint32_t sampleCount = blockCount * channelCount;
			const uint16_t bytesPerSample = bitsPerSample / 8;
			const uint16_t blockSize = channelCount * bytesPerSample;
			const uint32_t byteRate = blockSize * sampleRate;
			const uint32_t dataSize = blockSize * blockCount;

			for (uint16_t i = 1; i < channelCount; ++i)
			{
				assert(samples[i].size() == blockCount);
			}

			float* arrangedData = new float[sampleCount];
			float* itArrangedData = arrangedData;
			for (uint32_t i = 0; i < sampleCount; ++i)
			{
				for (uint16_t j = 0; j < channelCount; ++j)
				{
					*itArrangedData = samples[j][i];
					++itArrangedData;
				}
			}

			char* data = new char[dataSize];
			char* itData = data;
			itArrangedData = arrangedData;
			for (uint32_t i = 0; i < sampleCount; ++i)
			{
				if (bitsPerSample <= 8)
				{
					*((int8_t*) itData) = *itArrangedData * INT8_MAX;
				}
				else if (bitsPerSample <= 16)
				{
					*((int16_t*) itData) = *itArrangedData * INT16_MAX;
				}
				else if (bitsPerSample <= 32)
				{
					*((int32_t*) itData) = *itArrangedData * INT32_MAX;
				}

				itData += bytesPerSample;
				++itArrangedData;
			}
			delete[] arrangedData;

			const uint32_t totalSize = 12 + 24 + 8 + dataSize - 8;
			const uint32_t formatSectionSize = 16;
			const uint16_t formatCode = 1;

			stream.write("RIFF", 4);
			stream.write((char*) &totalSize, 4);
			stream.write("WAVE", 4);
			stream.write("fmt ", 4);
			stream.write((char*) &formatSectionSize, 4);
			stream.write((char*) &formatCode, 2);
			stream.write((char*) &channelCount, 2);
			stream.write((char*) &sampleRate, 4);
			stream.write((char*) &byteRate, 4);
			stream.write((char*) &blockSize, 2);
			stream.write((char*) &bitsPerSample, 2);
			stream.write("data", 4);
			stream.write((char*) &dataSize, 4);
			stream.write(data, dataSize);

			delete[] data;
		}
	}
}