///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Reiex
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Format/FormatDecl.hpp>

namespace dsk
{
	namespace fmt
	{
		namespace riff
		{
			constexpr uint32_t Chunk::computeInnerSize() const
			{
				assert((type == ChunkType::StandardChunk && subChunks.empty()) || (type != ChunkType::StandardChunk && data.empty()));

				if (type == ChunkType::StandardChunk)
				{
					return data.size();
				}
				else
				{
					uint32_t size = 4;
					for (const riff::Chunk& subChunk : subChunks)
					{
						size += subChunk.computeTotalSize();
					}
					return size;
				}
			}

			constexpr uint32_t Chunk::computeTotalSize() const
			{
				uint32_t totalSize = computeInnerSize() + 8;
				return totalSize + (totalSize & 1);
			}
		}

		template<typename TValue>
		const ruc::Status& RiffIStream::readChunkData(TValue& value)
		{
			DSKFMT_BEGIN();

			assert(_readingData);

			DSKFMT_CHECK(_remainingSizes.back()[1] >= sizeof(TValue), "Tried to read more data than contained in chunk size.");

			DSKFMT_CALL(_stream->read, value);

			_remainingSizes.back()[1] -= sizeof(TValue);
			if (_remainingSizes.back()[1] == 0)
			{
				DSKFMT_CALL(_readChunkEnd);
			}

			return _stream->getStatus();
		}

		template<typename TValue>
		const ruc::Status& RiffIStream::readChunkData(TValue* values, uint32_t count)
		{
			DSKFMT_BEGIN();

			assert(_readingData);

			const uint32_t size = count * sizeof(TValue);
			DSKFMT_CHECK(_remainingSizes.back()[1] >= size, "Tried to read more data than contained in chunk size.");

			DSKFMT_CALL(_stream->read, values, count);

			_remainingSizes.back()[1] -= size;
			if (_remainingSizes.back()[1] == 0)
			{
				DSKFMT_CALL(_readChunkEnd);
			}

			return _stream->getStatus();
		}

		template<typename TValue>
		const ruc::Status& RiffOStream::writeChunkData(const TValue& value)
		{
			DSKFMT_BEGIN();

			assert(_writingData);
			assert(_remainingSizes.back()[1] >= sizeof(TValue));

			DSKFMT_CALL(_stream->write, value);

			_remainingSizes.back()[1] -= sizeof(TValue);
			if (_remainingSizes.back()[1] == 0)
			{
				DSKFMT_CALL(_writeChunkEnd);
			}

			return _stream->getStatus();
		}

		template<typename TValue>
		const ruc::Status& RiffOStream::writeChunkData(const TValue* values, uint32_t count)
		{
			DSKFMT_BEGIN();

			assert(_writingData);

			const uint32_t size = count * sizeof(TValue);
			assert(_remainingSizes.back()[1] >= size);

			DSKFMT_CALL(_stream->write, values, count);

			_remainingSizes.back()[1] -= size;
			if (_remainingSizes.back()[1] == 0)
			{
				DSKFMT_CALL(_writeChunkEnd);
			}

			return _stream->getStatus();
		}
	}
}
