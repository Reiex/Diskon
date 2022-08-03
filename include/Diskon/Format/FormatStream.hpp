#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace fmt
	{
		struct FormatError
		{
			FormatError();
			FormatError(const FormatError& error) = default;
			FormatError(FormatError&& error) = default;

			FormatError& operator=(const FormatError& error) = default;
			FormatError& operator=(FormatError&& error) = default;

			explicit operator bool() const;

			void clear();

			~FormatError() = default;

			enum class ErrorCode
			{
				// Generic

				None,
				FileNotFound,
				FileOpenFailed,
				InvalidStream,
				NoStream,
				ParseFailed,

				// Png

				PngInvalidSignature,
				PngInvalidChunkOrdering,
				PngInvalidChunkType,
				PngInvalidChunkSize,
				PngInvalidChunkContent,
				PngInvalidChunkCRC,

				// Riff

				RiffIdentifierNotFound,
				RiffInvalidFileSize,
				RiffInvalidPadByte,

				// Wave

				WaveBadFormType,
				WaveNoFmtChunk,
				WaveBadFmtChunk,
				WaveBadFactChunk,
				WaveNoDataChunk,
				WaveInvalidHeader,

				// Xml

				XmlInvalidComment,
				XmlInvalidDeclaration,
				XmlInvalidDoctype,
				XmlInvalidProcessingInstruction,
				XmlInvalidElement,

				// Zlib

				ZlibInvalidCompressionMethod,
				ZlibInvalidCompressionInfo,
				ZlibInvalidCheck,
				ZlibDeflateInvalidCompression,
			};

			ErrorCode errorCode;
			std::string errorMessage;
		};

		/*
		
		TODO:
		- Un stream peut sauvegarder des infos pour la lecture/écriture
		- Un stream est "reset" quand on change le stream soujacent (et uniquement à ce moment là, pas de reset de l'exterieur)
		- On n'a plus accès a rien dans le stream soujacent (même setPos/getPos) tout doit passer par des fonctions spécifiques...
		
		*/

		class FormatStream
		{
			public:

				FormatStream(const FormatStream& stream) = delete;
				FormatStream(FormatStream&& stream) = delete;

				FormatStream& operator=(const FormatStream& stream) = delete;
				FormatStream& operator=(FormatStream&& stream) = delete;

				const FormatError& setSource(const std::filesystem::path& path);
				const FormatError& setSource(std::istream& stream);
				bool hasValidSource() const;
				std::streampos getSourcePos() const;
				const FormatError& setSourcePos(std::streampos pos);
				const FormatError& removeSource();

				const FormatError& setDestination(const std::filesystem::path& path);
				const FormatError& setDestination(std::ostream& stream);
				bool hasValidDestination() const;
				std::streampos getDestinationPos() const;
				const FormatError& setDestinationPos(std::streampos pos);
				const FormatError& removeDestination();

				const FormatError& getLastError() const;

				virtual ~FormatStream();

			protected:

				FormatStream(std::endian endianness);

				template<typename TValue> const FormatError& streamRead(TValue& value);
				template<typename TValue> const FormatError& streamRead(TValue* values, uint64_t count);

				template<typename TValue> const FormatError& streamWrite(TValue value);
				template<typename TValue> const FormatError& streamWrite(const TValue* values, uint64_t count);

				FormatError _error;
				std::istream* _srcStream;
				std::ostream* _dstStream;
				bool _srcStreamOwned;
				bool _dstStreamOwned;
				std::endian _endianness;
		};
	}
}

#include <Diskon/Format/templates/FormatStream.hpp>
