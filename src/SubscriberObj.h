#pragma once

#include "Publisher.h"

// Uses Bridge Pattern to decouple the Subscriber interface from a multitude of
// other Container Types.
//
// ContainerType classes must implement OnUpdate(MsgType&, TypeId) methods.
template <typename MsgType, typename ContainerType>
class SubscriberObj : public Subscriber
{
public:
	SubscriberObj(Publisher* publisher, ContainerType* obj)
		: Subscriber()
		, m_publisher(publisher)
		, m_containerObj(obj)
	{
		m_publisher->AddSubscriber(this, GetTypeId<MsgType>());
	}

	~SubscriberObj()
	{
		m_publisher->RemoveSubscriber(this);
	}

	bool Update(TypeId typeId, void* data, size_t size) override
	{
		auto msg = reinterpret_cast<const MsgType*>(data);
		m_containerObj->OnUpdate(*msg, typeId);
		return true;
	}

private:
	Publisher* m_publisher {nullptr};
	ContainerType* m_containerObj {nullptr};
};
