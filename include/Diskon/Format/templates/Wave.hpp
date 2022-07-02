#pragma once

#include <Diskon/Format/Wave.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace wave
		{
			namespace _wave
			{
				template<typename TFrom, typename TTo>
				void convertFromTo(const void* from, void* to)
				{
					using LimitsFrom = std::numeric_limits<TFrom>;
					using LimitsTo = std::numeric_limits<TTo>;

					static_assert(LimitsFrom::is_specialized);
					static_assert(LimitsTo::is_specialized);

					constexpr size_t sizeBitsFrom = sizeof(TFrom) * 8;
					constexpr size_t sizeBitsTo = sizeof(TTo) * 8;

					if constexpr (LimitsFrom::is_integer && LimitsTo::is_integer)
					{
						if constexpr (LimitsFrom::is_signed == LimitsTo::is_signed)
						{
							if constexpr (sizeBitsFrom > sizeBitsTo)
							{
								constexpr uint64_t divisor = 1ULL << (sizeBitsFrom - sizeBitsTo);
								*reinterpret_cast<TTo*>(to) = *reinterpret_cast<const TFrom*>(from) / divisor;
							}
							else if constexpr (sizeBitsFrom < sizeBitsTo)
							{
								constexpr uint64_t multiplier = 1ULL << (sizeBitsTo - sizeBitsFrom);
								*reinterpret_cast<TTo*>(to) = static_cast<TTo>(*reinterpret_cast<const TFrom*>(from)) * multiplier;
							}
							else
							{
								*reinterpret_cast<TTo*>(to) = *reinterpret_cast<const TFrom*>(from);
							}
						}
						else if  constexpr (LimitsFrom::is_signed && !LimitsTo::is_signed)
						{
							if constexpr (sizeBitsFrom > sizeBitsTo)
							{
								constexpr uint64_t divisor = 1ULL << (sizeBitsFrom - sizeBitsTo);
								*reinterpret_cast<TTo*>(to) = (*reinterpret_cast<const TFrom*>(from) / divisor) + (-LimitsFrom::min() / divisor);
							}
							else if constexpr (sizeBitsFrom < sizeBitsTo)
							{
								constexpr uint64_t multiplier = 1ULL << (sizeBitsTo - sizeBitsFrom);
								*reinterpret_cast<TTo*>(to) = (static_cast<TTo>(*reinterpret_cast<const TFrom*>(from)) + static_cast<TTo>(-LimitsFrom::min())) * multiplier;
							}
							else
							{
								*reinterpret_cast<TTo*>(to) = static_cast<TTo>(*reinterpret_cast<const TFrom*>(from)) + static_cast<TTo>(-LimitsFrom::min());
							}
						}
						else
						{
							if constexpr (sizeBitsFrom > sizeBitsTo)
							{
								constexpr uint64_t divisor = 1ULL << (sizeBitsFrom - sizeBitsTo);
								*reinterpret_cast<TTo*>(to) = LimitsTo::min() + *reinterpret_cast<const TFrom*>(from) / divisor;
							}
							else if constexpr (sizeBitsFrom < sizeBitsTo)
							{
								constexpr uint64_t multiplier = 1ULL << (sizeBitsTo - sizeBitsFrom);
								*reinterpret_cast<TTo*>(to) = LimitsTo::min() + static_cast<TTo>(*reinterpret_cast<const TFrom*>(from)) * multiplier;
							}
							else
							{
								*reinterpret_cast<TTo*>(to) = LimitsTo::min() + *reinterpret_cast<const TFrom*>(from);
							}
						}
					}
					else if constexpr (!LimitsFrom::is_integer && !LimitsTo::is_integer)
					{
						*reinterpret_cast<TTo*>(to) = *reinterpret_cast<const TFrom*>(from);
					}
					else if constexpr (LimitsFrom::is_integer && !LimitsTo::is_integer)
					{
						if constexpr (LimitsFrom::is_signed)
						{
							*reinterpret_cast<TTo*>(to) = (static_cast<TTo>(*reinterpret_cast<const TFrom*>(from))) / LimitsFrom::max();
						}
						else
						{
							*reinterpret_cast<TTo*>(to) = ((static_cast<TTo>(*reinterpret_cast<const TFrom*>(from))) / LimitsFrom::max()) * 2 - 1;
						}
					}
					else
					{
						if constexpr (LimitsTo::is_signed)
						{
							*reinterpret_cast<TTo*>(to) = *reinterpret_cast<const TFrom*>(from) * (LimitsTo::max() - 1);
						}
						else
						{
							*reinterpret_cast<TTo*>(to) = ((*reinterpret_cast<const TFrom*>(from) + 1) / 2) * LimitsTo::max();
						}
					}
				}
			}


			template<typename TTo>
			void File::_getSamples(uint16_t channel, TTo* samples) const
			{
				assert(header.isValid());
				assert(rawSamples.size() == header.blockAlign * header.blockCount);

				File::castFromRawSamples<TTo>(header, rawSamples.data(), samples, channel, header.channels, header.blockCount);
			}

			template<typename TFrom>
			void File::_setSamples(uint16_t channel, const TFrom* samples)
			{
				assert(header.isValid());
				assert(rawSamples.size() == header.blockAlign * header.blockCount);

				File::castToRawSamples<TFrom>(header, rawSamples.data(), samples, channel, header.channels, header.blockCount);
			}

			template<typename TTo>
			void File::castFromRawSamples(const Header& header, const void* raw, void* data, uint32_t offset, uint32_t stride, uint32_t count)
			{
				void (*conversionFunction)(const void*, void*) = nullptr;

				switch (header.formatTag)
				{
					case wave::Format::PCM:
					{
						if (header.bitsPerSample <= 8)			{ conversionFunction = wave::_wave::convertFromTo<uint8_t, TTo>; }
						else if (header.bitsPerSample <= 16)	{ conversionFunction = wave::_wave::convertFromTo<int16_t, TTo>; }
						else if (header.bitsPerSample <= 24)	{ /*TODO*/ }
						else if (header.bitsPerSample <= 32)	{ conversionFunction = wave::_wave::convertFromTo<int32_t, TTo>; }
						else if (header.bitsPerSample <= 40)	{ /*TODO*/ }
						else if (header.bitsPerSample <= 48)	{ /*TODO*/ }
						else if (header.bitsPerSample <= 56)	{ /*TODO*/ }
						else if (header.bitsPerSample <= 64)	{ conversionFunction = wave::_wave::convertFromTo<int64_t, TTo>; }

						const uint16_t bytesPerSample = (header.bitsPerSample + 7) / 8;
						offset *= bytesPerSample;
						stride *= bytesPerSample;

						break;
					}
					case wave::Format::Float:
					{
						if (header.bitsPerSample == 16)			{ /*TODO*/ }
						else if (header.bitsPerSample == 32)	{ conversionFunction = wave::_wave::convertFromTo<float, TTo>; }
						else if (header.bitsPerSample == 64)	{ conversionFunction = wave::_wave::convertFromTo<double, TTo>; }

						const uint16_t bytesPerSample = (header.bitsPerSample + 7) / 8;
						offset *= bytesPerSample;
						stride *= bytesPerSample;

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
					const char* itRaw = ((const char*) raw) + offset;
					char* itData = (char*) data;
					for (uint32_t i = 0; i < count; ++i)
					{
						conversionFunction(itRaw, itData);

						itRaw += stride;
						itData += sizeof(TTo);
					}
				}
			}

			template<typename TFrom>
			void File::castToRawSamples(const Header& header, void* raw, const void* data, uint32_t offset, uint32_t stride, uint32_t count)
			{
				void (*conversionFunction)(const void*, void*) = nullptr;

				switch (header.formatTag)
				{
					case wave::Format::PCM:
					{
						if (header.bitsPerSample <= 8)			{ conversionFunction = wave::_wave::convertFromTo<TFrom, uint8_t>; }
						else if (header.bitsPerSample <= 16)	{ conversionFunction = wave::_wave::convertFromTo<TFrom, int16_t>; }
						else if (header.bitsPerSample <= 24)	{ /*TODO*/ }
						else if (header.bitsPerSample <= 32)	{ conversionFunction = wave::_wave::convertFromTo<TFrom, int32_t>; }
						else if (header.bitsPerSample <= 40)	{ /*TODO*/ }
						else if (header.bitsPerSample <= 48)	{ /*TODO*/ }
						else if (header.bitsPerSample <= 56)	{ /*TODO*/ }
						else if (header.bitsPerSample <= 64)	{ conversionFunction = wave::_wave::convertFromTo<TFrom, int64_t>; }

						const uint16_t bytesPerSample = (header.bitsPerSample + 7) / 8;
						offset *= bytesPerSample;
						stride *= bytesPerSample;

						break;
					}
					case wave::Format::Float:
					{
						if (header.bitsPerSample == 16)			{ /*TODO*/ }
						else if (header.bitsPerSample == 32)	{ conversionFunction = wave::_wave::convertFromTo<TFrom, float>; }
						else if (header.bitsPerSample == 64)	{ conversionFunction = wave::_wave::convertFromTo<TFrom, double>; }

						const uint16_t bytesPerSample = (header.bitsPerSample + 7) / 8;
						offset *= bytesPerSample;
						stride *= bytesPerSample;

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
					char* itRaw = ((char*) raw) + offset;
					const char* itData = (const char*) data;
					for (uint32_t i = 0; i < count; ++i)
					{
						conversionFunction(itData, itRaw);

						itRaw += stride;
						itData += sizeof(TFrom);
					}
				}
			}
		}


		template<typename TTo>
		const FormatError& WaveStream::_readSampleBlock(const wave::Header& header, TTo* sampleBlock)
		{
			assert(header.isValid());

			FMTSTREAM_BEGIN_READ();

			char* buffer = (char*) alloca(header.blockAlign);
			FMTSTREAM_VERIFY(stream.read(buffer, header.blockAlign), InvalidStream, "WaveStream: Error while reading sample block.");

			wave::File::castFromRawSamples<TTo>(header, buffer, sampleBlock, 0, 1, header.channels);

			return error;
		}

		template<typename TFrom>
		const FormatError& WaveStream::_writeSampleBlock(const wave::Header& header, const TFrom* sampleBlock)
		{
			assert(header.isValid());

			FMTSTREAM_BEGIN_WRITE();

			char* buffer = (char*) alloca(header.blockAlign);
			wave::File::castToRawSamples<TFrom>(header, buffer, sampleBlock, 0, 1, header.channels);

			FMTSTREAM_VERIFY(stream.write(buffer, header.blockAlign), InvalidStream, "WaveStream: Error while writing sample block.");

			return error;
		}
	}
}
