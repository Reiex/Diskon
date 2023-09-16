///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Pélégrin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Diskon/Format/Format.hpp>


namespace dsk
{
	namespace fmt
	{
		PnmIStream::PnmIStream(IStream* stream) : FormatIStream(stream),
			_header(),
			_headerRead(false),
			_samplesPerPixel(0),
			_remainingSamples(0)
		{
			if (stream)
			{
				setStreamState();
			}
		}

		const ruc::Status& PnmIStream::readFile(pnm::File& file)
		{
			DSKFMT_BEGIN();

			DSKFMT_CALL(readHeader, file.header);

			file.samples.resize(_remainingSamples);
			DSKFMT_CALL(readPixels, file.samples.data(), _header.width * _header.height);

			return _stream->getStatus();
		}

		const ruc::Status& PnmIStream::readHeader(pnm::Header& header)
		{
			DSKFMT_BEGIN();

			assert(!_headerRead);

			char buffer;

			DSKFMT_CALL(_readHeaderChar, buffer);
			DSKFMT_CHECK(buffer == 'P', std::format("Expected magic number to start with 'P'. Instead, got '{}'.", buffer));
			DSKFMT_CALL(_readHeaderChar, buffer);
			DSKFMT_CHECK(buffer >= 49 && buffer < 55, std::format("Expected magic number to be between 'P1' and 'P6'. Instead, got 'P{}'.", buffer));

			_header.format = static_cast<pnm::Format>(buffer - 48);

			DSKFMT_CALL(_readHeaderSpaces);
			DSKFMT_CALL(_stream->readAsciiNumber, _header.width);
			DSKFMT_CALL(_readHeaderSpaces);
			DSKFMT_CALL(_stream->readAsciiNumber, _header.height);

			if (_header.format == pnm::Format::PlainPBM || _header.format == pnm::Format::RawPBM)
			{
				_header.maxSampleVal.reset();
			}
			else
			{
				uint64_t maxSampleVal;
				DSKFMT_CALL(_readHeaderSpaces);
				DSKFMT_CALL(_stream->readAsciiNumber, maxSampleVal);
				_header.maxSampleVal.emplace(maxSampleVal);
			}

			DSKFMT_CALL(_readHeaderSpaces);

			header = _header;
			_headerRead = true;
			_samplesPerPixel = (_header.format == pnm::Format::PlainPPM || _header.format == pnm::Format::RawPPM) ? 3 : 1;
			_remainingSamples = _header.width * _header.height * _samplesPerPixel;

			return _stream->getStatus();
		}

