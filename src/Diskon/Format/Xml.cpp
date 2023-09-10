///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Marius Pélégrin
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
			inline bool isNameStartChar(char x)
			{
				return std::isalpha(x) || x == ':' || x == '_';
			}

			inline bool isNameChar(char x)
			{
				return isNameStartChar(x) || std::isdigit(x) || x == '-' || x == '.';
			}

			inline bool isEncNameStartChar(char x)
			{
				return std::isalpha(x);
			}

			inline bool isEncNameChar(char x)
			{
				return std::isalnum(x) || x == '.' || x == '_' || x == '-';
			}

			inline bool isSpaceChar(char x)
			{
				return (x == 0x20 || x == 0x09 || x == 0x0D || x == 0x0A);
			}

			inline bool isPITarget(const std::string& name)
			{
				return name.size() != 3 || std::toupper(name[0]) != 'X' || std::toupper(name[1]) != 'M' || std::toupper(name[2]) != 'L';
			}
		}

		XmlIStream::XmlIStream(IStream* stream) : FormatIStream(stream),
			_tags(),
			_prologRead(false),
			_treeRead(false)
		{
		}

		const ruc::Status& XmlIStream::readFile(xml::File& file)
		{
			DSKFMT_BEGIN();

			DSKFMT_CALL(readProlog, file.prolog);
			DSKFMT_CALL(_readElement, file.root);
			DSKFMT_CALL(readEnding, file.ending);

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::readProlog(xml::Prolog& prolog)
		{
			DSKFMT_BEGIN();

			assert(!_prologRead);

			prolog.declaration.reset();
			prolog.doctype.reset();
			prolog.instructions.clear();

			DSKFMT_CALL(_readDeclaration, prolog.declaration);
			DSKFMT_CALL(_readPIsSpacesAndComments, prolog.instructions);
			DSKFMT_CALL(_readDoctype, prolog.doctype);

			if (prolog.doctype.has_value())
			{
				DSKFMT_CALL(_readPIsSpacesAndComments, prolog.instructions);
			}

			_prologRead = true;

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::readElementTag(xml::ElementTag& tag)
		{
			DSKFMT_BEGIN();

			assert(_prologRead);
			assert(!_treeRead);

			tag.name.clear();
			tag.attributes.clear();

			uint64_t skipCount;
			char buffer;

			DSKFMT_CALL(_stream->expect, '<');
			DSKFMT_CALL(_readName, tag.name);
			DSKFMT_CHECK(!tag.name.empty(), "Could not read tag name.");
			DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);

			tag.attributes.emplace_back();
			while (skipCount)
			{
				std::string& attName = tag.attributes.back().first;
				std::string& attValue = tag.attributes.back().second;

				DSKFMT_CALL(_readName, attName);
				if (attName.empty())
				{
					skipCount = 0;
				}
				else
				{
					DSKFMT_CALL(_readEq);
					DSKFMT_CALL(_readAttValue, attValue);
					DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);

					tag.attributes.emplace_back();
				}
			}

			tag.attributes.pop_back();

			DSKFMT_CALL(_stream->read, buffer);
			if (buffer != '>')
			{
				DSKFMT_CHECK(buffer == '/', std::format("Unexpected character in element tag: '{}'.", buffer));
				DSKFMT_CALL(_stream->expect, '>');
				tag.isEmpty = true;
				_treeRead = _tags.empty();
			}
			else
			{
				tag.isEmpty = false;
				_tags.push_back(tag.name);
			}

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::readElementContent(xml::ElementContent& content, xml::ContentType::Flags filter)
		{
			DSKFMT_BEGIN();

			assert(!_tags.empty());
			assert(((filter & xml::ContentType::Child) == 0) || ((filter & xml::ContentType::ChildTag) == 0));

			content.childTag.reset();
			content.child.reset();
			content.charData.reset();
			content.instruction.reset();

			char buffer[2];
			bool parsedSomething;
			uint64_t skipCount;

			// Get rid of comments (not spaces because it would belong to a charData

			do {
				DSKFMT_CALL(_readComment, parsedSomething);
			} while (parsedSomething);

			// Read the two first characters and guess what the next content type is

			DSKFMT_CALL(_stream->read, buffer, 2);
			if (buffer[0] == '<' && buffer[1] == '?')
			{
				content.type = xml::ContentType::ProcessingInstruction;
			}
			else if (buffer[0] == '<' && buffer[1] == '/')
			{
				content.type = xml::ContentType::EndOfElement;
			}
			else if (buffer[0] == '<')
			{
				if (filter & xml::ContentType::ChildTag)
				{
					content.type = xml::ContentType::ChildTag;
				}
				else
				{
					content.type = xml::ContentType::Child;
				}
			}
			else
			{
				content.type = xml::ContentType::CharData;
			}

			// Put back these characters and check if the content will be read or ignored

			DSKFMT_CALL(_stream->unread, 2);
			bool readContent = (content.type == xml::ContentType::EndOfElement) || (content.type & filter);

			// Read/Ignore content

			if (readContent)	// Read content
			{
				switch (content.type)
				{
					case xml::ContentType::EndOfElement:
					{
						uint64_t skipCount;
						const std::string etag = "</" + _tags.back();
						DSKFMT_CALL(_stream->expect, etag.data(), etag.size());
						DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);
						DSKFMT_CALL(_stream->expect, '>');
						_tags.pop_back();
						if (_tags.empty())
						{
							_treeRead = true;
						}

						break;
					}
					case xml::ContentType::ChildTag:
					{
						content.childTag = std::make_unique<xml::ElementTag>();
						DSKFMT_CALL(readElementTag, *content.childTag);

						break;
					}
					case xml::ContentType::Child:
					{
						content.child = std::make_unique<xml::Element>();
						DSKFMT_CALL(_readElement, *content.child);

						break;
					}
					case xml::ContentType::CharData:
					{
						content.charData = std::make_unique<std::string>();
						do {
							DSKFMT_CALL(_stream->readCharWhile, [](char x) { return x != '<'; }, *content.charData);
							DSKFMT_CALL(_readComment, parsedSomething);
						} while (parsedSomething);

						break;
					}
					case xml::ContentType::ProcessingInstruction:
					{
						content.instruction = std::make_unique<xml::ProcessingInstruction>();
						DSKFMT_CALL(_readProcessingInstruction, *content.instruction, parsedSomething);

						break;
					}
				}
			}
			else	// Ignore content
			{
				switch (content.type)
				{
					case xml::ContentType::Child:
					{
						xml::ElementTag trash;
						DSKFMT_CALL(readElementTag, trash);
						DSKFMT_CALL(readElementContent, content, xml::ContentType::EndOfElement);

						break;
					}
					case xml::ContentType::CharData:
					{
						do {
							DSKFMT_CALL(_stream->skipCharWhile, [](char x) { return x != '<'; }, skipCount);
							DSKFMT_CALL(_readComment, parsedSomething);
						} while (parsedSomething);

						break;
					}
					case xml::ContentType::ProcessingInstruction:
					{
						xml::ProcessingInstruction trash;
						DSKFMT_CALL(_readProcessingInstruction, trash, parsedSomething);

						break;
					}
				}

				// Read next content

				DSKFMT_CALL(readElementContent, content, filter);
			}

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::readEnding(xml::Ending& ending)
		{
			DSKFMT_BEGIN();

			assert(_treeRead);

			DSKFMT_CALL(_readPIsSpacesAndComments, ending.instructions);

			_tags.clear();
			_prologRead = false;
			_treeRead = false;

			return _stream->getStatus();
		}

		void XmlIStream::resetFormatState()
		{
			_tags.clear();
			_prologRead = false;
			_treeRead = false;
		}

		const ruc::Status& XmlIStream::_readPIsSpacesAndComments(std::vector<xml::ProcessingInstruction>& instructions)
		{
			DSKFMT_BEGIN();

			uint64_t skipCount;
			bool parsedSomething;

			do {

				do {
					DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);
					DSKFMT_CALL(_readComment, parsedSomething);
				} while (parsedSomething);

				instructions.emplace_back();
				DSKFMT_CALL(_readProcessingInstruction, instructions.back(), parsedSomething);

			} while (parsedSomething);

			instructions.pop_back();

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::_readElement(xml::Element& element)
		{
			DSKFMT_BEGIN();

			element.contents.clear();

			DSKFMT_CALL(readElementTag, element.tag);

			if (!element.tag.isEmpty)
			{
				element.contents.reserve(_singleBufferSize / sizeof(xml::ElementContent));
				
				element.contents.emplace_back();
				DSKFMT_CALL(readElementContent, element.contents.back());
				while (element.contents.back().type != xml::ContentType::EndOfElement)
				{
					element.contents.emplace_back();
					DSKFMT_CALL(readElementContent, element.contents.back());
				}
				element.contents.pop_back();
				
				element.contents.shrink_to_fit();
			}

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::_readDeclaration(std::optional<xml::Declaration>& declaration)
		{
			DSKFMT_BEGIN();

			declaration.reset();

			uint64_t skipCount;
			char buffer[5];

			// Read '<?xml'

			DSKFMT_CALL(_stream->read, buffer, 5);
			if (!std::equal(buffer, buffer + 5, "<?xml"))
			{
				DSKFMT_CALL(_stream->unread, 5);
				return _stream->getStatus();
			}

			declaration.emplace();

			// Read version

			DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);
			DSKFMT_CHECK(skipCount, "Expected spaces after '<?xml' but found none.");
			DSKFMT_CALL(_stream->expect, "version", 7);
			DSKFMT_CALL(_readEq);

			DSKFMT_CALL(_stream->read, buffer[0]);
			DSKFMT_CHECK(buffer[0] == '"' || buffer[0] == '\'', std::format("Expected ''' or '\"'. Instead, got '{}'.", buffer[0]));

			DSKFMT_CALL(_stream->expect, "1.", 2);
			declaration->versionMajor = 1;
			DSKFMT_CALL(_stream->readAsciiNumber, declaration->versionMinor);

			DSKFMT_CALL(_stream->expect, buffer[0]);

			// Read encoding

			DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);
			if (skipCount)
			{
				DSKFMT_CALL(_stream->read, buffer[0]);
				if (buffer[0] == 'e')
				{
					declaration->encoding.emplace();

					DSKFMT_CALL(_stream->expect, "ncoding", 7);
					DSKFMT_CALL(_readEq);
					DSKFMT_CALL(_stream->read, buffer[0]);
					DSKFMT_CHECK(buffer[0] == '"' || buffer[0] == '\'', std::format("Expected ''' or '\"'. Instead, got '{}'.", buffer[0]));
					DSKFMT_CALL(_readEncName, declaration->encoding.value());
					DSKFMT_CALL(_stream->expect, buffer[0]);
				}
				else
				{
					_stream->unread(1);
				}
			}

			// Read standalone

			DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);
			if (skipCount)
			{
				DSKFMT_CALL(_stream->read, buffer[0]);
				if (buffer[0] == 's')
				{
					declaration->standalone.emplace(true);

					DSKFMT_CALL(_stream->expect, "tandalone", 9);
					DSKFMT_CALL(_readEq);
					std::string value;
					DSKFMT_CALL(_readAttValue, value);
					DSKFMT_CHECK(value == "no" || value == "yes", std::format("Standalone can only take values 'yes' and 'no'. Instead, got '{}'.", value));
					declaration->standalone = (value == "yes");
				}
				else
				{
					_stream->unread(1);
				}
			}

			// Read '?>'

			DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);
			DSKFMT_CALL(_stream->expect, "?>", 2);

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::_readDoctype(std::optional<xml::Doctype>& doctype)
		{
			DSKFMT_BEGIN();

			doctype.reset();

			char buffer[9];
			uint64_t skipCount;

			// Read '<!DOCTYPE'

			DSKFMT_CALL(_stream->read, buffer, 9);
			if (!std::equal(buffer, buffer + 9, "<!DOCTYPE"))
			{
				DSKFMT_CALL(_stream->unread, 9);
				return _stream->getStatus();
			}

			doctype.emplace();

			// Read name

			DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);
			DSKFMT_CHECK(skipCount, "Expected space after '<!DOCTYPE' but found no space.");
			DSKFMT_CALL(_readName, doctype->name);
			DSKFMT_CHECK(!doctype->name.empty(), "Error while parsing doctype name.");

			// Read '>'

			DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);
			DSKFMT_CALL(_stream->expect, '>');

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::_readProcessingInstruction(xml::ProcessingInstruction& instruction, bool& parsedSomething)
		{
			DSKFMT_BEGIN();

			instruction.target.clear();
			instruction.instruction.clear();

			char buffer[2];
			uint64_t skipCount, readCount;

			if (_stream->eof())
			{
				parsedSomething = false;
				return _stream->getStatus();
			}

			// Read '<?'

			DSKFMT_CALL(_stream->read, buffer, 2);
			parsedSomething = (buffer[0] == '<' && buffer[1] == '?');
			if (!parsedSomething)
			{
				DSKFMT_CALL(_stream->unread, 2);
				return _stream->getStatus();
			}

			// Read target

			DSKFMT_CALL(_readName, instruction.target);
			DSKFMT_CHECK(!instruction.target.empty(), "Error while parsing processing instruction target.");
			DSKFMT_CHECK(isPITarget(instruction.target), std::format("PI Target cannot starts with 'XML'. PI Target: '{}'.", instruction.target));

			// Read instruction and '?>'

			instruction.instruction.clear();
			DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);
			if (skipCount)
			{
				auto conditionFunc = [&](char x) {
					return !(*buffer == '?' && x == '>') && (*buffer = x, true);
				};

				DSKFMT_CALL(_stream->readCharWhile, conditionFunc, instruction.instruction);
				DSKFMT_CHECK(!instruction.instruction.empty() && instruction.instruction.back() == '?', std::format("Encountered an unfinished processing instruction: '{}'", instruction.instruction));
				instruction.instruction.pop_back();
				DSKFMT_CALL(_stream->expect, '>');
			}

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::_readEq()
		{
			DSKFMT_BEGIN();

			uint64_t skipCount;

			DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);
			DSKFMT_CALL(_stream->expect, '=');
			DSKFMT_CALL(_stream->skipCharWhile, isSpaceChar, skipCount);

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::_readEncName(std::string& name)
		{
			DSKFMT_BEGIN();

			char x;

			DSKFMT_CALL(_stream->read, x);
			if (!isEncNameStartChar(x))
			{
				_stream->unread(1);
				return _stream->getStatus();
			}
			name.push_back(x);

			DSKFMT_CALL(_stream->readCharWhile, isEncNameChar, name);

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::_readName(std::string& name)
		{
			DSKFMT_BEGIN();

			char x;

			DSKFMT_CALL(_stream->read, x);
			if (!isNameStartChar(x))
			{
				_stream->unread(1);
				return _stream->getStatus();
			}
			name.push_back(x);

			DSKFMT_CALL(_stream->readCharWhile, isNameChar, name);

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::_readAttValue(std::string& value)
		{
			DSKFMT_BEGIN();

			char openChar;

			DSKFMT_CALL(_stream->read, openChar);
			DSKFMT_CHECK(openChar == '\'' || openChar == '"', std::format("Expected ''' or '\"', but instead got '{}'.", openChar));

			// TODO: '&' can appear in the attribute value, but it MUST be a ref, not just any '&'
			DSKFMT_CALL(_stream->readCharWhile, [&](char x) { return x != '<' && x != openChar; }, value);

			DSKFMT_CALL(_stream->expect, openChar);

			return _stream->getStatus();
		}

		const ruc::Status& XmlIStream::_readComment(bool& parsedSomething)
		{
			DSKFMT_BEGIN();

			char buffer[4];

			if (_stream->eof())
			{
				parsedSomething = false;
				return _stream->getStatus();
			}

			// Read "<!--"

			DSKFMT_CALL(_stream->read, buffer, 4);
			parsedSomething = std::equal(buffer, buffer + 4, "<!--");
			if (!parsedSomething)
			{
				DSKFMT_CALL(_stream->unread, 4);
				return _stream->getStatus();
			}

			// Search for "-->"

			uint64_t skipCount;
			auto conditionFunc = [&](char x) {
				return !(buffer[0] == '-' && buffer[1] == '-' && x == '>') && (buffer[0] = buffer[1], buffer[1] = x, true);
			};

			DSKFMT_CALL(_stream->skipCharWhile, conditionFunc, skipCount);

			return _stream->getStatus();
		}


		namespace
		{
			inline bool isName(const std::string& name)
			{
				if (name.empty())
				{
					return false;
				}

				if (!isNameStartChar(name[0]))
				{
					return false;
				}

				for (const char& c : name)
				{
					if (!isNameChar(c))
					{
						return false;
					}
				}

				return true;
			}

			inline bool isEncName(const std::string& name)
			{
				if (name.empty())
				{
					return false;
				}

				if (!isEncNameStartChar(name[0]))
				{
					return false;
				}

				for (const char& c : name)
				{
					if (!isEncNameChar(c))
					{
						return false;
					}
				}

				return true;
			}

			inline bool isAttValue(const std::string& name)
			{
				// TODO: '&' can appear in the attribute value, but it MUST be a ref, not just any '&'

				uint8_t found = 0;
				for (const char& c : name)
				{
					if (c == '<')
					{
						return false;
					}
					else if (c == '"')
					{
						if (found == 2)
						{
							return false;
						}
						found = 1;
					}
					else if (c == '\'')
					{
						if (found == 1)
						{
							return false;
						}
						found = 2;
					}
				}

				return true;
			}
		}

		XmlOStream::XmlOStream(OStream* stream) : FormatOStream(stream),
			_tags(),
			_prologWritten(false),
			_treeWritten(false)
		{
		}

		const ruc::Status& XmlOStream::writeFile(const xml::File& file)
		{
			DSKFMT_BEGIN();

			assert(!_prologWritten);

			DSKFMT_CALL(writeProlog, file.prolog);
			DSKFMT_CALL(_writeElement, file.root);
			DSKFMT_CALL(writeEnding, file.ending);

			return _stream->getStatus();
		}

		const ruc::Status& XmlOStream::writeProlog(const xml::Prolog& prolog)
		{
			DSKFMT_BEGIN();

			assert(!_prologWritten);

			if (prolog.declaration.has_value())
			{
				DSKFMT_CALL(_writeDeclaration, prolog.declaration.value());
			}

			if (prolog.doctype.has_value())
			{
				DSKFMT_CALL(_writeDoctype, prolog.doctype.value());
			}

			for (const xml::ProcessingInstruction& instruction : prolog.instructions)
			{
				DSKFMT_CALL(_writeProcessingInstruction, instruction);
			}

			_prologWritten = true;

			return _stream->getStatus();
		}

		const ruc::Status& XmlOStream::writeElementTag(const xml::ElementTag& tag)
		{
			DSKFMT_BEGIN();

			assert(_prologWritten);
			assert(!_treeWritten);

			assert(isName(tag.name));

			DSKFMT_CALL(_stream->write, '<');
			DSKFMT_CALL(_stream->write, tag.name.data(), tag.name.size());

			for (const std::pair<std::string, std::string>& attribute : tag.attributes)
			{
				DSKFMT_CALL(_stream->write, ' ');
				DSKFMT_CALL(_writeAttribute, attribute);
			}

			if (tag.isEmpty)
			{
				DSKFMT_CALL(_stream->write, '/');
			}
			else
			{
				_tags.push_back(tag.name);
			}
			DSKFMT_CALL(_stream->write, '>');

			return _stream->getStatus();
		}

		const ruc::Status& XmlOStream::writeElementContent(const xml::ElementContent& content)
		{
			DSKFMT_BEGIN();

			assert(!_tags.empty());

			switch (content.type)
			{
				case xml::ContentType::EndOfElement:
				{
					assert(!content.childTag);
					assert(!content.child);
					assert(!content.charData);
					assert(!content.instruction);

					DSKFMT_CALL(_stream->write, "</", 2);
					DSKFMT_CALL(_stream->write, _tags.back().data(), _tags.back().size());
					DSKFMT_CALL(_stream->write, '>');

					_tags.pop_back();
					if (_tags.empty())
					{
						_treeWritten = true;
					}

					break;
				}
				case xml::ContentType::ChildTag:
				{
					assert(!content.child);
					assert(!content.charData);
					assert(!content.instruction);

					DSKFMT_CALL(writeElementTag, *content.childTag);

					break;
				}
				case xml::ContentType::Child:
				{
					assert(!content.childTag);
					assert(!content.charData);
					assert(!content.instruction);

					DSKFMT_CALL(_writeElement, *content.child);

					break;
				}
				case xml::ContentType::CharData:
				{
					assert(!content.childTag);
					assert(!content.child);
					assert(!content.instruction);

					assert(std::find(content.charData->begin(), content.charData->end(), '<') == content.charData->end());
					// TODO: assert(std::find(content.charData.begin(), content.charData.end(), '&') == content.charData.end());

					DSKFMT_CALL(_stream->write, content.charData->data(), content.charData->size());

					break;
				}
				case xml::ContentType::ProcessingInstruction:
				{
					assert(!content.childTag);
					assert(!content.child);
					assert(!content.charData);

					DSKFMT_CALL(_writeProcessingInstruction, *content.instruction);

					break;
				}
				default:
				{
					assert(false);
				}
			}

			return _stream->getStatus();
		}

		const ruc::Status& XmlOStream::writeEnding(const xml::Ending& ending)
		{
			DSKFMT_BEGIN();

			assert(_treeWritten);

			for (const xml::ProcessingInstruction& instruction : ending.instructions)
			{
				DSKFMT_CALL(_writeProcessingInstruction, instruction);
			}

			_tags.clear();
			_prologWritten = false;
			_treeWritten = false;

			return _stream->getStatus();
		}

		void XmlOStream::resetFormatState()
		{
			_tags.clear();
			_prologWritten = false;
			_treeWritten = false;
		}

		const ruc::Status& XmlOStream::_writeDeclaration(const xml::Declaration& declaration)
		{
			DSKFMT_BEGIN();

			assert(declaration.versionMajor == 1);
			assert(!declaration.encoding.has_value() || isEncName(declaration.encoding.value()));

			DSKFMT_CALL(_stream->write, "<?xml version='1.", 17);
			DSKFMT_CALL(_stream->writeAsciiNumber, declaration.versionMinor);
			DSKFMT_CALL(_stream->write, '\'');

			if (declaration.encoding.has_value())
			{
				DSKFMT_CALL(_stream->write, " encoding='", 11);
				DSKFMT_CALL(_stream->write, declaration.encoding->data(), declaration.encoding->size());
				DSKFMT_CALL(_stream->write, '\'');
			}

			if (declaration.standalone.has_value())
			{
				if (declaration.standalone.value())
				{
					DSKFMT_CALL(_stream->write, " standalone='yes'", 17);
				}
				else
				{
					DSKFMT_CALL(_stream->write, " standalone='no'", 16);
				}
			}

			DSKFMT_CALL(_stream->write, "?>", 2);

			return _stream->getStatus();
		}

		const ruc::Status& XmlOStream::_writeDoctype(const xml::Doctype& doctype)
		{
			DSKFMT_BEGIN();

			assert(isName(doctype.name));

			DSKFMT_CALL(_stream->write, "<!DOCTYPE ", 10);
			DSKFMT_CALL(_stream->write, doctype.name.data(), doctype.name.size());
			DSKFMT_CALL(_stream->write, '>');

			return _stream->getStatus();
		}

		const ruc::Status& XmlOStream::_writeProcessingInstruction(const xml::ProcessingInstruction& instruction)
		{
			DSKFMT_BEGIN();

			assert(isPITarget(instruction.target));
			assert(instruction.instruction.find("<?") == std::string::npos);

			DSKFMT_CALL(_stream->write, "<?", 2);
			DSKFMT_CALL(_stream->write, instruction.target.data(), instruction.target.size());
			DSKFMT_CALL(_stream->write, ' ');
			DSKFMT_CALL(_stream->write, instruction.instruction.data(), instruction.instruction.size());
			DSKFMT_CALL(_stream->write, "?>", 2);

			return _stream->getStatus();
		}

		const ruc::Status& XmlOStream::_writeElement(const xml::Element& element)
		{
			DSKFMT_BEGIN();

			assert(!element.tag.isEmpty || element.contents.empty());

			DSKFMT_CALL(writeElementTag, element.tag);

			for (const xml::ElementContent& content : element.contents)
			{
				assert(content.type != xml::ContentType::EndOfElement);
				DSKFMT_CALL(writeElementContent, content);
			}

			if (!element.tag.isEmpty)
			{
				xml::ElementContent tagEnd;
				tagEnd.type = xml::ContentType::EndOfElement;
				DSKFMT_CALL(writeElementContent, tagEnd);
			}

			return _stream->getStatus();
		}

		const ruc::Status& XmlOStream::_writeAttribute(const std::pair<const std::string, std::string>& attribute)
		{
			DSKFMT_BEGIN();

			assert(isName(attribute.first));
			assert(isAttValue(attribute.second));

			DSKFMT_CALL(_stream->write, attribute.first.data(), attribute.first.size());
			DSKFMT_CALL(_stream->write, '=');

			char lim = '"';
			if (std::find(attribute.second.begin(), attribute.second.end(), '"') != attribute.second.end())
			{
				lim = '\'';
			}

			DSKFMT_CALL(_stream->write, lim);
			DSKFMT_CALL(_stream->write, attribute.second.data(), attribute.second.size());
			DSKFMT_CALL(_stream->write, lim);

			return _stream->getStatus();
		}
	}
}
