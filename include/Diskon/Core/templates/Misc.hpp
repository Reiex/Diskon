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
	constexpr uint8_t bitswap(uint8_t value)
	{
		constexpr uint8_t tab[256] = {
			0b00000000, 0b10000000, 0b01000000, 0b11000000, 0b00100000, 0b10100000, 0b01100000, 0b11100000,
			0b00010000, 0b10010000, 0b01010000, 0b11010000, 0b00110000, 0b10110000, 0b01110000, 0b11110000,
			0b00001000, 0b10001000, 0b01001000, 0b11001000, 0b00101000, 0b10101000, 0b01101000, 0b11101000,
			0b00011000, 0b10011000, 0b01011000, 0b11011000, 0b00111000, 0b10111000, 0b01111000, 0b11111000,
			0b00000100, 0b10000100, 0b01000100, 0b11000100, 0b00100100, 0b10100100, 0b01100100, 0b11100100,
			0b00010100, 0b10010100, 0b01010100, 0b11010100, 0b00110100, 0b10110100, 0b01110100, 0b11110100,
			0b00001100, 0b10001100, 0b01001100, 0b11001100, 0b00101100, 0b10101100, 0b01101100, 0b11101100,
			0b00011100, 0b10011100, 0b01011100, 0b11011100, 0b00111100, 0b10111100, 0b01111100, 0b11111100,
			0b00000010, 0b10000010, 0b01000010, 0b11000010, 0b00100010, 0b10100010, 0b01100010, 0b11100010,
			0b00010010, 0b10010010, 0b01010010, 0b11010010, 0b00110010, 0b10110010, 0b01110010, 0b11110010,
			0b00001010, 0b10001010, 0b01001010, 0b11001010, 0b00101010, 0b10101010, 0b01101010, 0b11101010,
			0b00011010, 0b10011010, 0b01011010, 0b11011010, 0b00111010, 0b10111010, 0b01111010, 0b11111010,
			0b00000110, 0b10000110, 0b01000110, 0b11000110, 0b00100110, 0b10100110, 0b01100110, 0b11100110,
			0b00010110, 0b10010110, 0b01010110, 0b11010110, 0b00110110, 0b10110110, 0b01110110, 0b11110110,
			0b00001110, 0b10001110, 0b01001110, 0b11001110, 0b00101110, 0b10101110, 0b01101110, 0b11101110,
			0b00011110, 0b10011110, 0b01011110, 0b11011110, 0b00111110, 0b10111110, 0b01111110, 0b11111110,
			0b00000001, 0b10000001, 0b01000001, 0b11000001, 0b00100001, 0b10100001, 0b01100001, 0b11100001,
			0b00010001, 0b10010001, 0b01010001, 0b11010001, 0b00110001, 0b10110001, 0b01110001, 0b11110001,
			0b00001001, 0b10001001, 0b01001001, 0b11001001, 0b00101001, 0b10101001, 0b01101001, 0b11101001,
			0b00011001, 0b10011001, 0b01011001, 0b11011001, 0b00111001, 0b10111001, 0b01111001, 0b11111001,
			0b00000101, 0b10000101, 0b01000101, 0b11000101, 0b00100101, 0b10100101, 0b01100101, 0b11100101,
			0b00010101, 0b10010101, 0b01010101, 0b11010101, 0b00110101, 0b10110101, 0b01110101, 0b11110101,
			0b00001101, 0b10001101, 0b01001101, 0b11001101, 0b00101101, 0b10101101, 0b01101101, 0b11101101,
			0b00011101, 0b10011101, 0b01011101, 0b11011101, 0b00111101, 0b10111101, 0b01111101, 0b11111101,
			0b00000011, 0b10000011, 0b01000011, 0b11000011, 0b00100011, 0b10100011, 0b01100011, 0b11100011,
			0b00010011, 0b10010011, 0b01010011, 0b11010011, 0b00110011, 0b10110011, 0b01110011, 0b11110011,
			0b00001011, 0b10001011, 0b01001011, 0b11001011, 0b00101011, 0b10101011, 0b01101011, 0b11101011,
			0b00011011, 0b10011011, 0b01011011, 0b11011011, 0b00111011, 0b10111011, 0b01111011, 0b11111011,
			0b00000111, 0b10000111, 0b01000111, 0b11000111, 0b00100111, 0b10100111, 0b01100111, 0b11100111,
			0b00010111, 0b10010111, 0b01010111, 0b11010111, 0b00110111, 0b10110111, 0b01110111, 0b11110111,
			0b00001111, 0b10001111, 0b01001111, 0b11001111, 0b00101111, 0b10101111, 0b01101111, 0b11101111,
			0b00011111, 0b10011111, 0b01011111, 0b11011111, 0b00111111, 0b10111111, 0b01111111, 0b11111111,
		};

		return tab[value];
	}


	template<std::endian Endianness, uint8_t SrcOffset, uint8_t DstOffset>
	constexpr void bitcpy(const uint8_t* src, uint8_t* dst, uint64_t byteCount, uint8_t bitCount)
	{
		static_assert(SrcOffset < 8);
		static_assert(DstOffset < 8);

		// TODO: Do not overwrite data in dst out of the range (it should already be the case before the range, but not after the range)

		constexpr uint8_t filterLsb[9] = { 0, 1, 3, 7, 15, 31, 63, 127, 255 };
		constexpr uint8_t filterMsb[9] = { 0, 128, 192, 224, 240, 248, 252, 254, 255 };

		constexpr uint8_t revSrcOffset = 8 - SrcOffset;
		constexpr uint8_t revDstOffset = 8 - DstOffset;

		if constexpr (SrcOffset == 0 && DstOffset == 0)
		{
			std::memcpy(dst, src, byteCount + (bitCount != 0));
			if (bitCount)
			{
				if constexpr (Endianness == std::endian::little)
				{
					*(dst + byteCount) &= filterLsb[bitCount];
				}
				else
				{
					*(dst + byteCount) &= filterMsb[bitCount];
				}
			}
		}
		else if constexpr (SrcOffset == 0)
		{
			const uint8_t* const srcEnd = src + byteCount;

			if constexpr (Endianness == std::endian::little)
			{
				*dst &= filterLsb[DstOffset];
				for (; src != srcEnd; ++src)
				{
					*dst |= (*src << DstOffset);
					*(++dst) = (*src >> revDstOffset);
				}

				if (bitCount)
				{
					*dst |= (*src << DstOffset);
					if (bitCount > revDstOffset)
					{
						*(dst + 1) = (*src >> revDstOffset) & filterLsb[bitCount - revDstOffset];
					}
					else
					{
						*dst &= filterLsb[bitCount + DstOffset];
					}
				}
			}
			else
			{
				*dst &= filterMsb[DstOffset];
				for (; src != srcEnd; ++src)
				{
					*dst |= (*src >> DstOffset);
					*(++dst) = (*src << revDstOffset);
				}

				if (bitCount)
				{
					*dst |= (*src >> DstOffset);
					if (bitCount > revDstOffset)
					{
						*(dst + 1) = (*src << revDstOffset) & filterMsb[bitCount - revDstOffset];
					}
					else
					{
						*dst &= filterMsb[bitCount + DstOffset];
					}
				}
			}
		}
		else if constexpr (DstOffset == 0)
		{
			const uint8_t* const srcEnd = src + byteCount;

			if constexpr (Endianness == std::endian::little)
			{
				for (; src != srcEnd; ++dst, ++src)
				{
					*dst = (*src >> SrcOffset) | (*(src + 1) << revSrcOffset);
				}

				if (bitCount)
				{
					*dst = (*src >> SrcOffset);
					if (bitCount > revSrcOffset)
					{
						*dst |= (*(src + 1) << revSrcOffset);
					}
					*dst &= filterLsb[bitCount];
				}
			}
			else
			{
				for (; src != srcEnd; ++dst, ++src)
				{
					*dst = (*src << SrcOffset) | (*(src + 1) >> revSrcOffset);
				}

				if (bitCount)
				{
					*dst = (*src << SrcOffset);
					if (bitCount > revSrcOffset)
					{
						*dst |= (*(src + 1) >> revSrcOffset);
					}
					*dst &= filterMsb[bitCount];
				}
			}
		}
		else if constexpr (DstOffset == SrcOffset)
		{
			if constexpr (Endianness == std::endian::little)
			{
				if (byteCount == 0 && bitCount <= revDstOffset)
				{
					*dst = (*dst & filterLsb[DstOffset]) | (*src & filterMsb[revDstOffset] & filterLsb[DstOffset + bitCount]);
				}
				else
				{
					byteCount = byteCount - (bitCount <= revDstOffset);
					bitCount = (8 + bitCount - revDstOffset) & 7;

					*dst = (*dst & filterLsb[DstOffset]) | (*src & filterMsb[revDstOffset]);
					std::memcpy(dst + 1, src + 1, byteCount);
					if (bitCount)
					{
						*(dst + byteCount + 1) &= filterLsb[bitCount];
					}
				}
			}
			else
			{
				if (byteCount == 0 && bitCount <= revDstOffset)
				{
					*dst = (*dst & filterMsb[DstOffset]) | (*src & filterLsb[revDstOffset] & filterMsb[DstOffset + bitCount]);
				}
				else
				{
					byteCount = byteCount - (bitCount <= revDstOffset);
					bitCount = (8 + bitCount - revDstOffset) & 7;

					*dst = (*dst & filterMsb[DstOffset]) | (*src & filterLsb[revDstOffset]);
					std::memcpy(dst + 1, src + 1, byteCount);
					if (bitCount)
					{
						*(dst + byteCount + 1) &= filterMsb[bitCount];
					}
				}
			}
		}
		else if constexpr (DstOffset > SrcOffset)
		{
			constexpr uint8_t offset = DstOffset - SrcOffset;
			constexpr uint8_t revOffset = 8 - offset;

			if constexpr (Endianness == std::endian::little)
			{
				if (byteCount == 0 && bitCount <= revDstOffset)
				{
					*dst = (*dst & filterLsb[DstOffset]) | ((*src << offset) & filterMsb[revDstOffset] & filterLsb[DstOffset + bitCount]);
				}
				else
				{
					byteCount = byteCount - (bitCount <= revDstOffset);
					bitCount = (8 + bitCount - revDstOffset) & 7;

					const uint8_t* const srcEnd = src + byteCount;

					*dst = (*dst & filterLsb[DstOffset]) | ((*src << offset) & filterMsb[revDstOffset]);

					for (; src != srcEnd; ++src)
					{
						*(++dst) = (*src >> revOffset) | (*(src + 1) << offset);
					}

					if (bitCount)
					{
						if (bitCount > offset)
						{
							*(++dst) = ((*src >> revOffset) | (*(src + 1) << offset)) & filterLsb[bitCount];
						}
						else
						{
							*(++dst) = (*src >> revOffset) & filterLsb[bitCount];
						}
					}
				}
			}
			else
			{
				if (byteCount == 0 && bitCount <= revDstOffset)
				{
					*dst = (*dst & filterMsb[DstOffset]) | ((*src >> offset) & filterLsb[revDstOffset] & filterMsb[DstOffset + bitCount]);
				}
				else
				{
					byteCount = byteCount - (bitCount <= revDstOffset);
					bitCount = (8 + bitCount - revDstOffset) & 7;

					const uint8_t* const srcEnd = src + byteCount;

					*dst = (*dst & filterMsb[DstOffset]) | ((*src >> offset) & filterLsb[revDstOffset]);

					for (; src != srcEnd; ++src)
					{
						*(++dst) = (*src << revOffset) | (*(src + 1) >> offset);
					}

					if (bitCount)
					{
						if (bitCount > offset)
						{
							*(++dst) = ((*src << revOffset) | (*(src + 1) >> offset)) & filterMsb[bitCount];
						}
						else
						{
							*(++dst) = (*src << revOffset) & filterMsb[bitCount];
						}
					}
				}
			}
		}
		else	// SrcOffset > DstOffset
		{
			constexpr uint8_t offset = DstOffset + revSrcOffset;
			constexpr uint8_t revOffset = 8 - offset;

			if constexpr (Endianness == std::endian::little)
			{
				if (byteCount == 0 && bitCount <= revDstOffset)
				{
					if (bitCount > revSrcOffset)
					{
						*dst = (*dst & filterLsb[DstOffset]) | ((*src >> (SrcOffset - DstOffset)) & filterMsb[revDstOffset]) | ((*(src + 1) << offset) & filterLsb[DstOffset + bitCount]);
					}
					else
					{
						*dst = (*dst & filterLsb[DstOffset]) | ((*src >> (SrcOffset - DstOffset)) & filterMsb[revDstOffset] & filterLsb[DstOffset + bitCount]);
					}
				}
				else
				{
					byteCount = byteCount - (bitCount <= revDstOffset);
					bitCount = (8 + bitCount - revDstOffset) & 7;

					const uint8_t* const dstEnd = dst + byteCount;

					*dst = (*dst & filterLsb[DstOffset]) | ((*src >> (SrcOffset - DstOffset)) & filterMsb[revDstOffset]) | (*(src + 1) << offset);

					for (++src; dst != dstEnd; ++src)
					{
						*(++dst) = (*src >> revOffset) | (*(src + 1) << offset);
					}

					if (bitCount)
					{
						if (bitCount > offset)
						{
							*(++dst) = ((*src >> revOffset) | (*(src + 1) << offset)) & filterLsb[bitCount];
						}
						else
						{
							*(++dst) = (*src >> revOffset) & filterLsb[bitCount];
						}
					}
				}
			}
			else
			{
				if (byteCount == 0 && bitCount <= revDstOffset)
				{
					if (bitCount > revSrcOffset)
					{
						*dst = (*dst & filterMsb[DstOffset]) | ((*src << (SrcOffset - DstOffset)) & filterLsb[revDstOffset]) | ((*(src + 1) >> offset) & filterMsb[DstOffset + bitCount]);
					}
					else
					{
						*dst = (*dst & filterMsb[DstOffset]) | ((*src << (SrcOffset - DstOffset)) & filterLsb[revDstOffset] & filterMsb[DstOffset + bitCount]);
					}
				}
				else
				{
					byteCount = byteCount - (bitCount <= revDstOffset);
					bitCount = (8 + bitCount - revDstOffset) & 7;

					const uint8_t* const dstEnd = dst + byteCount;

					*dst = (*dst & filterMsb[DstOffset]) | ((*src << (SrcOffset - DstOffset)) & filterLsb[revDstOffset]) | (*(src + 1) >> offset);

					for (++src; dst != dstEnd; ++src)
					{
						*(++dst) = (*src << revOffset) | (*(src + 1) >> offset);
					}

					if (bitCount)
					{
						if (bitCount > offset)
						{
							*(++dst) = ((*src << revOffset) | (*(src + 1) >> offset)) & filterMsb[bitCount];
						}
						else
						{
							*(++dst) = (*src << revOffset) & filterMsb[bitCount];
						}
					}
				}
			}
		}
	}

	template<std::endian Endianness, uint8_t SrcOffset, uint8_t DstOffset>
	constexpr void bitcpy(const uint8_t* src, uint8_t* dst, uint64_t bitCount)
	{
		bitcpy<Endianness, SrcOffset, DstOffset>(src, dst, bitCount >> 3, bitCount & 7);
	}

	template<std::endian Endianness>
	constexpr void bitcpy(const uint8_t* src, uint8_t srcOffset, uint8_t* dst, uint8_t dstOffset, uint64_t byteCount, uint8_t bitCount)
	{
		assert(srcOffset < 8);
		assert(dstOffset < 8);

		constexpr void(*tab[64])(const uint8_t*, uint8_t*, uint64_t, uint8_t) = {
			bitcpy<Endianness, 0, 0>, bitcpy<Endianness, 0, 1>, bitcpy<Endianness, 0, 2>, bitcpy<Endianness, 0, 3>,
			bitcpy<Endianness, 0, 4>, bitcpy<Endianness, 0, 5>, bitcpy<Endianness, 0, 6>, bitcpy<Endianness, 0, 7>,
			bitcpy<Endianness, 1, 0>, bitcpy<Endianness, 1, 1>, bitcpy<Endianness, 1, 2>, bitcpy<Endianness, 1, 3>,
			bitcpy<Endianness, 1, 4>, bitcpy<Endianness, 1, 5>, bitcpy<Endianness, 1, 6>, bitcpy<Endianness, 1, 7>,
			bitcpy<Endianness, 2, 0>, bitcpy<Endianness, 2, 1>, bitcpy<Endianness, 2, 2>, bitcpy<Endianness, 2, 3>,
			bitcpy<Endianness, 2, 4>, bitcpy<Endianness, 2, 5>, bitcpy<Endianness, 2, 6>, bitcpy<Endianness, 2, 7>,
			bitcpy<Endianness, 3, 0>, bitcpy<Endianness, 3, 1>, bitcpy<Endianness, 3, 2>, bitcpy<Endianness, 3, 3>,
			bitcpy<Endianness, 3, 4>, bitcpy<Endianness, 3, 5>, bitcpy<Endianness, 3, 6>, bitcpy<Endianness, 3, 7>,
			bitcpy<Endianness, 4, 0>, bitcpy<Endianness, 4, 1>, bitcpy<Endianness, 4, 2>, bitcpy<Endianness, 4, 3>,
			bitcpy<Endianness, 4, 4>, bitcpy<Endianness, 4, 5>, bitcpy<Endianness, 4, 6>, bitcpy<Endianness, 4, 7>,
			bitcpy<Endianness, 5, 0>, bitcpy<Endianness, 5, 1>, bitcpy<Endianness, 5, 2>, bitcpy<Endianness, 5, 3>,
			bitcpy<Endianness, 5, 4>, bitcpy<Endianness, 5, 5>, bitcpy<Endianness, 5, 6>, bitcpy<Endianness, 5, 7>,
			bitcpy<Endianness, 6, 0>, bitcpy<Endianness, 6, 1>, bitcpy<Endianness, 6, 2>, bitcpy<Endianness, 6, 3>,
			bitcpy<Endianness, 6, 4>, bitcpy<Endianness, 6, 5>, bitcpy<Endianness, 6, 6>, bitcpy<Endianness, 6, 7>,
			bitcpy<Endianness, 7, 0>, bitcpy<Endianness, 7, 1>, bitcpy<Endianness, 7, 2>, bitcpy<Endianness, 7, 3>,
			bitcpy<Endianness, 7, 4>, bitcpy<Endianness, 7, 5>, bitcpy<Endianness, 7, 6>, bitcpy<Endianness, 7, 7>,
		};

		tab[(srcOffset << 3) | dstOffset](src, dst, byteCount, bitCount);
	}

	template<std::endian Endianness>
	constexpr void bitcpy(const uint8_t* src, uint8_t srcOffset, uint8_t* dst, uint8_t dstOffset, uint64_t bitCount)
	{
		bitcpy<Endianness>(src, srcOffset, dst, dstOffset, bitCount >> 3, bitCount & 7);
	}

	constexpr void bitcpy(const uint8_t* src, uint8_t srcOffset, uint8_t* dst, uint8_t dstOffset, uint64_t byteCount, uint8_t bitCount, std::endian endianness)
	{
		switch (endianness)
		{
			case std::endian::little:
				return bitcpy<std::endian::little>(src, srcOffset, dst, dstOffset, byteCount, bitCount);
			case std::endian::big:
				return bitcpy<std::endian::big>(src, srcOffset, dst, dstOffset, byteCount, bitCount);
		}
	}

	constexpr void bitcpy(const uint8_t* src, uint8_t srcOffset, uint8_t* dst, uint8_t dstOffset, uint64_t bitCount, std::endian endianness)
	{
		bitcpy(src, srcOffset, dst, dstOffset, bitCount >> 3, bitCount & 7, endianness);
	}
	

	template<CUnsignedIntegral TTo, CUnsignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from)
	{
		if constexpr (sizeof(TFrom) > sizeof(TTo))
		{
			static constexpr uint8_t shift = (sizeof(TFrom) - sizeof(TTo)) * 8;
			return from >> shift;
		}
		else if constexpr (sizeof(TFrom) < sizeof(TTo))
		{
			static constexpr uint8_t shiftInit = ((sizeof(TTo) - sizeof(TFrom)) % sizeof(TFrom)) * 8;
			static constexpr uint8_t shift = sizeof(TFrom) * 8;
			static constexpr uint8_t iter = (sizeof(TTo) - sizeof(TFrom)) / sizeof(TFrom);

			const TTo castedFrom = static_cast<TTo>(from);

			TTo to = castedFrom << shiftInit;
			for (uint8_t i = 0; i < iter; ++i)
			{
				to = (to << shift) | castedFrom;
			}
			return to;
		}
		else
		{
			return from;
		}
	}

	template<CSignedIntegral TTo, CUnsignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from)
	{
		if constexpr (sizeof(TFrom) > sizeof(TTo))
		{
			static constexpr uint8_t shift = (sizeof(TFrom) - sizeof(TTo)) * 8;
			static constexpr TFrom offset = std::numeric_limits<TFrom>::max() / 2;
			return static_cast<TTo>((from + offset) >> shift);
		}
		else if constexpr (sizeof(TFrom) < sizeof(TTo))
		{
			static constexpr uint8_t shift = (sizeof(TTo) - sizeof(TFrom)) * 8;
			static constexpr TTo offset = std::numeric_limits<TTo>::lowest();
			return (static_cast<TTo>(from) << shift) + offset;
		}
		else
		{
			static constexpr TFrom offset = std::numeric_limits<TFrom>::max() / 2;
			return static_cast<TTo>(from + offset);
		}
	}

	template<CFloatingPoint TTo, CUnsignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from)
	{
		static constexpr TTo factor = 2 / static_cast<TTo>(std::numeric_limits<TFrom>::max());
		return static_cast<TTo>(from) * factor - 1;
	}

	template<CUnsignedIntegral TTo, CSignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from)
	{
		static constexpr TTo offset = std::numeric_limits<TTo>::max() / 2;

		if constexpr (sizeof(TFrom) > sizeof(TTo))
		{
			static constexpr uint8_t shift = (sizeof(TFrom) - sizeof(TTo)) * 8;
			return static_cast<TTo>(from >> shift) + offset;
		}
		else if constexpr (sizeof(TFrom) < sizeof(TTo))
		{
			static constexpr TTo smallOffset = (TTo(1) << (sizeof(TFrom) - 1)) - 1;
			static constexpr uint8_t shiftInit = ((sizeof(TTo) - sizeof(TFrom)) % sizeof(TFrom)) * 8;
			static constexpr uint8_t shift = sizeof(TFrom) * 8;
			static constexpr uint8_t iter = (sizeof(TTo) - sizeof(TFrom)) / sizeof(TFrom);

			TTo castedFrom = static_cast<TTo>(from) + smallOffset;

			TTo to = castedFrom << shiftInit;
			for (uint8_t i = 0; i < iter; ++i)
			{
				to = (to << shift) | castedFrom;
			}
			return to;
		}
		else
		{
			return static_cast<TTo>(from) + offset;
		}
	}

	template<CSignedIntegral TTo, CSignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from)
	{
		if constexpr (sizeof(TFrom) > sizeof(TTo))
		{
			static constexpr uint8_t shift = (sizeof(TFrom) - sizeof(TTo)) * 8;
			return from >> shift;
		}
		else if constexpr (sizeof(TFrom) < sizeof(TTo))
		{
			// TODO: Enhance that.
			static constexpr uint8_t shift = (sizeof(TTo) - sizeof(TFrom)) * 8;
			return from << shift;
		}
		else
		{
			return from;
		}
	}

	template<CFloatingPoint TTo, CSignedIntegral TFrom>
	constexpr TTo convertNum(const TFrom from)
	{
		static constexpr TTo divisor = -1 / static_cast<TTo>(std::numeric_limits<TFrom>::lowest());
		return static_cast<TTo>(from) * divisor;
	}

	template<CUnsignedIntegral TTo, CFloatingPoint TFrom>
	constexpr TTo convertNum(const TFrom from)
	{
		static constexpr TFrom factor = static_cast<TFrom>(std::numeric_limits<TTo>::max()) / 2;
		return static_cast<TTo>(from * factor + factor);
	}

	template<CSignedIntegral TTo, CFloatingPoint TFrom>
	constexpr TTo convertNum(const TFrom from)
	{
		static constexpr TFrom factor = -static_cast<TFrom>(std::numeric_limits<TTo>::lowest());
		return static_cast<TTo>(from * factor);
	}

	template<CFloatingPoint TTo, CFloatingPoint TFrom>
	constexpr TTo convertNum(const TFrom from)
	{
		return static_cast<TTo>(from);
	}
}
