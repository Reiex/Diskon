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
	namespace fmt
	{
		class FormatIStream;
		class FormatOStream;

		namespace deflate
		{
			enum class CompressionType : uint8_t;
			struct BlockHeader;
			struct Block;
			struct File;
		}
		class DeflateIStream;
		class DeflateOStream;

		namespace zlib
		{
			enum class CompressionMethod : uint8_t;
			enum class CompressionLevel : uint8_t;
			struct Header;
			struct File;
		}
		class ZlibIStream;
		class ZlibOStream;

		namespace png
		{
			enum class ColorType : uint8_t;
			enum class CompressionMethod : uint8_t;
			enum class FilterMethod : uint8_t;
			enum class InterlaceMethod : uint8_t;
			struct ImageStructure;
			struct Chromaticity;
			struct ICCProfile;
			struct SignificantBits;
			enum class SRGBIntent : uint8_t;
			struct BackgroundColor;
			union TransparencyMask;
			enum class PixelDimensionUnit : uint8_t;
			struct PhysicalPixelDimensions;
			struct PaletteEntry;
			struct Palette;
			struct LastModification;
			struct TextualData;
			struct Header;
			struct Ending;
			template<typename TSample>
			struct File;
		}
		class PngIStream;
		class PngOStream;

		namespace riff
		{
			enum class ChunkType;
			struct ChunkHeader;
			struct Chunk;
			using FileHeader = ChunkHeader;
			using File = Chunk;
		}
		class RiffIStream;
		class RiffOStream;

		namespace wave
		{
			enum class Format : uint16_t;
			struct Metadata;
			struct Header;
			template<typename TSample> struct File;
		}
		class WaveIStream;
		class WaveOStream;

		namespace xml
		{
			struct Declaration;
			struct Doctype;
			struct ProcessingInstruction;
			struct Prolog;
			struct ElementTag;
			struct Element;
			namespace ContentType { enum Flags : uint8_t; }
			struct ElementContent;
			struct Ending;
			struct File;
		}
		class XmlIStream;
		class XmlOStream;

		namespace pnm
		{
			enum class Format;
			struct Header;
			struct File;
		}
		class PnmIStream;
		class PnmOStream;

		namespace obj
		{
			struct VertexPosition;
			struct VertexTextureCoordinate;
			struct VertexNormal;
			struct PointCloud;
			struct LineVertex;
			struct Line;
			struct FaceVertex;
			struct Face;
			struct File;
		}
		class ObjIStream;
		class ObjOStream;
	}
}
