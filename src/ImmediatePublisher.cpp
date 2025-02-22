#include "ImmediatePublisher.h"

bool ImmediatePublisher::Update_(TypeId typeId, void* data, size_t size)
{
	// NOTE: data is deleted in UpdateInternal_().
	return Publisher::UpdateInternal_(typeId, data, size);
}
