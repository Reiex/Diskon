///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace
		{
			constexpr bool isSpaceChar(char x)
			{
				return x == ' ' || x == '\t';
			}

			constexpr bool isTokenChar(char x)
			{
				return x != ' ' && x != '\t' && x != '\r' && x != '\n' && x != '#';
			}

			constexpr bool isFirstIntegerChar(char x)
			{
				return x == '-' || x == '+' || (x >= '0' && x <= '9');
			}

			constexpr bool isFirstNumberChar(char x)
			{
				return x == '.' || isFirstIntegerChar(x);
			}

			constexpr uint64_t hashToken(const char* token)
			{
				uint64_t h = 0xEDB88320;
				while (*token)
				{
					h = (h << 4) + h + *token;
					++token;
				}
				return h;
			}
		}

		ObjIStream::ObjIStream(IStream* stream) : FormatIStream(stream)
		{
		}

		void ObjIStream::readFile(obj::File& file)
		{
			DSKFMT_BEGIN();

			file = obj::File();

			char buffer[11];
			const char* const bufferEnd = buffer + 11;
			uint64_t skipCount, readCount;
			while (!_stream->eof())
			{
				DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);

				std::fill_n(buffer, 11, '\0');
				DSKFMT_STREAM_CALL(readCharWhile, isTokenChar, buffer, 11, readCount);
				DSK_CHECK(readCount != 11, "Token too long to be recognized.");

				if (readCount == 0)
				{
					if (!_stream->eof())
					{
						DSKFMT_STREAM_CALL(read, buffer[0]);
						switch (buffer[0])
						{
							case '#':
								DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);
								break;
							case '\r':
								DSKFMT_STREAM_CALL(read, buffer[0]);
								DSK_CHECK(buffer[0] == '\n', std::format("Expected \\n, instead got '{}'", buffer[0]));
								break;
							case '\n':
								break;
							default:
								assert(false);
								break;
						}
					}

					continue;
				}

				switch (hashToken(buffer))
				{
					case hashToken("#"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);
						break;
					}
					case hashToken("bevel"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("bmat"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("c_interp"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("con"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("cstype"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("ctech"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("curv"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("curv2"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("d_interp"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("deg"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("end"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("f"):
					{
						file.faces.emplace_back();
						DSK_CALL(_readFace, file, file.faces.back());
						break;
					}
					case hashToken("g"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("hole"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("l"):
					{
						file.lines.emplace_back();
						DSK_CALL(_readLine, file, file.lines.back());
						break;
					}
					case hashToken("lod"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("mg"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("mtllib"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("o"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("p"):
					{
						file.pointClouds.emplace_back();
						DSK_CALL(_readPointCloud, file, file.pointClouds.back());
						break;
					}
					case hashToken("parm"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("s"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("scrv"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("shadow_obj"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("sp"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("stech"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("step"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("surf"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("trace_obj"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("trim"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("usemtl"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("v"):
					{
						file.positions.emplace_back();
						DSK_CALL(_readVertexPosition, file.positions.back());
						break;
					}
					case hashToken("vn"):
					{
						file.normals.emplace_back();
						DSK_CALL(_readVertexNormal, file.normals.back());
						break;
					}
					case hashToken("vp"):
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);	// TODO
						break;
					}
					case hashToken("vt"):
					{
						file.texCoords.emplace_back();
						DSK_CALL(_readVertexTextureCoordinate, file.texCoords.back());
						break;
					}
					default:
					{
						return _status.setErrorMessage(__PRETTY_FUNCTION__, __LINE__, std::format("Unrecognized start of token '{}'", std::string_view(buffer, skipCount)));
					}
				}

				DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
				if (!_stream->eof())
				{
					DSKFMT_STREAM_CALL(read, buffer[0]);
					if (buffer[0] == '#')
					{
						DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '\n'; }, skipCount);
						if (!_stream->eof())
						{
							DSKFMT_STREAM_CALL(expect, '\n');
						}
					}
					else
					{
						if (buffer[0] == '\r')
						{
							DSKFMT_STREAM_CALL(read, buffer[0]);
						}
						DSK_CHECK(buffer[0] == '\n', std::format("Expected \\n, instead got '{}'", buffer[0]));
					}
				}
			}
		}

		void ObjIStream::resetFormatState()
		{
		}

		void ObjIStream::_readVertexPosition(obj::VertexPosition& position)
		{
			DSKFMT_BEGIN();

			uint64_t skipCount;

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSK_CHECK(skipCount, "Missing space before x-coord.");
			DSKFMT_STREAM_CALL(readAsciiNumber, position.x);

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSK_CHECK(skipCount, "Missing space before y-coord.");
			DSKFMT_STREAM_CALL(readAsciiNumber, position.y);

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSK_CHECK(skipCount, "Missing space before z-coord.");
			DSKFMT_STREAM_CALL(readAsciiNumber, position.z);

			position.w = 1.0;

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			if (skipCount && !_stream->eof())
			{
				char buffer;
				DSKFMT_STREAM_CALL(read, buffer);
				DSKFMT_STREAM_CALL(unread, 1);

				if (isFirstNumberChar(buffer))
				{
					DSKFMT_STREAM_CALL(readAsciiNumber, position.w);
				}
			}
		}

		void ObjIStream::_readVertexTextureCoordinate(obj::VertexTextureCoordinate& texCoord)
		{
			DSKFMT_BEGIN();

			uint64_t skipCount;

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSK_CHECK(skipCount, "Missing space before u-coord.");
			DSKFMT_STREAM_CALL(readAsciiNumber, texCoord.u);

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSK_CHECK(skipCount, "Missing space before v-coord.");
			DSKFMT_STREAM_CALL(readAsciiNumber, texCoord.v);

			texCoord.w = 1.0;

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			if (skipCount && !_stream->eof())
			{
				char buffer;
				DSKFMT_STREAM_CALL(read, buffer);
				DSKFMT_STREAM_CALL(unread, 1);

				if (isFirstNumberChar(buffer))
				{
					DSKFMT_STREAM_CALL(readAsciiNumber, texCoord.w);
				}
			}
		}

		void ObjIStream::_readVertexNormal(obj::VertexNormal& normal)
		{
			DSKFMT_BEGIN();

			uint64_t skipCount;

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSK_CHECK(skipCount, "Missing space before i-coord.");
			DSKFMT_STREAM_CALL(readAsciiNumber, normal.i);

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSK_CHECK(skipCount, "Missing space before j-coord.");
			DSKFMT_STREAM_CALL(readAsciiNumber, normal.j);

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSK_CHECK(skipCount, "Missing space before k-coord.");
			DSKFMT_STREAM_CALL(readAsciiNumber, normal.k);
		}

		void ObjIStream::_readPointCloud(const obj::File& file, obj::PointCloud& pointCloud)
		{
			DSKFMT_BEGIN();

			char buffer;
			uint64_t skipCount;
			
			const uint64_t sizes[1] = { file.positions.size() };
			uint64_t indices[1];

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			while (skipCount && !_stream->eof())
			{
				DSKFMT_STREAM_CALL(read, buffer);
				DSKFMT_STREAM_CALL(unread, 1);

				if (isFirstIntegerChar(buffer))
				{
					DSK_CALL(_readSlashSeparatedIndices, sizes, indices, 1);
					pointCloud.vertices.emplace_back(indices[0]);

					DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
				}
				else
				{
					break;
				}
			}

			DSK_CHECK(!pointCloud.vertices.empty(), "Found point cloud with no vertex in it.");
		}

		void ObjIStream::_readLine(const obj::File& file, obj::Line& line)
		{
			DSKFMT_BEGIN();

			char buffer;
			uint64_t skipCount;

			const uint64_t sizes[2] = { file.positions.size(), file.texCoords.size() };
			uint64_t indices[2];

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			while (skipCount && !_stream->eof())
			{
				DSKFMT_STREAM_CALL(read, buffer);
				DSKFMT_STREAM_CALL(unread, 1);

				if (isFirstIntegerChar(buffer))
				{
					DSK_CALL(_readSlashSeparatedIndices, sizes, indices, 2);
					line.vertices.emplace_back(indices[0], indices[1]);

					DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
				}
				else
				{
					break;
				}
			}

			DSK_CHECK(line.vertices.size() >= 2, "Line requires at least two vertices.");
		}

		void ObjIStream::_readFace(const obj::File& file, obj::Face& face)
		{
			DSKFMT_BEGIN();

			char buffer;
			uint64_t skipCount;

			const uint64_t sizes[3] = { file.positions.size(), file.texCoords.size(), file.normals.size() };
			uint64_t indices[3];

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			while (skipCount && !_stream->eof())
			{
				DSKFMT_STREAM_CALL(read, buffer);
				DSKFMT_STREAM_CALL(unread, 1);

				if (isFirstIntegerChar(buffer))
				{
					DSK_CALL(_readSlashSeparatedIndices, sizes, indices, 3);
					face.vertices.emplace_back(indices[0], indices[1], indices[2]);

					DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
				}
				else
				{
					break;
				}
			}

			DSK_CHECK(face.vertices.size() >= 3, "Face requires at least three vertices.");
		}

		void ObjIStream::_readSlashSeparatedIndices(const uint64_t* sizes, uint64_t* indices, uint64_t count)
		{
			DSKFMT_BEGIN();

			char buffer;
			int64_t index;

			DSKFMT_STREAM_CALL(readAsciiNumber, index);
			DSK_CHECK(index != 0 && std::abs(index) <= sizes[0], std::format("Invalid index: {}. Size: {}.", index, sizes[0]));
			indices[0] = index > 0 ? index - 1 : sizes[0] - index;

			for (uint64_t i = 1; i < count && !_stream->eof(); ++i)
			{
				DSKFMT_STREAM_CALL(read, buffer);
				if (buffer != '/')
				{
					DSKFMT_STREAM_CALL(unread, 1);
					std::fill_n(indices + i, count - i, UINT64_MAX);
					break;
				}

				DSKFMT_STREAM_CALL(readAsciiNumber, index);
				DSK_CHECK(index != 0 && std::abs(index) <= sizes[i], std::format("Invalid index: {}. Size: {}.", index, sizes[i]));
				indices[i] = index > 0 ? index - 1 : sizes[i] - index;
			}
		}


		ObjOStream::ObjOStream(OStream* stream) : FormatOStream(stream)
		{
		}

		void ObjOStream::writeFile(const obj::File& file)
		{
			DSKFMT_BEGIN();

			// TODO: Implement all keywords ignored !

			for (const obj::VertexPosition& position : file.positions)
			{
				DSK_CALL(_writeVertexPosition, position);
			}
			for (const obj::VertexTextureCoordinate& texCoord : file.texCoords)
			{
				DSK_CALL(_writeVertexTextureCoordinate, texCoord);
			}
			for (const obj::VertexNormal& normal : file.normals)
			{
				DSK_CALL(_writeVertexNormal, normal);
			}

			for (const obj::PointCloud& pointCloud : file.pointClouds)
			{
				DSK_CALL(_writePointCloud, file, pointCloud);
			}
			for (const obj::Line& line : file.lines)
			{
				DSK_CALL(_writeLine, file, line);
			}
			for (const obj::Face& face : file.faces)
			{
				DSK_CALL(_writeFace, file, face);
			}
		}

		void ObjOStream::resetFormatState()
		{
		}

		void ObjOStream::_writeVertexPosition(const obj::VertexPosition& position)
		{
			DSKFMT_BEGIN();

			DSKFMT_STREAM_CALL(write, "v ", 2);

			DSKFMT_STREAM_CALL(writeAsciiNumber, position.x);
			DSKFMT_STREAM_CALL(write, ' ');

			DSKFMT_STREAM_CALL(writeAsciiNumber, position.y);
			DSKFMT_STREAM_CALL(write, ' ');

			DSKFMT_STREAM_CALL(writeAsciiNumber, position.z);
			DSKFMT_STREAM_CALL(write, ' ');

			DSKFMT_STREAM_CALL(writeAsciiNumber, position.w);
			DSKFMT_STREAM_CALL(write, '\n');
		}

		void ObjOStream::_writeVertexTextureCoordinate(const obj::VertexTextureCoordinate& texCoord)
		{
			DSKFMT_BEGIN();

			DSKFMT_STREAM_CALL(write, "vt ", 3);

			DSKFMT_STREAM_CALL(writeAsciiNumber, texCoord.u);
			DSKFMT_STREAM_CALL(write, ' ');

			DSKFMT_STREAM_CALL(writeAsciiNumber, texCoord.v);
			DSKFMT_STREAM_CALL(write, ' ');

			DSKFMT_STREAM_CALL(writeAsciiNumber, texCoord.w);
			DSKFMT_STREAM_CALL(write, '\n');
		}

		void ObjOStream::_writeVertexNormal(const obj::VertexNormal& normal)
		{
			DSKFMT_BEGIN();

			DSKFMT_STREAM_CALL(write, "vn ", 3);

			DSKFMT_STREAM_CALL(writeAsciiNumber, normal.i);
			DSKFMT_STREAM_CALL(write, ' ');

			DSKFMT_STREAM_CALL(writeAsciiNumber, normal.j);
			DSKFMT_STREAM_CALL(write, ' ');

			DSKFMT_STREAM_CALL(writeAsciiNumber, normal.k);
			DSKFMT_STREAM_CALL(write, '\n');
		}

		void ObjOStream::_writePointCloud(const obj::File& file, const obj::PointCloud& pointCloud)
		{
			DSKFMT_BEGIN();

			assert(!pointCloud.vertices.empty());

			DSKFMT_STREAM_CALL(write, 'p');

			for (const uint64_t& index : pointCloud.vertices)
			{
				assert(index < file.positions.size());

				DSKFMT_STREAM_CALL(write, ' ');
				DSKFMT_STREAM_CALL(writeAsciiNumber, index + 1);
			}

			DSKFMT_STREAM_CALL(write, '\n');
		}

		void ObjOStream::_writeLine(const obj::File& file, const obj::Line& line)
		{
			DSKFMT_BEGIN();

			assert(line.vertices.size() >= 2);

			DSKFMT_STREAM_CALL(write, 'l');

			for (const obj::LineVertex& lineVertex : line.vertices)
			{
				assert(lineVertex.position < file.positions.size());

				DSKFMT_STREAM_CALL(write, ' ');
				DSKFMT_STREAM_CALL(writeAsciiNumber, lineVertex.position + 1);

				if (lineVertex.texCoord < file.texCoords.size())
				{
					DSKFMT_STREAM_CALL(write, '/');
					DSKFMT_STREAM_CALL(writeAsciiNumber, lineVertex.texCoord + 1);
				}
			}

			DSKFMT_STREAM_CALL(write, '\n');
		}

		void ObjOStream::_writeFace(const obj::File& file, const obj::Face& face)
		{
			DSKFMT_BEGIN();

			assert(face.vertices.size() >= 3);

			DSKFMT_STREAM_CALL(write, 'f');

			for (const obj::FaceVertex& faceVertex : face.vertices)
			{
				assert(faceVertex.position < file.positions.size());

				DSKFMT_STREAM_CALL(write, ' ');
				DSKFMT_STREAM_CALL(writeAsciiNumber, faceVertex.position + 1);

				if (faceVertex.texCoord < file.texCoords.size())
				{
					DSKFMT_STREAM_CALL(write, '/');
					DSKFMT_STREAM_CALL(writeAsciiNumber, faceVertex.texCoord + 1);
				}
				else if (faceVertex.normal < file.normals.size())
				{
					DSKFMT_STREAM_CALL(write, '/');
				}

				if (faceVertex.normal < file.normals.size())
				{
					DSKFMT_STREAM_CALL(write, '/');
					DSKFMT_STREAM_CALL(writeAsciiNumber, faceVertex.normal + 1);
				}
			}

			DSKFMT_STREAM_CALL(write, '\n');
		}
	}
}
