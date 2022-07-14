#pragma once

#include <Diskon/Core/types.hpp>

namespace dsk
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

		virtual ~FormatError() = default;

		enum class ErrorCode
		{
			// Generic

			None,
			FileNotFound,
			FileOpenFailed,
			InvalidStream,
			NoStream,
			ParseFailed,

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
			WaveInvalidHeader

			// Xml
		};

		ErrorCode errorCode;
		std::string errorMessage;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//! - The position of the cursor on source and destination is the same before and after the call of any read/write
	//! function (but obvisouly move during the call)
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

			FormatStream();

			std::istream* _srcStream;
			bool _srcStreamOwned;
			std::ostream* _dstStream;
			bool _dstStreamOwned;

			FormatError _error;
	};
}
