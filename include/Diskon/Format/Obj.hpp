#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace obj
	{
		struct Vec3
		{
			long double x = 0.0L;
			long double y = 0.0L;
			long double z = 0.0L;
		};

		struct Vec4
		{
			long double x = 0.0L;
			long double y = 0.0L;
			long double z = 0.0L;
			long double w = 0.0L;
		};

		struct Vertex
		{
			Vec4* position = nullptr;
			Vec3* texCoord = nullptr;
			Vec3* normal = nullptr;
		};
	}

	struct ObjFile
	{
		bool loadFromFile(const std::filesystem::path& path);
		// bool saveToFile(const std::filesystem::path& path);

		std::vector<obj::Vec4> positions;
		std::vector<obj::Vec3> texCoords;
		std::vector<obj::Vec3> normals;
		std::vector<std::vector<obj::Vertex>> faces;
	};
}
