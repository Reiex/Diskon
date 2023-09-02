///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Reiex
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Core/CoreTypes.hpp>

namespace dsk
{
	// See: http://locklessinc.com/articles/sat_arithmetic/

	// TODO: Implement integer promotion
	/*
		- If uint_sat16_t and int16_t or less, saturated arithmetic.
		- If uint_sat16_t and int32_t or more, modulo/UB arithmetic.
	*/

	#pragma pack(push, 1)
	template<typename TInt>
	class UnsignedIntSat
	{
		public:

			using IntType = TInt;

			constexpr UnsignedIntSat() = default;
			constexpr UnsignedIntSat(TInt x);
			constexpr UnsignedIntSat(const UnsignedIntSat<TInt>& integer) = default;
			constexpr UnsignedIntSat(UnsignedIntSat<TInt>&& integer) = default;

			constexpr explicit operator TInt() const;

			constexpr UnsignedIntSat<TInt>& operator=(const UnsignedIntSat<TInt>& integer) = default;
			constexpr UnsignedIntSat<TInt>& operator=(UnsignedIntSat<TInt>&& integer) = default;

			constexpr UnsignedIntSat<TInt>& operator+=(const UnsignedIntSat<TInt>& integer);
			constexpr UnsignedIntSat<TInt>& operator-=(const UnsignedIntSat<TInt>& integer);
			constexpr UnsignedIntSat<TInt>& operator*=(const UnsignedIntSat<TInt>& integer);
			constexpr UnsignedIntSat<TInt>& operator/=(const UnsignedIntSat<TInt>& integer);
			constexpr UnsignedIntSat<TInt>& operator%=(const UnsignedIntSat<TInt>& integer);

			constexpr UnsignedIntSat<TInt>& operator&=(const UnsignedIntSat<TInt>& integer);
			constexpr UnsignedIntSat<TInt>& operator|=(const UnsignedIntSat<TInt>& integer);
			constexpr UnsignedIntSat<TInt>& operator^=(const UnsignedIntSat<TInt>& integer);
			constexpr UnsignedIntSat<TInt>& operator<<=(uint8_t x);
			constexpr UnsignedIntSat<TInt>& operator>>=(uint8_t x);

			constexpr UnsignedIntSat<TInt>& operator++();
			constexpr UnsignedIntSat<TInt> operator++(int);
			constexpr UnsignedIntSat<TInt>& operator--();
			constexpr UnsignedIntSat<TInt> operator--(int);
			constexpr UnsignedIntSat<TInt> operator~() const;

			constexpr bool operator==(const UnsignedIntSat<TInt>& integer) const;
			constexpr bool operator!=(const UnsignedIntSat<TInt>& integer) const;
			constexpr bool operator<(const UnsignedIntSat<TInt>& integer) const;
			constexpr bool operator>(const UnsignedIntSat<TInt>& integer) const;
			constexpr bool operator<=(const UnsignedIntSat<TInt>& integer) const;
			constexpr bool operator>=(const UnsignedIntSat<TInt>& integer) const;

			constexpr ~UnsignedIntSat() = default;

		private:

			TInt _data;
	};
	#pragma pack(pop)


	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator+(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y);
	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator-(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y);
	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator*(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y);
	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator/(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y);
	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator%(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y);

	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator&(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y);
	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator|(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y);
	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator^(UnsignedIntSat<TInt> x, UnsignedIntSat<TInt> y);
	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator<<(UnsignedIntSat<TInt> x, uint8_t y);
	template<typename TInt>
	constexpr UnsignedIntSat<TInt> operator>>(UnsignedIntSat<TInt> x, uint8_t y);

	
	#pragma pack(push, 1)
	template<typename TInt>
	class SignedIntSat
	{
		public:

			using IntType = TInt;

			constexpr SignedIntSat() = default;
			constexpr SignedIntSat(TInt x);
			constexpr SignedIntSat(const SignedIntSat<TInt>& integer) = default;
			constexpr SignedIntSat(SignedIntSat<TInt>&& integer) = default;

			constexpr explicit operator TInt() const;

			constexpr SignedIntSat<TInt>& operator=(const SignedIntSat<TInt>& integer) = default;
			constexpr SignedIntSat<TInt>& operator=(SignedIntSat<TInt>&& integer) = default;

			constexpr SignedIntSat<TInt>& operator+=(const SignedIntSat<TInt>& integer);
			constexpr SignedIntSat<TInt>& operator-=(const SignedIntSat<TInt>& integer);
			constexpr SignedIntSat<TInt>& operator*=(const SignedIntSat<TInt>& integer);
			constexpr SignedIntSat<TInt>& operator/=(const SignedIntSat<TInt>& integer);
			constexpr SignedIntSat<TInt>& operator%=(const SignedIntSat<TInt>& integer);

			constexpr SignedIntSat<TInt>& operator&=(const SignedIntSat<TInt>& integer);
			constexpr SignedIntSat<TInt>& operator|=(const SignedIntSat<TInt>& integer);
			constexpr SignedIntSat<TInt>& operator^=(const SignedIntSat<TInt>& integer);
			constexpr SignedIntSat<TInt>& operator<<=(uint8_t x);
			constexpr SignedIntSat<TInt>& operator>>=(uint8_t x);

