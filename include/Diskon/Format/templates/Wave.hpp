#pragma once

#include <Diskon/Format/Wave.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace _fmt
		{
			template<typename TFrom, typename TTo>
			void convertFromTo(const void* from, void* to)
			{
				using LimitsFrom = std::numeric_limits<TFrom>;
				using LimitsTo = std::numeric_limits<TTo>;

				static_assert(LimitsFrom::is_specialized);
				static_assert(LimitsFrom::is_signed);
				static_assert(LimitsTo::is_specialized);
				static_assert(LimitsTo::is_signed);

				constexpr size_t sizeBitsFrom = sizeof(TFrom) * 8;
				constexpr size_t sizeBitsTo = sizeof(TTo) * 8;

				if constexpr (LimitsFrom::is_integer && LimitsTo::is_integer)
				{
					if constexpr (sizeBitsFrom > sizeBitsTo)
					{
						constexpr uint64_t divisor = 1 << (sizeBitsFrom - sizeBitsTo);
						*reinterpret_cast<TTo*>(to) = *reinterpret_cast<const TFrom*>(from) / divisor;
					}
					else if constexpr (sizeBitsFrom < sizeBitsTo)
					{
						constexpr uint64_t multiplier = 1 << (sizeBitsTo - sizeBitsFrom);
						*reinterpret_cast<TTo*>(to) = *reinterpret_cast<const TFrom*>(from) * multiplier;
					}
					else
					{
						*reinterpret_cast<TTo*>(to) = *reinterpret_cast<const TFrom*>(from);
					}
				}
				else if constexpr (!LimitsFrom::is_integer && !LimitsTo::is_integer)
				{
					*reinterpret_cast<TTo*>(to) = *reinterpret_cast<const TFrom*>(from);
				}
				else if constexpr (LimitsFrom::is_integer && !LimitsTo::is_integer)
				{
					*reinterpret_cast<TTo*>(to) = (static_cast<TTo>(*reinterpret_cast<const TFrom*>(from))) / LimitsFrom::max();
				}
				else
				{
					*reinterpret_cast<TTo*>(to) = *reinterpret_cast<const TFrom*>(from) * (LimitsTo::max() - 1);
				}
			}
		}

		template<typename TTo>
		void WaveFile::getRawSamples(uint16_t channel, void* data) const
		{
			assert(_sampleCount > 0);
			assert(channel < _formatChunk.channels);

			void (*conversionFunction)(const void*, void*) = nullptr;

			switch (_formatChunk.formatTag)
			{
				case wave::WaveFormat::PCM:
				{
					switch (_formatChunk.bitsPerSample)
					{
						case 8:
							conversionFunction = _fmt::convertFromTo<int8_t, TTo>;
							break;
						case 16:
							conversionFunction = _fmt::convertFromTo<int16_t, TTo>;
							break;
						case 32:
							conversionFunction = _fmt::convertFromTo<int32_t, TTo>;
							break;
						case 64:
							conversionFunction = _fmt::convertFromTo<int64_t, TTo>;
							break;
						default:
							assert(false);
							break;
					}
					break;
				}
				case wave::WaveFormat::Float:
				{
					switch (_formatChunk.bitsPerSample)
					{
						case 32:
							conversionFunction = _fmt::convertFromTo<float, TTo>;
							break;
						case 64:
							conversionFunction = _fmt::convertFromTo<double, TTo>;
							break;
						default:
							assert(false);
							break;
					}
					break;
				}
				default:
				{
					assert(false);
					break;
				}
			}

			if (conversionFunction)
			{
				const char* itSamples = _rawSamples.data() + channel * (_formatChunk.blockAlign / _formatChunk.channels);
				const char* const itSamplesEnd = itSamples + _sampleCount * _formatChunk.blockAlign;
				char* itData = static_cast<char*>(data);
				while (itSamples != itSamplesEnd)
				{
					conversionFunction(itSamples, itData);
					itSamples += _formatChunk.blockAlign;
					itData += sizeof(TTo);
				}
			}
		}

		template<typename TFrom>
		void WaveFile::setRawSamples(uint16_t channel, const void* data)
		{
			assert(_sampleCount > 0);
			assert(channel < _formatChunk.channels);

			void (*conversionFunction)(const void*, void*) = nullptr;

			switch (_formatChunk.formatTag)
			{
				case wave::WaveFormat::PCM:
				{
					switch (_formatChunk.bitsPerSample)
					{
						case 8:
							conversionFunction = _fmt::convertFromTo<TFrom, int8_t>;
							break;
						case 16:
							conversionFunction = _fmt::convertFromTo<TFrom, int16_t>;
							break;
						case 32:
							conversionFunction = _fmt::convertFromTo<TFrom, int32_t>;
							break;
						case 64:
							conversionFunction = _fmt::convertFromTo<TFrom, int64_t>;
							break;
						default:
							assert(false);
							break;
					}
					break;
				}
				case wave::WaveFormat::Float:
				{
					switch (_formatChunk.bitsPerSample)
					{
						case 32:
							conversionFunction = _fmt::convertFromTo<TFrom, float>;
							break;
						case 64:
							conversionFunction = _fmt::convertFromTo<TFrom, double>;
							break;
						default:
							assert(false);
							break;
					}
					break;
				}
				default:
				{
					assert(false);
					break;
				}
			}

			if (conversionFunction)
			{
				char* itSamples = _rawSamples.data() + channel * (_formatChunk.blockAlign / _formatChunk.channels);
				char* const itSamplesEnd = itSamples + _sampleCount * _formatChunk.blockAlign;
				const char* itData = static_cast<const char*>(data);
				while (itSamples != itSamplesEnd)
				{
					conversionFunction(itData, itSamples);
					itSamples += _formatChunk.blockAlign;
					itData += sizeof(TFrom);
				}
			}
		}
	}
}
