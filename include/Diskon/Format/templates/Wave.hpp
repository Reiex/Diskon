///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Format/FormatDecl.hpp>

namespace dsk
{
	namespace fmt
	{
		template<typename TSample>
		const ruc::Status& WaveIStream::readFile(wave::File<TSample>& file)
		{
			DSKFMT_BEGIN();
		
			DSKFMT_CALL(readHeader, _header);
			file.metadata = _header.metadata;
			file.samples.resize(_header.blockCount * _header.metadata.channelCount);
		
			DSKFMT_CALL(readSampleBlocks, file.samples.data(), _header.blockCount);
		
			DSKFMT_CALL(readEndFile);
		
			return _stream->getStatus();
		}
		
		template<typename TSample>
		const ruc::Status& WaveIStream::readSampleBlocks(TSample* samples, uint32_t blockCount)
		{
			DSKFMT_BEGIN();
		
			assert(_headerRead);
			assert(_remainingBlocks >= blockCount);
		
			switch (_header.metadata.format)
			{
				case wave::Format::PCM:
				{
					const ruc::Status& (WaveIStream::*reader)(TSample*, uint32_t) = nullptr;
		
					switch (_header.metadata.bitsPerSample)
					{
						case 8:
						{
							reader = &WaveIStream::_readRawSampleBlocks<uint8_t, TSample>;
							break;
						}
						case 16:
						{
							reader = &WaveIStream::_readRawSampleBlocks<int16_t, TSample>;
							break;
						}
						case 24:
						{
							reader = &WaveIStream::_readRawSampleBlocks<int24_t, TSample>;
							break;
						}
						case 32:
						{
							reader = &WaveIStream::_readRawSampleBlocks<int32_t, TSample>;
							break;
						}
						case 64:
						{
							reader = &WaveIStream::_readRawSampleBlocks<int64_t, TSample>;
							break;
						}
					}
		
					DSKFMT_CALL((this->*reader), samples, blockCount);
		
					break;
				}
				case wave::Format::Float:
				{
					const ruc::Status& (WaveIStream::*reader)(TSample*, uint32_t) = nullptr;
		
					switch (_header.metadata.bitsPerSample)
					{
						case 32:
						{
							reader = &WaveIStream::_readRawSampleBlocks<float, TSample>;
							break;
						}
						case 64:
						{
							reader = &WaveIStream::_readRawSampleBlocks<double, TSample>;
							break;
						}
					}
		
					DSKFMT_CALL((this->*reader), samples, blockCount);
		
					break;
				}
			}
		
			_remainingBlocks -= blockCount;
		
			return _stream->getStatus();
		}
		
		template<typename TRaw, typename TSample>
		const ruc::Status& WaveIStream::_readRawSampleBlocks(TSample* samples, uint32_t blockCount)
		{
			DSKFMT_BEGIN();
		
			const uint32_t sampleCount = _header.metadata.channelCount * blockCount;

			if constexpr (std::same_as<TRaw, TSample>)
			{
				DSKFMT_CALL(_riffStream->readChunkData, samples, sampleCount);
			}
			else if constexpr (sizeof(TRaw) <= sizeof(TSample))
			{
				TRaw* raw = reinterpret_cast<TRaw*>(samples);
				DSKFMT_CALL(_riffStream->readChunkData, raw, sampleCount);

				const TSample* const samplesEnd = samples;

				samples += sampleCount;
				raw += sampleCount;
				for (; samples != samplesEnd;)
				{
					*(--samples) = convertNum<TSample>(*(--raw));
				}
			}
			else
			{
				constexpr uint64_t bufferCount = _singleBufferSize / sizeof(TRaw);
				TRaw buffer[bufferCount];
				
				uint32_t remainingSamples = sampleCount;
				while (remainingSamples > bufferCount)
				{
					DSKFMT_CALL(_riffStream->readChunkData, buffer, bufferCount);
					std::transform(buffer, buffer + bufferCount, samples, convertNum<TSample, TRaw>);

					samples += bufferCount;
					remainingSamples -= bufferCount;
				}

				DSKFMT_CALL(_riffStream->readChunkData, buffer, remainingSamples);
				std::transform(buffer, buffer + remainingSamples, samples, convertNum<TSample, TRaw>);
			}
		
			return _stream->getStatus();
		}
		
		
		template<typename TSample>
		const ruc::Status& WaveOStream::writeFile(const wave::File<TSample>& file)
		{
			DSKFMT_BEGIN();
		
			assert(!_headerWritten);
			assert(_remainingBlocks == 0);
		
			_header.metadata = file.metadata;
			_header.blockCount = file.samples.size() / file.metadata.channelCount;
		
			DSKFMT_CALL(writeHeader, _header);
			DSKFMT_CALL(writeSampleBlocks, file.samples.data(), _header.blockCount);
		
			return _stream->getStatus();
		}
		
