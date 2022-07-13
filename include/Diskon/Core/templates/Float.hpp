#pragma once

#include <Diskon/Core/Float.hpp>

namespace dsk
{
	template<uint8_t ExpSize, uint8_t MantissaSize>
	constexpr Float<ExpSize, MantissaSize>::Float(double x)
	{
		uint64_t xInt = *reinterpret_cast<uint64_t*>(&x);

		setPositive(!(xInt & (1ULL << 63)));
		setExponent((xInt << 1) >> (64 - ExpSize));
		setMantissa((xInt << 12) >> (64 - MantissaSize));
	}

	template<uint8_t ExpSize, uint8_t MantissaSize>
	constexpr Float<ExpSize, MantissaSize>::Float(bool isPositive, uint64_t exponent, uint64_t mantissa)
	{
		setPositive(isPositive);
		setExponent(exponent);
		setMantissa(mantissa);
	}

	template<uint8_t ExpSize, uint8_t MantissaSize>
	constexpr Float<ExpSize, MantissaSize>::operator double() const
	{
		Float<11, 52> result;
		result.setPositive(getPositive());
		result.setExponent((getExponent() << (11 - ExpSize)) | ((1 << (11 - ExpSize)) - 1));
		result.setMantissa(getMantissa() << (52 - MantissaSize));

		return *reinterpret_cast<double*>(&result);
	}

	template<uint8_t ExpSize, uint8_t MantissaSize>
	constexpr bool Float<ExpSize, MantissaSize>::getPositive() const
	{
		return !((*reinterpret_cast<const uint64_t*>(_data) >> (MantissaSize + ExpSize)) & 1);
	}

	template<uint8_t ExpSize, uint8_t MantissaSize>
	constexpr uint64_t Float<ExpSize, MantissaSize>::getExponent() const
	{
		return (*reinterpret_cast<const uint64_t*>(_data) & _expFilter) >> MantissaSize;
	}

	template<uint8_t ExpSize, uint8_t MantissaSize>
	constexpr uint64_t Float<ExpSize, MantissaSize>::getMantissa() const
	{
		return *reinterpret_cast<const uint64_t*>(_data) & _mantissaFilter;
	}

	template<uint8_t ExpSize, uint8_t MantissaSize>
	constexpr void Float<ExpSize, MantissaSize>::setPositive(bool positive)
	{
		if (positive)
		{
			*reinterpret_cast<uint64_t*>(_data) &= ~(1ULL << (MantissaSize + ExpSize));
		}
		else
		{
			*reinterpret_cast<uint64_t*>(_data) |= 1ULL << (MantissaSize + ExpSize);
		}
	}

	template<uint8_t ExpSize, uint8_t MantissaSize>
	constexpr void Float<ExpSize, MantissaSize>::setExponent(uint64_t exponent)
	{
		// TODO: assert exponent in good range

		uint64_t* pData = reinterpret_cast<uint64_t*>(_data);
		*pData &= ~_expFilter;
		*pData |= _expFilter & (exponent << MantissaSize);
	}

	template<uint8_t ExpSize, uint8_t MantissaSize>
	constexpr void Float<ExpSize, MantissaSize>::setMantissa(uint64_t mantissa)
	{
		// TODO: assert mantissa in good range

		uint64_t* pData = reinterpret_cast<uint64_t*>(_data);
		*pData &= ~_mantissaFilter;
		*pData |= _mantissaFilter & mantissa;
	}
}
