#pragma once

#include "Subscriber.h"
#include "SubscriberList.h"
#include "TypeId.h"

class Publisher
{
public:
	Publisher();
	virtual ~Publisher();

	void AddSubscriber(Subscriber* subscriber, TypeId typeId);
	void RemoveSubscriber(Subscriber* subscriber);

	bool Update(TypeId typeId, void* data, size_t size)
	{
		return Update_(typeId, data, size);
	}

	virtual void Dispatch() {}
	virtual bool HasMessages() const { return false; }
	virtual size_t GetMessageCount() const { return 0; }

protected:
	// Derived classes must determine what to do when Update() is called.
	virtual bool Update_(TypeId typeId, void* data, size_t size) = 0;

	// Calls the Notify() for all Subscribers registered for the TypeId.
	bool UpdateInternal_(TypeId typeId, void* data, size_t size);

	void UpdateSubscribers_(
		SubscriberList::SubscriberVector* subscribers,
		TypeId typeId,
		void* data,
		size_t size);

protected:
	SubscriberList m_subscriberList;
};