		const ruc::Status& PnmIStream::readPixels(uint16_t* samples, uint64_t pixelCount)
		{
			DSKFMT_BEGIN();

			assert(_headerRead);
			assert(_remainingSamples >= pixelCount * _samplesPerPixel);

			char buffer;
			uint64_t skipCount;

			const uint64_t constSampleCount = pixelCount * _samplesPerPixel;
			uint64_t sampleCount = constSampleCount;

			switch (_header.format)
			{
				case pnm::Format::PlainPBM:
				{
					for (; pixelCount; --pixelCount, ++samples)
					{
						DSKFMT_CALL(_stream->read, buffer);
						DSKFMT_CHECK(buffer == '0' || buffer == '1', std::format("Expected '0' or '1' as sample value for plain PBM but instead got '{}'.", std::to_string(buffer)));
						*samples = buffer - 48;
						DSKFMT_CALL(_stream->skipCharWhile, [](char x) { return x == ' ' || x == '\t' || x == '\r' || x == '\n'; }, skipCount);
					}

					break;
				}
				case pnm::Format::PlainPGM:
				case pnm::Format::PlainPPM:
				{
					for (; sampleCount != 1; --sampleCount, ++samples)
					{
						DSKFMT_CALL(_stream->readAsciiNumber, *samples);
						DSKFMT_CHECK(*samples <= _header.maxSampleVal.value(), "Parsed pixel component value superior to maximum value specified.");
						DSKFMT_CALL(_stream->skipCharWhile, [](char x) { return x == ' ' || x == '\t' || x == '\r' || x == '\n'; }, skipCount);
						DSKFMT_CHECK(skipCount, "Expected space(s) after not-last pixel in plain PPM/PGM.");
					}

					DSKFMT_CALL(_stream->readAsciiNumber, *samples);
					DSKFMT_CHECK(*samples <= _header.maxSampleVal.value(), "Parsed pixel component value superior to maximum value specified.");
					DSKFMT_CALL(_stream->skipCharWhile, [](char x) { return x == ' ' || x == '\t' || x == '\r' || x == '\n'; }, skipCount);

					break;
				}
				case pnm::Format::RawPBM:
				{
					constexpr uint8_t filter[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };
					const uint64_t scanlineSize = (_header.width >> 3) + ((_header.width & 7) != 0);
					const uint8_t initialBitOffset = ((_header.width - 1) & 7);

					for (uint64_t i = 0; i < _header.height; ++i)
					{
						uint8_t* bitSamples = reinterpret_cast<uint8_t*>(samples);
						DSKFMT_CALL(_stream->read, bitSamples, scanlineSize);

						uint8_t bitOffset = initialBitOffset;
						bitSamples += scanlineSize - 1;

						for (uint16_t* it = samples + _header.width - 1; it != samples; --it)
						{
							*it = ((*bitSamples & filter[bitOffset]) != 0);
							bitSamples -= (bitOffset == 0);
							bitOffset = ((bitOffset + 7) & 7);
						}
						*samples >>= 15;

						samples += _header.width;
					}

					break;
				}
				case pnm::Format::RawPGM:
				case pnm::Format::RawPPM:
				{
					// TODO: DSKFMT_CHECK on values (<= maxSampleValue)

					if (_header.maxSampleVal.value() < 256)
					{
						uint8_t* it = reinterpret_cast<uint8_t*>(samples) + sampleCount;
						DSKFMT_CALL(_stream->read, it, sampleCount);
						std::copy(it, it + sampleCount, samples);
					}
					else
					{
						DSKFMT_CALL(_stream->read, samples, sampleCount);
					}

					break;
				}
				default:
				{
					assert(false);
					break;
				}
			}

			_remainingSamples -= constSampleCount;
			if (_remainingSamples == 0)
			{
				_headerRead = false;
				_samplesPerPixel = 0;
			}

			return _stream->getStatus();
		}

		void PnmIStream::setStreamState()
		{
			_stream->setByteEndianness(std::endian::big);
			_stream->setBitEndianness(std::endian::big);
		}

		void PnmIStream::resetFormatState()
		{
			_headerRead = false;
			_samplesPerPixel = 0;
			_remainingSamples = 0;
		}

		const ruc::Status& PnmIStream::_readHeaderChar(char& value)
		{
			DSKFMT_BEGIN();

			uint64_t skipCount;
			bool inComment = false;
			DSKFMT_CALL(_stream->skipCharWhile, [&inComment](char x) { return (inComment = (x == '#' || (inComment && x != '\n'))); }, skipCount);
			DSKFMT_CALL(_stream->read, value);

			return _stream->getStatus();
		}

		const ruc::Status& PnmIStream::_readHeaderSpaces()
		{
			DSKFMT_BEGIN();

			char buffer;
			DSKFMT_CALL(_readHeaderChar, buffer);
			DSKFMT_CHECK(buffer == ' ' || buffer == '\t' || buffer == '\r' || buffer == '\n', std::format("Expected space. Instead, got '{}'.", buffer));
			while (buffer == ' ' || buffer == '\t' || buffer == '\r' || buffer == '\n')
			{
				DSKFMT_CALL(_readHeaderChar, buffer);
			}
			_stream->unread(1);

			return _stream->getStatus();
		}


		PnmOStream::PnmOStream(OStream* stream) : FormatOStream(stream),
			_header(),
			_headerWritten(false),
			_samplesPerPixel(0),
			_remainingSamples(0)
		{
			if (stream)
			{
				setStreamState();
			}
		}

		const ruc::Status& PnmOStream::writeFile(const pnm::File& file)
		{
			DSKFMT_BEGIN();

			DSKFMT_CALL(writeHeader, file.header);

			assert(file.samples.size() == _remainingSamples);
			DSKFMT_CALL(writePixels, file.samples.data(), file.header.width * file.header.height);

			return _stream->getStatus();
		}

