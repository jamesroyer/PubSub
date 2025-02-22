#pragma once

#include "Publisher.h"

class ImmediatePublisher : public Publisher
{
public:
	ImmediatePublisher() = default;
	ImmediatePublisher(const ImmediatePublisher&) = delete;
	ImmediatePublisher& operator=(const ImmediatePublisher&) = delete;

protected:
	bool Update_(TypeId typeId, void* data, size_t size) override;
};
