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
	#pragma pack(push, 1)
	template<uint8_t ExpSize, uint8_t MantissaSize>
	class Float
	{
		static_assert(ExpSize + MantissaSize + 1 <= 64);

		public:

			static constexpr uint8_t expSize = ExpSize;
			static constexpr uint8_t mantissaSize = MantissaSize;

			constexpr Float() = default;
			constexpr Float(double x);
			constexpr Float(bool isPositive, uint64_t exponent, uint64_t mantissa);
			constexpr Float(const Float<ExpSize, MantissaSize>& number) = default;
			constexpr Float(Float<ExpSize, MantissaSize>&& number) = default;

			constexpr operator double() const;

			constexpr Float<ExpSize, MantissaSize>& operator=(const Float<ExpSize, MantissaSize>& number) = default;
			constexpr Float<ExpSize, MantissaSize>& operator=(Float<ExpSize, MantissaSize>&& number) = default;

			constexpr bool getPositive() const;
			constexpr uint64_t getExponent() const;
			constexpr uint64_t getMantissa() const;
			constexpr void setPositive(bool positive);
			constexpr void setExponent(uint64_t exponent);
			constexpr void setMantissa(uint64_t mantissa);

			constexpr ~Float() = default;

		private:

			static constexpr uint8_t _byteSize = (ExpSize + MantissaSize + CHAR_BIT) / CHAR_BIT;
			static constexpr uint64_t _expFilter = ((1ULL << ExpSize) - 1) << MantissaSize;
			static constexpr uint64_t _mantissaFilter = (1ULL << MantissaSize) - 1;

			unsigned char _data[_byteSize];
	};
	#pragma pack(pop)
}

namespace std
{
	template<uint8_t ExpSize, uint8_t MantissaSize>
	struct numeric_limits<dsk::Float<ExpSize, MantissaSize>>
	{
		// TODO: Handle NaN

		static constexpr bool is_specialized = true;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = false;
		static constexpr bool is_exact = false;
		static constexpr bool has_infinity = false;
		static constexpr bool has_quiet_NaN = false;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = denorm_present;
		static constexpr bool has_denorm_loss = true;
		static constexpr float_round_style round_style = round_to_nearest;
		static constexpr bool is_iec559 = true;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;
		static constexpr int digits = MantissaSize;
		static constexpr int digits10 = digits * 0.30103;
		static constexpr int max_digits10 = digits10 + 1;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 3 - (1 << (ExpSize - 1));
		static constexpr int min_exponent10 = min_exponent * 0.30103 + 1;
		static constexpr int max_exponent = 1 << (ExpSize - 1);
		static constexpr int max_exponent10 = max_exponent * 0.30103;
		static constexpr bool traps = false;
		static constexpr bool tinyness_before = false;

		static constexpr dsk::Float<ExpSize, MantissaSize> min() noexcept { return { true, 1, 0 }; }
		static constexpr dsk::Float<ExpSize, MantissaSize> lowest() noexcept { return { false, (1ULL << ExpSize) - 2, (1ULL << MantissaSize) - 1 }; }
		static constexpr dsk::Float<ExpSize, MantissaSize> max() noexcept { return { true, (1ULL << ExpSize) - 2, (1ULL << MantissaSize) - 1 }; }
		static constexpr dsk::Float<ExpSize, MantissaSize> epsilon() noexcept { return { true, (((1ULL << ExpSize) - 1) >> 1) - MantissaSize, 0 }; }
		static constexpr dsk::Float<ExpSize, MantissaSize> round_error() noexcept { return 0.5; }
		static constexpr dsk::Float<ExpSize, MantissaSize> infinity() noexcept { return { true, (1ULL << ExpSize) - 1, 0 }; }
		static constexpr dsk::Float<ExpSize, MantissaSize> quiet_NaN() noexcept { return 0; }
		static constexpr dsk::Float<ExpSize, MantissaSize> signaling_NaN() noexcept { return 0; }
		static constexpr dsk::Float<ExpSize, MantissaSize> denorm_min() noexcept { return { true, 0, 1 }; }
	};
}
