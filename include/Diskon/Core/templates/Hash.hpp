///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Reiex
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Core/CoreDecl.hpp>

namespace dsk
{
	constexpr uint32_t fourcc(const char* id)
	{
		return (id[0] << 24) | (id[1] << 16) | (id[2] << 8) | id[3];
	}

	namespace _dsk
	{
		template<typename TValue, TValue RevPoly>
		constexpr Crc<TValue, RevPoly>::Crc()
		{
			for (uint16_t i = 0; i < 256; ++i)
			{
				_table[i] = i;
				for (uint8_t j = 0; j < 8; ++j)
				{
					if (_table[i] & 1)
					{
						_table[i] = RevPoly ^ (_table[i] >> 1);
					}
					else
					{
						_table[i] >>= 1;
					}
				}
			}
		}

		template<typename TValue, TValue RevPoly>
		constexpr TValue Crc<TValue, RevPoly>::operator()(const void* src, uint64_t size, TValue crc) const
		{
			crc = ~crc;

			const uint8_t* it = reinterpret_cast<const uint8_t*>(src);
			const uint8_t* const itEnd = it + size;
			for (; it != itEnd; ++it)
			{
				crc = _table[static_cast<uint8_t>(crc & 0xFF) ^ *it] ^ (crc >> 8);
			}

			return ~crc;
		}


		template<typename TValue, typename THalf, TValue Modulus>
		constexpr TValue Fletcher<TValue, THalf, Modulus>::operator()(const void* src, uint64_t size, TValue initialValue) const
		{
			static constexpr uint8_t halfShift = sizeof(TValue) * 4;
			static constexpr TValue lowFilter = std::numeric_limits<TValue>::max() >> halfShift;
			static constexpr uint64_t sizeFilter = ~(sizeof(THalf) - 1);

			TValue a = initialValue & lowFilter;
			TValue b = initialValue >> halfShift;

			const THalf* it = reinterpret_cast<const THalf*>(src);
			const THalf* const itEnd = it + (size & sizeFilter);
			for (; it != itEnd; ++it)
			{
				a = (a + *it) % Modulus;
				b = (a + b) % Modulus;
			}

			return (b << halfShift) | a;
		}
	}

	constexpr _dsk::Crc<uint32_t, 0xEDB88320> crc32;

	constexpr _dsk::Fletcher<uint16_t, uint8_t, 0xFF> fletcher16;
	constexpr _dsk::Fletcher<uint32_t, uint16_t, 0xFFFF> fletcher32;
	constexpr _dsk::Fletcher<uint64_t, uint32_t, 0xFFFFFFFF> fletcher64;
	constexpr _dsk::Fletcher<uint32_t, uint8_t, 65521> adler32;
}
