///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Reiex
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

				const ruc::Status& readFile(obj::File& file);

				~ObjIStream() = default;

			private:

				void resetFormatState() override final;

				const ruc::Status& _readVertexPosition(obj::VertexPosition& position);
				const ruc::Status& _readVertexTextureCoordinate(obj::VertexTextureCoordinate& texCoord);
				const ruc::Status& _readVertexNormal(obj::VertexNormal& normal);

				const ruc::Status& _readPointCloud(const obj::File& file, obj::PointCloud& pointCloud);
				const ruc::Status& _readLine(const obj::File& file, obj::Line& line);
				const ruc::Status& _readFace(const obj::File& file, obj::Face& face);

				const ruc::Status& _readSlashSeparatedIndices(const uint64_t* sizes, uint64_t* indices, uint64_t count);
		};

		class DSK_API ObjOStream : public FormatOStream
		{
			public:

				ObjOStream(OStream* stream);
				ObjOStream(const ObjOStream& stream) = delete;
				ObjOStream(ObjOStream&& streal) = delete;

				ObjOStream& operator=(const ObjOStream& stream) = delete;
				ObjOStream& operator=(ObjOStream&& stream) = delete;

				const ruc::Status& writeFile(const obj::File& file);

				~ObjOStream() = default;

			private:

				void resetFormatState() override final;

				const ruc::Status& _writeVertexPosition(const obj::VertexPosition& position);
				const ruc::Status& _writeVertexTextureCoordinate(const obj::VertexTextureCoordinate& texCoord);
				const ruc::Status& _writeVertexNormal(const obj::VertexNormal& normal);

				const ruc::Status& _writePointCloud(const obj::File& file, const obj::PointCloud& pointCloud);
				const ruc::Status& _writeLine(const obj::File& file, const obj::Line& line);
				const ruc::Status& _writeFace(const obj::File& file, const obj::Face& face);
		};
	}
}
