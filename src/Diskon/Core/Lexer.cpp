#include <Diskon/Diskon.hpp>

namespace dsk
{
	namespace
	{
		bool startsWithString(const std::deque<char>& content, const std::string& string)
		{
			if (content.size() < string.size())
			{
				return false;
			}

			for (uint64_t i = 0; i < string.size(); ++i)
			{
				if (content[i] != string[i])
				{
					return false;
				}
			}

			return true;
		}
	}

	LexDefKeyword::LexDefKeyword(uint64_t id, const std::string& keyword) :
		_id(id),
		_keywords({ keyword })
	{
	}

	LexDefKeyword::LexDefKeyword(uint64_t id, const std::vector<std::string>& keywords) :
		_id(id),
		_keywords(keywords)
	{
	}

	uint64_t LexDefKeyword::tryToLex(const std::deque<char>& content, LexemeBase*& lexeme) const
	{
		LexemeKeyword* lexemeKeyword = new LexemeKeyword;
		lexemeKeyword->id = _id;
		lexemeKeyword->keyword = "";

		for (const std::string& string : _keywords)
		{
			if (string.size() > lexemeKeyword->keyword.size())
			{
				if (startsWithString(content, string))
				{
					lexemeKeyword->keyword = string;
				}
			}
		}

		if (lexemeKeyword->keyword.empty())
		{
			lexeme = nullptr;
			return 0;
		}
		else
		{
			lexeme = lexemeKeyword;
			return lexemeKeyword->keyword.size();
		}
	}

	LexDefBase* LexDefKeyword::copy() const
	{
		return new LexDefKeyword(_id, _keywords);
	}

	Lexer::Lexer(const std::vector<std::array<std::string, 2>>& commentDelimiters, const std::vector<char> spaces) :
		_content(),
		_commentDelimiters(commentDelimiters),
		_spaces(spaces),
		_lexemeDefinitions(),
		_currentLexeme(nullptr)
	{
	}

	void Lexer::addLexemeDefinition(const LexDefBase& scheme)
	{
		_lexemeDefinitions.push_back(scheme.copy());
	}

	void Lexer::addContent(const std::string& content)
	{
		for (const char& c : content)
		{
			_content.push_back(c);
		}
	}

	const LexemeBase* Lexer::lex()
	{
		if (_currentLexeme)
		{
			delete _currentLexeme;
			_currentLexeme = nullptr;
		}

		// Remove spaces and comments

		bool commentFound = true;
		while (commentFound && !_content.empty())
		{
			bool spaceFound = true;
			while (spaceFound && !_content.empty())
			{
				spaceFound = false;
				for (const char& c : _spaces)
				{
					if (_content.front() == c)
					{
						_content.pop_front();
						spaceFound = true;
						break;
					}
				}
			}

			commentFound = false;
			for (const std::array<std::string, 2>& delimiters : _commentDelimiters)
			{
				commentFound = startsWithString(_content, delimiters[0]);

				if (commentFound)
				{
					while (!startsWithString(_content, delimiters[1]))
					{
						_content.pop_front();
					}

					if (_content.size() < delimiters[1].size())
					{
						_content.empty();
					}
					else
					{
						for (uint64_t i = 0; i < delimiters[1].size(); ++i)
						{
							_content.pop_front();
						}
					}

					break;
				}
			}
		}

		// Find the best lexeme that correspond

		uint64_t maxLength = 0;
		for (LexDefBase* lexDef : _lexemeDefinitions)
		{
			LexemeBase* lexeme = nullptr;
			uint64_t length = lexDef->tryToLex(_content, lexeme);

			if (length > maxLength)
			{
				if (_currentLexeme)
				{
					delete _currentLexeme;
				}
				_currentLexeme = lexeme;
				maxLength = length;
			}
			else if (length > 0)
			{
				delete lexeme;
			}
		}

		if (_currentLexeme)
		{
			for (uint64_t i = 0; i < maxLength; ++i)
			{
				_content.pop_front();
			}
		}

		// Handle unrecognized lexeme

		else if (!_content.empty())
		{
			std::string lexeme;

			bool endFound = false;
			while (!endFound && !_content.empty())
			{
				for (const char& c : _spaces)
				{
					if (_content.front() == c)
					{
						endFound = true;
						break;
					}
				}

				if (!endFound)
				{
					for (const std::array<std::string, 2>& delimiters : _commentDelimiters)
					{
						if (startsWithString(_content, delimiters[0]))
						{
							endFound = true;
							break;
						}
					}
				}

				if (!endFound)
				{
					lexeme += _content.front();
					_content.pop_front();
				}
			}

			std::cerr << "Unrecognized lexeme: " << lexeme << std::endl;

			return lex();
		}

		return _currentLexeme;
	}

	Lexer::~Lexer()
	{
		if (_currentLexeme)
		{
			delete _currentLexeme;
		}

		for (LexDefBase* lexDef : _lexemeDefinitions)
		{
			delete lexDef;
		}
	}
}
