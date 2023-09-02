///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \file
//! \author Reiex
//! \copyright The MIT License (MIT)
//! \date 2022-2023
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <Diskon/Core/CoreTypes.hpp>

namespace dsk
{
	template<typename TKey, typename TValue>
	class LookupMultitable
	{
		public:

			union Entry
			{
				Entry* next = nullptr;
				TValue* value;
			};

			constexpr LookupMultitable();
			constexpr LookupMultitable(const LookupMultitable<TKey, TValue>& table);
			constexpr LookupMultitable(LookupMultitable<TKey, TValue>&& table);

			constexpr LookupMultitable<TKey, TValue>& operator=(const LookupMultitable<TKey, TValue>& table);
			constexpr LookupMultitable<TKey, TValue>& operator=(LookupMultitable<TKey, TValue>&& table);

			constexpr LookupMultitableIterator<TKey, TValue> begin();
			constexpr LookupMultitableConstIterator<TKey, TValue> begin() const;
			constexpr LookupMultitableConstIterator<TKey, TValue> cbegin() const;
			constexpr const LookupMultitableIterator<TKey, TValue>& end();
			constexpr const LookupMultitableConstIterator<TKey, TValue>& end() const;
			constexpr const LookupMultitableConstIterator<TKey, TValue>& cend() const;

			constexpr bool empty() const;

			constexpr void clear();
			constexpr bool insertOrAssign(const TKey& key, const TValue& value);
			template<typename... TArgs> constexpr bool emplace(const TKey& key, TArgs&&... args);

			constexpr TValue& at(const TKey& key);
			constexpr const TValue& at(const TKey& key) const;
			constexpr TValue& operator[](const TKey& key);
			constexpr LookupMultitableIterator<TKey, TValue> find(const TKey& key);
			constexpr LookupMultitableConstIterator<TKey, TValue> find(const TKey& key) const;
			constexpr bool contains(const TKey& key) const;

			constexpr ~LookupMultitable();

		private:

			template<uint64_t Level = 1> static constexpr void _copyTable(const Entry* table, Entry* to);
			template<uint64_t Level = 1> static constexpr void _clearTable(Entry* table);

			static constexpr uint64_t _keySize = sizeof(TKey);

			Entry* _table;
			const Entry* _tableEnd;
			LookupMultitableIterator<TKey, TValue> _endIterator;
			LookupMultitableConstIterator<TKey, TValue> _endConstIterator;
	};

	template<typename TKey, typename TValue>
	class LookupMultitableConstIterator
	{
		public:

			using Entry = LookupMultitable<TKey, TValue>::Entry;

			// operator<> ? operator++(int) ? operator+- ? etc...

			constexpr LookupMultitableConstIterator(const Entry* table, bool end);
			constexpr LookupMultitableConstIterator(const Entry* table, const TKey& key);
			constexpr LookupMultitableConstIterator(const LookupMultitableConstIterator<TKey, TValue>& iterator) = default;
			constexpr LookupMultitableConstIterator(LookupMultitableConstIterator<TKey, TValue>&& iterator) = default;

			constexpr LookupMultitableConstIterator<TKey, TValue>& operator=(const LookupMultitableConstIterator<TKey, TValue>& iterator) = default;
			constexpr LookupMultitableConstIterator<TKey, TValue>& operator=(LookupMultitableConstIterator<TKey, TValue>&& iterator) = default;

			constexpr const std::pair<TKey, const TValue*>& operator*() const;
			constexpr const std::pair<TKey, const TValue*>* operator->() const;

			constexpr LookupMultitableConstIterator<TKey, TValue>& operator++();

			constexpr bool operator==(const LookupMultitableConstIterator<TKey, TValue>& iterator) const;
			constexpr bool operator!=(const LookupMultitableConstIterator<TKey, TValue>& iterator) const;

			constexpr ~LookupMultitableConstIterator() = default;

		private:

			static constexpr uint64_t _keySize = sizeof(TKey);

			std::pair<TKey, const TValue*> _value;
			const Entry* _stack[_keySize];
	};

	template<typename TKey, typename TValue>
	class LookupMultitableIterator : public LookupMultitableConstIterator<TKey, TValue>
	{
		public:

			using Entry = LookupMultitable<TKey, TValue>::Entry;

			constexpr LookupMultitableIterator(Entry* table, bool end);
			constexpr LookupMultitableIterator(Entry* table, const TKey& key);
			constexpr LookupMultitableIterator(const LookupMultitableIterator<TKey, TValue>& iterator) = default;
			constexpr LookupMultitableIterator(LookupMultitableIterator<TKey, TValue>&& iterator) = default;

			constexpr LookupMultitableIterator<TKey, TValue>& operator=(const LookupMultitableIterator<TKey, TValue>& iterator) = default;
			constexpr LookupMultitableIterator<TKey, TValue>& operator=(LookupMultitableIterator<TKey, TValue>&& iterator) = default;

			constexpr const std::pair<TKey, TValue*>& operator*() const;
			constexpr const std::pair<TKey, TValue*>* operator->() const;

			constexpr LookupMultitableIterator<TKey, TValue>& operator++();

			constexpr ~LookupMultitableIterator() = default;

		private:

			using Super = LookupMultitableConstIterator<TKey, TValue>;
	};
}
