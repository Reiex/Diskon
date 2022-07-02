#include <Diskon/Diskon.hpp>

int main()
{
	dsk::fmt::WaveStream stream;
	stream.setSource("build/input_test.wav");
	stream.setDestination("build/output_test.wav");

	dsk::fmt::wave::File file;
	stream.readFile(file);

	dsk::fmt::wave::File copy;
	copy.header.formatTag = dsk::fmt::wave::Format::PCM;
	copy.header.channels = file.header.channels;
	copy.header.samplesPerSec = 22050;
	copy.header.bitsPerSample = 8;
	copy.header.blockAlign = ((copy.header.bitsPerSample + 7) / 8) * copy.header.channels;
	copy.header.avgBytesPerSec = copy.header.blockAlign * copy.header.samplesPerSec;
	const uint32_t factor = file.header.samplesPerSec / copy.header.samplesPerSec;
	copy.header.blockCount = file.header.blockCount / factor;
	copy.rawSamples.resize(copy.header.blockAlign * copy.header.blockCount);
	
	int64_t* buffer = new int64_t[file.header.blockCount];
	for (uint16_t i = 0; i < copy.header.channels; ++i)
	{
		file.getSamples(i, buffer);

		for (uint32_t j = 0; j < copy.header.blockCount; ++j)
		{
			buffer[j] = buffer[factor * j];
		}

		copy.setSamples(i, buffer);
	}

	stream.writeFile(copy);

	return 0;
}