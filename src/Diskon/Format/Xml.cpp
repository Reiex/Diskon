#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		/*
		namespace
		{
			bool parseSpaces(std::istream& stream)
			{
				char token;
				stream >> token;
				if (!stream || (token != 0x20 && token != 0x9 && token != 0xD && token != 0xA))
				{
					stream.putback(token);
					return false;
				}

				while (stream && (token == 0x20 || token == 0x9 || token == 0xD || token == 0xA))
				{
					stream >> token;
				}

				stream.putback(token);
				return true;
			}

			bool parseComments(std::istream& stream)
			{
				char a, b, c, d;
				stream >> a >> b >> c >> d;
				if (!stream || a != '<' || b != '!' || c != '-' || d != '-')
				{
					stream.putback(d);
					stream.putback(c);
					stream.putback(b);
					stream.putback(a);
					return false;
				}

				a = '-';
				while (stream)
				{
					stream >> b;
					if (b == '-')
					{
						stream >> c >> d;
						if (!stream || c != '-' || d != '>')
						{
							stream.putback(d);
							stream.putback(c);
						}
						else
						{
							return true;
						}
					}

					a = b;
				}

				return false;
			}

			bool parseSpacesAndComments(std::istream& stream)
			{
				if (!parseSpaces(stream) && !parseComments(stream))
				{
					return false;
				}

				while (parseSpaces(stream) || parseComments(stream));

				return true;
			}
		
			std::array<std::string, 2> parseAttribute(std::istream& stream, IOResult& result, std::string& attribute, std::string& value)
			{
				/*if (attribute.empty())
				{

				}
				else
				{
					char* buffer = 
					stream.read(buffer, expectedAttributeName.size());
					if (!stream || !std::equal(buffer, buffer + 7, "version"))
					{
						result.failedStep = IOResult::FailedStep::ParseFailed;
						result.errorMessage = "'version' not found in XML declaration.";
						return;
					}
				}

				

				parseSpaces(stream);
				stream.read(buffer, 1);
				parseSpaces(stream);
				if (!stream || buffer[0] != '"')
				{
					result.failedStep = IOResult::FailedStep::ParseFailed;
					result.errorMessage = "Error while parsing '=' after 'version' in XML declaration";
					return;
				}

				stream.read(buffer, 5);
				if (!stream || !(std::equal(buffer, buffer + 5, "'1.0'") || std::equal(buffer, buffer + 5, "\"1.0\"")))
				{
					result.failedStep = IOResult::FailedStep::ParseFailed;
					result.errorMessage = "Error while parsing version number in XML declaration";
					return;
				}
			}
		}

		namespace xml
		{
			void XmlDeclaration::read(std::istream& stream, IOResult& result)
			{
				char buffer[10]; // The longest word parsed is "standalone", this might change !
				stream.read(buffer, 5);
				if (!stream || !std::equal(buffer, buffer + 5, "<?xml"))
				{
					result.failedStep = IOResult::FailedStep::ParseFailed;
					result.errorMessage = "'<?xml' not found at start of XML declaration.";
					return;
				}

				if (!parseSpaces(stream) || !stream)
				{
					result.failedStep = IOResult::FailedStep::ParseFailed;
					result.errorMessage = "Failed to find spaces between '<?xml' and 'version' in XML declaration.";
					return;
				}

				stream.read(buffer, 7);
				if (!stream || !std::equal(buffer, buffer + 7, "version"))
				{
					result.failedStep = IOResult::FailedStep::ParseFailed;
					result.errorMessage = "'version' not found in XML declaration.";
					return;
				}

				parseSpaces(stream);
				stream.read(buffer, 1);
				parseSpaces(stream);
				if (!stream || buffer[0] != '"')
				{
					result.failedStep = IOResult::FailedStep::ParseFailed;
					result.errorMessage = "Error while parsing '=' after 'version' in XML declaration";
					return;
				}

				stream.read(buffer, 5);
				if (!stream || !(std::equal(buffer, buffer + 5, "'1.0'") || std::equal(buffer, buffer + 5, "\"1.0\"")))
				{
					result.failedStep = IOResult::FailedStep::ParseFailed;
					result.errorMessage = "Error while parsing version number in XML declaration";
					return;
				}

				versionMajor = buffer[1] - 48;
				versionMinor = buffer[3] - 48;

				// Do a "parse attribute" with a potential "expected attribute name"
			}

			void XmlDeclaration::write(std::ostream& stream, IOResult& result)
			{

			}

			void ProcessingInstructions::read(std::istream& stream, IOResult& result)
			{

			}

			void ProcessingInstructions::write(std::ostream& stream, IOResult& result)
			{

			}

			void DocumentType::read(std::istream& stream, IOResult& result)
			{

			}

			void DocumentType::write(std::ostream& stream, IOResult& result)
			{

			}

			void XmlElement::read(std::istream& stream, IOResult& result)
			{

			}

			void XmlElement::write(std::ostream& stream, IOResult& result)
			{

			}
		}

		XmlFile::XmlFile() :
			_hasDeclaration(false),
			_hasDoctype(false)
		{
		}

		void XmlFile::clear()
		{
			_hasDeclaration = false;
			_hasDoctype = false;
			_root.tag.clear();
			_root.attributes.clear();
			_root.childs.clear();
		}

		void XmlFile::read(std::istream& stream, IOResult& result)
		{
			// XML declaration, if present, is at the first line of the document

			char a, b, c, d, e;
			stream >> a >> b >> c >> d >> e;
			stream.putback(e);
			stream.putback(d);
			stream.putback(c);
			stream.putback(b);
			stream.putback(a);
			if (a == '<' && b == '?' && c == 'x' && d == 'm' && e == 'l')
			{
				_declaration.read(stream, result);
				if (!result)
				{
					return;
				}
			}

			// Mix of optionnal processing instructions, comments, spaces, and somewhere eventually a single doctype

			bool parsedSomething = true;
			while (parsedSomething || parseSpacesAndComments(stream))
			{
				stream >> a >> b;
				stream.putback(b);
				stream.putback(a);
				if (a == '<' && b == '?')
				{
					xml::ProcessingInstructions instruction;
					instruction.read(stream, result);
					if (!result)
					{
						return;
					}
					_processingInstructions.push_back(instruction);

					parsedSomething = true;
				}
				else if (a == '<' && b == '!' && !_hasDoctype)
				{
					_doctype.read(stream, result);
					if (!result)
					{
						return;
					}
					_hasDoctype = true;

					parsedSomething = true;
				}
				else
				{
					parsedSomething = false;
				}
			}

			// Body of the document

			_root.read(stream, result);

			// Additionnal processing instructions

			parsedSomething = true;
			while (parsedSomething || parseSpacesAndComments(stream))
			{
				stream >> a >> b;
				stream.putback(b);
				stream.putback(a);
				if (a == '<' && b == '?')
				{
					xml::ProcessingInstructions instruction;
					instruction.read(stream, result);
					if (!result)
					{
						return;
					}
					_processingInstructions.push_back(instruction);

					parsedSomething = true;
				}
				else
				{
					parsedSomething = false;
				}
			}

			// It should be end of file

			if (!stream.eof())
			{
				result.failedStep = IOResult::FailedStep::ParseFailed;
				result.errorMessage = "Expected end of XML file but found something else that is not a comment, spaces or processing instructions.";
				return;
			}
		}

		void XmlFile::write(std::ostream& stream, IOResult& result)
		{
			_declaration.write(stream, result);
			if (!result)
			{
				return;
			}

			if (_hasDoctype)
			{
				_doctype.write(stream, result);
				if (!result)
				{
					return;
				}
			}

			for (xml::ProcessingInstructions& instruction : _processingInstructions)
			{
				instruction.write(stream, result);
				if (!result)
				{
					return;
				}
			}

			_root.write(stream, result);
			if (!result)
			{
				return;
			}
		}
		*/
	}
}