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

	IOResult FormatHandler::readFromPath(const std::filesystem::path& path)
	{
		IOResult result;

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

	IOResult FormatHandler::writeToPath(const std::filesystem::path& path)
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
}