			constexpr SignedIntSat<TInt>& operator++();
			constexpr SignedIntSat<TInt> operator++(int);
			constexpr SignedIntSat<TInt>& operator--();
			constexpr SignedIntSat<TInt> operator--(int);
			constexpr SignedIntSat<TInt> operator-() const;
			constexpr SignedIntSat<TInt> operator~() const;

			constexpr bool operator==(const SignedIntSat<TInt>& integer) const;
			constexpr bool operator!=(const SignedIntSat<TInt>& integer) const;
			constexpr bool operator<(const SignedIntSat<TInt>& integer) const;
			constexpr bool operator>(const SignedIntSat<TInt>& integer) const;
			constexpr bool operator<=(const SignedIntSat<TInt>& integer) const;
			constexpr bool operator>=(const SignedIntSat<TInt>& integer) const;

			constexpr ~SignedIntSat() = default;

		private:

			using UIntType = _dsk::UnsignedIntBase<std::numeric_limits<TInt>::digits>;

			TInt _data;
	};
	#pragma pack(pop)


	template<typename TInt>
	constexpr SignedIntSat<TInt> operator+(SignedIntSat<TInt> x, SignedIntSat<TInt> y);
	template<typename TInt>
	constexpr SignedIntSat<TInt> operator-(SignedIntSat<TInt> x, SignedIntSat<TInt> y);
	template<typename TInt>
	constexpr SignedIntSat<TInt> operator*(SignedIntSat<TInt> x, SignedIntSat<TInt> y);
	template<typename TInt>
	constexpr SignedIntSat<TInt> operator/(SignedIntSat<TInt> x, SignedIntSat<TInt> y);
	template<typename TInt>
	constexpr SignedIntSat<TInt> operator%(SignedIntSat<TInt> x, SignedIntSat<TInt> y);

	template<typename TInt>
	constexpr SignedIntSat<TInt> operator&(SignedIntSat<TInt> x, SignedIntSat<TInt> y);
	template<typename TInt>
	constexpr SignedIntSat<TInt> operator|(SignedIntSat<TInt> x, SignedIntSat<TInt> y);
	template<typename TInt>
	constexpr SignedIntSat<TInt> operator^(SignedIntSat<TInt> x, SignedIntSat<TInt> y);
	template<typename TInt>
	constexpr SignedIntSat<TInt> operator<<(SignedIntSat<TInt> x, uint8_t y);
	template<typename TInt>
	constexpr SignedIntSat<TInt> operator>>(SignedIntSat<TInt> x, uint8_t y);
}

namespace std
{
	template<typename TInt>
	struct numeric_limits<dsk::UnsignedIntSat<TInt>>
	{
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = false;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr int digits = std::numeric_limits<TInt>::digits;
		static constexpr int digits10 = digits * 0.30103;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		static constexpr bool traps = std::numeric_limits<TInt>::traps;
		static constexpr bool tinyness_before = false;

		static constexpr dsk::UnsignedIntSat<TInt> min() noexcept { return 0; }
		static constexpr dsk::UnsignedIntSat<TInt> lowest() noexcept { return 0; }
		static constexpr dsk::UnsignedIntSat<TInt> max() noexcept { return -1; }
		static constexpr dsk::UnsignedIntSat<TInt> epsilon() noexcept { return 0; }
		static constexpr dsk::UnsignedIntSat<TInt> round_error() noexcept { return 0; }
		static constexpr dsk::UnsignedIntSat<TInt> infinity() noexcept { return 0; }
		static constexpr dsk::UnsignedIntSat<TInt> quiet_NaN() noexcept { return 0; }
		static constexpr dsk::UnsignedIntSat<TInt> signaling_NaN() noexcept { return 0; }
		static constexpr dsk::UnsignedIntSat<TInt> denorm_min() noexcept { return 0; }
	};

	template<typename TInt>
	struct numeric_limits<dsk::SignedIntSat<TInt>>
	{
		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_exact = true;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_absent;
		static constexpr bool has_denorm_loss = false;
		static constexpr float_round_style round_style = round_toward_zero;
		static constexpr bool is_iec559 = false;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = true;
		static constexpr int digits = std::numeric_limits<TInt>::digits;
		static constexpr int digits10 = digits * 0.30103;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		static constexpr bool traps = std::numeric_limits<TInt>::traps;
		static constexpr bool tinyness_before = false;

		static constexpr dsk::SignedIntSat<TInt> min() noexcept { return std::numeric_limits<TInt>::min(); }
		static constexpr dsk::SignedIntSat<TInt> lowest() noexcept { return std::numeric_limits<TInt>::lowest(); }
		static constexpr dsk::SignedIntSat<TInt> max() noexcept { return std::numeric_limits<TInt>::max(); }
		static constexpr dsk::SignedIntSat<TInt> epsilon() noexcept { return 0; }
		static constexpr dsk::SignedIntSat<TInt> round_error() noexcept { return 0; }
		static constexpr dsk::SignedIntSat<TInt> infinity() noexcept { return 0; }
		static constexpr dsk::SignedIntSat<TInt> quiet_NaN() noexcept { return 0; }
		static constexpr dsk::SignedIntSat<TInt> signaling_NaN() noexcept { return 0; }
		static constexpr dsk::SignedIntSat<TInt> denorm_min() noexcept { return 0; }
	};
}
