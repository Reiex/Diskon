///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author P�l�grin Marius
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Diskon/Format/Format.hpp>

namespace dsk
{
	namespace fmt
	{
		RiffIStream::RiffIStream(IStream* stream) : FormatIStream(stream),
			_remainingSizes(),
			_readingData(false)
		{
			if (stream)
			{
				setStreamState();
			}
		}

		void RiffIStream::readFile(riff::File& file)
		{
			assert(_remainingSizes.empty());
			_readChunk(file);
		}

		void RiffIStream::readFileHeader(riff::FileHeader& header)
		{
			assert(_remainingSizes.empty());
			_readChunkHeader(header);
		}

		void RiffIStream::readChunk(riff::Chunk& chunk)
		{
			assert(!_remainingSizes.empty());
			_readChunk(chunk);
		}

		void RiffIStream::readChunkHeader(riff::ChunkHeader& header)
		{
			assert(!_remainingSizes.empty());
			_readChunkHeader(header);
		}

		void RiffIStream::skipChunkData(uint32_t size)
		{
			DSKFMT_BEGIN();

			assert(_readingData);
			assert(size <= _remainingSizes.back()[1]);

			DSKFMT_STREAM_CALL(skip, size);
			_remainingSizes.back()[1] -= size;

			if (_remainingSizes.back()[1] == 0)
			{
				DSK_CALL(_readChunkEnd);
			}
		}

		void RiffIStream::finishCurrentChunk()
		{
			DSKFMT_BEGIN();

			assert(_readingData);

			DSKFMT_STREAM_CALL(skip, _remainingSizes.back()[1]);
			_remainingSizes.back()[1] = 0;
			DSK_CALL(_readChunkEnd);
		}

		uint32_t RiffIStream::computeRemainingSize() const
		{
			if (_remainingSizes.empty())
			{
				return 0;
			}
			else
			{
				uint32_t size = _remainingSizes.front()[0];
				for (const std::array<uint32_t, 2>& remainingSize : _remainingSizes)
				{
					size += remainingSize[1] - remainingSize[0];
				}
				return size;
			}
		}

		void RiffIStream::setStreamState()
		{
			_stream->setByteEndianness(std::endian::little);
		}

		void RiffIStream::resetFormatState()
		{
			_remainingSizes.clear();
			_readingData = false;
		}

		void RiffIStream::_readChunk(riff::Chunk& chunk)
		{
			DSKFMT_BEGIN();

			const uint32_t depth = _remainingSizes.size();

			riff::ChunkHeader header;
			DSK_CALL(_readChunkHeader, header);

			chunk.type = header.type;
			std::copy_n(header.id, 4, chunk.id);
			chunk.subChunks.clear();
			chunk.data.clear();

			if (_remainingSizes.size() > depth)
			{
				switch (chunk.type)
				{
					case riff::ChunkType::StandardChunk:
					{
						chunk.data.resize(header.size);
						DSK_CALL(readChunkData, chunk.data.data(), header.size);

						break;
					}
					case riff::ChunkType::RiffChunk:
					case riff::ChunkType::ListChunk:
					{
						while (_remainingSizes.size() > depth)
						{
							chunk.subChunks.emplace_back();
							DSK_CALL(_readChunk, chunk.subChunks.back());
						}

						break;
					}
				}
			}
		}

