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
		void WaveIStream::readFile(wave::File<TSample>& file)
		{
			DSKFMT_BEGIN();
		
			DSK_CALL(readHeader, _header);
			file.metadata = _header.metadata;
			file.samples.resize(_header.blockCount * _header.metadata.channelCount);
		
			DSK_CALL(readSampleBlocks, file.samples.data(), _header.blockCount);
		
			DSK_CALL(readEndFile);
		}
		
		template<typename TSample>
		void WaveIStream::readSampleBlocks(TSample* samples, uint32_t blockCount)
		{
			DSKFMT_BEGIN();
		
			assert(_headerRead);
			assert(_remainingBlocks >= blockCount);
		
			switch (_header.metadata.format)
			{
				case wave::Format::PCM:
				{
					void (WaveIStream::*reader)(TSample*, uint32_t) = nullptr;
		
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
		
					DSK_CALL((this->*reader), samples, blockCount);
		
					break;
				}
				case wave::Format::Float:
				{
					void (WaveIStream::*reader)(TSample*, uint32_t) = nullptr;
		
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
		
					DSK_CALL((this->*reader), samples, blockCount);
		
					break;
				}
			}
		
			_remainingBlocks -= blockCount;
		}
		
		template<typename TRaw, typename TSample>
		void WaveIStream::_readRawSampleBlocks(TSample* samples, uint32_t blockCount)
		{
			DSKFMT_BEGIN();
		
			const uint32_t sampleCount = _header.metadata.channelCount * blockCount;

			if constexpr (std::same_as<TRaw, TSample>)
			{
				DSK_CALL(_riffStream->readChunkData, samples, sampleCount);
			}
			else if constexpr (sizeof(TRaw) <= sizeof(TSample))
			{
				TRaw* raw = reinterpret_cast<TRaw*>(samples);
				DSK_CALL(_riffStream->readChunkData, raw, sampleCount);

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
					DSK_CALL(_riffStream->readChunkData, buffer, bufferCount);
					std::transform(buffer, buffer + bufferCount, samples, convertNum<TSample, TRaw>);

					samples += bufferCount;
					remainingSamples -= bufferCount;
				}

				DSK_CALL(_riffStream->readChunkData, buffer, remainingSamples);
				std::transform(buffer, buffer + remainingSamples, samples, convertNum<TSample, TRaw>);
			}
		}
		
		
		template<typename TSample>
		void WaveOStream::writeFile(const wave::File<TSample>& file)
		{
			DSKFMT_BEGIN();
		
			assert(!_headerWritten);
			assert(_remainingBlocks == 0);
		
			_header.metadata = file.metadata;
			_header.blockCount = file.samples.size() / file.metadata.channelCount;
		
			DSK_CALL(writeHeader, _header);
			DSK_CALL(writeSampleBlocks, file.samples.data(), _header.blockCount);
		}
		
		template<typename TSample>
		void WaveOStream::writeSampleBlocks(const TSample* samples, uint32_t blockCount)
		{
			DSKFMT_BEGIN();
		
			assert(_headerWritten);
			assert(_remainingBlocks >= blockCount);
		
			switch (_header.metadata.format)
			{
				case wave::Format::PCM:
				{
					void (WaveOStream::*writer)(const TSample*, uint32_t) = nullptr;
		
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
		
					DSK_CALL((this->*writer), samples, blockCount);
		
					break;
				}
				case wave::Format::Float:
				{
					void (WaveOStream::*writer)(const TSample*, uint32_t) = nullptr;
		
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
		
					DSK_CALL((this->*writer), samples, blockCount);
		
					break;
				}
			}
		
			_remainingBlocks -= blockCount;
		}
		
		template<typename TRaw, typename TSample>
		void WaveOStream::_writeRawSampleBlocks(const TSample* samples, uint32_t blockCount)
		{
			DSKFMT_BEGIN();
		
			const uint32_t sampleCount = _header.metadata.channelCount * blockCount;

			if constexpr (std::same_as<TRaw, TSample>)
			{
				DSK_CALL(_riffStream->writeChunkData, samples, sampleCount);
			}
			else
			{
				constexpr uint64_t bufferCount = _singleBufferSize / sizeof(TRaw);
				TRaw buffer[bufferCount];

				uint32_t remainingSamples = sampleCount;
				while (remainingSamples > bufferCount)
				{
					std::transform(samples, samples + bufferCount, buffer, convertNum<TRaw, TSample>);
					DSK_CALL(_riffStream->writeChunkData, buffer, bufferCount);

					samples += bufferCount;
					remainingSamples -= bufferCount;
				}

				std::transform(samples, samples + remainingSamples, buffer, convertNum<TRaw, TSample>);
				DSK_CALL(_riffStream->writeChunkData, buffer, remainingSamples);
			}
		}
	}
}
