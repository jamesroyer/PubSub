#include "TypeId.h"

std::map<TypeId, std::function<void(void*)>> g_typeDeleter;

void DeleteTypeIdPtr(TypeId typeId, void* data)
{
	auto it = g_typeDeleter.find(typeId);
	if (it != g_typeDeleter.end())
	{
		it->second(data);
	}
	else
	{
		throw std::runtime_error("DeleteTypeIdPtr(): Deleter not found for TypeId");
	}
}
