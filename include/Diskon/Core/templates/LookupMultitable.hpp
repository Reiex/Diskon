///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Reiex
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Diskon/Core/CoreDecl.hpp>

namespace dsk
{
	template<typename TKey, typename TValue>
	constexpr LookupMultitable<TKey, TValue>::LookupMultitable() :
		_table(new Entry[256]),
		_tableEnd(_table + 256),
		_endIterator(_table, true),
		_endConstIterator(_table, true)
	{
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitable<TKey, TValue>::LookupMultitable(const LookupMultitable<TKey, TValue>& table) : LookupMultitable<TKey, TValue>()
	{
		assert(table._table);
		_copyTable(table._table, _table);
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitable<TKey, TValue>::LookupMultitable(LookupMultitable<TKey, TValue>&& table) :
		_table(table._table),
		_tableEnd(table._tableEnd),
		_endIterator(_table, true),
		_endConstIterator(_table, true)
	{
		assert(table._table);
		table._table = nullptr;
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitable<TKey, TValue>& LookupMultitable<TKey, TValue>::operator=(const LookupMultitable<TKey, TValue>& table)
	{
		assert(_table);
		assert(table._table);

		_clearTable(_table);
		_copyTable(table._table, _table);

		return *this;
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitable<TKey, TValue>& LookupMultitable<TKey, TValue>::operator=(LookupMultitable<TKey, TValue>&& table)
	{
		assert(_table);
		assert(table._table);

		_clearTable(_table);
		delete[] _table;

		_table = table._table;
		_tableEnd = table._tableEnd;

		table._table = nullptr;

		return *this;
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitableIterator<TKey, TValue> LookupMultitable<TKey, TValue>::begin()
	{
		assert(_table);
		return LookupMultitableIterator<TKey, TValue>(_table, false);
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitableConstIterator<TKey, TValue> LookupMultitable<TKey, TValue>::begin() const
	{
		assert(_table);
		return LookupMultitableConstIterator<TKey, TValue>(_table, false);
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitableConstIterator<TKey, TValue> LookupMultitable<TKey, TValue>::cbegin() const
	{
		assert(_table);
		return LookupMultitableConstIterator<TKey, TValue>(_table, false);
	}

	template<typename TKey, typename TValue>
	constexpr const LookupMultitableIterator<TKey, TValue>& LookupMultitable<TKey, TValue>::end()
	{
		assert(_table);
		return _endIterator;
	}

	template<typename TKey, typename TValue>
	constexpr const LookupMultitableConstIterator<TKey, TValue>& LookupMultitable<TKey, TValue>::end() const
	{
		assert(_table);
		return _endConstIterator;
	}

	template<typename TKey, typename TValue>
	constexpr const LookupMultitableConstIterator<TKey, TValue>& LookupMultitable<TKey, TValue>::cend() const
	{
		assert(_table);
		return _endConstIterator;
	}

	template<typename TKey, typename TValue>
	constexpr bool LookupMultitable<TKey, TValue>::empty() const
	{
		assert(_table);

		Entry* table = _table;
		for (; table != _tableEnd; ++table)
		{
			if (table->next)
			{
				return false;
			}
		}

		return true;
	}

	template<typename TKey, typename TValue>
	constexpr void LookupMultitable<TKey, TValue>::clear()
	{
		assert(_table);
		_clearTable(_table);
	}

	template<typename TKey, typename TValue>
	constexpr bool LookupMultitable<TKey, TValue>::insertOrAssign(const TKey& key, const TValue& value)
	{
		assert(_table);

		Entry* entry = _table;
		const uint8_t* address = reinterpret_cast<const uint8_t*>(&key);

		for (uint64_t i = 1; i < _keySize; ++i, ++address)
		{
			entry += *address;
			if (!entry->next)
			{
				entry->next = new Entry[256];
			}
			entry = entry->next;
		}

		entry += *address;
		if (!entry->value)
		{
			entry->value = new TValue(value);
			return true;
		}
		else
		{
			*(entry->value) = value;
			return false;
		}
	}

	template<typename TKey, typename TValue>
	template<typename... TArgs>
	constexpr bool LookupMultitable<TKey, TValue>::emplace(const TKey& key, TArgs&&... args)
	{
		assert(_table);

		Entry* entry = _table;
		const uint8_t* address = reinterpret_cast<const uint8_t*>(&key);

		for (uint64_t i = 1; i < _keySize; ++i, ++address)
		{
			entry += *address;
			if (!entry->next)
			{
				entry->next = new Entry[256];
			}
			entry = entry->next;
		}

		entry += *address;
		if (!entry->value)
		{
			entry->value = new TValue(std::forward<TArgs>(args)...);
			return true;
		}

		return false;
	}

	template<typename TKey, typename TValue>
	constexpr const TValue& LookupMultitable<TKey, TValue>::at(const TKey& key) const
	{
		assert(_table);

		Entry* entry = _table;
		const uint8_t* address = reinterpret_cast<const uint8_t*>(&key);

		for (uint64_t i = 1; i < _keySize; ++i, ++address)
		{
			entry += *address;
			if (!entry->next)
			{
				throw std::out_of_range("LookupMultitable bad key.");
			}
			entry = entry->next;
		}

		entry += *address;
		if (!entry->value)
		{
			throw std::out_of_range("LookupMultitable bad key.");
		}

		return *(entry->value);
	}

	template<typename TKey, typename TValue>
	constexpr TValue& LookupMultitable<TKey, TValue>::at(const TKey& key)
	{
		const LookupMultitable<TKey, TValue>* constThis = this;
		return const_cast<TValue&>(constThis->at(key));
	}

	template<typename TKey, typename TValue>
	constexpr TValue& LookupMultitable<TKey, TValue>::operator[](const TKey& key)
	{
		assert(_table);

		Entry* entry = _table;
		const uint8_t* address = reinterpret_cast<const uint8_t*>(&key);

		for (uint64_t i = 1; i < _keySize; ++i, ++address)
		{
			entry += *address;
			if (!entry->next)
			{
				entry->next = new Entry[256];
			}
			entry = entry->next;
		}

		entry += *address;
		if (!entry->value)
		{
			entry->value = new TValue;
		}

		return *(entry->value);
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitableIterator<TKey, TValue> LookupMultitable<TKey, TValue>::find(const TKey& key)
	{
		assert(_table);
		return LookupMultitableIterator<TKey, TValue>(_table, key);
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitableConstIterator<TKey, TValue> LookupMultitable<TKey, TValue>::find(const TKey& key) const
	{
		assert(_table);
		return LookupMultitableConstIterator<TKey, TValue>(_table, key);
	}

	template<typename TKey, typename TValue>
	constexpr bool LookupMultitable<TKey, TValue>::contains(const TKey& key) const
	{
		assert(_table);

		Entry* entry = _table;
		const uint8_t* address = reinterpret_cast<const uint8_t*>(&key);

		for (uint64_t i = 1; i < _keySize; ++i, ++address)
		{
			entry += *address;
			if (!entry->next)
			{
				return false;
			}
			entry = entry->next;
		}

		entry += *address;
		return entry->value;
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitable<TKey, TValue>::~LookupMultitable()
	{
		if (_table)
		{
			_clearTable(_table);
			delete[] _table;
		}
	}

	template<typename TKey, typename TValue>
	template<uint64_t Level>
	constexpr void LookupMultitable<TKey, TValue>::_copyTable(const Entry* table, Entry* to)
	{
		const Entry* const tableEnd = table + 256;
		for (; table != tableEnd; ++table)
		{
			if constexpr (Level == _keySize)
			{
				if (table->value)
				{
					to->value = new TValue(table->value);
				}
			}
			else
			{
				if (table->next)
				{
					to->next = new Entry[256];
					_copyTable<Level + 1>(table->next, to->next);
				}
			}
		}
	}

	template<typename TKey, typename TValue>
	template<uint64_t Level>
	constexpr void LookupMultitable<TKey, TValue>::_clearTable(Entry* table)
	{
		const Entry* const tableEnd = table + 256;
		for (; table != tableEnd; ++table)
		{
			if constexpr (Level == _keySize)
			{
				if (table->value)
				{
					delete table->value;
				}
			}
			else
			{
				if (table->next)
				{
					_clearTable<Level + 1>(table->next);
					delete[](table->next);
				}
			}

			table->next = nullptr;
		}
	}


	template<typename TKey, typename TValue>
	constexpr LookupMultitableConstIterator<TKey, TValue>::LookupMultitableConstIterator(const Entry* table, bool end)
	{
		if (end)
		{
			_value.second = nullptr;
		}
		else
		{
			const Entry* entry = table;
			const Entry* const tableEnd = table + 256;
			uint8_t* address = reinterpret_cast<uint8_t*>(&_value.first);

			// Go through the whole stack except the last table (because we must not search for next but value)

			for (uint64_t i = 1; i < _keySize; ++i, ++address)
			{
				for (uint16_t j = 0; j < 256; ++j, ++entry)
				{
					if (entry->next)
					{
						*address = j;
						_stack[i - 1] = entry;
						entry = entry->next;
						break;
					}
				}

				if (entry == tableEnd)
				{
					break;
				}
			}

			// Go through the last table if necessary (entry == tableEnd means the table is empty)

			if (entry == tableEnd)
			{
				_value.second = nullptr;
			}
			else
			{
				for (uint16_t i = 0; i < 256; ++i, ++entry)
				{
					if (entry->value)
					{
						_stack[_keySize - 1] = entry;
						*address = i;
						_value.second = entry->value;
						break;
					}
				}
			}
		}
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitableConstIterator<TKey, TValue>::LookupMultitableConstIterator(const Entry* table, const TKey& key)
	{
		_value.first = key;

		const Entry* entry = table;
		const uint8_t* address = reinterpret_cast<const uint8_t*>(&key);

		for (uint64_t i = 1; i < _keySize; ++i, ++address)
		{
			entry += *address;
			_stack[i - 1] = entry;
			if (!entry->next)
			{
				_value.second = nullptr;
				return;
			}
			entry = entry->next;
		}

		entry += *address;
		_stack[_keySize - 1] = entry;
		if (!entry->value)
		{
			_value.second = nullptr;
			return;
		}

		_value.second = entry->value;
	}

	template<typename TKey, typename TValue>
	constexpr const std::pair<TKey, const TValue*>& LookupMultitableConstIterator<TKey, TValue>::operator*() const
	{
		assert(_value.second);
		return _value;
	}

	template<typename TKey, typename TValue>
	constexpr const std::pair<TKey, const TValue*>* LookupMultitableConstIterator<TKey, TValue>::operator->() const
	{
		assert(_value.second);
		return &_value;
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitableConstIterator<TKey, TValue>& LookupMultitableConstIterator<TKey, TValue>::operator++()
	{
		assert(_value.second);

		uint64_t level = _keySize - 1;
		uint8_t* address = reinterpret_cast<uint8_t*>(&_value.first) + level;
		
		do {
			while (*address == 255)
			{
				if (level == 0)
				{
					_value.second = nullptr;
					return *this;
				}
				else
				{
					--address;
					--level;
				}
			}

			++(*address);
			++_stack[level];
		
		} while (!_stack[level]->next);


		for (++level, ++address; level < _keySize; ++level, ++address)
		{
			_stack[level] = _stack[level - 1]->next;
			*address = 0;

			while (!_stack[level]->next)
			{
				++(*address);
				++_stack[level];
			}
		}

		_value.second = _stack[_keySize - 1]->value;

		return *this;
	}

	template<typename TKey, typename TValue>
	constexpr bool LookupMultitableConstIterator<TKey, TValue>::operator==(const LookupMultitableConstIterator<TKey, TValue>& iterator) const
	{
		return _value.second == iterator._value.second;
	}

	template<typename TKey, typename TValue>
	constexpr bool LookupMultitableConstIterator<TKey, TValue>::operator!=(const LookupMultitableConstIterator<TKey, TValue>& iterator) const
	{
		return _value.second != iterator._value.second;
	}


	template<typename TKey, typename TValue>
	constexpr LookupMultitableIterator<TKey, TValue>::LookupMultitableIterator(Entry* table, bool end) : Super(table, end)
	{
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitableIterator<TKey, TValue>::LookupMultitableIterator(Entry* table, const TKey& key) : Super(table, key)
	{
	}

	template<typename TKey, typename TValue>
	constexpr const std::pair<TKey, TValue*>& LookupMultitableIterator<TKey, TValue>::operator*() const
	{
		return reinterpret_cast<const std::pair<TKey, TValue*>&>(Super::operator*());
	}

	template<typename TKey, typename TValue>
	constexpr const std::pair<TKey, TValue*>* LookupMultitableIterator<TKey, TValue>::operator->() const
	{
		return reinterpret_cast<const std::pair<TKey, TValue*>*>(Super::operator->());
	}

	template<typename TKey, typename TValue>
	constexpr LookupMultitableIterator<TKey, TValue>& LookupMultitableIterator<TKey, TValue>::operator++()
	{
		Super::operator++();
		return *this;
	}
}
