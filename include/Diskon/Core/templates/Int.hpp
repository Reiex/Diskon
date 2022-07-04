#pragma once

#include <Diskon/Core/Int.hpp>

namespace dsk
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
		if constexpr (std::numeric_limits<TInt>::is_signed)
		{
			TInt x = (_data[_byteSize - 1] >> ((BitSize - 1) % CHAR_BIT)) * TInt(-1);
			std::memcpy(&x, _data, _byteSize);
			return x;
		}
		else
		{
			TInt x = 0;
			std::memcpy(&x, _data, _byteSize);
			return x;
		}
	}

	template<uint8_t BitSize, typename TInt>
	constexpr void BaseInt<BitSize, TInt>::setInt(TInt x)
	{
		x &= _dataFilter;
		std::memcpy(_data, &x, _byteSize);
	}


	template<uint8_t BitSize>
	constexpr SignedInt<BitSize>::SignedInt(typename Super::IntType x) : Super(x)
	{
	}


	template<uint8_t BitSize>
	constexpr UnsignedInt<BitSize>::UnsignedInt(typename Super::IntType x) : Super(x)
	{
	}
}
