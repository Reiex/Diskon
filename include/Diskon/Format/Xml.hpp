#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace xml
		{
			struct Declaration
			{
				uint16_t versionMajor;
				uint16_t versionMinor;
				std::optional<std::string> encoding;
				std::optional<bool> standalone;
			};

			struct DocType
			{
				std::string name;
				// TODO
			};

			struct ProcessingInstruction
			{
				std::string target;
				std::string instruction;
			};

			struct ElementData
			{
				std::string tag;
				std::unordered_map<std::string, std::string> attributes;
				std::vector<ProcessingInstruction> processingInstructions;
				// TODO: CDSect
				std::string content;
			};

			struct Element
			{
				ElementData data;
				std::vector<std::streampos> childs;
				std::streampos endOfElement;	// TODO: Find a way to remove this
			};

			struct ElementTree
			{
				ElementData data;
				std::vector<ElementTree> childs;
				std::streampos endOfElement;	// TODO: Find a way to remove this
			};

			struct Prolog
			{
				std::optional<Declaration> declaration;
				std::optional<DocType> doctype;
				std::vector<ProcessingInstruction> processingInstructions;
			};

			struct File
			{
				Prolog prolog;
				ElementTree rootTree;
			};
		}

		class XmlStream : public FormatStream
		{
			public:

				XmlStream();
				XmlStream(const XmlStream& stream) = default;
				XmlStream(XmlStream&& stream) = default;

				XmlStream& operator=(const XmlStream& stream) = default;
				XmlStream& operator=(XmlStream&& stream) = default;

				const FormatError& readFile(xml::File& file);
				const FormatError& readProlog(xml::Prolog& prolog);
				const FormatError& readElementTree(xml::ElementTree& elementTree);	//! Reads the whole tree, place the cursor on the next start of element (after element tree)
				const FormatError& readElement(xml::Element& element);				//! Reads everything except the childs, place the cursor on the next start of element (including inside element tree)

				const FormatError& writeFile(const xml::File& file);
				const FormatError& writeProlog(const xml::Prolog& prolog);
				const FormatError& writeDeclaration(const xml::Declaration& declaration);
				const FormatError& writeDoctype(const xml::DocType& doctype);
				const FormatError& writeProcessingInstruction(const xml::ProcessingInstruction& instruction);
				const FormatError& writeElementTree(const xml::ElementTree& elementTree);	//! Cursor right after tree
				const FormatError& writeElementData(const xml::ElementData& elementData);	//! Cursor after content (and PIs, etc...)
				const FormatError& writeAscend(uint32_t count);

				~XmlStream() = default;

			private:

				const FormatError& readSpaces(bool& parsedSomething);
				const FormatError& readComment(bool& parsedSomething);
				
				const FormatError& readName(std::string& name);
				const FormatError& readSTag(xml::ElementData& elementData, bool& emptyElement);
				const FormatError& readETag(const std::string& tag);

				const FormatError& readDeclaration(xml::Declaration& declaration, bool& parsedSomething);
				const FormatError& readDoctype(xml::DocType& doctype, bool& parsedSomething);
				const FormatError& readProcessingInstruction(xml::ProcessingInstruction& instruction, bool& parsedSomething);
				const FormatError& readPIsSpacesAndComments(std::vector<xml::ProcessingInstruction>& instructions);
				const FormatError& readRawElement(void* elementPtr, bool tree);

				std::stack<std::string> _tagStack;
		};
	}
}
