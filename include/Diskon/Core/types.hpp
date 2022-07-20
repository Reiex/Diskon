#pragma once

#include <array>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace dsk
{
	template<uint8_t BitSize, typename TInt> class BaseInt;
	template<uint8_t BitSize> class SignedInt;
	template<uint8_t BitSize> class UnsignedInt;
	template<uint8_t ExpSize, uint8_t MantissaSize> class Float;
	using Half = Float<5, 10>;

	struct FormatError;
	class FormatStream;
}
