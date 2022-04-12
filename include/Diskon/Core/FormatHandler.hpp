#pragma once

#include <Diskon/Core/types.hpp>

namespace dsk
{
	struct IOResult
	{
		IOResult();
		IOResult(const IOResult& result) = default;
		IOResult(IOResult&& result) = default;

		IOResult& operator=(const IOResult& result) = default;
		IOResult& operator=(IOResult&& result) = default;

		explicit operator bool() const;

		enum FailedStep
		{
			None,
			FileNotFound,
			FileOpenFailed,
			ParseFailed
		};

		std::string errorMessage;
		FailedStep failedStep;

		~IOResult() = default;
	};

	class FormatHandler
	{
		public:

			FormatHandler(const FormatHandler& handler) = delete;
			FormatHandler(FormatHandler&& handler) = delete;

			FormatHandler& operator=(const FormatHandler& handler) = delete;
			FormatHandler& operator=(FormatHandler&& handler) = delete;

			IOResult readFromPath(const std::filesystem::path& path);
			IOResult writeToPath(const std::filesystem::path& path);

			virtual void clear() = 0;

			virtual ~FormatHandler() = default;

		protected:

			FormatHandler() = default;

			virtual void read(std::istream& stream, IOResult& result) = 0;
			virtual void write(std::ostream& stream, IOResult& result) = 0;
	};
}
