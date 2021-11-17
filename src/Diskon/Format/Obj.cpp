#include <Diskon/Diskon.hpp>

namespace dsk
{
	bool ObjFile::loadFromFile(const std::filesystem::path& path)
	{
		positions.clear();
		texCoords.clear();
		normals.clear();
		faces.clear();

		if (!std::filesystem::exists(path))
		{
			return false;
		}

		dsk::Lexer lexer({ {"#", "\n"} }, { ' ', '\n', '\t', '\v', '\r', '\f' });
		lexer.addLexemeDefinition(dsk::LexDefKeyword(0, "v"));
		lexer.addLexemeDefinition(dsk::LexDefKeyword(1, "vt"));
		lexer.addLexemeDefinition(dsk::LexDefKeyword(2, "vn"));
		lexer.addLexemeDefinition(dsk::LexDefKeyword(3, "f"));
		lexer.addLexemeDefinition(dsk::LexDefNumber(4));

		std::ifstream file(path);
		if (!file)
		{
			return false;
		}

		file.seekg(0, std::ios::end);
		uint64_t length = file.tellg();
		file.seekg(0, std::ios::beg);
		char* buffer = new char[length + 1];
		file.read(buffer, length);
		buffer[length] = 0;
		lexer.addContent(buffer);
		delete[] buffer;

		file.close();

		const dsk::LexemeBase* lexeme = lexer.lex();
		while (lexeme)
		{
			switch (lexeme->id)
			{
				case 0:
				{
					obj::Vec4 pos;

					lexeme = lexer.lex();
					if (lexeme->id != 4) return false;
					pos.x = ((dsk::LexemeNumber*) lexeme)->value;

					lexeme = lexer.lex();
					if (lexeme->id != 4) return false;
					pos.y = ((dsk::LexemeNumber*)lexeme)->value;

					lexeme = lexer.lex();
					if (lexeme->id != 4) return false;
					pos.z = ((dsk::LexemeNumber*)lexeme)->value;

					lexeme = lexer.lex();
					if (lexeme->id == 4)
					{
						pos.w = ((dsk::LexemeNumber*)lexeme)->value;
						lexeme = lexer.lex();
					}

					positions.push_back(pos);

					break;
				}
				case 1:
				{
					obj::Vec3 tex;

					lexeme = lexer.lex();
					if (lexeme->id != 4) return false;
					tex.x = ((dsk::LexemeNumber*)lexeme)->value;

					lexeme = lexer.lex();
					if (lexeme->id == 4)
					{
						tex.y = ((dsk::LexemeNumber*)lexeme)->value;
						lexeme = lexer.lex();

						if (lexeme->id == 4)
						{
							tex.z = ((dsk::LexemeNumber*)lexeme)->value;
							lexeme = lexer.lex();
						}
					}

					texCoords.push_back(tex);

					break;
				}
				case 2:
				{
					obj::Vec3 normal;

					lexeme = lexer.lex();
					if (lexeme->id != 4) return false;
					normal.x = ((dsk::LexemeNumber*)lexeme)->value;

					lexeme = lexer.lex();
					if (lexeme->id != 4) return false;
					normal.y = ((dsk::LexemeNumber*)lexeme)->value;

					lexeme = lexer.lex();
					if (lexeme->id != 4) return false;
					normal.z = ((dsk::LexemeNumber*)lexeme)->value;

					lexeme = lexer.lex();

					normals.push_back(normal);

					break;
				}
				case 3:
				{
					std::vector<obj::Vertex> vertices;

					lexeme = lexer.lex();
					if (lexeme->id != 4) return false;

					while (lexeme && lexeme->id == 4)
					{
						std::array<int64_t, 3> indices = { INT64_MAX, INT64_MAX, INT64_MAX };

						indices[0] = (int64_t)((dsk::LexemeNumber*)lexeme)->value - 1;

						if (lexer.getContent().size() > 1 && lexer.getContent()[0] == '/' || lexer.getContent()[0] == '\\')
						{
							if (lexer.getContent().size() > 2 && lexer.getContent()[1] == '/' || lexer.getContent()[1] == '\\')
							{
								lexeme = lexer.lex();
								if (lexeme->id != 4) return false;
								indices[2] = (int64_t)((dsk::LexemeNumber*)lexeme)->value - 1;
							}
							else
							{
								lexeme = lexer.lex();
								if (lexeme->id != 4) return false;
								indices[1] = (int64_t)((dsk::LexemeNumber*)lexeme)->value - 1;

								if (lexer.getContent().size() > 1 && lexer.getContent()[0] == '/' || lexer.getContent()[0] == '\\')
								{
									lexeme = lexer.lex();
									if (lexeme->id != 4) return false;
									indices[2] = (int64_t)((dsk::LexemeNumber*)lexeme)->value - 1;
								}
							}
						}

						obj::Vertex vertex;

						if (std::abs(indices[0]) < positions.size())
						{
							if (indices[0] < 0)
							{
								indices[0] = positions.size() - indices[0];
							}

							vertex.position = &positions[indices[0]];
						}

						if (std::abs(indices[1]) < texCoords.size())
						{
							if (indices[1] < 0)
							{
								indices[1] = texCoords.size() - indices[1];
							}

							vertex.texCoord = &texCoords[indices[1]];
						}

						if (std::abs(indices[2]) < normals.size())
						{
							if (indices[2] < 0)
							{
								indices[2] = normals.size() - indices[2];
							}

							vertex.normal = &normals[indices[2]];
						}

						vertices.push_back(vertex);

						lexeme = lexer.lex();
					}

					if (vertices.size() < 3)
					{
						return false;
					}

					faces.push_back(vertices);

					break;
				}
				default:
				{
					return false;
				}
			}
		}

		return true;
	}
}
