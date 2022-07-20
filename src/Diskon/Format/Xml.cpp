#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		const FormatError& XmlStream::readFile(xml::File& file)
		{
			FMTSTREAM_BEGIN_READ();

			FMTSTREAM_VERIFY_SELF_CALL(readProlog, file.prolog);
			FMTSTREAM_VERIFY_SELF_CALL(readElementTree, file.rootTree);

			return error;
		}

		const FormatError& XmlStream::readProlog(xml::Prolog& prolog)
		{
			FMTSTREAM_BEGIN_READ();

			bool parsedSomething;

			// Load XML declaration

			xml::Declaration declaration;
			FMTSTREAM_VERIFY_SELF_CALL(readDeclaration, declaration, parsedSomething);
			if (parsedSomething)
			{
				prolog.declaration = declaration;
			}
			else
			{
				prolog.declaration.reset();
			}

			// Load PIs, spaces and comments between declaration and doctype

			FMTSTREAM_VERIFY_SELF_CALL(readPIsSpacesAndComments, prolog.processingInstructions);

			// Load doctype

			xml::DocType doctype;
			FMTSTREAM_VERIFY_SELF_CALL(readDoctype, doctype, parsedSomething);
			if (parsedSomething)
			{
				prolog.doctype = doctype;
			}
			else
			{
				prolog.doctype.reset();
			}

			// Load PIs, spaces and comments between doctype and XML tree

			FMTSTREAM_VERIFY_SELF_CALL(readPIsSpacesAndComments, prolog.processingInstructions);

			// Go through XML tree

			std::streampos rootPos = getSourcePos();
			xml::Element rootElement;
			FMTSTREAM_VERIFY_SELF_CALL(readElement, rootElement);
			FMTSTREAM_VERIFY_SELF_CALL(setSourcePos, rootElement.endOfElement);

			// Load PIs, spaces and comments after XML tree

			if (!readPIsSpacesAndComments(prolog.processingInstructions))
			{
				if (error.errorCode == FormatError::ErrorCode::InvalidStream && stream.eof())
				{
					stream.clear();
					error.clear();
				}
				else
				{
					return error;
				}
			}

			// Set the cursor position right before XML tree

			FMTSTREAM_VERIFY_SELF_CALL(setSourcePos, rootPos);

			return error;
		}

		const FormatError& XmlStream::readElementTree(xml::ElementTree& elementTree)
		{
			FMTSTREAM_BEGIN_READ();

			char buffer[2];
			bool parsedSomething;

			elementTree.data.content.clear();
			elementTree.data.processingInstructions.clear();
			elementTree.childs.clear();

			// Read STag

			bool emptyElement;
			FMTSTREAM_VERIFY_SELF_CALL(readSTag, elementTree.data, emptyElement);
			if (emptyElement)
			{
				return error;
			}

			// Read content

			FMTSTREAM_VERIFY(stream.read(buffer, 2), InvalidStream, "XmlStream: Error while starting to read element content.");
			while (buffer[0] != '<' || buffer[1] != '/')
			{
				if (buffer[0] == '<')
				{
					FMTSTREAM_VERIFY(stream.putback(buffer[1]), InvalidStream, "XmlStream::readElementTree: Error while putting back character.");
					FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream::readElementTree: Error while putting back character.");

					if (buffer[1] == '?')
					{
						elementTree.data.processingInstructions.emplace_back();
						FMTSTREAM_VERIFY_SELF_CALL(readProcessingInstruction, elementTree.data.processingInstructions.back(), parsedSomething);
					}
					else if (buffer[1] == '!')
					{
						FMTSTREAM_VERIFY_SELF_CALL(readComment, parsedSomething);
						FMTSTREAM_VERIFY(parsedSomething, XmlInvalidElement, "XmlStream: Expected comment but could not parse one in element.");
						// TODO: if (!parsedSomehting) readCDSect();
					}
					else
					{
						elementTree.childs.emplace_back();
						FMTSTREAM_VERIFY_SELF_CALL(readElementTree, elementTree.childs.back());
					}

					FMTSTREAM_VERIFY(stream.read(buffer, 2), InvalidStream, "XmlStream: Error while reading element content.");
				}
				else
				{
					elementTree.data.content.push_back(buffer[0]);
					buffer[0] = buffer[1];
					FMTSTREAM_VERIFY(stream.read(buffer + 1, 1), InvalidStream, "XmlStream: Error while reading element content.");
				}
			}
			FMTSTREAM_VERIFY(stream.putback(buffer[1]), InvalidStream, "XmlStream::readElementTree: Error while putting back character.");
			FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream::readElementTree: Error while putting back character.");

			// Read ETag

			FMTSTREAM_VERIFY_SELF_CALL(readETag, elementTree.data.tag);
			elementTree.endOfElement = getSourcePos();

			// Go to next element

			stream.read(buffer, 2);
			while (stream && !(buffer[0] == '<' && (buffer[1] != '?' && buffer[1] != '!')))
			{
				buffer[0] = buffer[1];
				stream.read(buffer + 1, 1);
			}

			if (stream.eof())
			{
				stream.clear();
				return error;
			}

			FMTSTREAM_VERIFY(stream, InvalidStream, "XmlStream::readElementTree: Error while searching next element.");
			FMTSTREAM_VERIFY(stream.putback(buffer[1]), InvalidStream, "XmlStream::readElementTree: Error while putting back character.");
			FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream::readElementTree: Error while putting back character.");

			return error;
		}

		const FormatError& XmlStream::readElement(xml::Element& element)
		{
			FMTSTREAM_BEGIN_READ();

			char buffer[2];
			bool parsedSomething;

			element.data.content.clear();
			element.data.processingInstructions.clear();
			element.childs.clear();

			// Read STag

			bool emptyElement;
			FMTSTREAM_VERIFY_SELF_CALL(readSTag, element.data, emptyElement);
			if (emptyElement)
			{
				return error;
			}

			// Read content

			FMTSTREAM_VERIFY(stream.read(buffer, 2), InvalidStream, "XmlStream: Error while starting to read element content.");
			while (buffer[0] != '<' || buffer[1] != '/')
			{
				if (buffer[0] == '<')
				{
					FMTSTREAM_VERIFY(stream.putback(buffer[1]), InvalidStream, "XmlStream::readElement: Error while putting back character.");
					FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream::readElement: Error while putting back character.");

					if (buffer[1] == '?')
					{
						element.data.processingInstructions.emplace_back();
						FMTSTREAM_VERIFY_SELF_CALL(readProcessingInstruction, element.data.processingInstructions.back(), parsedSomething);
					}
					else if (buffer[1] == '!')
					{
						FMTSTREAM_VERIFY_SELF_CALL(readComment, parsedSomething);
						FMTSTREAM_VERIFY(parsedSomething, XmlInvalidElement, "XmlStream: Expected comment but could not parse one in element.");
						// TODO: if (!parsedSomehting) readCDSect();
					}
					else
					{
						// TODO: Function for not parsing child !
						element.childs.emplace_back(getSourcePos());
						xml::Element child;
						FMTSTREAM_VERIFY_SELF_CALL(readElement, child);
					}

					FMTSTREAM_VERIFY(stream.read(buffer, 2), InvalidStream, "XmlStream: Error while reading element content.");
				}
				else
				{
					element.data.content.push_back(buffer[0]);
					buffer[0] = buffer[1];
					FMTSTREAM_VERIFY(stream.read(buffer + 1, 1), InvalidStream, "XmlStream: Error while reading element content.");
				}
			}
			FMTSTREAM_VERIFY(stream.putback(buffer[1]), InvalidStream, "XmlStream::readElement: Error while putting back character.");
			FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream::readElement: Error while putting back character.");

			// Read ETag

			FMTSTREAM_VERIFY_SELF_CALL(readETag, element.data.tag);
			element.endOfElement = getSourcePos();

			// Go to next element

			stream.read(buffer, 2);
			while (stream && !(buffer[0] == '<' && (buffer[1] != '?' && buffer[1] != '!')))
			{
				buffer[0] = buffer[1];
				stream.read(buffer + 1, 1);
			}

			if (stream.eof())
			{
				stream.clear();
				return error;
			}

			FMTSTREAM_VERIFY(stream, InvalidStream, "XmlStream::readElement: Error while searching next element.");
			FMTSTREAM_VERIFY(stream.putback(buffer[1]), InvalidStream, "XmlStream::readElement: Error while putting back character.");
			FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream::readElement: Error while putting back character.");

			return error;
		}

		const FormatError& XmlStream::writeFile(const xml::File& file)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& XmlStream::writeProlog(const xml::Prolog& prolog)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& XmlStream::writeDeclaration(const xml::Declaration& declaration)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& XmlStream::writeDoctype(const xml::DocType& doctype)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& XmlStream::writeProcessingInstruction(const xml::ProcessingInstruction& instruction)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& XmlStream::writeElementTree(const xml::ElementTree& elementTree)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& XmlStream::writeElementData(const xml::ElementData& elementData)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& XmlStream::writeAscend(uint32_t count)
		{
			FMTSTREAM_BEGIN_WRITE();

			return error;
		}

		const FormatError& XmlStream::readSpaces(bool& parsedSomething)
		{
			FMTSTREAM_BEGIN_READ();

			char x;

			FMTSTREAM_VERIFY(stream.read(&x, 1), InvalidStream, "XmlStream::readSpaces: Error while extracting a character.");
			parsedSomething = (x == 0x20 || x == 0x09 || x == 0x0D || x == 0x0A);

			while (x == 0x20 || x == 0x09 || x == 0x0D || x == 0x0A)
			{
				FMTSTREAM_VERIFY(stream.read(&x, 1), InvalidStream, "XmlStream::readSpaces: Error while extracting a character.");
			}

			FMTSTREAM_VERIFY(stream.putback(x), InvalidStream, "XmlStream::readSpaces: Error while putting back a character.");

			return error;
		}

		const FormatError& XmlStream::readComment(bool& parsedSomething)
		{
			FMTSTREAM_BEGIN_READ();

			char buffer[4];

			// Read "<!--"

			FMTSTREAM_VERIFY(stream.read(buffer, 4), InvalidStream, "XmlStream: Error while reading '<!--'.");
			parsedSomething = std::equal(buffer, buffer + 4, "<!--");
			if (!parsedSomething)
			{
				FMTSTREAM_VERIFY(stream.putback(buffer[3]), InvalidStream, "XmlStream::readComment: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[2]), InvalidStream, "XmlStream::readComment: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[1]), InvalidStream, "XmlStream::readComment: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream::readComment: Error while putting back a character.");
				return error;
			}

			// Search for "--"

			FMTSTREAM_VERIFY(stream.read(buffer, 2), InvalidStream, "XmlStream::readComment: Error while extracting a character.");
			while (buffer[0] != '-' || buffer[1] != '-')
			{
				buffer[1] = buffer[0];
				FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream::readComment: Error while extracting a character.");
			}

			// Check the next character is '>'

			FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream::readComment: Error while reading '>'.");
			FMTSTREAM_VERIFY(buffer[0] == '>', XmlInvalidComment, "XmlStream: Invalid double-hyphen ('--') in an XML comment without '>' behind it.");

			return error;
		}

		namespace
		{
			bool isNameStartChar(char x)
			{
				return std::isalpha(x) || x == ':' || x == '_';
			}

			bool isNameChar(char x)
			{
				return isNameStartChar(x) || std::isdigit(x) || x == '-' || x == '.';
			}
		}

		const FormatError& XmlStream::readName(std::string& name)
		{
			FMTSTREAM_BEGIN_READ();

			char x;

			name.clear();

			FMTSTREAM_VERIFY(stream.read(&x, 1), InvalidStream, "XmlStream: Error while reading first character of a name.");
			if (!isNameStartChar(x))
			{
				FMTSTREAM_VERIFY(stream.putback(x), InvalidStream, "XmlStream: Error while putting back first character of a name.");
				return error;
			}

			while (isNameChar(x))
			{
				name.push_back(x);
				FMTSTREAM_VERIFY(stream.read(&x, 1), InvalidStream, "XmlStream: Error while reading character of a name.");
			}
			FMTSTREAM_VERIFY(stream.putback(x), InvalidStream, "XmlStream: Error while putting back character of a name.");

			return error;
		}

		const FormatError& XmlStream::readSTag(xml::ElementData& elementData, bool& emptyElement)
		{
			FMTSTREAM_BEGIN_READ();

			char x;
			bool parsedSomething;

			// Read '<'

			FMTSTREAM_VERIFY(stream.read(&x, 1), InvalidStream, "XmlStream: Error while reading '<' at start of element.");
			FMTSTREAM_VERIFY(x == '<', XmlInvalidElement, "XmlStream: Expected '<' to start an element but instead found '" + std::string(&x, 1) + "'.");

			// Read tag

			FMTSTREAM_VERIFY_SELF_CALL(readName, elementData.tag);
			FMTSTREAM_VERIFY(!elementData.tag.empty(), XmlInvalidElement, "XmlStream: Expected element tag but could not read one.");

			// Read attributes

			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSomething);
			while (parsedSomething)
			{
				std::string attributeName;
				FMTSTREAM_VERIFY_SELF_CALL(readName, attributeName);
				if (!attributeName.empty())
				{
					FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSomething);
					FMTSTREAM_VERIFY(stream.read(&x, 1), InvalidStream, "XmlStream: Error while reading '=' after attribute name in element.");
					FMTSTREAM_VERIFY(x == '=', XmlInvalidElement, "XmlStream: '=' not found attribute name in element.");
					FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSomething);

					char opening;
					FMTSTREAM_VERIFY(stream.read(&opening, 1), InvalidStream, "XmlStream: Error while reading attribute value opening in element.");
					FMTSTREAM_VERIFY(opening == '\'' || opening == '"', XmlInvalidElement, "XmlStream: Expected attribute value opening to be ''' or '\"' but instead found '" + std::string(&opening, 1) + "'.");

					std::string attributeValue;
					FMTSTREAM_VERIFY(stream.read(&x, 1), InvalidStream, "XmlStream: Error while reading attribute value in element.");
					while (x != opening)
					{
						FMTSTREAM_VERIFY(x != '<', XmlInvalidElement, "XmlStream: Found '<' in attribute value in element, which is forbidden.");
						attributeValue.push_back(x);
						FMTSTREAM_VERIFY(stream.read(&x, 1), InvalidStream, "XmlStream: Error while reading attribute value in element.");
					}

					elementData.attributes[attributeName] = attributeValue;
				}

				FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSomething);
			}

			// Read '>' or '/>'

			FMTSTREAM_VERIFY(stream.read(&x, 1), InvalidStream, "XmlStream: Error while reading '>' or '/' at start of element.");
			emptyElement = (x == '/');
			if (emptyElement)
			{
				FMTSTREAM_VERIFY(stream.read(&x, 1), InvalidStream, "XmlStream: Error while reading '>' at start of element.");
			}
			FMTSTREAM_VERIFY(x == '>', XmlInvalidElement, "XmlStream: Expected '>' at start of element but instead found '" + std::string(&x, 1) + "'.");

			return error;
		}

		const FormatError& XmlStream::readETag(const std::string& tag)
		{
			FMTSTREAM_BEGIN_READ();

			char buffer[2];
			bool parsedSomething;

			// Read '</'

			FMTSTREAM_VERIFY(stream.read(buffer, 2), InvalidStream, "XmlStream: Error while reading '</' at end of element.");
			FMTSTREAM_VERIFY(buffer[0] == '<' && buffer[1] == '/', XmlInvalidElement, "XmlStream: Expected '</' at end of element but instead got '" + std::string(buffer, 2) + "'.");

			// Read tag

			std::string tagFound;
			FMTSTREAM_VERIFY_SELF_CALL(readName, tagFound);
			FMTSTREAM_VERIFY(tagFound == tag, XmlInvalidElement, "XmlStream: Expected '</" + tag + "' but instead found '</" + tagFound + "'.");

			// Read '>'

			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSomething);
			FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while reading '>' at end of element.");
			FMTSTREAM_VERIFY(buffer[0] == '>', XmlInvalidElement, "XmlStream: Expected '>' at end of element but instead got '" + std::string(buffer, 1) + "'.");

			return error;
		}

		const FormatError& XmlStream::readDeclaration(xml::Declaration& declaration, bool& parsedSomething)
		{
			FMTSTREAM_BEGIN_READ();

			char buffer[10];
			bool parsedSpace;

			// Read '<?xml'

			FMTSTREAM_VERIFY(stream.read(buffer, 5), InvalidStream, "XmlStream: Error while reading '<?xml' in XML declaration.");
			parsedSomething = std::equal(buffer, buffer + 5, "<?xml");
			if (!parsedSomething)
			{
				FMTSTREAM_VERIFY(stream.putback(buffer[4]), InvalidStream, "XmlStream::readDeclaration: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[3]), InvalidStream, "XmlStream::readDeclaration: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[2]), InvalidStream, "XmlStream::readDeclaration: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[1]), InvalidStream, "XmlStream::readDeclaration: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream::readDeclaration: Error while putting back a character.");
				return error;
			}

			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);

			// Read version

			FMTSTREAM_VERIFY(stream.read(buffer, 7), InvalidStream, "XmlStream: Error while reading 'version' in XML declaration.");
			FMTSTREAM_VERIFY(std::equal(buffer, buffer + 7, "version"), XmlInvalidDeclaration, "XmlStream: Expected 'version' in XML declaration but instead got '" + std::string(buffer, 7) + "'.");

			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);
			FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while reading '=' after 'version' in XML declaration.");
			FMTSTREAM_VERIFY(buffer[0] == '=', XmlInvalidDeclaration, "XmlStream: '=' not found after 'version' in XML declaration.");
			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);

			FMTSTREAM_VERIFY(stream.read(buffer, 3), InvalidStream, "XmlStream: Error while reading begining of version in XML declaration.");
			FMTSTREAM_VERIFY(buffer[0] == '\'' || buffer[0] == '"', XmlInvalidDeclaration, "XmlStream: Error while reading version in XML declaration. Expected ''' or '\"' but instead got '" + std::string(buffer, 1) + "'.");
			FMTSTREAM_VERIFY(buffer[1] == '1' && buffer[2] == '.', XmlInvalidDeclaration, "XmlStream: Invalid version in XML declaration. Should start with '1.' but instead started with '" + std::string(buffer + 1, 2) + "'.");
			declaration.versionMajor = 1;

			buffer[1] = buffer[0];
			FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while reading character for version in XML declaration.");
			FMTSTREAM_VERIFY(buffer[0] >= 0x30 && buffer[0] <= 0x39, XmlInvalidDeclaration, "XmlStream: Invalid version in XML declaration: '1." + std::string(buffer, 1) + "'.");
			declaration.versionMinor = 0;
			while (buffer[0] >= 0x30 && buffer[0] <= 0x39)
			{
				declaration.versionMinor = 10 * declaration.versionMinor + (buffer[0] - 0x30);
				FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while reading character for version in XML declaration.");
			}
			FMTSTREAM_VERIFY(buffer[0] == buffer[1], XmlInvalidDeclaration, "XmlStream: Error while reading version in XML declaration. Expected '" + std::string(buffer + 1, 1) + "' but got '" + std::string(buffer, 1) + "'.");

			// Read encoding

			declaration.encoding.reset();

			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);
			if (parsedSpace)
			{
				FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while extracting character for encoding presence in XML declaration.");
				if (buffer[0] == 'e')
				{
					FMTSTREAM_VERIFY(stream.read(buffer + 1, 7), InvalidStream, "XmlStream: Error while reading 'encoding' in XML declaration.");
					FMTSTREAM_VERIFY(std::equal(buffer, buffer + 8, "encoding"), XmlInvalidDeclaration, "XmlStream: 'encoding' could not completely be read. Instead, got '" + std::string(buffer, 8) + "'.");

					FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);
					FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while reading '=' after 'encoding' in XML declaration.");
					FMTSTREAM_VERIFY(buffer[0] == '=', XmlInvalidDeclaration, "XmlStream: '=' not found after 'encoding' in XML declaration.");
					FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);

					FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while reading ''' or '\"' after 'encoding = ' in XML declaration.");
					FMTSTREAM_VERIFY(buffer[0] == '\'' || buffer[0] == '"', XmlInvalidDeclaration, "XmlStream: ''' or '\"' not found after 'encoding = '. Instead, found '" + std::string(buffer, 1) + "'.");
					
					buffer[1] = buffer[0];
					FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while reading first character of encoding in XML declaration.");
					FMTSTREAM_VERIFY(std::isalpha(buffer[0]), XmlInvalidDeclaration, "XmlStream: Invalid first character for encoding name in XML declaration. Expected alphabetic character, got '" + std::string(buffer, 1) + "'.");

					declaration.encoding = std::string();
					while (std::isalnum(buffer[0]) || buffer[0] == '-' || buffer[0] == '_' || buffer[0] == '.')
					{
						declaration.encoding->push_back(buffer[0]);
						FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while reading character of encoding in XML declaration.");
					}
					FMTSTREAM_VERIFY(buffer[0] == buffer[1], XmlInvalidDeclaration, "XmlStream: Invalid character in encoding name in XML declaration: '" + std::string(buffer, 1) + "'.");
				}
				else
				{
					FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream: Error while putting back character for encoding presence in XML declaration.");
				}
			}

			// Read standalone

			declaration.standalone.reset();

			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);
			if (parsedSpace)
			{
				FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while extracting character for standalone presence in XML declaration.");
				if (buffer[0] == 's')
				{
					FMTSTREAM_VERIFY(stream.read(buffer + 1, 10), InvalidStream, "XmlStream: Error while reading 'standalone' in XML declaration.");
					FMTSTREAM_VERIFY(std::equal(buffer, buffer + 10, "standalone"), XmlInvalidDeclaration, "XmlStream: 'standalone' could not completely be read. Instead, got '" + std::string(buffer, 10) + "'.");

					FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);
					FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while reading '=' after 'standalone' in XML declaration.");
					FMTSTREAM_VERIFY(buffer[0] == '=', XmlInvalidDeclaration, "XmlStream: '=' not found after 'standalone' in XML declaration.");
					FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);

					FMTSTREAM_VERIFY(stream.read(buffer, 4), InvalidStream, "XmlStream: Error while reading standalone in XML declaration.");
					if (buffer[1] == 'y')
					{
						FMTSTREAM_VERIFY(stream.read(buffer + 4, 1), InvalidStream, "XmlStream: Error while reading standalone in XML declaration.");
						FMTSTREAM_VERIFY(std::equal(buffer, buffer + 5, "'yes'") || std::equal(buffer, buffer + 5, "\"yes\""), XmlInvalidDeclaration, "XmlStream: Expected standalone to be 'yes' in XML declaration but instead got " + std::string(buffer, 5) + ".");
						declaration.standalone = true;
					}
					else
					{
						FMTSTREAM_VERIFY(std::equal(buffer, buffer + 4, "'no'") || std::equal(buffer, buffer + 4, "\"no\""), XmlInvalidDeclaration, "XmlStream: Expected standalone to be 'no' in XML declaration but instead got " + std::string(buffer, 4) + ".");
						declaration.standalone = false;
					}
				}
				else
				{
					FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream: Error while putting back character for standalone presence in XML declaration.");
				}
			}

			// Read '?>'

			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);
			FMTSTREAM_VERIFY(stream.read(buffer, 2), InvalidStream, "XmlStream: Error while reading '?>' in XML declaration.");
			FMTSTREAM_VERIFY(buffer[0] == '?' && buffer[1] == '>', XmlInvalidDeclaration, "XmlStream: Expected '?>' in XML declaration but instead got '" + std::string(buffer, 2) + "'.");

			return error;
		}

		const FormatError& XmlStream::readDoctype(xml::DocType& doctype, bool& parsedSomething)
		{
			FMTSTREAM_BEGIN_READ();

			char buffer[9];
			bool parsedSpace;

			// Read '<!DOCTYPE'

			FMTSTREAM_VERIFY(stream.read(buffer, 9), InvalidStream, "XmlStream: Error while reading '<!DOCTYPE'.");
			parsedSomething = std::equal(buffer, buffer + 9, "<!DOCTYPE");
			if (!parsedSomething)
			{
				FMTSTREAM_VERIFY(stream.putback(buffer[8]), InvalidStream, "XmlStream::readDoctype: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[7]), InvalidStream, "XmlStream::readDoctype: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[6]), InvalidStream, "XmlStream::readDoctype: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[5]), InvalidStream, "XmlStream::readDoctype: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[4]), InvalidStream, "XmlStream::readDoctype: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[3]), InvalidStream, "XmlStream::readDoctype: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[2]), InvalidStream, "XmlStream::readDoctype: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[1]), InvalidStream, "XmlStream::readDoctype: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream::readDoctype: Error while putting back a character.");
				return error;
			}

			// Read name

			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);
			FMTSTREAM_VERIFY(parsedSpace, XmlInvalidDoctype, "XmlStream: Expected space after '<!DOCTYPE' but found no space.");
			FMTSTREAM_VERIFY_SELF_CALL(readName, doctype.name);
			FMTSTREAM_VERIFY(!doctype.name.empty(), XmlInvalidDoctype, "XmlStream: Error while parsing doctype name.");

			// Read '>'

			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);
			FMTSTREAM_VERIFY(stream.read(buffer, 1), InvalidStream, "XmlStream: Error while reading '>' in doctype.");
			FMTSTREAM_VERIFY(buffer[0] == '>', XmlInvalidDoctype, "XmlStream: Expected '>' to end doctype but instead found '" + std::string(buffer, 1) + "'.");

			return error;
		}

		const FormatError& XmlStream::readProcessingInstruction(xml::ProcessingInstruction& instruction, bool& parsedSomething)
		{
			FMTSTREAM_BEGIN_READ();

			char buffer[2];
			bool parsedSpace;

			// Read '<?'

			FMTSTREAM_VERIFY(stream.read(buffer, 2), InvalidStream, "XmlStream: Error while reading '<?' in processing instruction.");
			parsedSomething = std::equal(buffer, buffer + 2, "<?");
			if (!parsedSomething)
			{
				FMTSTREAM_VERIFY(stream.putback(buffer[1]), InvalidStream, "XmlStream::readProcessingInstruction: Error while putting back a character.");
				FMTSTREAM_VERIFY(stream.putback(buffer[0]), InvalidStream, "XmlStream::readProcessingInstruction: Error while putting back a character.");
				return error;
			}

			// Read target

			FMTSTREAM_VERIFY_SELF_CALL(readName, instruction.target);
			FMTSTREAM_VERIFY(!instruction.target.empty(), XmlInvalidProcessingInstruction, "XmlStream: Error while parsing processing instruction target.");
			bool targetStartNotXML =
				std::toupper(instruction.target[0]) != 'X'
				|| std::toupper(instruction.target[1]) != 'M'
				|| std::toupper(instruction.target[2]) != 'L';
			FMTSTREAM_VERIFY(targetStartNotXML, XmlInvalidProcessingInstruction, "XmlStream: PI Target cannot starts with 'XML'. PI Target: '" + instruction.target + "'.");

			// Read instruction and '?>'

			instruction.instruction.clear();
			FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpace);
			if (parsedSpace)
			{
				FMTSTREAM_VERIFY(stream.read(buffer, 2), InvalidStream, "XmlStream: Error while reading PI instruction.");
				while (buffer[0] != '?' || buffer[1] != '>')
				{
					instruction.instruction.push_back(buffer[0]);
					buffer[0] = buffer[1];
					FMTSTREAM_VERIFY(stream.read(buffer + 1, 1), InvalidStream, "XmlStream: Error while reading PI instruction.");
				}
			}

			return error;
		}

		const FormatError& XmlStream::readPIsSpacesAndComments(std::vector<xml::ProcessingInstruction>& instructions)
		{
			FMTSTREAM_BEGIN_READ();
			
			bool parsedSpaces, parsedComment, parsedPI;
			xml::ProcessingInstruction instruction;

			do {
				FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpaces);
				FMTSTREAM_VERIFY_SELF_CALL(readComment, parsedComment);
			} while (parsedSpaces || parsedComment);

			FMTSTREAM_VERIFY_SELF_CALL(readProcessingInstruction, instruction, parsedPI);
			while (parsedPI)
			{
				instructions.push_back(instruction);

				do {
					FMTSTREAM_VERIFY_SELF_CALL(readSpaces, parsedSpaces);
					FMTSTREAM_VERIFY_SELF_CALL(readComment, parsedComment);
				} while (parsedSpaces || parsedComment);

				FMTSTREAM_VERIFY_SELF_CALL(readProcessingInstruction, instruction, parsedPI);
			}

			return error;
		}
	}
}