		void RiffIStream::_readChunkHeader(riff::ChunkHeader& header)
		{
			DSKFMT_BEGIN();

			assert(!_readingData);

			// Read the real chunk header (id and size)

			DSKFMT_STREAM_CALL(read, header.id, 4);
			DSKFMT_STREAM_CALL(read, header.size);

			// Check chunk type, read additional information if special chunk type.

			uint32_t headerSize;
			bool chunkEmpty;
			switch (fourcc(header.id))
			{
				case fourcc("RIFF"):
				{
					DSK_CHECK(_remainingSizes.empty(), "Found 'RIFF' chunk not at root of file.");
					DSK_CHECK(header.size >= 4, std::format("Expected 'RIFF' chunk size to be more than or equal to 4. Instead, got size of {}.", header.size));

					header.type = riff::ChunkType::RiffChunk;
					DSKFMT_STREAM_CALL(read, header.id, 4);

					headerSize = 12;
					chunkEmpty = (header.size == 4);

					break;
				}
				case fourcc("LIST"):
				{
					DSK_CHECK(!_remainingSizes.empty(), "Found 'LIST' chunk at root of file.");
					DSK_CHECK(header.size >= 4, std::format("Expected 'LIST' chunk size to be more than or equal to 4. Instead, got size of {}.", header.size));

					header.type = riff::ChunkType::ListChunk;
					DSKFMT_STREAM_CALL(read, header.id, 4);

					headerSize = 12;
					chunkEmpty = (header.size == 4);

					break;
				}
				default:
				{
					DSK_CHECK(!_remainingSizes.empty(), std::format("Found '{}' chunk at root of file.", std::string_view(header.id, 4)));
					header.type = riff::ChunkType::StandardChunk;

					headerSize = 8;
					chunkEmpty = (header.size == 0);
					_readingData = !chunkEmpty;

					break;
				}
			}

			// If the chunk is not empty, add size to stack (minus header size because it's already read)

			if (!chunkEmpty)
			{
				DSK_CHECK(_remainingSizes.empty() || _remainingSizes.back()[1] >= header.size + 8 + (header.size & 1), "Invalid chunk size found.");
				_remainingSizes.push_back({ header.size + 8, header.size + 8 - headerSize });
			}

			// If the chuck is empty, just substract header size to top of the stack

			else if (!_remainingSizes.empty())
			{
				DSK_CHECK(_remainingSizes.back()[1] >= headerSize, "Invalid chunk size found.");
				_remainingSizes.back()[1] -= headerSize;

				if (_remainingSizes.back()[1] == 0)
				{
					DSK_CALL(_readChunkEnd);
				}
			}
		}

		void RiffIStream::_readChunkEnd()
		{
			DSKFMT_BEGIN();

			assert(_readingData);
			assert(_remainingSizes.back()[1] == 0);

			_readingData = false;
			while (!_remainingSizes.empty() && _remainingSizes.back()[1] == 0)
			{
				if (_remainingSizes.back()[0] & 1)
				{
					uint8_t padByte;
					DSKFMT_STREAM_CALL(read, padByte);
					DSK_CHECK(padByte == 0, std::format("Pad byte expected to be 0. Instead, got '{}'.", padByte));
					++_remainingSizes.back()[0];
				}

				if (_remainingSizes.size() > 1)
				{
					_remainingSizes[_remainingSizes.size() - 2][1] -= _remainingSizes.back()[0];
				}

				_remainingSizes.pop_back();
			}
		}


		RiffOStream::RiffOStream(OStream* stream) : FormatOStream(stream),
			_remainingSizes(),
			_writingData(false)
		{
			if (stream)
			{
				setStreamState();
			}
		}

		void RiffOStream::writeFile(const riff::File& file)
		{
			assert(_remainingSizes.empty());
			_writeChunk(file);
		}

		void RiffOStream::writeFileHeader(const riff::FileHeader& header)
		{
			assert(_remainingSizes.empty());
			_writeChunkHeader(header);
		}

		void RiffOStream::writeChunk(const riff::Chunk& chunk)
		{
			assert(!_remainingSizes.empty());
			_writeChunk(chunk);
		}

		void RiffOStream::writeChunkHeader(const riff::ChunkHeader& header)
		{
			assert(!_remainingSizes.empty());
			_writeChunkHeader(header);
		}

		uint32_t RiffOStream::computeRemainingSize() const
		{
			if (_remainingSizes.empty())
			{
				return 0;
			}
			else
			{
				uint32_t size = _remainingSizes.front()[0];
				for (const std::array<uint32_t, 2>&remainingSize : _remainingSizes)
				{
					size += remainingSize[1] - remainingSize[0];
				}
				return size;
			}
		}

