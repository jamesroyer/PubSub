#include "Publisher.h"

Publisher::Publisher()
{
}

Publisher::~Publisher()
{
}

void Publisher::AddSubscriber(Subscriber* subscriber, TypeId typeId)
{
	m_subscriberList.AddSubscriber(subscriber, typeId);
}

void Publisher::RemoveSubscriber(Subscriber* subscriber)
{
	m_subscriberList.RemoveSubscriber(subscriber);
}

void Publisher::UpdateSubscribers_(
	SubscriberList::SubscriberVector* subscribers,
	TypeId typeId,
	void* data,
	size_t size)
{
	if (subscribers != nullptr)
	{
		auto it = subscribers->begin();
		while (it != subscribers->end())
		{
			(*it)->Update(typeId, data, size);
			++it;
		}
	}
}

bool Publisher::UpdateInternal_(TypeId typeId, void* data, size_t size)
{
	m_subscriberList.LockTables();

	SubscriberList::SubscriberVector* msgChain;

	msgChain = m_subscriberList.FindSubscriberType(typeId);
	UpdateSubscribers_(msgChain, typeId, data, size);

	// NOTE: Last chance to use 'data' here before it is deleted.

	DeleteTypeIdPtr(typeId, data);

	m_subscriberList.UnlockTables();
	return true;
}
