#pragma once

#include <Diskon/Core/types.hpp>

namespace dsk
{
	namespace _dsk
	{
		constexpr uint8_t baseTypeIndex(uint8_t n)
		{
			if (n <= 8)
			{
				return 0;
			}
			else if (n <= 16)
			{
				return 1;
			}
			else if (n <= 32)
			{
				return 2;
			}
			else
			{
				return 3;
			}
		}
	
		template<uint8_t BitSize>
		struct SignedIntType { using Type = typename std::tuple_element<_dsk::baseTypeIndex(BitSize), std::tuple<int8_t, int16_t, int32_t, int64_t>>::type; };

		template<uint8_t BitSize>
		struct UnsignedIntType { using Type = typename std::tuple_element<_dsk::baseTypeIndex(BitSize), std::tuple<uint8_t, uint16_t, uint32_t, uint64_t>>::type; };
	}

	template<uint8_t BitSize, typename TInt>
	class BaseInt
	{
		public:

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


	template<uint8_t BitSize>
	class SignedInt : public BaseInt<BitSize, typename _dsk::SignedIntType<BitSize>::Type>
	{
		public:

			using Super = BaseInt<BitSize, typename _dsk::SignedIntType<BitSize>::Type>;

			constexpr SignedInt() = default;
			constexpr SignedInt(typename Super::IntType x);
			constexpr SignedInt(const SignedInt<BitSize>& integer) = default;
			constexpr SignedInt(SignedInt<BitSize>&& integer) = default;

			constexpr SignedInt<BitSize>& operator=(const SignedInt<BitSize>& integer) = default;
			constexpr SignedInt<BitSize>& operator=(SignedInt<BitSize>&& integer) = default;

			constexpr ~SignedInt() = default;
	};


	template<uint8_t BitSize>
	class UnsignedInt : public BaseInt<BitSize, typename _dsk::UnsignedIntType<BitSize>::Type>
	{
		public:

			using Super = BaseInt<BitSize, typename _dsk::UnsignedIntType<BitSize>::Type>;

			constexpr UnsignedInt() = default;
			constexpr UnsignedInt(typename Super::IntType x);
			constexpr UnsignedInt(const UnsignedInt<BitSize>& integer) = default;
			constexpr UnsignedInt(UnsignedInt<BitSize>&& integer) = default;

			constexpr UnsignedInt<BitSize>& operator=(const UnsignedInt<BitSize>& integer) = default;
			constexpr UnsignedInt<BitSize>& operator=(UnsignedInt<BitSize>&& integer) = default;

			constexpr ~UnsignedInt() = default;
	};
}

namespace std
{
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
		static constexpr int digits10 = BitSize * 0.30103;
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
		static constexpr int digits10 = digits * log10(2);
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
}

#include <Diskon/Core/templates/Int.hpp>
