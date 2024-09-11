#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>
#include <typeindex>
#include <unordered_map>
#include <vector>

struct TypeData
{
	std::vector<uint8_t*> memory;
	std::atomic_bool is_locked = false;
};

template<class T>
class TypeFunnel
{
public:
	TypeFunnel(TypeData &data) :
		m_data(data)
	{
		m_data.is_locked = true;
	}

	~TypeFunnel()
	{
		m_data.is_locked = false;
	}

	size_t size() const
	{
		return m_data.memory.size();
	}

	T* next()
	{
		if (m_offset > m_data.memory.size())
		{
			return nullptr;
		}

		auto *ptr = m_data.memory[m_offset++];

		return (T*)ptr;
	}

private:
	size_t m_offset = 0;
	TypeData &m_data;
};

class TypeBox
{
public:

	~TypeBox()
	{
		clear_all();
	}

	template<class T>
	void put(T &&data)
	{
		auto &values = get_or_emplace(typeid(T));

		values.emplace_back((uint8_t*)new T(std::forward<T>(data)));
	}

	template<class T, class ...Args>
	void emplace(Args &&...args)
	{
		auto &values = get_or_emplace(typeid(T));

		values.emplace_back((uint8_t*)new T(std::forward<Args>(args)...));
	}

	template<class T>
	std::optional<TypeFunnel<T>> fetch()
	{
		auto iter = m_type_container.find(typeid(T));

		if (iter == m_type_container.end())
		{
			return std::nullopt;
		}

		if (iter->second.is_locked)
		{
			return std::nullopt;
		}

		return iter->second;
	}

	template<class T>
	void clear()
	{
		clear_implementation(typeid(T));
	}

	void clear_all()
	{
		for (auto &[index, type_data] : m_type_container)
		{
			for (auto &data : type_data.memory)
			{
				delete data;
			}
		}

		m_type_container.clear();
	}
private:
	// NOTE: this is a horrible way to store data but for this example its fine
	std::unordered_map<std::type_index, TypeData> m_type_container;

	std::vector<uint8_t*>& get_or_emplace(std::type_index type)
	{
		auto iter = m_type_container.find(type);

		if (iter == m_type_container.end())
		{
			iter = m_type_container.emplace(type, std::vector<uint8_t*>()).first;
		}

		return iter->second.memory;
	}

	void clear_implementation(std::type_index index)
	{
		auto iter = m_type_container.find(index);

		if (iter == m_type_container.end())
		{
			return;
		}

		for (auto &values : iter->second.memory)
		{
			delete values;
		}

		m_type_container.erase(iter);
	}
};
