#pragma once

#include <Diskon/Core/Core.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace riff
		{
			struct ChunkHeader;
			struct Chunk;
			struct FileHeader;
			struct File;
		}
		class RifStream;
		
		namespace wave
		{
			enum class Format : uint16_t;
			struct Header;
			struct File;
		}
		class WaveStream;
		
		namespace xml
		{

		}
		class XmlStream;
	}
}
