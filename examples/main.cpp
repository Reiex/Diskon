#include <Diskon/Diskon.hpp>

int main()
{
	dsk::fmt::WaveFile file;
	file.bitsPerSample = 32;
	file.channelCount = 1;
	file.sampleRate = 44100;
	file.samples.resize(1);
	for (uint64_t i = 0; i < file.sampleRate * 5ull; ++i)
	{
		float t = float(i) / file.sampleRate;
		file.samples[0].push_back(std::sin(2 * 3.1415926 * 100 * t));
	}

	file.writeToPath("build/test2.wav");

	return 0;
}