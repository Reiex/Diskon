#include <Diskon/Diskon.hpp>

int main()
{
	/*dsk::fmt::WaveFile file;
	dsk::fmt::wave::FormatChunk formatChunk;
	formatChunk.formatTag = dsk::fmt::wave::WaveFormat::PCM;
	formatChunk.channels = 1;
	formatChunk.samplesPerSec = 44100;
	formatChunk.avgBytesPerSec = 4 * 44100;
	formatChunk.blockAlign = 4;
	formatChunk.bitsPerSample = 32;
	formatChunk.extSize = 0;

	float samples[44100 * 5];
	for (uint64_t i = 0; i < _countof(samples); ++i)
	{
		float t = float(i) / formatChunk.samplesPerSec;
		samples[i] = std::sin(2 * 3.1415926 * 100 * t);
	}

	file.setFormatChunk(formatChunk, _countof(samples));
	file.setSamples(0, samples);
	file.writeToFile("build/test_sine.wav");*/

	/*dsk::fmt::WaveFile inputFile;
	inputFile.readFromFile("build/input_test.wav");

	dsk::fmt::wave::FormatChunk formatChunk = inputFile.getFormatChunk();
	formatChunk.formatTag = dsk::fmt::wave::WaveFormat::PCM;
	formatChunk.bitsPerSample = 8;
	formatChunk.blockAlign = formatChunk.channels * (formatChunk.bitsPerSample / 8);
	formatChunk.avgBytesPerSec = formatChunk.blockAlign * formatChunk.samplesPerSec;

	dsk::fmt::WaveFile outputFile;
	outputFile.setFormatChunk(formatChunk, inputFile.getSampleCount());

	for (uint16_t i = 0; i < formatChunk.channels; ++i)
	{
		std::vector<double> samples;
		inputFile.getSamples(i, samples);
		outputFile.setSamples(i, samples.data());
	}

	outputFile.writeToFile("build/output_test.wav");*/


	return 0;
}