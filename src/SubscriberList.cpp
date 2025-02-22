#include "SubscriberList.h"
#include <stdexcept>

SubscriberList::SubscriberVector* SubscriberList::FindSubscriberType(TypeId typeId) const
{
	auto it = m_subscriberTypes.find(typeId);
	if (it != m_subscriberTypes.end())
	{
		return it->second;
	}
	return {};
}

void SubscriberList::AddSubscriber(Subscriber* subscriber, TypeId typeId)
{
	std::unique_lock lock(m_mutex);

	if (!m_locked)
	{
		AddSubscriber_(subscriber, typeId);
	}
	else
	{
		QueueAddSubscriber_(subscriber, typeId);
	}
}

void SubscriberList::RemoveSubscriber(Subscriber* subscriber)
{
	std::unique_lock lock(m_mutex);

	if (!m_locked)
	{
		RemoveSubscriber_(subscriber);
	}
	else
	{
		QueueRemoveSubscriber_(subscriber);
	}
}

void SubscriberList::AddSubscriber_(Subscriber* subscriber, TypeId typeId)
{
	auto typeIt = m_subscriberTypes.find(typeId);
	if (typeIt == m_subscriberTypes.end())
	{
		auto sv = new SubscriberVector;
		auto kv = m_subscriberTypes.insert(std::make_pair(typeId, sv));
		typeIt = kv.first;
	}

	auto subIt = std::find(typeIt->second->begin(), typeIt->second->end(), subscriber);
	if (subIt != typeIt->second->end())
	{
		return;
	}
	typeIt->second->push_back(subscriber);
}

void SubscriberList::RemoveSubscriber_(Subscriber* subscriber)
{
	auto typeItEnd = m_subscriberTypes.end();
	for (auto typeIt = m_subscriberTypes.begin(); typeIt != typeItEnd; ++typeIt)
	{
		typeIt->second->erase(
			std::remove_if(
				typeIt->second->begin(),
				typeIt->second->end(),
				[subscriber](Subscriber* s) { return s == subscriber; }
			),
			typeIt->second->end()
		);
	}
}

void SubscriberList::QueueAddSubscriber_(Subscriber* subscriber, TypeId typeId)
{
	m_pendingCommandQueue.push_back(PendingCommand{CommandAction::ADD, subscriber, typeId});
}

void SubscriberList::QueueRemoveSubscriber_(Subscriber* subscriber)
{
	m_pendingCommandQueue.push_back(PendingCommand{CommandAction::REMOVE, subscriber, 0});
}

void SubscriberList::ProcessCommandQueue_()
{
	for (auto cmd : m_pendingCommandQueue)
	{
		if (cmd.action == CommandAction::ADD)
		{
			AddSubscriber_(cmd.subscriber, cmd.typeId);
		}
		else if (cmd.action == CommandAction::REMOVE)
		{
			RemoveSubscriber_(cmd.subscriber);
		}
	}

	m_pendingCommandQueue.clear();
}

void SubscriberList::LockTables()
{
	std::unique_lock lock(m_mutex);

	if (m_locked)
	{
		throw std::runtime_error("SubscriberList::LockTables(): Already locked");
	}
	m_locked = true;
}

void SubscriberList::UnlockTables()
{
	std::unique_lock lock(m_mutex);

	if (!m_locked)
	{
		throw std::runtime_error("SubscriberList::UnlockTables(): Already unlocked");
	}
	m_locked = false;
}
