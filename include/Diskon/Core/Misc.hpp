///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Marius Pélégrin
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Core/CoreTypes.hpp>

namespace dsk
{
	constexpr uint8_t bitswap(uint8_t value);

	template<std::endian Endianness, uint8_t SrcOffset, uint8_t DstOffset>
	constexpr void bitcpy(const uint8_t* src, uint8_t* dst, uint64_t byteCount, uint8_t bitCount);
	template<std::endian Endianness, uint8_t SrcOffset, uint8_t DstOffset>
	constexpr void bitcpy(const uint8_t* src, uint8_t* dst, uint64_t bitCount);
	template<std::endian Endianness>
	constexpr void bitcpy(const uint8_t* src, uint8_t srcOffset, uint8_t* dst, uint8_t dstOffset, uint64_t byteCount, uint8_t bitCount);
	template<std::endian Endianness>
	constexpr void bitcpy(const uint8_t* src, uint8_t srcOffset, uint8_t* dst, uint8_t dstOffset, uint64_t bitCount);
	constexpr void bitcpy(const uint8_t* src, uint8_t srcOffset, uint8_t* dst, uint8_t dstOffset, uint64_t byteCount, uint8_t bitCount, std::endian endianness);
	constexpr void bitcpy(const uint8_t* src, uint8_t srcOffset, uint8_t* dst, uint8_t dstOffset, uint64_t bitCount, std::endian endianness);

	template<CUnsignedIntegral TTo, CUnsignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from);
	template<CSignedIntegral TTo, CUnsignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from);
	template<CFloatingPoint TTo, CUnsignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from);
	template<CUnsignedIntegral TTo, CSignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from);
	template<CSignedIntegral TTo, CSignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from);
	template<CFloatingPoint TTo, CSignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from);
	template<CUnsignedIntegral TTo, CFloatingPoint TFrom>
	constexpr TTo convertNum(const TFrom from);
	template<CSignedIntegral TTo, CFloatingPoint TFrom>
	constexpr TTo convertNum(const TFrom from);
	template<CFloatingPoint TTo, CFloatingPoint TFrom>
	constexpr TTo convertNum(const TFrom from);
}
