#pragma once

#include "Publisher.h"

class QueuedPublisher : public Publisher
{
public:
	QueuedPublisher() = default;
	QueuedPublisher(const QueuedPublisher&) = delete;
	QueuedPublisher& operator=(const QueuedPublisher&) = delete;

	void Dispatch() override;

	bool HasMessages() const override;
	size_t GetMessageCount() const override;

protected:
	// Queue the message for delivery.
	// Must call Dispatch() periodically to actually deliver the message.
	bool Update_(TypeId typeId, void* data, size_t size) override;

private:
	struct PendingMessage
	{
		TypeId typeId;
		void* data;
		size_t size;
	};

	std::mutex m_mutex;
	std::vector<PendingMessage> m_pendingMessages;
};