		void RiffOStream::setStreamState()
		{
			_stream->setByteEndianness(std::endian::little);
		}

		void RiffOStream::resetFormatState()
		{
			_remainingSizes.clear();
			_writingData = false;
		}

		void RiffOStream::_writeChunk(const riff::Chunk& chunk)
		{
			DSKFMT_BEGIN();

			riff::ChunkHeader header;
			header.type = chunk.type;
			std::copy_n(chunk.id, 4, header.id);
			header.size = chunk.computeInnerSize();

			DSK_CALL(_writeChunkHeader, header);

			switch (chunk.type)
			{
				case riff::ChunkType::StandardChunk:
				{
					DSK_CALL(writeChunkData, chunk.data.data(), chunk.data.size());

					break;
				}
				case riff::ChunkType::RiffChunk:
				case riff::ChunkType::ListChunk:
				{
					for (const riff::Chunk& subChunk : chunk.subChunks)
					{
						DSK_CALL(_writeChunk, subChunk);
					}

					break;
				}
			}
		}

		void RiffOStream::_writeChunkHeader(const riff::ChunkHeader& header)
		{
			DSKFMT_BEGIN();

			assert(!_writingData);

			uint32_t headerSize;
			bool chunkEmpty;
			switch (header.type)
			{
				case riff::ChunkType::StandardChunk:
				{
					assert(!_remainingSizes.empty());

					DSKFMT_STREAM_CALL(write, header.id, 4);
					DSKFMT_STREAM_CALL(write, header.size);

					headerSize = 8;
					chunkEmpty = (header.size == 0);
					_writingData = !chunkEmpty;

					break;
				}
				case riff::ChunkType::RiffChunk:
				{
					assert(_remainingSizes.empty());
					assert(header.size >= 4);

					DSKFMT_STREAM_CALL(write, "RIFF", 4);
					DSKFMT_STREAM_CALL(write, header.size);
					DSKFMT_STREAM_CALL(write, header.id, 4);

					headerSize = 12;
					chunkEmpty = (header.size == 4);

					break;
				}
				case riff::ChunkType::ListChunk:
				{
					assert(!_remainingSizes.empty());
					assert(header.size >= 4);

					DSKFMT_STREAM_CALL(write, "LIST", 4);
					DSKFMT_STREAM_CALL(write, header.size);
					DSKFMT_STREAM_CALL(write, header.id, 4);

					headerSize = 12;
					chunkEmpty = (header.size == 4);

					break;
				}
			}

			// If the chunk is not empty, add size to stack (minus header size because it's already written)

			if (!chunkEmpty)
			{
				assert(_remainingSizes.empty() || _remainingSizes.back()[1] >= header.size + 8 + (header.size & 1));
				_remainingSizes.push_back({ header.size + 8, header.size + 8 - headerSize });
			}

			// If the chuck is empty, just substract header size to top of the stack

			else if (!_remainingSizes.empty())
			{
				assert(_remainingSizes.back()[1] >= headerSize);

				_remainingSizes.back()[1] -= headerSize;
				if (_remainingSizes.back()[1] == 0)
				{
					DSK_CALL(_writeChunkEnd);
				}
			}
		}

		void RiffOStream::_writeChunkEnd()
		{
			DSKFMT_BEGIN();

			assert(_writingData);
			assert(_remainingSizes.back()[1] == 0);

			_writingData = false;
			while (!_remainingSizes.empty() && _remainingSizes.back()[1] == 0)
			{
				if (_remainingSizes.back()[0] & 1)
				{
					constexpr uint8_t padByte = 0;
					DSKFMT_STREAM_CALL(write, padByte);
					++_remainingSizes.back()[0];
				}

				if (_remainingSizes.size() > 1)
				{
					_remainingSizes[_remainingSizes.size() - 2][1] -= _remainingSizes.back()[0];
				}

				_remainingSizes.pop_back();
			}
		}
	}
}
