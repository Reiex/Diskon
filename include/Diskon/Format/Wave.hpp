#pragma once

#include <Diskon/Format/types.hpp>

namespace dsk
{
	namespace fmt
	{
		class WaveFile : public FormatHandler
		{
			public:

				uint16_t channelCount;
				uint32_t sampleRate;
				uint16_t bitsPerSample;

				std::vector<std::vector<float>> samples;	// [-1, 1]

				void clear() override;

			private:

				void read(std::istream& stream, IOResult& result) override;
				void write(std::ostream& stream, IOResult& result) override;
		};
	}
}
