///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include <Diskon/Diskon.hpp>


uint64_t read(void* handle, uint8_t* data, uint64_t size)
{
	return std::fread(data, 1, size, reinterpret_cast<std::FILE*>(handle));
}

bool eof(void* handle)
{
	return std::feof(reinterpret_cast<std::FILE*>(handle));
}

uint64_t write(void* handle, const uint8_t* data, uint64_t size)
{
	return std::fwrite(data, 1, size, reinterpret_cast<std::FILE*>(handle));
}


template<typename IType, typename OType, typename FileType>
void formatExample(const std::initializer_list<std::string>& filenames)
{
	IType formatIStream(nullptr);
	OType formatOStream(nullptr);

	for (const std::string& filename : filenames)
	{
		std::FILE* iFile = std::fopen(("examples/assets/" + filename).c_str(), "rb");
		dsk::IStream* iStream = new dsk::IStream(iFile, read, eof);
		formatIStream.setStream(iStream);

		FileType formatFile;
		formatIStream.readFile(formatFile);
		assert(formatIStream.getStatus());

		delete iStream;
		std::fclose(iFile);

		std::FILE* oFile = std::fopen(("build/" + filename).c_str(), "wb");
		dsk::OStream* oStream = new dsk::OStream(oFile, write);
		formatOStream.setStream(oStream);

		formatOStream.writeFile(formatFile);
		assert(formatOStream.getStatus());

		oStream->flush();
		delete oStream;
		std::fclose(oFile);
	}
}

template<std::endian BitEndianness>
void huffmanExample()
{
	std::FILE* file = std::fopen("examples/assets/vk.xml", "rb");
	std::fseek(file, 0, SEEK_END);
	const uint64_t strSize = std::ftell(file);
	std::fseek(file, 0, SEEK_SET);
	
	char* str = new char[strSize];
	std::fread(str, 1, strSize, file);
	uint8_t* buffer = new uint8_t[strSize];
	std::memset(buffer, 0, strSize);
	char* decoded = new char[strSize];
	
	std::vector<char> symbols;
	std::vector<uint64_t> symbolOccurences;
	dsk::HuffmanEncoder<char>::symbolBufferToSymbolOccurences(str, strSize, symbols, symbolOccurences);
	std::vector<uint64_t> codeLengths(symbols.size());
	dsk::HuffmanEncoder<char>::symbolOccurencesToCodeLengths(symbols.data(), symbolOccurences.data(), codeLengths.data(), symbols.size());
	
	dsk::HuffmanEncoder<char, BitEndianness> encoder(symbols.data(), codeLengths.data(), symbols.size());
	
	uint8_t bitCount = 0;
	uint64_t byteCount = 0;
	for (uint64_t i = 0; i < strSize; ++i)
	{
		uint8_t bitsWritten;
		uint64_t bytesWritten;
		encoder.writeSymbol(str[i], buffer + byteCount, bitCount, bytesWritten, bitsWritten);
		bitCount += bitsWritten;
		byteCount += bytesWritten + (bitCount >> 3);
		bitCount &= 7;
	}
	
	dsk::HuffmanDecoder<char, BitEndianness> decoder(symbols.data(), codeLengths.data(), symbols.size());
	
	bitCount = 0;
	byteCount = 0;
	for (uint64_t i = 0; i < strSize; ++i)
	{
		uint8_t bitsRead;
		uint64_t bytesRead;
		decoder.readSymbol(decoded[i], buffer + byteCount, bitCount, bytesRead, bitsRead);
		bitCount += bitsRead;
		byteCount += bytesRead + (bitCount >> 3);
		bitCount &= 7;
	}

	assert(std::memcmp(str, decoded, strSize) == 0);
}

#include <filesystem>

int main()
{
	std::string x = std::filesystem::current_path().string();

	formatExample<dsk::fmt::WaveIStream, dsk::fmt::WaveOStream, dsk::fmt::wave::File<int16_t>>(
		{
			"Balavoine8.wav",
			"Balavoine16.wav",
			"Balavoine24.wav",
			"Balavoine32.wav",
			"Balavoine32F.wav",
			"Balavoine64F.wav"
		}
	);
	formatExample<dsk::fmt::XmlIStream, dsk::fmt::XmlOStream, dsk::fmt::xml::File>(
		{
			"vk.xml",
			"video.xml"
		}
	);
	formatExample<dsk::fmt::PnmIStream, dsk::fmt::PnmOStream, dsk::fmt::pnm::File>(
		{
			"Gentianes.ppm",
			"LenaPlain16.ppm",
			"LenaPlain8.ppm",
			"LenaRaw16.ppm",
			"LenaRaw8.ppm",
			"LenaPlain.pgm",
			"LenaRaw.pgm",
			"LenaPlain.pbm",
			"LenaRaw.pbm"
		}
	);
	formatExample<dsk::fmt::ObjIStream, dsk::fmt::ObjOStream, dsk::fmt::obj::File>(
		{
			"suzanne.obj",
			"teapot.obj"
		}
	);

	huffmanExample<std::endian::big>();
	huffmanExample<std::endian::little>();

	constexpr char str[] = "Hello world ! Ceci est une merveilleuse chaine de caracteres...";
	
	dsk::fmt::deflate::File formatFile;
	
	formatFile.blocks.emplace_back();
	formatFile.blocks[0].header.compressionType = dsk::fmt::deflate::CompressionType::FixedHuffman;
	formatFile.blocks[0].header.isFinal = true;
	formatFile.blocks[0].data.resize(sizeof(str));
	std::copy_n(str, formatFile.blocks[0].data.size(), formatFile.blocks[0].data.data());
	
	dsk::fmt::DeflateOStream formatOStream(nullptr);
	
	std::FILE* oFile = std::fopen("build/moncul.deflate", "wb");
	dsk::OStream* oStream = new dsk::OStream(oFile, write);
	formatOStream.setStream(oStream);
	
	formatOStream.writeFile(formatFile);
	assert(formatOStream.getStatus());

	oStream->write(uint16_t(0xFFFF));
	oStream->flush();
	delete oStream;
	std::fclose(oFile);
	
	
	dsk::fmt::DeflateIStream formatIStream(nullptr);
	
	std::FILE* iFile = std::fopen("build/moncul.deflate", "rb");
	dsk::IStream* iStream = new dsk::IStream(iFile, read, eof);
	formatIStream.setStream(iStream);
	
	dsk::fmt::deflate::File decodedFile;
	formatIStream.readFile(decodedFile);
	assert(formatIStream.getStatus());
	
	delete iStream;
	std::fclose(iFile);

	return 0;
}
