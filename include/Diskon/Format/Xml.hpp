#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace fmt
	{
		/*namespace xml
		{
			struct XmlDeclaration
			{
				uint16_t versionMajor;
				uint16_t versionMinor;
				std::string encoding;
				bool standalone;

				void read(std::istream& stream, IOResult& result);
				void write(std::ostream& stream, IOResult& result);
			};

			struct ProcessingInstructions
			{
				std::string target;
				std::string instruction;

				void read(std::istream& stream, IOResult& result);
				void write(std::ostream& stream, IOResult& result);
			};

			struct DocumentType
			{
				std::string name;
				// TODO

				void read(std::istream& stream, IOResult& result);
				void write(std::ostream& stream, IOResult& result);
			};

			struct XmlElement
			{
				std::string tag;
				std::unordered_map<std::string, std::string> attributes;
				std::vector<XmlElement> childs;
				std::string content;

				void read(std::istream& stream, IOResult& result);
				void write(std::ostream& stream, IOResult& result);
			};
		}

		class XmlFile : public FormatHandler
		{
			public:

				XmlFile();
				XmlFile(const XmlFile& file) = default;
				XmlFile(XmlFile&& file) = default;

				XmlFile& operator=(const XmlFile& file) = default;
				XmlFile& operator=(XmlFile&& file) = default;
				
				void clear() override;

				~XmlFile() = default;

			private:

				void read(std::istream& stream, IOResult& result) override;
				void write(std::ostream& stream, IOResult& result) override;

				bool _hasDeclaration;
				xml::XmlDeclaration _declaration;
				bool _hasDoctype;
				xml::DocumentType _doctype;
				std::vector<xml::ProcessingInstructions> _processingInstructions;
				xml::XmlElement _root;
		};*/
	}
}
