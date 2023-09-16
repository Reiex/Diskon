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

				const ruc::Status& readFile(xml::File& file);
				const ruc::Status& readProlog(xml::Prolog& prolog);
				const ruc::Status& readElementTag(xml::ElementTag& tag);
				const ruc::Status& readElementContent(xml::ElementContent& content, xml::ContentType::Flags filter = xml::ContentType::Default);
				const ruc::Status& readEnding(xml::Ending& ending);

				~XmlIStream() = default;

			private:

				void resetFormatState() override final;

				const ruc::Status& _readPIsSpacesAndComments(std::vector<xml::ProcessingInstruction>& instructions);
				const ruc::Status& _readElement(xml::Element& element);

				const ruc::Status& _readDeclaration(std::optional<xml::Declaration>& declaration);
				const ruc::Status& _readDoctype(std::optional<xml::Doctype>& doctype);
				const ruc::Status& _readProcessingInstruction(xml::ProcessingInstruction& instruction, bool& parsedSomething);

				const ruc::Status& _readEq();
				const ruc::Status& _readEncName(std::string& name);
				const ruc::Status& _readName(std::string& name);
				const ruc::Status& _readAttValue(std::string& value);

				const ruc::Status& _readComment(bool& parsedSomething);

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

				const ruc::Status& writeFile(const xml::File& file);
				const ruc::Status& writeProlog(const xml::Prolog& prolog);
				const ruc::Status& writeElementTag(const xml::ElementTag& tag);
				const ruc::Status& writeElementContent(const xml::ElementContent& content);
				const ruc::Status& writeEnding(const xml::Ending& ending);

				~XmlOStream() = default;

			private:

				void resetFormatState() override final;

				const ruc::Status& _writeDeclaration(const xml::Declaration& declaration);
				const ruc::Status& _writeDoctype(const xml::Doctype& doctype);
				const ruc::Status& _writeProcessingInstruction(const xml::ProcessingInstruction& instruction);
				const ruc::Status& _writeElement(const xml::Element& element);

				const ruc::Status& _writeAttribute(const std::pair<const std::string, std::string>& attribute);

				std::vector<std::string> _tags;
				bool _prologWritten;
				bool _treeWritten;
		};
	}
}
