#include <Diskon/Core/Core.hpp>

namespace dsk
{
	IOResult::IOResult() :
		errorMessage(),
		failedStep(FailedStep::None)
	{
	}

	IOResult::operator bool() const
	{
		return (failedStep == FailedStep::None);
	}

	IOResult FormatHandler::readFromFile(const std::filesystem::path& path)
	{
		IOResult result;

		clear();

		if (!std::filesystem::exists(path))
		{
			result.failedStep = IOResult::FailedStep::FileNotFound;
			return result;
		}

		std::ifstream file(path, std::ios::in | std::ios::binary);

		if (file.fail())
		{
			result.failedStep = IOResult::FailedStep::FileOpenFailed;
			return result;
		}

		read(file, result);

		return result;
	}

	IOResult FormatHandler::readFromStream(std::istream& stream)
	{
		assert(stream);

		clear();

		IOResult result;
		read(stream, result);
		return result;
	}

	IOResult FormatHandler::readFromString(const std::string& string)
	{
		IOResult result;

		clear();

		std::istringstream stream(string, std::ios::in | std::ios::binary);
		read(stream, result);

		return result;
	}

	IOResult FormatHandler::writeToFile(const std::filesystem::path& path)
	{
		IOResult result;

		std::ofstream file(path, std::ios::out | std::ios::binary);

		if (file.fail())
		{
			result.failedStep = IOResult::FailedStep::FileOpenFailed;
			return result;
		}

		write(file, result);

		return result;
	}

	IOResult FormatHandler::writeToStream(std::ostream& stream)
	{
		assert(stream);

		IOResult result;
		write(stream, result);
		return result;
	}

	IOResult FormatHandler::writeToString(std::string& string)
	{
		IOResult result;

		std::ostringstream stream;
		write(stream, result);

		if (!result)
		{
			return result;
		}

		string = stream.str();

		return result;
	}
}
