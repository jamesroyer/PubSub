#include "QueuedPublisher.h"

void QueuedPublisher::Dispatch()
{
	std::unique_lock lock(m_mutex);

	// Copy all pending messages and immediately clear the queue.
	auto pendingMessages = m_pendingMessages;
	m_pendingMessages.clear();

	// Unlock here to prevent blocking other threads while messages are being
	// dispatched.
	lock.unlock();

	for (auto kv : pendingMessages)
	{
		UpdateInternal_(kv.typeId, kv.data, kv.size);

		// NOTE: kv.data is deleted in UpdateInternal_().
	}
}

bool QueuedPublisher::Update_(TypeId typeId, void* data, size_t size)
{
	std::unique_lock lock(m_mutex);

//	m_pendingMessages.emplace_back(typeId, data, size);
	m_pendingMessages.push_back({typeId, data, size});
	return true;
}

bool QueuedPublisher::HasMessages() const
{
	return !m_pendingMessages.empty();
}

size_t QueuedPublisher::GetMessageCount() const
{
	return m_pendingMessages.size();
}
