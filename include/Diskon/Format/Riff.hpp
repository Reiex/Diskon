#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace riff
		{
			struct Chunk
			{
				std::array<char, 4> id;
				std::vector<char> data;
			};
		}

		class RiffFile : public FormatHandler
		{
			public:

				std::array<char, 4> formType;
				std::vector<riff::Chunk> chunks;

				void clear() override;

			private:

				void read(std::istream& stream, IOResult& result) override;
				void write(std::ostream& stream, IOResult& result) override;

				void readChunk(std::istream& stream, IOResult& result, riff::Chunk& chunk, unsigned long& remainingSize);
				void writeChunk(std::ostream& stream, IOResult& result, const riff::Chunk& chunk);
		};
	}
}
