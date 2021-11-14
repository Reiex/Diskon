#pragma once

#include <Diskon/Core/types.hpp>

namespace dsk
{
	struct LexemeBase
	{
		int64_t id;
	};

	class LexDefBase
	{
		protected:

			virtual uint64_t tryToLex(const std::deque<char>& content, LexemeBase*& lexeme) const = 0;
			virtual LexDefBase* copy() const = 0;

		friend class Lexer;
	};

	struct LexemeKeyword : public LexemeBase
	{
		std::string keyword;
	};

	class LexDefKeyword : public LexDefBase
	{
		public:

			LexDefKeyword(int64_t id, const std::string& keyword);
			LexDefKeyword(int64_t id, const std::vector<std::string>& keywords);

		protected:

			virtual uint64_t tryToLex(const std::deque<char>& content, LexemeBase*& lexeme) const;
			virtual LexDefBase* copy() const;

		private:

			int64_t _id;
			std::vector<std::string> _keywords;
	};

	struct LexemeNumber : public LexemeBase
	{
		long double value;
	};

	class LexDefNumber : public LexDefBase
	{
		public:

			LexDefNumber(int64_t id);

		protected:

			virtual uint64_t tryToLex(const std::deque<char>& content, LexemeBase*& lexeme) const;
			virtual LexDefBase* copy() const;

		private:

			int64_t _id;
	};

	class Lexer
	{
		public:

			Lexer(const std::vector<std::array<std::string, 2>>& commentDelimiters, const std::vector<char> spaces);
			Lexer(const Lexer& lexer) = delete;
			Lexer(Lexer&& lexer) = delete;

			Lexer& operator=(const Lexer& lexer) = delete;
			Lexer& operator=(Lexer&& lexer) = delete;

			void addLexemeDefinition(const LexDefBase& scheme);

			void addContent(const std::string& content);

			const LexemeBase* lex();

			std::deque<char>& getContent();
			const std::deque<char>& getContent() const;

			~Lexer();

		private:

			std::deque<char> _content;

			std::vector<std::array<std::string, 2>> _commentDelimiters;
			std::vector<char> _spaces;

			std::vector<LexDefBase*> _lexemeDefinitions;

			LexemeBase* _currentLexeme;
	};
}
