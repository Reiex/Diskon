#include <Diskon/Diskon.hpp>

#include <fstream>
#include <iostream>

int main()
{
	std::ifstream file("examples/assets/teapot.obj");
	file.seekg(0, std::ios::end);
	uint64_t length = file.tellg();
	file.seekg(0, std::ios::beg);
	char* buffer = new char[length + 1];
	file.read(buffer, length);
	buffer[length] = 0;
	file.close();

	dsk::Lexer lexer({ {"#", "\n"} }, {' ', '\n', '\t', '\v', '\r', '\f'});
	lexer.addLexemeDefinition(dsk::LexDefKeyword(0, "v"));
	lexer.addLexemeDefinition(dsk::LexDefKeyword(1, "vt"));
	lexer.addLexemeDefinition(dsk::LexDefKeyword(2, "vn"));
	lexer.addLexemeDefinition(dsk::LexDefKeyword(3, "f"));

	lexer.addContent(buffer);

	dsk::LexemeKeyword* lexeme;
	while (lexeme = (dsk::LexemeKeyword*) lexer.lex())
	{
		std::cout << "Lexeme: " << lexeme->keyword << std::endl;
	}

	return 0;
}