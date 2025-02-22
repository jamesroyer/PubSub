#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>

using TypeId = size_t;

inline TypeId CalculateHashId(const std::string& key)
{
	return std::hash<std::string>{}(key);
}

template <typename T>
TypeId GetTypeId()
{
	throw std::invalid_argument("GetTypeId(): T not defined - did you forget to register it?");
}

// This PubSub solution uses void pointers to pass messages between components.
// A deleter for each message type id is needed to ensure messages are deleted
// after attempting to dispatch the message to zero or more subscribers.
extern std::map<TypeId, std::function<void(void*)>> g_typeDeleter;

void DeleteTypeIdPtr(TypeId typeId, void* data);

#define REGISTER_TYPEID(type) \
template <> \
inline TypeId GetTypeId<type>() \
{ \
	static TypeId typeId = 0; \
	if (typeId == 0) \
	{ \
		typeId = CalculateHashId(#type); \
		auto deleter = [](void* data) { \
			delete reinterpret_cast<type*>(data); \
		}; \
		g_typeDeleter.insert(std::make_pair(typeId, deleter)); \
	} \
	return typeId; \
}
