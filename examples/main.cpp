#include <Diskon/Diskon.hpp>

void waveExample()
{
	dsk::fmt::WaveStream waveStream;
	waveStream.setSource("build/input_test.wav");
	waveStream.setDestination("build/output_test.wav");

	dsk::fmt::wave::Header inHeader;
	waveStream.readHeader(inHeader);

	dsk::fmt::wave::Header outHeader;
	outHeader.formatTag = dsk::fmt::wave::Format::PCM;
	outHeader.channels = inHeader.channels;
	outHeader.samplesPerSec = 44100;
	outHeader.bitsPerSample = 8;
	outHeader.blockAlign = ((outHeader.bitsPerSample + 7) / 8) * outHeader.channels;
	outHeader.avgBytesPerSec = outHeader.blockAlign * outHeader.samplesPerSec;
	const uint32_t factor = inHeader.samplesPerSec / outHeader.samplesPerSec;
	outHeader.blockCount = inHeader.blockCount / factor;
	waveStream.writeHeader(outHeader);

	double* buffer = (double*)alloca(sizeof(double) * inHeader.channels);
	for (uint32_t i = 0; i < outHeader.blockCount; ++i)
	{
		for (uint32_t j = 0; j < factor; ++j)
		{
			waveStream.readSampleBlock(inHeader, buffer);
		}

		waveStream.writeSampleBlock(outHeader, buffer);
	}
}

void xmlExample()
{
	dsk::fmt::XmlStream xmlStream;
	xmlStream.setSource("build/input_test.xml");

	dsk::fmt::xml::File xmlFile;
	xmlStream.readFile(xmlFile);

	xmlStream.setDestination("build/output_test.xml");
	xmlStream.writeFile(xmlFile);
}

void pngExample()
{
	// dsk::fmt::PngStream pngStream;
	// pngStream.setSource("build/input_test.png");
	// 
	// dsk::fmt::png::File pngFile;
	// pngStream.readFile(pngFile);
}

int main()
{
	waveExample();
	xmlExample();
	pngExample();

 	return 0;
}