#pragma once

#include "TypeId.h"

class Subscriber
{
public:
	virtual ~Subscriber() = default;
	Subscriber() = default;
	Subscriber(const Subscriber&) = delete;
	Subscriber& operator=(const Subscriber&) = delete;

	virtual bool Update(TypeId typeId, void* data, size_t size) = 0;
};