		const ruc::Status& PnmOStream::writeHeader(const pnm::Header& header)
		{
			DSKFMT_BEGIN();

			assert(!_headerWritten);
			assert(header.width != 0);
			assert(header.height != 0);

			_header = header;

			DSKFMT_CALL(_stream->write, 'P');
			DSKFMT_CALL(_stream->writeAsciiNumber, static_cast<uint8_t>(_header.format));
			DSKFMT_CALL(_stream->write, '\n');

			DSKFMT_CALL(_stream->writeAsciiNumber, _header.width);
			DSKFMT_CALL(_stream->write, ' ');
			DSKFMT_CALL(_stream->writeAsciiNumber, _header.height);

			if (_header.format == pnm::Format::PlainPBM || _header.format == pnm::Format::RawPBM)
			{
				assert(!header.maxSampleVal.has_value());
			}
			else
			{
				assert(header.maxSampleVal.has_value());

				DSKFMT_CALL(_stream->write, '\n');
				DSKFMT_CALL(_stream->writeAsciiNumber, _header.maxSampleVal.value());
			}

			if (_header.format != pnm::Format::PlainPGM && _header.format != pnm::Format::PlainPPM)
			{
				DSKFMT_CALL(_stream->write, '\n');
			}

			_headerWritten = true;
			_samplesPerPixel = (_header.format == pnm::Format::PlainPPM || _header.format == pnm::Format::RawPPM) ? 3 : 1;
			_remainingSamples = _header.width * _header.height * _samplesPerPixel;

			return _stream->getStatus();
		}

		const ruc::Status& PnmOStream::writePixels(const uint16_t* samples, uint64_t pixelCount)
		{
			DSKFMT_BEGIN();

			assert(_headerWritten);
			assert(pixelCount * _samplesPerPixel <= _remainingSamples);

			const uint64_t constSampleCount = pixelCount * _samplesPerPixel;
			uint64_t sampleCount = constSampleCount;

			switch (_header.format)
			{
				case pnm::Format::PlainPBM:
				{
					for (; pixelCount; --pixelCount, ++samples)
					{
						assert(*samples == 0 || *samples == 1);
						const uint8_t buffer = *samples + 48;
						DSKFMT_CALL(_stream->write, buffer);
					}

					break;
				}
				case pnm::Format::PlainPGM:
				case pnm::Format::PlainPPM:
				{
					for (; sampleCount; --sampleCount, ++samples)
					{
						assert(*samples <= _header.maxSampleVal.value());
						DSKFMT_CALL(_stream->write, '\n');
						DSKFMT_CALL(_stream->writeAsciiNumber, *samples);
					}

					break;
				}
				case pnm::Format::RawPBM:
				{
					for (uint64_t i = 0; i < _header.height; ++i)
					{
						for (uint64_t j = 0; j < _header.width; ++j, ++samples)
						{
							assert(*samples == 0 || *samples == 1);
							DSKFMT_CALL(_stream->bitWrite, *samples);
						}

						DSKFMT_CALL(_stream->finishByte);
					}

					break;
				}
				case pnm::Format::RawPGM:
				case pnm::Format::RawPPM:
				{
					// TODO: Asserts on sample value (<= maxSampleValue)

					if (_header.maxSampleVal.value() < 256)
					{
						uint8_t buffer[_singleBufferSize];
						while (sampleCount > _singleBufferSize)
						{
							std::copy_n(samples, _singleBufferSize, buffer);
							DSKFMT_CALL(_stream->write, buffer, _singleBufferSize);

							samples += _singleBufferSize;
							sampleCount -= _singleBufferSize;
						}

						std::copy(samples, samples + sampleCount, buffer);
						DSKFMT_CALL(_stream->write, buffer, sampleCount);
					}
					else
					{
						DSKFMT_CALL(_stream->write, samples, sampleCount);
					}

					break;
				}
			}

			_remainingSamples -= constSampleCount;
			if (_remainingSamples == 0)
			{
				_headerWritten = false;
				_samplesPerPixel = 0;
			}

			return _stream->getStatus();
		}

		void PnmOStream::setStreamState()
		{
			_stream->setByteEndianness(std::endian::big);
			_stream->setBitEndianness(std::endian::big);
		}

		void PnmOStream::resetFormatState()
		{
			_headerWritten = false;
			_samplesPerPixel = 0;
			_remainingSamples = 0;
		}
	}
}
