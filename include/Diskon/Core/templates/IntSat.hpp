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
	template<typename TInt>
	constexpr UnsignedIntSat<TInt>::UnsignedIntSat(TInt x) :
		_data(x)
	{
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>::operator TInt() const
	{
		return _data;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator+=(const UnsignedIntSat<TInt>& integer)
	{
		_data += integer._data;
		_data |= -(_data < integer._data);
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator-=(const UnsignedIntSat<TInt>& integer)
	{
		TInt tmp = _data;
		_data -= integer._data;
		_data &= -(_data < tmp);
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator*=(const UnsignedIntSat<TInt>& integer)
	{
		constexpr uint8_t bitSize = std::numeric_limits<TInt>::digits;
		constexpr uint8_t bufferBitSize = 2 * bitSize;

		_dsk::UnsignedIntBase<bufferBitSize> buffer = static_cast<_dsk::UnsignedIntBase<bufferBitSize>>(_data) * static_cast<_dsk::UnsignedIntBase<bufferBitSize>>(integer._data);
		_data = buffer | -static_cast<bool>(buffer >> bitSize);

		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator/=(const UnsignedIntSat<TInt>& integer)
	{
		_data /= integer._data;
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator%=(const UnsignedIntSat<TInt>& integer)
	{
		_data %= integer._data;
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator&=(const UnsignedIntSat<TInt>& integer)
	{
		_data &= integer._data;
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator|=(const UnsignedIntSat<TInt>& integer)
	{
		_data |= integer._data;
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator^=(const UnsignedIntSat<TInt>& integer)
	{
		_data ^= integer._data;
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator<<=(uint8_t x)
	{
		// TODO: Saturating ?
		_data <<= x;
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator>>=(uint8_t x)
	{
		_data >>= x;
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator++()
	{
		_data += (_data != std::numeric_limits<TInt>::max());
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> UnsignedIntSat<TInt>::operator++(int)
	{
		UnsignedIntSat<TInt> tmp(*this);
		operator++();
		return tmp;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt>& UnsignedIntSat<TInt>::operator--()
	{
		_data -= (_data != 0);
		return *this;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> UnsignedIntSat<TInt>::operator--(int)
	{
		UnsignedIntSat<TInt> tmp(*this);
		operator--();
		return tmp;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> UnsignedIntSat<TInt>::operator~() const
	{
		return ~_data;
	}

	template<typename TInt>
	constexpr bool UnsignedIntSat<TInt>::operator==(const UnsignedIntSat<TInt>& integer) const
	{
		return _data == integer._data;
	}

	template<typename TInt>
	constexpr bool UnsignedIntSat<TInt>::operator!=(const UnsignedIntSat<TInt>& integer) const
	{
		return _data != integer._data;
	}

	template<typename TInt>
	constexpr bool UnsignedIntSat<TInt>::operator<(const UnsignedIntSat<TInt>& integer) const
	{
		return _data < integer._data;
	}

	template<typename TInt>
	constexpr bool UnsignedIntSat<TInt>::operator>(const UnsignedIntSat<TInt>& integer) const
	{
		return _data > integer._data;
	}

	template<typename TInt>
	constexpr bool UnsignedIntSat<TInt>::operator<=(const UnsignedIntSat<TInt>& integer) const
	{
		return _data <= integer._data;
	}

	template<typename TInt>
	constexpr bool UnsignedIntSat<TInt>::operator>=(const UnsignedIntSat<TInt>& integer) const
	{
		return _data >= integer._data;
	}


	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator+(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y)
	{
		x += y;
		return x;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator-(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y)
	{
		x -= y;
		return x;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator*(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y)
	{
		x *= y;
		return x;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator/(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y)
	{
		x /= y;
		return x;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator%(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y)
	{
		x %= y;
		return x;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator&(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y)
	{
		x &= y;
		return x;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator|(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y)
	{
		x |= y;
		return x;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator^(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y)
	{
		x ^= y;
		return x;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator<<(UnsignedIntSat<TInt> x, uint8_t y)
	{
		x <<= y;
		return x;
	}

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator>>(UnsignedIntSat<TInt> x, uint8_t y)
	{
		x >>= y;
		return x;
	}


	template<typename TInt>
	constexpr SignedIntSat<TInt>::SignedIntSat(TInt x) :
		_data(x)
	{
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>::operator TInt() const
	{
		return _data;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator+=(const SignedIntSat<TInt>& integer)
	{
		UIntType ux = _data;
		UIntType uy = integer._data;
		UIntType res = ux + uy;

		ux = (ux >> std::numeric_limits<TInt>::digits) + std::numeric_limits<TInt>::max();

		if (static_cast<TInt>((ux ^ uy) | ~(uy ^ res)) >= 0)
		{
			res = ux;
		}

		_data = static_cast<TInt>(res);

		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator-=(const SignedIntSat<TInt>& integer)
	{
		UIntType ux = _data;
		UIntType uy = integer._data;
		UIntType res = ux - uy;

		ux = (ux >> std::numeric_limits<TInt>::digits) + std::numeric_limits<TInt>::max();

		if (static_cast<TInt>((ux ^ uy) & (ux ^ res)) >= 0)
		{
			res = ux;
		}

		_data = static_cast<TInt>(res);

		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator*=(const SignedIntSat<TInt>& integer)
	{
		constexpr uint8_t bitSize = std::numeric_limits<TInt>::digits;
		constexpr uint8_t bufferBitSize = 2 * bitSize;

		_dsk::SignedIntBase<bufferBitSize> buffer = static_cast<_dsk::SignedIntBase<bufferBitSize>>(_data) * static_cast<_dsk::SignedIntBase<bufferBitSize>>(integer._data);
		UIntType res = (static_cast<UIntType>(_data ^ integer._data) >> bitSize) + std::numeric_limits<TInt>::max();

		TInt hi = buffer >> bitSize;
		_data = buffer;

		if (hi != (_data >> bitSize))
		{
			_data = res;
		}

		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator/=(const SignedIntSat<TInt>& integer)
	{
		_data += !((integer._data + 1) | (static_cast<UIntType>(_data) + std::numeric_limits<TInt>::max()));
		_data /= integer._data;

		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator%=(const SignedIntSat<TInt>& integer)
	{
		_data %= integer._data;
		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator&=(const SignedIntSat<TInt>& integer)
	{
		_data &= integer._data;
		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator|=(const SignedIntSat<TInt>& integer)
	{
		_data |= integer._data;
		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator^=(const SignedIntSat<TInt>& integer)
	{
		_data ^= integer._data;
		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator<<=(uint8_t x)
	{
		// TODO: saturate ?
		_data <<= x;
		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator>>=(uint8_t x)
	{
		_data >>= x;
		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator++()
	{
		_data += (_data != std::numeric_limits<TInt>::max());
		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> SignedIntSat<TInt>::operator++(int)
	{
		SignedIntSat<TInt> tmp(*this);
		operator++();
		return tmp;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt>& SignedIntSat<TInt>::operator--()
	{
		_data -= (_data != std::numeric_limits<TInt>::min());
		return *this;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> SignedIntSat<TInt>::operator--(int)
	{
		SignedIntSat<TInt> tmp(*this);
		operator--();
		return tmp;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> SignedIntSat<TInt>::operator-() const
	{
		// TODO: if _data == min, return max and not "-_data"
		return -_data;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> SignedIntSat<TInt>::operator~() const
	{
		return ~_data;
	}

	template<typename TInt>
	constexpr bool SignedIntSat<TInt>::operator==(const SignedIntSat<TInt>& integer) const
	{
		return _data == integer._data;
	}

	template<typename TInt>
	constexpr bool SignedIntSat<TInt>::operator!=(const SignedIntSat<TInt>& integer) const
	{
		return _data != integer._data;
	}

	template<typename TInt>
	constexpr bool SignedIntSat<TInt>::operator<(const SignedIntSat<TInt>& integer) const
	{
		return _data < integer._data;
	}

	template<typename TInt>
	constexpr bool SignedIntSat<TInt>::operator>(const SignedIntSat<TInt>& integer) const
	{
		return _data > integer._data;
	}

	template<typename TInt>
	constexpr bool SignedIntSat<TInt>::operator<=(const SignedIntSat<TInt>& integer) const
	{
		return _data <= integer._data;
	}

	template<typename TInt>
	constexpr bool SignedIntSat<TInt>::operator>=(const SignedIntSat<TInt>& integer) const
	{
		return _data >= integer._data;
	}


	template<typename TInt>
	constexpr SignedIntSat<TInt> operator+(SignedIntSat<TInt> x, SignedIntSat<TInt> y)
	{
		x += y;
		return x;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> operator-(SignedIntSat<TInt> x, SignedIntSat<TInt> y)
	{
		x -= y;
		return x;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> operator*(SignedIntSat<TInt> x, SignedIntSat<TInt> y)
	{
		x *= y;
		return x;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> operator/(SignedIntSat<TInt> x, SignedIntSat<TInt> y)
	{
		x /= y;
		return x;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> operator%(SignedIntSat<TInt> x, SignedIntSat<TInt> y)
	{
		x %= y;
		return x;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> operator&(SignedIntSat<TInt> x, SignedIntSat<TInt> y)
	{
		x &= y;
		return x;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> operator|(SignedIntSat<TInt> x, SignedIntSat<TInt> y)
	{
		x |= y;
		return x;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> operator^(SignedIntSat<TInt> x, SignedIntSat<TInt> y)
	{
		x ^= y;
		return x;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> operator<<(SignedIntSat<TInt> x, uint8_t y)
	{
		x <<= y;
		return x;
	}

	template<typename TInt>
	constexpr SignedIntSat<TInt> operator>>(SignedIntSat<TInt> x, uint8_t y)
	{
		x >>= y;
		return x;
	}
}
