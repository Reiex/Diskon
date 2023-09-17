///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Format/FormatTypes.hpp>

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

			struct Doctype
			{
				std::string name;
				// TODO
			};

			struct ProcessingInstruction
			{
				std::string target;
				std::string instruction;
			};

			struct Prolog
			{
				std::optional<Declaration> declaration;
				std::optional<Doctype> doctype;
				std::vector<ProcessingInstruction> instructions;
			};


			struct ElementTag
			{
				std::string name;
				std::vector<std::pair<std::string, std::string>> attributes;
				bool isEmpty;
			};

			struct Element
			{
				ElementTag tag;
				std::vector<ElementContent> contents;
			};

			namespace ContentType
			{
				enum Flags : uint8_t
				{
					EndOfElement			= 0,		///< Cannot be filtered away, exits the tag
					ChildTag				= 1 << 0,	///< Enters the child but do not exit it, so the next readContent is inside the child !
					Child					= 1 << 1,	///< On children, the filter does not apply. It applies only on the content directly read.
					CharData				= 1 << 2,	///< Any charData between two tags except comments
					ProcessingInstruction	= 1 << 3,
					// TODO: Add flag "NonEmptyCharData"

					Default					= 0b11111110
				};
			}

			struct ElementContent
			{
				ContentType::Flags type;

				std::unique_ptr<ElementTag> childTag;
				std::unique_ptr<Element> child;
				std::unique_ptr<std::string> charData;
				std::unique_ptr<ProcessingInstruction> instruction;
			};


			struct Ending
			{
				std::vector<ProcessingInstruction> instructions;
			};


			struct File
			{
				Prolog prolog;
				Element root;
				Ending ending;
			};
		}

		class DSK_API XmlIStream : public FormatIStream
		{
			public:

				XmlIStream(IStream* stream);
				XmlIStream(const XmlIStream& stream) = delete;
				XmlIStream(XmlIStream&& stream) = delete;

				XmlIStream& operator=(const XmlIStream& stream) = delete;
				XmlIStream& operator=(XmlIStream&& stream) = delete;

				void readFile(xml::File& file);
				void readProlog(xml::Prolog& prolog);
				void readElementTag(xml::ElementTag& tag);
				void readElementContent(xml::ElementContent& content, xml::ContentType::Flags filter = xml::ContentType::Default);
				void readEnding(xml::Ending& ending);

				~XmlIStream() = default;

			private:

				void resetFormatState() override final;

				void _readPIsSpacesAndComments(std::vector<xml::ProcessingInstruction>& instructions);
				void _readElement(xml::Element& element);

				void _readDeclaration(std::optional<xml::Declaration>& declaration);
				void _readDoctype(std::optional<xml::Doctype>& doctype);
				void _readProcessingInstruction(xml::ProcessingInstruction& instruction, bool& parsedSomething);

				void _readEq();
				void _readEncName(std::string& name);
				void _readName(std::string& name);
				void _readAttValue(std::string& value);

				void _readComment(bool& parsedSomething);

				std::vector<std::string> _tags;
				bool _prologRead;
				bool _treeRead;
		};

		class DSK_API XmlOStream : public FormatOStream
		{
			public:

				XmlOStream(OStream* stream);
				XmlOStream(const XmlOStream& stream) = delete;
				XmlOStream(XmlOStream&& stream) = delete;

				XmlOStream& operator=(const XmlOStream& stream) = delete;
				XmlOStream& operator=(XmlOStream&& stream) = delete;

				void writeFile(const xml::File& file);
				void writeProlog(const xml::Prolog& prolog);
				void writeElementTag(const xml::ElementTag& tag);
				void writeElementContent(const xml::ElementContent& content);
				void writeEnding(const xml::Ending& ending);

				~XmlOStream() = default;

			private:

				void resetFormatState() override final;

				void _writeDeclaration(const xml::Declaration& declaration);
				void _writeDoctype(const xml::Doctype& doctype);
				void _writeProcessingInstruction(const xml::ProcessingInstruction& instruction);
				void _writeElement(const xml::Element& element);

				void _writeAttribute(const std::pair<const std::string, std::string>& attribute);

				std::vector<std::string> _tags;
				bool _prologWritten;
				bool _treeWritten;
		};
	}
}
