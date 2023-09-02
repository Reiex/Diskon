///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Reiex
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Format/FormatDecl.hpp>

namespace dsk
{
	namespace fmt
	{
		// template<typename TSample>
		// const ruc::Status& PngIStream::readFile(png::File<TSample>& file)
		// {
		// 	FMT_BEGIN("PngIStream::readFile(png::File<TSample>& file)");
		// 
		// 	// Read header
		// 
		// 	FMT_CALL(readHeader, file.header);
		// 
		// 	// Read pixels
		// 
		// 	const uint64_t pixelCount = file.header.imageStructure.width * file.header.imageStructure.height;
		// 	file.pixels.resize(pixelCount * 4);
		// 	FMT_CALL(readPixels, file.pixels.data(), pixelCount);
		// 
		// 	// Read ending
		// 
		// 	FMT_CALL(readEnding, file.ending);
		// 
		// 	return *_error;
		// }
		// 
		// template<typename TSample>
		// const ruc::Status& PngIStream::readPixels(TSample* samples, uint64_t pixelCount)
		// {
		// 	FMT_BEGIN("PngIStream::readPixels(TSample* samples, uint64_t pixelCount)");
		// 
		// 	assert(_headerRead);
		// 
		// 	TSample sampleBuffer;
		// 	constexpr TSample sampleMin = std::numeric_limits<TSample>::min();
		// 	constexpr TSample sampleMax = std::numeric_limits<TSample>::max();
		// 
		// 	switch (_header.imageStructure.colorType)
		// 	{
		// 		case png::ColorType::GreyScale:
		// 		{
		// 			if (_header.imageStructure.bitDepth <= 8)
		// 			{
		// 				std::vector<uint8_t> buffer(pixelCount);
		// 				FMT_CALL(_readRawPixels, buffer.data(), pixelCount, 1);
		// 				--samples;
		// 				for (const uint8_t& raw : buffer)
		// 				{
		// 					convertNum<uint8_t, TSample>(&raw, &sampleBuffer);
		// 					*(++samples) = sampleBuffer;
		// 					*(++samples) = sampleBuffer;
		// 					*(++samples) = sampleBuffer;
		// 					*(++samples) = sampleMax;
		// 				}
		// 			}
		// 			else
		// 			{
		// 				std::vector<uint16_t> buffer(pixelCount);
		// 				FMT_CALL(_readRawPixels, buffer.data(), pixelCount, 2);
		// 				--samples;
		// 				for (const uint16_t& raw : buffer)
		// 				{
		// 					convertNum<uint16_t, TSample>(&raw, &sampleBuffer);
		// 					*(++samples) = sampleBuffer;
		// 					*(++samples) = sampleBuffer;
		// 					*(++samples) = sampleBuffer;
		// 					*(++samples) = sampleMax;
		// 				}
		// 			}
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColor:
		// 		{
		// 			if (_header.imageStructure.bitDepth == 8)
		// 			{
		// 				std::vector<uint8_t> buffer(pixelCount * 3);
		// 				FMT_CALL(_readRawPixels, buffer.data(), pixelCount, 3);
		// 				const uint8_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<uint8_t, TSample>(++it, ++samples);
		// 					convertNum<uint8_t, TSample>(++it, ++samples);
		// 					convertNum<uint8_t, TSample>(++it, ++samples);
		// 					*(++samples) = sampleMax;
		// 				}
		// 			}
		// 			else
		// 			{
		// 				std::vector<uint16_t> buffer(pixelCount * 3);
		// 				FMT_CALL(_readRawPixels, buffer.data(), pixelCount, 6);
		// 				const uint16_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<uint16_t, TSample>(++it, ++samples);
		// 					convertNum<uint16_t, TSample>(++it, ++samples);
		// 					convertNum<uint16_t, TSample>(++it, ++samples);
		// 					*(++samples) = sampleMax;
		// 				}
		// 			}
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::IndexedColor:
		// 		{
		// 			std::vector<uint8_t> buffer(pixelCount);
		// 			FMT_CALL(_readRawPixels, buffer.data(), pixelCount, 1);
		// 			--samples;
		// 			for (const uint8_t& index : buffer)
		// 			{
		// 				convertNum<uint8_t, TSample>(&_palette.entries[index].r, ++samples);
		// 				convertNum<uint8_t, TSample>(&_palette.entries[index].g, ++samples);
		// 				convertNum<uint8_t, TSample>(&_palette.entries[index].b, ++samples);
		// 				convertNum<uint8_t, TSample>(&_palette.entries[index].a, ++samples);
		// 			}
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::GreyScaleAlpha:
		// 		{
		// 			if (_header.imageStructure.bitDepth == 8)
		// 			{
		// 				std::vector<uint8_t> buffer(pixelCount * 2);
		// 				FMT_CALL(_readRawPixels, buffer.data(), pixelCount, 2);
		// 				const uint8_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<uint8_t, TSample>(++it, &sampleBuffer);
		// 					*(++samples) = sampleBuffer;
		// 					*(++samples) = sampleBuffer;
		// 					*(++samples) = sampleBuffer;
		// 					convertNum<uint8_t, TSample>(++it, ++samples);
		// 				}
		// 			}
		// 			else
		// 			{
		// 				std::vector<uint16_t> buffer(pixelCount * 2);
		// 				FMT_CALL(_readRawPixels, buffer.data(), pixelCount, 4);
		// 				const uint16_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<uint16_t, TSample>(++it, &sampleBuffer);
		// 					*(++samples) = sampleBuffer;
		// 					*(++samples) = sampleBuffer;
		// 					*(++samples) = sampleBuffer;
		// 					convertNum<uint16_t, TSample>(++it, ++samples);
		// 				}
		// 			}
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColorAlpha:
		// 		{
		// 			if (_header.imageStructure.bitDepth == 8)
		// 			{
		// 				std::vector<uint8_t> buffer(pixelCount * 4);
		// 				FMT_CALL(_readRawPixels, buffer.data(), pixelCount, 4);
		// 				const uint8_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<uint8_t, TSample>(++it, ++samples);
		// 					convertNum<uint8_t, TSample>(++it, ++samples);
		// 					convertNum<uint8_t, TSample>(++it, ++samples);
		// 					convertNum<uint8_t, TSample>(++it, ++samples);
		// 				}
		// 			}
		// 			else
		// 			{
		// 				std::vector<uint16_t> buffer(pixelCount * 4);
		// 				FMT_CALL(_readRawPixels, buffer.data(), pixelCount, 8);
		// 				const uint16_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<uint16_t, TSample>(++it, ++samples);
		// 					convertNum<uint16_t, TSample>(++it, ++samples);
		// 					convertNum<uint16_t, TSample>(++it, ++samples);
		// 					convertNum<uint16_t, TSample>(++it, ++samples);
		// 				}
		// 			}
		// 
		// 			break;
		// 		}
		// 	}
		// 
		// 	return *_error;
		// }
		// 
		// 
		// template<typename TSample>
		// const ruc::Status& PngOStream::writeFile(const png::File<TSample>& file, const png::Palette* palette)
		// {
		// 	FMT_BEGIN("PngOStream::writeFile(const png::File<TSample>& file, const png::Palette* palette)");
		// 
		// 	assert(!_headerWritten);
		// 
		// 	// Write header
		// 
		// 	FMT_CALL(writeHeader, file.header, palette);
		// 
		// 	// Write data
		// 
		// 	const uint64_t pixelCount = _header.imageStructure.width * _header.imageStructure.height;
		// 	assert(file.pixels.size() == pixelCount * 4);
		// 	FMT_CALL(writePixels, file.pixels.data(), pixelCount);
		// 
		// 	// Write ending
		// 
		// 	FMT_CALL(writeEnding, file.ending);
		// 
		// 	return *_error;
		// }
		// 
		// template<typename TSample>
		// const ruc::Status& PngOStream::writePixels(const TSample* samples, uint64_t pixelCount)
		// {
		// 	FMT_BEGIN("PngOStream::writePixels(const TSample* samples, uint64_t pixelCount)");
		// 
		// 	assert(_headerWritten);
		// 
		// 	// TODO: Check pixels are coherent with the image structure
		// 
		// 	switch (_header.imageStructure.colorType)
		// 	{
		// 		case png::ColorType::GreyScale:
		// 		{
		// 			if (_header.imageStructure.bitDepth <= 8)
		// 			{
		// 				std::vector<uint8_t> buffer(pixelCount);
		// 				for (uint8_t& raw : buffer)
		// 				{
		// 					convertNum<TSample, uint8_t>(samples, &raw);
		// 					samples += 4;
		// 				}
		// 				FMT_CALL(_writeRawPixels, buffer.data(), pixelCount, 1);
		// 			}
		// 			else
		// 			{
		// 				std::vector<uint16_t> buffer(pixelCount);
		// 				for (uint16_t& raw : buffer)
		// 				{
		// 					convertNum<TSample, uint16_t>(samples, &raw);
		// 					samples += 4;
		// 				}
		// 				FMT_CALL(_writeRawPixels, buffer.data(), pixelCount, 2);
		// 			}
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColor:
		// 		{
		// 			if (_header.imageStructure.bitDepth == 8)
		// 			{
		// 				std::vector<uint8_t> buffer(pixelCount * 3);
		// 				uint8_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<TSample, uint8_t>(++samples, ++it);
		// 					convertNum<TSample, uint8_t>(++samples, ++it);
		// 					convertNum<TSample, uint8_t>(++samples, ++it);
		// 					++samples;
		// 				}
		// 				FMT_CALL(_writeRawPixels, buffer.data(), pixelCount, 3);
		// 			}
		// 			else
		// 			{
		// 				std::vector<uint16_t> buffer(pixelCount * 3);
		// 				uint16_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<TSample, uint16_t>(++samples, ++it);
		// 					convertNum<TSample, uint16_t>(++samples, ++it);
		// 					convertNum<TSample, uint16_t>(++samples, ++it);
		// 					++samples;
		// 				}
		// 				FMT_CALL(_writeRawPixels, buffer.data(), pixelCount, 6);
		// 			}
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::IndexedColor:
		// 		{
		// 			// TODO
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::GreyScaleAlpha:
		// 		{
		// 			if (_header.imageStructure.bitDepth == 8)
		// 			{
		// 				std::vector<uint8_t> buffer(pixelCount * 2);
		// 				uint8_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<TSample, uint8_t>(++samples, ++it);
		// 					++samples;
		// 					++samples;
		// 					convertNum<TSample, uint8_t>(++samples, ++it);
		// 				}
		// 				FMT_CALL(_writeRawPixels, buffer.data(), pixelCount, 2);
		// 			}
		// 			else
		// 			{
		// 				std::vector<uint16_t> buffer(pixelCount * 2);
		// 				uint16_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<TSample, uint16_t>(++samples, ++it);
		// 					++samples;
		// 					++samples;
		// 					convertNum<TSample, uint16_t>(++samples, ++it);
		// 				}
		// 				FMT_CALL(_writeRawPixels, buffer.data(), pixelCount, 4);
		// 			}
		// 
		// 			break;
		// 		}
		// 		case png::ColorType::TrueColorAlpha:
		// 		{
		// 			if (_header.imageStructure.bitDepth == 8)
		// 			{
		// 				std::vector<uint8_t> buffer(pixelCount * 4);
		// 				uint8_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<TSample, uint8_t>(++samples, ++it);
		// 					convertNum<TSample, uint8_t>(++samples, ++it);
		// 					convertNum<TSample, uint8_t>(++samples, ++it);
		// 					convertNum<TSample, uint8_t>(++samples, ++it);
		// 				}
		// 				FMT_CALL(_writeRawPixels, buffer.data(), pixelCount, 4);
		// 			}
		// 			else
		// 			{
		// 				std::vector<uint16_t> buffer(pixelCount * 4);
		// 				uint16_t* it = buffer.data();
		// 				--samples;
		// 				--it;
		// 				for (uint64_t i = 0; i < pixelCount; ++i)
		// 				{
		// 					convertNum<TSample, uint16_t>(++samples, ++it);
		// 					convertNum<TSample, uint16_t>(++samples, ++it);
		// 					convertNum<TSample, uint16_t>(++samples, ++it);
		// 					convertNum<TSample, uint16_t>(++samples, ++it);
		// 				}
		// 				FMT_CALL(_writeRawPixels, buffer.data(), pixelCount, 8);
		// 			}
		// 
		// 			break;
		// 		}
		// 	}
		// 
		// 	return *_error;
		// }
		// 
		// template<typename TValue>
		// const ruc::Status& PngOStream::_writeRawMetadata(const TValue& value, uint32_t& crc)
		// {
		// 	FMT_BEGIN("PngOStream::_writeRawMetadata(const TValue& value, uint32_t& crc)");
		// 
		// 	FMT_CALL(_writeRawMetadata, &value, 1, crc);
		// 
		// 	return *_error;
		// }
		// 
		// template<typename TValue>
		// const ruc::Status& PngOStream::_writeRawMetadata(const TValue* values, uint32_t count, uint32_t& crc)
		// {
		// 	FMT_BEGIN("PngOStream::_writeRawMetadata(const TValue* values, uint32_t count, uint32_t& crc)");
		// 
		// 	if constexpr (std::endian::native != std::endian::big && sizeof(TValue) > 1)
		// 	{
		// 		TValue buffer;
		// 		const TValue* it = values;
		// 		const TValue* const itEnd = it + count;
		// 		for (; it != itEnd; ++it)
		// 		{
		// 			buffer = byteswap(*it);
		// 			crc = cksm::crc32(&buffer, sizeof(TValue), crc);
		// 		}
		// 	}
		// 	else
		// 	{
		// 		crc = cksm::crc32(values, sizeof(TValue) * count, crc);
		// 	}
		// 
		// 	FMT_WRITE(values, count);
		// 
		// 	return *_error;
		// }
	}
}
