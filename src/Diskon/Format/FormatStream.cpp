#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		FormatError::FormatError() :
			errorCode(ErrorCode::None),
			errorMessage()
		{
		}

		FormatError::operator bool() const
		{
			return (errorCode == ErrorCode::None);
		}

		void FormatError::clear()
		{
			errorCode = ErrorCode::None;
			errorMessage.clear();
		}


		FormatStream::FormatStream(std::endian endianness) :
			_error(),
			_endianness(endianness),
			_srcStream(nullptr),
			_srcStreamOwned(true),
			_dstStream(nullptr),
			_dstStreamOwned(true)
		{
		}

		const FormatError& FormatStream::setSource(const std::filesystem::path& path)
		{
			_error.clear();

			if (_srcStream && _srcStreamOwned)
			{
				delete _srcStream;
			}
			_srcStream = nullptr;
			_srcStreamOwned = true;

			if (!std::filesystem::exists(path))
			{
				_error.errorCode = FormatError::ErrorCode::FileNotFound;
				_error.errorMessage = "FormatStream: Could not find '" + path.string() + "'.";
				return _error;
			}

			_srcStream = new std::ifstream(path, std::ios::in | std::ios::binary);

			if (!(*_srcStream))
			{
				_error.errorCode = FormatError::ErrorCode::FileOpenFailed;
				_error.errorMessage = "FormatStream: Could not open '" + path.string() + "' for source.";
				return _error;
			}

			return _error;
		}

		const FormatError& FormatStream::setSource(std::istream& stream)
		{
			_error.clear();

			if (_srcStream && _srcStreamOwned)
			{
				delete _srcStream;
			}

			_srcStream = &stream;
			_srcStreamOwned = false;

			if (!(*_srcStream))
			{
				_error.errorCode = FormatError::ErrorCode::InvalidStream;
				_error.errorMessage = "FormatStream: Invalid stream set as source.";
				return _error;
			}

			return _error;
		}

		bool FormatStream::hasValidSource() const
		{
			return _srcStream && *_srcStream;
		}

		std::streampos FormatStream::getSourcePos() const
		{
			if (!hasValidSource())
			{
				return std::streampos(-1);
			}

			return _srcStream->tellg();
		}

		const FormatError& FormatStream::setSourcePos(std::streampos pos)
		{
			_error.clear();

			if (!_srcStream)
			{
				_error.errorCode = FormatError::ErrorCode::NoStream;
				_error.errorMessage = "FormatStream: Try to set source pos without source set.";
				return _error;
			}

			if (!(*_srcStream))
			{
				_error.errorCode = FormatError::ErrorCode::InvalidStream;
				_error.errorMessage = "FormatStream: Try to set source pos with source invalid.";
				return _error;
			}

			_srcStream->seekg(pos);

			return _error;
		}

		const FormatError& FormatStream::removeSource()
		{
			_error.clear();

			if (!_srcStream)
			{
				_error.errorCode = FormatError::ErrorCode::NoStream;
				_error.errorMessage = "FormatStream: Try to remove inexistant source.";
				return _error;
			}

			if (_srcStream && _srcStreamOwned)
			{
				delete _srcStream;
			}

			_srcStream = nullptr;
			_srcStreamOwned = true;

			return _error;
		}

		const FormatError& FormatStream::setDestination(const std::filesystem::path& path)
		{
			_error.clear();

			if (_dstStream && _dstStreamOwned)
			{
				delete _dstStream;
			}

			_dstStream = new std::ofstream(path, std::ios::out | std::ios::binary);
			_dstStreamOwned = true;

			if (!(*_dstStream))
			{
				_error.errorCode = FormatError::ErrorCode::FileOpenFailed;
				_error.errorMessage = "FormatStream: Could not open '" + path.string() + "' for destination.";
				return _error;
			}

			return _error;
		}

		const FormatError& FormatStream::setDestination(std::ostream& stream)
		{
			_error.clear();

			if (_dstStream && _dstStreamOwned)
			{
				delete _dstStream;
			}

			_dstStream = &stream;
			_dstStreamOwned = false;

			if (!(*_dstStream))
			{
				_error.errorCode = FormatError::ErrorCode::InvalidStream;
				_error.errorMessage = "FormatStream: Invalid stream set as destination.";
				return _error;
			}

			return _error;
		}

		bool FormatStream::hasValidDestination() const
		{
			return _dstStream && *_dstStream;
		}

		std::streampos FormatStream::getDestinationPos() const
		{
			if (!hasValidDestination())
			{
				return std::streampos(-1);
			}

			return _dstStream->tellp();
		}

		const FormatError& FormatStream::setDestinationPos(std::streampos pos)
		{
			_error.clear();

			if (!_dstStream)
			{
				_error.errorCode = FormatError::ErrorCode::NoStream;
				_error.errorMessage = "FormatStream: Try to set source pos without source set.";
				return _error;
			}

			if (!(*_dstStream))
			{
				_error.errorCode = FormatError::ErrorCode::InvalidStream;
				_error.errorMessage = "FormatStream: Try to set source pos with source invalid.";
				return _error;
			}

			_dstStream->seekp(pos);

			return _error;
		}

		const FormatError& FormatStream::removeDestination()
		{
			_error.clear();

			if (!_dstStream)
			{
				_error.errorCode = FormatError::ErrorCode::NoStream;
				_error.errorMessage = "FormatStream: Try to remove inexistant destination.";
				return _error;
			}

			if (_dstStream && _dstStreamOwned)
			{
				delete _dstStream;
			}

			_dstStream = nullptr;
			_dstStreamOwned = true;

			return _error;
		}

		const FormatError& FormatStream::getLastError() const
		{
			return _error;
		}

		FormatStream::~FormatStream()
		{
			removeSource();
			removeDestination();
		}
	}
}
