#include <Diskon/Diskon.hpp>

int main()
{
	// dsk::fmt::WaveStream stream;
	// stream.setSource("build/input_test.wav");
	// stream.setDestination("build/output_test.wav");
	// 
	// dsk::fmt::wave::Header inHeader;
	// stream.readHeader(inHeader);
	// 
	// dsk::fmt::wave::Header outHeader;
	// outHeader.formatTag = dsk::fmt::wave::Format::PCM;
	// outHeader.channels = inHeader.channels;
	// outHeader.samplesPerSec = 22050;
	// outHeader.bitsPerSample = 8;
	// outHeader.blockAlign = ((outHeader.bitsPerSample + 7) / 8) * outHeader.channels;
	// outHeader.avgBytesPerSec = outHeader.blockAlign * outHeader.samplesPerSec;
	// const uint32_t factor = inHeader.samplesPerSec / outHeader.samplesPerSec;
	// outHeader.blockCount = inHeader.blockCount / factor;
	// stream.writeHeader(outHeader);
	// 
	// double* buffer = (double*) alloca(sizeof(double) * inHeader.channels);
	// for (uint32_t i = 0; i < outHeader.blockCount; ++i)
	// {
	// 	for (uint32_t j = 0; j < factor; ++j)
	// 	{
	// 		stream.readSampleBlock(inHeader, buffer);
	// 	}
	// 
	// 	stream.writeSampleBlock(outHeader, buffer);
	// }

	// dsk::fmt::XmlStream stream;
	// stream.setSource("build/input_test.xml");
	// 
	// dsk::fmt::xml::File xmlFile;
	// stream.readFile(xmlFile);
	// 
	// stream.setDestination("build/output_test.xml");
	// stream.writeFile(xmlFile);

	dsk::fmt::PngStream stream;
	stream.setSource("build/input_test.png");

	dsk::fmt::png::File pngFile;
	stream.readFile(pngFile);

 	return 0;
}