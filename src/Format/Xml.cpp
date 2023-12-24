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

		void XmlIStream::readFile(xml::File& file)
		{
			DSKFMT_BEGIN();

			DSK_CALL(readProlog, file.prolog);
			DSK_CALL(_readElement, file.root);
			DSK_CALL(readEnding, file.ending);
		}

		void XmlIStream::readProlog(xml::Prolog& prolog)
		{
			DSKFMT_BEGIN();

			assert(!_prologRead);

			prolog.declaration.reset();
			prolog.doctype.reset();
			prolog.instructions.clear();

			DSK_CALL(_readDeclaration, prolog.declaration);
			DSK_CALL(_readPIsSpacesAndComments, prolog.instructions);
			DSK_CALL(_readDoctype, prolog.doctype);

			if (prolog.doctype.has_value())
			{
				DSK_CALL(_readPIsSpacesAndComments, prolog.instructions);
			}

			_prologRead = true;
		}

		void XmlIStream::readElementTag(xml::ElementTag& tag)
		{
			DSKFMT_BEGIN();

			assert(_prologRead);
			assert(!_treeRead);

			tag.name.clear();
			tag.attributes.clear();

			uint64_t skipCount;
			char buffer;

			DSKFMT_STREAM_CALL(expect, '<');
			DSK_CALL(_readName, tag.name);
			DSK_CHECK(!tag.name.empty(), "Could not read tag name.");
			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);

			std::string attName;
			while (skipCount)
			{
				attName.clear();
				DSK_CALL(_readName, attName);
				if (attName.empty())
				{
					skipCount = 0;
				}
				else
				{
					DSK_CHECK(!tag.attributes.contains(attName), std::format("Attribute name found twice in element '{}': '{}'.", tag.name, attName));
					DSK_CALL(_readEq);
					DSK_CALL(_readAttValue, tag.attributes[attName]);
					DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
				}
			}

			DSKFMT_STREAM_CALL(read, buffer);
			if (buffer != '>')
			{
				DSK_CHECK(buffer == '/', std::format("Unexpected character in element tag: '{}'.", buffer));
				DSKFMT_STREAM_CALL(expect, '>');
				tag.isEmpty = true;
				_treeRead = _tags.empty();
			}
			else
			{
				tag.isEmpty = false;
				_tags.push_back(tag.name);
			}
		}

		void XmlIStream::readElementContent(xml::ElementContent& content, xml::ContentType::Flags filter)
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
				DSK_CALL(_readComment, parsedSomething);
			} while (parsedSomething);

			// Read the two first characters and guess what the next content type is

			DSKFMT_STREAM_CALL(read, buffer, 2);
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

			DSKFMT_STREAM_CALL(unread, 2);
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
						DSKFMT_STREAM_CALL(expect, etag.data(), etag.size());
						DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
						DSKFMT_STREAM_CALL(expect, '>');
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
						DSK_CALL(readElementTag, *content.childTag);

						break;
					}
					case xml::ContentType::Child:
					{
						content.child = std::make_unique<xml::Element>();
						DSK_CALL(_readElement, *content.child);

						break;
					}
					case xml::ContentType::CharData:
					{
						content.charData = std::make_unique<std::string>();
						do {
							DSKFMT_STREAM_CALL(readCharWhile, [](char x) { return x != '<'; }, *content.charData);
							DSK_CALL(_readComment, parsedSomething);
						} while (parsedSomething);

						break;
					}
					case xml::ContentType::ProcessingInstruction:
					{
						content.instruction = std::make_unique<xml::ProcessingInstruction>();
						DSK_CALL(_readProcessingInstruction, *content.instruction, parsedSomething);

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
						DSK_CALL(readElementTag, trash);
						DSK_CALL(readElementContent, content, xml::ContentType::EndOfElement);

						break;
					}
					case xml::ContentType::CharData:
					{
						do {
							DSKFMT_STREAM_CALL(skipCharWhile, [](char x) { return x != '<'; }, skipCount);
							DSK_CALL(_readComment, parsedSomething);
						} while (parsedSomething);

						break;
					}
					case xml::ContentType::ProcessingInstruction:
					{
						xml::ProcessingInstruction trash;
						DSK_CALL(_readProcessingInstruction, trash, parsedSomething);

						break;
					}
				}

				// Read next content

				DSK_CALL(readElementContent, content, filter);
			}
		}

		void XmlIStream::readEnding(xml::Ending& ending)
		{
			DSKFMT_BEGIN();

			assert(_treeRead);

			DSK_CALL(_readPIsSpacesAndComments, ending.instructions);

			_tags.clear();
			_prologRead = false;
			_treeRead = false;
		}

		void XmlIStream::resetFormatState()
		{
			_tags.clear();
			_prologRead = false;
			_treeRead = false;
		}

		void XmlIStream::_readPIsSpacesAndComments(std::vector<xml::ProcessingInstruction>& instructions)
		{
			DSKFMT_BEGIN();

			uint64_t skipCount;
			bool parsedSomething;

			do {

				do {
					DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
					DSK_CALL(_readComment, parsedSomething);
				} while (parsedSomething);

				instructions.emplace_back();
				DSK_CALL(_readProcessingInstruction, instructions.back(), parsedSomething);

			} while (parsedSomething);

			instructions.pop_back();
		}

		void XmlIStream::_readElement(xml::Element& element)
		{
			DSKFMT_BEGIN();

			element.contents.clear();

			DSK_CALL(readElementTag, element.tag);

			if (!element.tag.isEmpty)
			{
				element.contents.reserve(_singleBufferSize / sizeof(xml::ElementContent));
				
				element.contents.emplace_back();
				DSK_CALL(readElementContent, element.contents.back());
				while (element.contents.back().type != xml::ContentType::EndOfElement)
				{
					element.contents.emplace_back();
					DSK_CALL(readElementContent, element.contents.back());
				}
				element.contents.pop_back();
				
				element.contents.shrink_to_fit();
			}
		}

		void XmlIStream::_readDeclaration(std::optional<xml::Declaration>& declaration)
		{
			DSKFMT_BEGIN();

			declaration.reset();

			uint64_t skipCount;
			char buffer[5];

			// Read '<?xml'

			DSKFMT_STREAM_CALL(read, buffer, 5);
			if (!std::equal(buffer, buffer + 5, "<?xml"))
			{
				DSKFMT_STREAM_CALL(unread, 5);
				return;
			}

			declaration.emplace();

			// Read version

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSK_CHECK(skipCount, "Expected spaces after '<?xml' but found none.");
			DSKFMT_STREAM_CALL(expect, "version", 7);
			DSK_CALL(_readEq);

			DSKFMT_STREAM_CALL(read, buffer[0]);
			DSK_CHECK(buffer[0] == '"' || buffer[0] == '\'', std::format("Expected ''' or '\"'. Instead, got '{}'.", buffer[0]));

			DSKFMT_STREAM_CALL(expect, "1.", 2);
			declaration->versionMajor = 1;
			DSKFMT_STREAM_CALL(readAsciiNumber, declaration->versionMinor);

			DSKFMT_STREAM_CALL(expect, buffer[0]);

			// Read encoding

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			if (skipCount)
			{
				DSKFMT_STREAM_CALL(read, buffer[0]);
				if (buffer[0] == 'e')
				{
					declaration->encoding.emplace();

					DSKFMT_STREAM_CALL(expect, "ncoding", 7);
					DSK_CALL(_readEq);
					DSKFMT_STREAM_CALL(read, buffer[0]);
					DSK_CHECK(buffer[0] == '"' || buffer[0] == '\'', std::format("Expected ''' or '\"'. Instead, got '{}'.", buffer[0]));
					DSK_CALL(_readEncName, declaration->encoding.value());
					DSKFMT_STREAM_CALL(expect, buffer[0]);
				}
				else
				{
					_stream->unread(1);
				}
			}

			// Read standalone

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			if (skipCount)
			{
				DSKFMT_STREAM_CALL(read, buffer[0]);
				if (buffer[0] == 's')
				{
					declaration->standalone.emplace(true);

					DSKFMT_STREAM_CALL(expect, "tandalone", 9);
					DSK_CALL(_readEq);
					std::string value;
					DSK_CALL(_readAttValue, value);
					DSK_CHECK(value == "no" || value == "yes", std::format("Standalone can only take values 'yes' and 'no'. Instead, got '{}'.", value));
					declaration->standalone = (value == "yes");
				}
				else
				{
					_stream->unread(1);
				}
			}

			// Read '?>'

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSKFMT_STREAM_CALL(expect, "?>", 2);
		}

		void XmlIStream::_readDoctype(std::optional<xml::Doctype>& doctype)
		{
			DSKFMT_BEGIN();

			doctype.reset();

			char buffer[9];
			uint64_t skipCount;

			// Read '<!DOCTYPE'

			DSKFMT_STREAM_CALL(read, buffer, 9);
			if (!std::equal(buffer, buffer + 9, "<!DOCTYPE"))
			{
				DSKFMT_STREAM_CALL(unread, 9);
				return;
			}

			doctype.emplace();

			// Read name

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSK_CHECK(skipCount, "Expected space after '<!DOCTYPE' but found no space.");
			DSK_CALL(_readName, doctype->name);
			DSK_CHECK(!doctype->name.empty(), "Error while parsing doctype name.");

			// Read '>'

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSKFMT_STREAM_CALL(expect, '>');
		}

		void XmlIStream::_readProcessingInstruction(xml::ProcessingInstruction& instruction, bool& parsedSomething)
		{
			DSKFMT_BEGIN();

			instruction.target.clear();
			instruction.instruction.clear();

			char buffer[2];
			uint64_t skipCount, readCount;

			if (_stream->eof())
			{
				parsedSomething = false;
				return;
			}

			// Read '<?'

			DSKFMT_STREAM_CALL(read, buffer, 2);
			parsedSomething = (buffer[0] == '<' && buffer[1] == '?');
			if (!parsedSomething)
			{
				DSKFMT_STREAM_CALL(unread, 2);
				return;
			}

			// Read target

			DSK_CALL(_readName, instruction.target);
			DSK_CHECK(!instruction.target.empty(), "Error while parsing processing instruction target.");
			DSK_CHECK(isPITarget(instruction.target), std::format("PI Target cannot starts with 'XML'. PI Target: '{}'.", instruction.target));

			// Read instruction and '?>'

			instruction.instruction.clear();
			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			if (skipCount)
			{
				auto conditionFunc = [&](char x) {
					return !(*buffer == '?' && x == '>') && (*buffer = x, true);
				};

				DSKFMT_STREAM_CALL(readCharWhile, conditionFunc, instruction.instruction);
				DSK_CHECK(!instruction.instruction.empty() && instruction.instruction.back() == '?', std::format("Encountered an unfinished processing instruction: '{}'", instruction.instruction));
				instruction.instruction.pop_back();
				DSKFMT_STREAM_CALL(expect, '>');
			}
		}

		void XmlIStream::_readEq()
		{
			DSKFMT_BEGIN();

			uint64_t skipCount;

			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
			DSKFMT_STREAM_CALL(expect, '=');
			DSKFMT_STREAM_CALL(skipCharWhile, isSpaceChar, skipCount);
		}

		void XmlIStream::_readEncName(std::string& name)
		{
			DSKFMT_BEGIN();

			char x;

			DSKFMT_STREAM_CALL(read, x);
			if (!isEncNameStartChar(x))
			{
				_stream->unread(1);
				return;
			}
			name.push_back(x);

			DSKFMT_STREAM_CALL(readCharWhile, isEncNameChar, name);
		}

		void XmlIStream::_readName(std::string& name)
		{
			DSKFMT_BEGIN();

			char x;

			DSKFMT_STREAM_CALL(read, x);
			if (!isNameStartChar(x))
			{
				_stream->unread(1);
				return;
			}
			name.push_back(x);

			DSKFMT_STREAM_CALL(readCharWhile, isNameChar, name);
		}

		void XmlIStream::_readAttValue(std::string& value)
		{
			DSKFMT_BEGIN();

			char openChar;

			DSKFMT_STREAM_CALL(read, openChar);
			DSK_CHECK(openChar == '\'' || openChar == '"', std::format("Expected ''' or '\"', but instead got '{}'.", openChar));

			// TODO: '&' can appear in the attribute value, but it MUST be a ref, not just any '&'
			DSKFMT_STREAM_CALL(readCharWhile, [&](char x) { return x != '<' && x != openChar; }, value);

			DSKFMT_STREAM_CALL(expect, openChar);
		}

		void XmlIStream::_readComment(bool& parsedSomething)
		{
			DSKFMT_BEGIN();

			char buffer[4];

			if (_stream->eof())
			{
				parsedSomething = false;
				return;
			}

			// Read "<!--"

			DSKFMT_STREAM_CALL(read, buffer, 4);
			parsedSomething = std::equal(buffer, buffer + 4, "<!--");
			if (!parsedSomething)
			{
				DSKFMT_STREAM_CALL(unread, 4);
				return;
			}

			// Search for "-->"

			uint64_t skipCount;
			auto conditionFunc = [&](char x) {
				return !(buffer[0] == '-' && buffer[1] == '-' && x == '>') && (buffer[0] = buffer[1], buffer[1] = x, true);
			};

			DSKFMT_STREAM_CALL(skipCharWhile, conditionFunc, skipCount);
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

		void XmlOStream::writeFile(const xml::File& file)
		{
			DSKFMT_BEGIN();

			assert(!_prologWritten);

			DSK_CALL(writeProlog, file.prolog);
			DSK_CALL(_writeElement, file.root);
			DSK_CALL(writeEnding, file.ending);
		}

		void XmlOStream::writeProlog(const xml::Prolog& prolog)
		{
			DSKFMT_BEGIN();

			assert(!_prologWritten);

			if (prolog.declaration.has_value())
			{
				DSK_CALL(_writeDeclaration, prolog.declaration.value());
			}

			if (prolog.doctype.has_value())
			{
				DSK_CALL(_writeDoctype, prolog.doctype.value());
			}

			for (const xml::ProcessingInstruction& instruction : prolog.instructions)
			{
				DSK_CALL(_writeProcessingInstruction, instruction);
			}

			_prologWritten = true;
		}

		void XmlOStream::writeElementTag(const xml::ElementTag& tag)
		{
			DSKFMT_BEGIN();

			assert(_prologWritten);
			assert(!_treeWritten);

			assert(isName(tag.name));

			DSKFMT_STREAM_CALL(write, '<');
			DSKFMT_STREAM_CALL(write, tag.name.data(), tag.name.size());

			for (const std::pair<std::string, std::string>& attribute : tag.attributes)
			{
				DSKFMT_STREAM_CALL(write, ' ');
				DSK_CALL(_writeAttribute, attribute);
			}

			if (tag.isEmpty)
			{
				DSKFMT_STREAM_CALL(write, '/');
			}
			else
			{
				_tags.push_back(tag.name);
			}
			DSKFMT_STREAM_CALL(write, '>');
		}

		void XmlOStream::writeElementContent(const xml::ElementContent& content)
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

					DSKFMT_STREAM_CALL(write, "</", 2);
					DSKFMT_STREAM_CALL(write, _tags.back().data(), _tags.back().size());
					DSKFMT_STREAM_CALL(write, '>');

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

					DSK_CALL(writeElementTag, *content.childTag);

					break;
				}
				case xml::ContentType::Child:
				{
					assert(!content.childTag);
					assert(!content.charData);
					assert(!content.instruction);

					DSK_CALL(_writeElement, *content.child);

					break;
				}
				case xml::ContentType::CharData:
				{
					assert(!content.childTag);
					assert(!content.child);
					assert(!content.instruction);

					assert(std::find(content.charData->begin(), content.charData->end(), '<') == content.charData->end());
					// TODO: assert(std::find(content.charData.begin(), content.charData.end(), '&') == content.charData.end());

					DSKFMT_STREAM_CALL(write, content.charData->data(), content.charData->size());

					break;
				}
				case xml::ContentType::ProcessingInstruction:
				{
					assert(!content.childTag);
					assert(!content.child);
					assert(!content.charData);

					DSK_CALL(_writeProcessingInstruction, *content.instruction);

					break;
				}
				default:
				{
					assert(false);
				}
			}
		}

		void XmlOStream::writeEnding(const xml::Ending& ending)
		{
			DSKFMT_BEGIN();

			assert(_treeWritten);

			for (const xml::ProcessingInstruction& instruction : ending.instructions)
			{
				DSK_CALL(_writeProcessingInstruction, instruction);
			}

			_tags.clear();
			_prologWritten = false;
			_treeWritten = false;
		}

		void XmlOStream::resetFormatState()
		{
			_tags.clear();
			_prologWritten = false;
			_treeWritten = false;
		}

		void XmlOStream::_writeDeclaration(const xml::Declaration& declaration)
		{
			DSKFMT_BEGIN();

			assert(declaration.versionMajor == 1);
			assert(!declaration.encoding.has_value() || isEncName(declaration.encoding.value()));

			DSKFMT_STREAM_CALL(write, "<?xml version='1.", 17);
			DSKFMT_STREAM_CALL(writeAsciiNumber, declaration.versionMinor);
			DSKFMT_STREAM_CALL(write, '\'');

			if (declaration.encoding.has_value())
			{
				DSKFMT_STREAM_CALL(write, " encoding='", 11);
				DSKFMT_STREAM_CALL(write, declaration.encoding->data(), declaration.encoding->size());
				DSKFMT_STREAM_CALL(write, '\'');
			}

			if (declaration.standalone.has_value())
			{
				if (declaration.standalone.value())
				{
					DSKFMT_STREAM_CALL(write, " standalone='yes'", 17);
				}
				else
				{
					DSKFMT_STREAM_CALL(write, " standalone='no'", 16);
				}
			}

			DSKFMT_STREAM_CALL(write, "?>", 2);
		}

		void XmlOStream::_writeDoctype(const xml::Doctype& doctype)
		{
			DSKFMT_BEGIN();

			assert(isName(doctype.name));

			DSKFMT_STREAM_CALL(write, "<!DOCTYPE ", 10);
			DSKFMT_STREAM_CALL(write, doctype.name.data(), doctype.name.size());
			DSKFMT_STREAM_CALL(write, '>');
		}

		void XmlOStream::_writeProcessingInstruction(const xml::ProcessingInstruction& instruction)
		{
			DSKFMT_BEGIN();

			assert(isPITarget(instruction.target));
			assert(instruction.instruction.find("<?") == std::string::npos);

			DSKFMT_STREAM_CALL(write, "<?", 2);
			DSKFMT_STREAM_CALL(write, instruction.target.data(), instruction.target.size());
			DSKFMT_STREAM_CALL(write, ' ');
			DSKFMT_STREAM_CALL(write, instruction.instruction.data(), instruction.instruction.size());
			DSKFMT_STREAM_CALL(write, "?>", 2);
		}

		void XmlOStream::_writeElement(const xml::Element& element)
		{
			DSKFMT_BEGIN();

			assert(!element.tag.isEmpty || element.contents.empty());

			DSK_CALL(writeElementTag, element.tag);

			for (const xml::ElementContent& content : element.contents)
			{
				assert(content.type != xml::ContentType::EndOfElement);
				DSK_CALL(writeElementContent, content);
			}

			if (!element.tag.isEmpty)
			{
				xml::ElementContent tagEnd;
				tagEnd.type = xml::ContentType::EndOfElement;
				DSK_CALL(writeElementContent, tagEnd);
			}
		}

		void XmlOStream::_writeAttribute(const std::pair<const std::string, std::string>& attribute)
		{
			DSKFMT_BEGIN();

			assert(isName(attribute.first));
			assert(isAttValue(attribute.second));

			DSKFMT_STREAM_CALL(write, attribute.first.data(), attribute.first.size());
			DSKFMT_STREAM_CALL(write, '=');

			char lim = '"';
			if (std::find(attribute.second.begin(), attribute.second.end(), '"') != attribute.second.end())
			{
				lim = '\'';
			}

			DSKFMT_STREAM_CALL(write, lim);
			DSKFMT_STREAM_CALL(write, attribute.second.data(), attribute.second.size());
			DSKFMT_STREAM_CALL(write, lim);
		}
	}
}