		template<typename TSample>
		const ruc::Status& WaveOStream::writeSampleBlocks(const TSample* samples, uint32_t blockCount)
		{
			DSKFMT_BEGIN();
		
			assert(_headerWritten);
			assert(_remainingBlocks >= blockCount);
		
			switch (_header.metadata.format)
			{
				case wave::Format::PCM:
				{
					const ruc::Status& (WaveOStream::*writer)(const TSample*, uint32_t) = nullptr;
		
					switch (_header.metadata.bitsPerSample)
					{
						case 8:
						{
							writer = &WaveOStream::_writeRawSampleBlocks<uint8_t, TSample>;
							break;
						}
						case 16:
						{
							writer = &WaveOStream::_writeRawSampleBlocks<int16_t, TSample>;
							break;
						}
						case 24:
						{
							writer = &WaveOStream::_writeRawSampleBlocks<int24_t, TSample>;
							break;
						}
						case 32:
						{
							writer = &WaveOStream::_writeRawSampleBlocks<int32_t, TSample>;
							break;
						}
						case 64:
						{
							writer = &WaveOStream::_writeRawSampleBlocks<int64_t, TSample>;
							break;
						}
					}
		
					DSKFMT_CALL((this->*writer), samples, blockCount);
		
					break;
				}
				case wave::Format::Float:
				{
					const ruc::Status& (WaveOStream::*writer)(const TSample*, uint32_t) = nullptr;
		
					switch (_header.metadata.bitsPerSample)
					{
						case 32:
						{
							writer = &WaveOStream::_writeRawSampleBlocks<float, TSample>;
							break;
						}
						case 64:
						{
							writer = &WaveOStream::_writeRawSampleBlocks<double, TSample>;
							break;
						}
					}
		
					DSKFMT_CALL((this->*writer), samples, blockCount);
		
					break;
				}
			}
		
			_remainingBlocks -= blockCount;
		
			return _stream->getStatus();
		}
		
		template<typename TRaw, typename TSample>
		const ruc::Status& WaveOStream::_writeRawSampleBlocks(const TSample* samples, uint32_t blockCount)
		{
			DSKFMT_BEGIN();
		
			const uint32_t sampleCount = _header.metadata.channelCount * blockCount;

			if constexpr (std::same_as<TRaw, TSample>)
			{
				DSKFMT_CALL(_riffStream->writeChunkData, samples, sampleCount);
			}
			else
			{
				constexpr uint64_t bufferCount = _singleBufferSize / sizeof(TRaw);
				TRaw buffer[bufferCount];

				uint32_t remainingSamples = sampleCount;
				while (remainingSamples > bufferCount)
				{
					std::transform(samples, samples + bufferCount, buffer, convertNum<TRaw, TSample>);
					DSKFMT_CALL(_riffStream->writeChunkData, buffer, bufferCount);

					samples += bufferCount;
					remainingSamples -= bufferCount;
				}

				std::transform(samples, samples + remainingSamples, buffer, convertNum<TRaw, TSample>);
				DSKFMT_CALL(_riffStream->writeChunkData, buffer, remainingSamples);
			}
		
			return _stream->getStatus();
		}
	}
}
