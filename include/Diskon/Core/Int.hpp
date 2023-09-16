///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Core/CoreTypes.hpp>

namespace dsk
{
	namespace _dsk
	{
		#pragma pack(push, 1)
		template<uint8_t BitSize, typename TInt>
		class BaseInt
		{
			static_assert(BitSize <= 64);

			public:

				static constexpr uint8_t bitSize = BitSize;
				using IntType = TInt;

				constexpr BaseInt() = default;
				constexpr BaseInt(TInt x);
				constexpr BaseInt(const BaseInt<BitSize, TInt>& integer) = default;
				constexpr BaseInt(BaseInt<BitSize, TInt>&& integer) = default;

				constexpr operator TInt() const;

				constexpr BaseInt<BitSize, TInt>& operator=(const BaseInt<BitSize, TInt>& integer) = default;
				constexpr BaseInt<BitSize, TInt>& operator=(BaseInt<BitSize, TInt>&& integer) = default;

				constexpr BaseInt<BitSize, TInt>& operator+=(const BaseInt<BitSize, TInt>& integer);
				constexpr BaseInt<BitSize, TInt>& operator-=(const BaseInt<BitSize, TInt>& integer);
				constexpr BaseInt<BitSize, TInt>& operator*=(const BaseInt<BitSize, TInt>& integer);
				constexpr BaseInt<BitSize, TInt>& operator/=(const BaseInt<BitSize, TInt>& integer);
				constexpr BaseInt<BitSize, TInt>& operator%=(const BaseInt<BitSize, TInt>& integer);

				constexpr BaseInt<BitSize, TInt>& operator&=(const BaseInt<BitSize, TInt>& integer);
				constexpr BaseInt<BitSize, TInt>& operator|=(const BaseInt<BitSize, TInt>& integer);
				constexpr BaseInt<BitSize, TInt>& operator^=(const BaseInt<BitSize, TInt>& integer);
				constexpr BaseInt<BitSize, TInt>& operator<<=(uint8_t x);
				constexpr BaseInt<BitSize, TInt>& operator>>=(uint8_t x);

				constexpr BaseInt<BitSize, TInt>& operator++();
				constexpr BaseInt<BitSize, TInt> operator++(int);
				constexpr BaseInt<BitSize, TInt>& operator--();
				constexpr BaseInt<BitSize, TInt> operator--(int);
				constexpr BaseInt<BitSize, TInt> operator-() const;
				constexpr BaseInt<BitSize, TInt> operator~() const;

				constexpr bool operator==(const BaseInt<BitSize, TInt>& integer) const;
				constexpr bool operator!=(const BaseInt<BitSize, TInt>& integer) const;
				constexpr bool operator<(const BaseInt<BitSize, TInt>& integer) const;
				constexpr bool operator>(const BaseInt<BitSize, TInt>& integer) const;
				constexpr bool operator<=(const BaseInt<BitSize, TInt>& integer) const;
				constexpr bool operator>=(const BaseInt<BitSize, TInt>& integer) const;

				constexpr ~BaseInt() = default;

			private:

				constexpr TInt getInt() const;
				constexpr void setInt(TInt x);

				static constexpr uint8_t _byteSize = (BitSize + CHAR_BIT - 1) / CHAR_BIT;
				static constexpr TInt _dataFilter = ~((TInt(-1) >> BitSize) << BitSize);

				unsigned char _data[_byteSize];
		};
		#pragma pack(pop)
	}
}

namespace std
{
	template<uint8_t BitSize>
	struct numeric_limits<dsk::UnsignedInt<BitSize>>
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
		static constexpr int digits = BitSize;
		static constexpr int digits10 = digits * 0.30103;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		static constexpr bool traps = std::numeric_limits<typename dsk::UnsignedInt<BitSize>::IntType>::traps;
		static constexpr bool tinyness_before = false;

		static constexpr dsk::UnsignedInt<BitSize> min() noexcept { return 0; }
		static constexpr dsk::UnsignedInt<BitSize> lowest() noexcept { return 0; }
		static constexpr dsk::UnsignedInt<BitSize> max() noexcept { return -1; }
		static constexpr dsk::UnsignedInt<BitSize> epsilon() noexcept { return 0; }
		static constexpr dsk::UnsignedInt<BitSize> round_error() noexcept { return 0; }
		static constexpr dsk::UnsignedInt<BitSize> infinity() noexcept { return 0; }
		static constexpr dsk::UnsignedInt<BitSize> quiet_NaN() noexcept { return 0; }
		static constexpr dsk::UnsignedInt<BitSize> signaling_NaN() noexcept { return 0; }
		static constexpr dsk::UnsignedInt<BitSize> denorm_min() noexcept { return 0; }
	};

	template<uint8_t BitSize>
	struct numeric_limits<dsk::SignedInt<BitSize>>
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
		static constexpr int digits = BitSize;
		static constexpr int digits10 = digits * 0.30103;
		static constexpr int max_digits10 = 0;
		static constexpr int radix = 2;
		static constexpr int min_exponent = 0;
		static constexpr int min_exponent10 = 0;
		static constexpr int max_exponent = 0;
		static constexpr int max_exponent10 = 0;
		static constexpr bool traps = std::numeric_limits<typename dsk::SignedInt<BitSize>::IntType>::traps;
		static constexpr bool tinyness_before = false;

		static constexpr dsk::SignedInt<BitSize> min() noexcept { return 1 << (BitSize - 1); }
		static constexpr dsk::SignedInt<BitSize> lowest() noexcept { return 1 << (BitSize - 1); }
		static constexpr dsk::SignedInt<BitSize> max() noexcept { return (1 << (BitSize - 1)) - 1; }
		static constexpr dsk::SignedInt<BitSize> epsilon() noexcept { return 0; }
		static constexpr dsk::SignedInt<BitSize> round_error() noexcept { return 0; }
		static constexpr dsk::SignedInt<BitSize> infinity() noexcept { return 0; }
		static constexpr dsk::SignedInt<BitSize> quiet_NaN() noexcept { return 0; }
		static constexpr dsk::SignedInt<BitSize> signaling_NaN() noexcept { return 0; }
		static constexpr dsk::SignedInt<BitSize> denorm_min() noexcept { return 0; }
	};
}
