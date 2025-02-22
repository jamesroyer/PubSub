#pragma once

#include "Publisher.h"
#include "SubscriberObj.h"
#include "TypeId.h"

#include <iostream>

struct MsgType1
{
	int i;
	MsgType1(int i) : i(i) { std::cout << "A::A(i=" << i << ")\n"; }
	~MsgType1() { std::cout << "A::~A()\n"; }
};
REGISTER_TYPEID(MsgType1)

class SimpleClient
{
public:
	SimpleClient(Publisher* publisher)
		: m_aObj(publisher, this)
	{
//		m_publisher->AddSubscriber(&m_aObj, GetTypeId<MsgType1>());
	}

	~SimpleClient()
	{
//		m_publisher->RemoveSubscriber(&m_aObj);
	}

	void OnUpdate(const MsgType1& a, TypeId typeId)
	{
		std::cout << "Test::OnUpdate(A, TypeId) called.\n";
	}

private:
//	Publisher* m_publisher {nullptr};
	SubscriberObj<MsgType1, SimpleClient> m_aObj;
};
