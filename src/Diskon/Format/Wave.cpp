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
		WaveIStream::WaveIStream(IStream* stream) : FormatIStream(stream),
			_headerRead(false),
			_header(),
			_remainingBlocks(0),
			_riffStream(new RiffIStream(stream))
		{
			_subStreams.push_back(_riffStream);
		}
		
		const ruc::Status& WaveIStream::readHeader(wave::Header& header)
		{
			DSKFMT_BEGIN();
		
			assert(!_headerRead);
			assert(_remainingBlocks == 0);
		
			riff::ChunkHeader chunkHeader;
		
			// Read file header
		
			DSKFMT_CALL(_riffStream->readFileHeader, chunkHeader);
			DSKFMT_CHECK(fourcc(chunkHeader.id) == fourcc("WAVE"), std::format("Expected RIFF form-type to be 'WAVE' but instead got '{}'.", std::string_view(chunkHeader.id, 4)));
		
			// Find and read format chunk
		
			DSKFMT_CALL(_riffStream->readChunkHeader, chunkHeader);
			while (fourcc(chunkHeader.id) != fourcc("fmt "))
			{
				DSKFMT_CALL(_riffStream->finishCurrentChunk);
				DSKFMT_CALL(_riffStream->readChunkHeader, chunkHeader);
			}
		
			DSKFMT_CALL(_readFormatChunk, chunkHeader);
			_header.blockCount = 0;
		
			// Read/Skip all chunks until data chunk
		
			DSKFMT_CALL(_riffStream->readChunkHeader, chunkHeader);
			while (fourcc(chunkHeader.id) != fourcc("data"))
			{
				switch (fourcc(chunkHeader.id))
				{
					case fourcc("fact"):
					{
						DSKFMT_CALL(_readFactChunk, chunkHeader);
						break;
					}
					default:	// TODO: Other chunk types
					{
						DSKFMT_CALL(_riffStream->finishCurrentChunk);
						break;
					}
				}
		
				DSKFMT_CALL(_riffStream->readChunkHeader, chunkHeader);
			}
		
			// Parse data chunk header
		
			const uint32_t blockCount = chunkHeader.size / ((_header.metadata.bitsPerSample >> 3) * _header.metadata.channelCount);
			if (_header.blockCount != 0)
			{
				switch (_header.metadata.format)
				{
					case wave::Format::PCM:
					case wave::Format::Float:
					{
						DSKFMT_CHECK(_header.blockCount == blockCount, std::format("The block count given in the 'fact' chunk ({}) differs from the one computed from data chunk: {}.", header.blockCount, blockCount));
						break;
					}
					default:
					{
						break;
					}
				}
			}
			else
			{
				_header.blockCount = blockCount;
			}
		
			DSKFMT_CHECK(_header.blockCount != 0, "Block count cannot be 0.");
		
			// Assign header if necessary and prepare data reading
		
			if (&header != &_header)
			{
				header = _header;
			}
		
			_headerRead = true;
			_remainingBlocks = _header.blockCount;
		
			return _stream->getStatus();
		}
		
		const ruc::Status& WaveIStream::skipBlocks(uint32_t blockCount)
		{
			DSKFMT_BEGIN();
		
			assert(_headerRead);
			assert(_remainingBlocks >= blockCount);

			DSKFMT_CALL(_riffStream->skipChunkData, blockCount * _header.metadata.channelCount * (_header.metadata.bitsPerSample >> 3));
			_remainingBlocks -= blockCount;
		
			return _stream->getStatus();
		}
		
		const ruc::Status& WaveIStream::readEndFile()
		{
			DSKFMT_BEGIN();
		
			assert(_headerRead);
			assert(_remainingBlocks == 0);
		
			riff::ChunkHeader chunkHeader;
			while (_riffStream->computeRemainingSize())
			{
				DSKFMT_CALL(_riffStream->readChunkHeader, chunkHeader);
				DSKFMT_CALL(_riffStream->finishCurrentChunk);
			}
		
			_headerRead = false;
		
			return _stream->getStatus();
		}
		
		void WaveIStream::setStreamState()
		{
		}

		void WaveIStream::resetFormatState()
		{
			_headerRead = false;
			_remainingBlocks = 0;
		}

		WaveIStream::~WaveIStream()
		{
			delete _riffStream;
		}

		const ruc::Status& WaveIStream::_readFormatChunk(const riff::ChunkHeader& chunkHeader)
		{
			DSKFMT_BEGIN();
			
			DSKFMT_CHECK(chunkHeader.type == riff::ChunkType::StandardChunk, "Format chunk should be a standard chunk. Instead, got a chunk list.");
			DSKFMT_CHECK(chunkHeader.size >= 14, std::format("Expected format chunk size to be equal or greater than 14. Instead, got {}.", chunkHeader.size));
		
			uint16_t formatTag;
			DSKFMT_CALL(_riffStream->readChunkData, formatTag);
			_header.metadata.format = static_cast<wave::Format>(formatTag);
		
			DSKFMT_CALL(_riffStream->readChunkData, _header.metadata.channelCount);
			DSKFMT_CALL(_riffStream->readChunkData, _header.metadata.frequency);
		
			uint32_t avgBytesPerSec;
			DSKFMT_CALL(_riffStream->readChunkData, avgBytesPerSec);
		
			uint16_t blockAlign;
			DSKFMT_CALL(_riffStream->readChunkData, blockAlign);
		
			uint16_t extensionSize;
			switch (_header.metadata.format)
			{
				case wave::Format::PCM:
				{
					DSKFMT_CHECK(chunkHeader.size == 16 || chunkHeader.size == 18, std::format("Expected PCM format chunk size to be 16 or 18. Instead, got {}.", chunkHeader.size));
		
					DSKFMT_CALL(_riffStream->readChunkData, _header.metadata.bitsPerSample);
		
					if (chunkHeader.size == 18)
					{
						DSKFMT_CALL(_riffStream->readChunkData, extensionSize);
						DSKFMT_CHECK(extensionSize == 0, std::format("Expected PCM format chunk extension size to be 0. Instead, got {}.", extensionSize));
					}
		
					_header.metadata.bitsPerSample = (_header.metadata.bitsPerSample + 7) & 0xFFF8;
					DSKFMT_CHECK(_header.metadata.bitsPerSample > 0 && _header.metadata.bitsPerSample <= 32, std::format("Expected PCM bitsPerSample in [0, 32]. Instead, got {}.", _header.metadata.bitsPerSample));
		
					const uint32_t expectedAvgBytesPerSec = _header.metadata.channelCount * _header.metadata.frequency * _header.metadata.bitsPerSample >> 3;
					DSKFMT_CHECK(avgBytesPerSec == expectedAvgBytesPerSec, std::format("Expected PCM avgBytesPerSec={}. Instead, got {}.", expectedAvgBytesPerSec, avgBytesPerSec));
		
					const uint16_t expectedBlockAlign = _header.metadata.channelCount * _header.metadata.bitsPerSample >> 3;
					DSKFMT_CHECK(blockAlign == expectedBlockAlign, std::format("Expected PCM blockAlign={}. Instead, got {}.", expectedBlockAlign, blockAlign));
		
					break;
				}
				case wave::Format::Float:
				{
					DSKFMT_CHECK(chunkHeader.size == 16 || chunkHeader.size == 18, std::format("Expected FLOAT format chunk size to be 16 or 18. Instead, got {}.", chunkHeader.size));
					
					DSKFMT_CALL(_riffStream->readChunkData, _header.metadata.bitsPerSample);
		
					if (chunkHeader.size == 18)
					{
						DSKFMT_CALL(_riffStream->readChunkData, extensionSize);
						DSKFMT_CHECK(extensionSize == 0, std::format("Expected FLOAT format chunk extension size to be 0. Instead, got {}.", extensionSize));
					}
		
					DSKFMT_CHECK(_header.metadata.bitsPerSample == 32 || _header.metadata.bitsPerSample == 64, std::format("Expected FLOAT bitsPerSample in {{32, 64}}. Instead, got {}.", _header.metadata.bitsPerSample));
		
					const uint32_t expectedAvgBytesPerSec = _header.metadata.channelCount * _header.metadata.frequency * _header.metadata.bitsPerSample >> 3;
					DSKFMT_CHECK(avgBytesPerSec == expectedAvgBytesPerSec, std::format("Expected FLOAT avgBytesPerSec={}. Instead, got {}.", expectedAvgBytesPerSec, avgBytesPerSec));
		
					const uint16_t expectedBlockAlign = _header.metadata.channelCount * _header.metadata.bitsPerSample >> 3;
					DSKFMT_CHECK(blockAlign == expectedBlockAlign, std::format("Expected FLOAT blockAlign={}. Instead, got {}.", expectedBlockAlign, blockAlign));
		
					break;
				}
				default:
				{
					DSKFMT_CHECK(false, std::format("Unknown format : {}.", static_cast<uint16_t>(_header.metadata.format)));
				}
			}
		
			return _stream->getStatus();
		}
		
		const ruc::Status& WaveIStream::_readFactChunk(const riff::ChunkHeader& chunkHeader)
		{
			DSKFMT_BEGIN();
		
			DSKFMT_CHECK(chunkHeader.type == riff::ChunkType::StandardChunk, "Fact chunk should be a standard chunk. Instead, got a chunk list.");
			DSKFMT_CHECK(chunkHeader.size == 4, std::format("Expected fact chunk size to be 4. Instead, got {}.", chunkHeader.size));
		
			DSKFMT_CALL(_riffStream->readChunkData, _header.blockCount);
			DSKFMT_CHECK(_header.blockCount != 0, "Sample length cannot be 0.");
		
			return _stream->getStatus();
		}
		
		
		WaveOStream::WaveOStream(OStream* stream) : FormatOStream(stream),
			_headerWritten(false),
			_header(),
			_remainingBlocks(0),
			_riffStream(new RiffOStream(stream))
		{
			_subStreams.push_back(_riffStream);
		}
		
		const ruc::Status& WaveOStream::writeHeader(const wave::Header& header)
		{
			DSKFMT_BEGIN();
			
			assert(!_headerWritten);
			assert(_remainingBlocks == 0);
		
			assert(header.metadata.bitsPerSample != 0);
			assert(header.metadata.channelCount != 0);
			assert(header.metadata.frequency != 0);
			assert(header.blockCount != 0);
		
			riff::ChunkHeader chunkHeader;
		
			_header = header;
		
			// Compute chunks size
		
			constexpr uint32_t formatChunkSize = 8 + 18;
			constexpr uint32_t factChunkSize = 8 + 4;
			uint32_t dataChunkSize = 8 + header.blockCount * header.metadata.channelCount * ((_header.metadata.bitsPerSample + 7) >> 3);
			dataChunkSize += dataChunkSize & 1;
		
			// Write riff file header
		
			chunkHeader.type = riff::ChunkType::RiffChunk;
			std::copy_n("WAVE", 4, chunkHeader.id);
			chunkHeader.size = 4 + formatChunkSize + factChunkSize + dataChunkSize;
		
			DSKFMT_CALL(_riffStream->writeFileHeader, chunkHeader);
		
			// Write format chunk
		
			chunkHeader.type = riff::ChunkType::StandardChunk;
			std::copy_n("fmt ", 4, chunkHeader.id);
			chunkHeader.size = formatChunkSize - 8;
		
			DSKFMT_CALL(_riffStream->writeChunkHeader, chunkHeader);
		
			DSKFMT_CALL(_riffStream->writeChunkData, static_cast<uint16_t>(_header.metadata.format));
			DSKFMT_CALL(_riffStream->writeChunkData, _header.metadata.channelCount);
			DSKFMT_CALL(_riffStream->writeChunkData, _header.metadata.frequency);
		
			switch (header.metadata.format)
			{
				case wave::Format::PCM:
				{
					assert(_header.metadata.bitsPerSample <= 32);
		
					_header.metadata.bitsPerSample = (_header.metadata.bitsPerSample + 7) & 0xFFF8;
		
					const uint32_t avgBytesPerSec = _header.metadata.channelCount * _header.metadata.frequency * _header.metadata.bitsPerSample >> 3;
					DSKFMT_CALL(_riffStream->writeChunkData, avgBytesPerSec);
		
					const uint16_t blockAlign = _header.metadata.channelCount * _header.metadata.bitsPerSample >> 3;
					DSKFMT_CALL(_riffStream->writeChunkData, blockAlign);
		
					DSKFMT_CALL(_riffStream->writeChunkData, _header.metadata.bitsPerSample);
		
					constexpr uint16_t extensionSize = 0;
					DSKFMT_CALL(_riffStream->writeChunkData, extensionSize);
		
					break;
				}
				case wave::Format::Float:
				{
					assert(_header.metadata.bitsPerSample == 32 || _header.metadata.bitsPerSample == 64);
		
					const uint32_t avgBytesPerSec = _header.metadata.channelCount * _header.metadata.frequency * _header.metadata.bitsPerSample >> 3;
					DSKFMT_CALL(_riffStream->writeChunkData, avgBytesPerSec);
		
					const uint16_t blockAlign = _header.metadata.channelCount * _header.metadata.bitsPerSample >> 3;
					DSKFMT_CALL(_riffStream->writeChunkData, blockAlign);
		
					DSKFMT_CALL(_riffStream->writeChunkData, _header.metadata.bitsPerSample);
		
					constexpr uint16_t extensionSize = 0;
					DSKFMT_CALL(_riffStream->writeChunkData, extensionSize);
		
					break;
				}
				default:
				{
					assert(false);
				}
			}
		
			// Write fact chunk
		
			chunkHeader.type = riff::ChunkType::StandardChunk;
			std::copy_n("fact", 4, chunkHeader.id);
			chunkHeader.size = factChunkSize - 8;
		
			DSKFMT_CALL(_riffStream->writeChunkHeader, chunkHeader);
			DSKFMT_CALL(_riffStream->writeChunkData, _header.blockCount);
		
			// Write data chunk header
		
			chunkHeader.type = riff::ChunkType::StandardChunk;
			std::copy_n("data", 4, chunkHeader.id);
			chunkHeader.size = dataChunkSize - 8;
		
			DSKFMT_CALL(_riffStream->writeChunkHeader, chunkHeader);
		
			_headerWritten = true;
			_remainingBlocks = _header.blockCount;
		
			return _stream->getStatus();
		}
		
		void WaveOStream::setStreamState()
		{
		}

		void WaveOStream::resetFormatState()
		{
			_headerWritten = false;
			_remainingBlocks = 0;
		}

		WaveOStream::~WaveOStream()
		{
			delete _riffStream;
		}
	}
}
