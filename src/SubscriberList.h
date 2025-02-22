#pragma once

#include "Subscriber.h"
#include "TypeId.h"

#include <mutex>

class Publisher;

class SubscriberList
{
public:
	using SubscriberVector = std::vector<Subscriber*>;

	SubscriberList() = default;
	virtual ~SubscriberList() = default;

	SubscriberVector* FindSubscriberType(TypeId typeId) const;
	void AddSubscriber(Subscriber* subscriber, TypeId typeId);
	void RemoveSubscriber(Subscriber* subscriber);
	void LockTables();
	void UnlockTables();

private:
	void AddSubscriber_(Subscriber* subscriber, TypeId typeId);
	void RemoveSubscriber_(Subscriber* subscriber);
	void QueueAddSubscriber_(Subscriber* subscriber, TypeId typeId);
	void QueueRemoveSubscriber_(Subscriber* subscriber);
	void ProcessCommandQueue_();

private:
	enum class CommandAction { ADD, REMOVE };

	struct PendingCommand
	{
		CommandAction action;
		Subscriber* subscriber;
		TypeId typeId;
	};

	using SubscriberMap = std::map<TypeId, SubscriberVector*>;
	using PendingCommandQueue = std::vector<PendingCommand>;

	std::mutex m_mutex;
	bool m_locked {false};

	SubscriberMap m_subscriberTypes;
	PendingCommandQueue m_pendingCommandQueue;
};
