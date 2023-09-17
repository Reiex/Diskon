///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Format/FormatTypes.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace obj
		{
			struct VertexPosition
			{
				double x, y, z, w;
			};

			struct VertexTextureCoordinate
			{
				double u, v, w;
			};

			struct VertexNormal
			{
				double i, j, k;
			};

			struct PointCloud
			{
				std::vector<uint64_t> vertices;
			};

			struct LineVertex
			{
				uint64_t position;
				uint64_t texCoord;
			};

			struct Line
			{
				std::vector<LineVertex> vertices;
			};

			struct FaceVertex
			{
				uint64_t position;
				uint64_t texCoord;
				uint64_t normal;
			};

			struct Face
			{
				std::vector<FaceVertex> vertices;
			};

			struct File
			{
				std::vector<VertexPosition> positions;
				std::vector<VertexTextureCoordinate> texCoords;
				std::vector<VertexNormal> normals;
				// TODO: Free form attributes

				std::vector<PointCloud> pointClouds;
				std::vector<Line> lines;
				std::vector<Face> faces;
				// TODO: curv, curv2, surf...

				// TODO: Others...
			};
		}

		class DSK_API ObjIStream : public FormatIStream
		{
			public:

				ObjIStream(IStream* stream);
				ObjIStream(const ObjIStream& stream) = delete;
				ObjIStream(ObjIStream&& stream) = delete;

				ObjIStream& operator=(const ObjIStream& stream) = delete;
				ObjIStream& operator=(ObjIStream&& stream) = delete;

				void readFile(obj::File& file);

				~ObjIStream() = default;

			private:

				void resetFormatState() override final;

				void _readVertexPosition(obj::VertexPosition& position);
				void _readVertexTextureCoordinate(obj::VertexTextureCoordinate& texCoord);
				void _readVertexNormal(obj::VertexNormal& normal);

				void _readPointCloud(const obj::File& file, obj::PointCloud& pointCloud);
				void _readLine(const obj::File& file, obj::Line& line);
				void _readFace(const obj::File& file, obj::Face& face);

				void _readSlashSeparatedIndices(const uint64_t* sizes, uint64_t* indices, uint64_t count);
		};

		class DSK_API ObjOStream : public FormatOStream
		{
			public:

				ObjOStream(OStream* stream);
				ObjOStream(const ObjOStream& stream) = delete;
				ObjOStream(ObjOStream&& streal) = delete;

				ObjOStream& operator=(const ObjOStream& stream) = delete;
				ObjOStream& operator=(ObjOStream&& stream) = delete;

				void writeFile(const obj::File& file);

				~ObjOStream() = default;

			private:

				void resetFormatState() override final;

				void _writeVertexPosition(const obj::VertexPosition& position);
				void _writeVertexTextureCoordinate(const obj::VertexTextureCoordinate& texCoord);
				void _writeVertexNormal(const obj::VertexNormal& normal);

				void _writePointCloud(const obj::File& file, const obj::PointCloud& pointCloud);
				void _writeLine(const obj::File& file, const obj::Line& line);
				void _writeFace(const obj::File& file, const obj::Face& face);
		};
	}
}
