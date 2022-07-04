#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
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

	struct FormatError;
	class FormatStream;
}
