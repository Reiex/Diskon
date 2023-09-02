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
	namespace _dsk
	{
		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>::BaseInt(TInt x)
		{
			setInt(x);
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>::operator TInt() const
		{
			return getInt();
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator+=(const BaseInt<BitSize, TInt>& integer)
		{
			setInt(getInt() + integer);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator-=(const BaseInt<BitSize, TInt>& integer)
		{
			setInt(getInt() - integer);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator*=(const BaseInt<BitSize, TInt>& integer)
		{
			setInt(getInt() * integer);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator/=(const BaseInt<BitSize, TInt>& integer)
		{
			setInt(getInt() / integer);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator%=(const BaseInt<BitSize, TInt>& integer)
		{
			setInt(getInt() % integer);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator&=(const BaseInt<BitSize, TInt>& integer)
		{
			setInt(getInt() & integer);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator|=(const BaseInt<BitSize, TInt>& integer)
		{
			setInt(getInt() | integer);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator^=(const BaseInt<BitSize, TInt>& integer)
		{
			setInt(getInt() ^ integer);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator<<=(uint8_t x)
		{
			setInt(getInt() << x);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator>>=(uint8_t x)
		{
			setInt(getInt() >> x);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator++()
		{
			setInt(getInt() + 1);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt> BaseInt<BitSize, TInt>::operator++(int)
		{
			TInt tmp = getInt();
			setInt(tmp + 1);
			return tmp;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt>& BaseInt<BitSize, TInt>::operator--()
		{
			setInt(getInt() - 1);
			return *this;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt> BaseInt<BitSize, TInt>::operator--(int)
		{
			TInt tmp = getInt();
			setInt(tmp - 1);
			return tmp;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt> BaseInt<BitSize, TInt>::operator-() const
		{
			return -getInt();
		}

		template<uint8_t BitSize, typename TInt>
		constexpr BaseInt<BitSize, TInt> BaseInt<BitSize, TInt>::operator~() const
		{
			return ~getInt();
		}

		template<uint8_t BitSize, typename TInt>
		constexpr bool BaseInt<BitSize, TInt>::operator==(const BaseInt<BitSize, TInt>& integer) const
		{
			return std::equal(_data, _data + _byteSize, integer._data);
		}

		template<uint8_t BitSize, typename TInt>
		constexpr bool BaseInt<BitSize, TInt>::operator!=(const BaseInt<BitSize, TInt>& integer) const
		{
			return !std::equal(_data, _data + _byteSize, integer._data);
		}

		template<uint8_t BitSize, typename TInt>
		constexpr bool BaseInt<BitSize, TInt>::operator<(const BaseInt<BitSize, TInt>& integer) const
		{
			return getInt() < integer.getInt();
		}

		template<uint8_t BitSize, typename TInt>
		constexpr bool BaseInt<BitSize, TInt>::operator>(const BaseInt<BitSize, TInt>& integer) const
		{
			return getInt() > integer.getInt();
		}

		template<uint8_t BitSize, typename TInt>
		constexpr bool BaseInt<BitSize, TInt>::operator<=(const BaseInt<BitSize, TInt>& integer) const
		{
			return getInt() <= integer.getInt();
		}

		template<uint8_t BitSize, typename TInt>
		constexpr bool BaseInt<BitSize, TInt>::operator>=(const BaseInt<BitSize, TInt>& integer) const
		{
			return getInt() >= integer.getInt();
		}

		template<uint8_t BitSize, typename TInt>
		constexpr TInt BaseInt<BitSize, TInt>::getInt() const
		{
			TInt x;
			if constexpr (std::numeric_limits<TInt>::is_signed)
			{
				x = (_data[_byteSize - 1] >> ((BitSize - 1) % CHAR_BIT)) * TInt(-1);
			}
			else
			{
				x = 0;
			}

			if constexpr (std::is_constant_evaluated())
			{
				for (uint8_t i = 0; i < _byteSize; ++i)
				{
					x <<= CHAR_BIT;
					if (std::endian::native == std::endian::little)
					{
						x |= _data[_byteSize - i - 1];
					}
					else
					{
						x |= _data[i];
					}
				}
			}
			else
			{
				std::copy_n(reinterpret_cast<const uint8_t*>(&x), _byteSize, _data);
			}

			return x;
		}

		template<uint8_t BitSize, typename TInt>
		constexpr void BaseInt<BitSize, TInt>::setInt(TInt x)
		{
			x &= _dataFilter;
			if constexpr (std::is_constant_evaluated())
			{
				for (uint8_t i = 0; i < _byteSize; ++i)
				{
					if (std::endian::native == std::endian::little)
					{
						_data[i] = x & 255;
					}
					else
					{
						_data[_byteSize - i - 1] = x & 255;
					}
					x >>= CHAR_BIT;
				}
			}
			else
			{
				std::copy_n(reinterpret_cast<const uint8_t*>(&x), _byteSize, _data);
			}
		}
	}
}
