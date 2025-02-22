/*
 * Using SubscriberObj class objects rather than deriving from the Subscriber
 * class directly.
 */
#include "JobManager.h"
#include "Logger.h"
#include "QueuedPublisher.h"
#include "SubscriberObj.h"

#include <chrono>
#include <functional>
#include <thread>

// Set to 1 if any test fails.
int g_result = 0;

// Use at end of application to verify that the Publisher dispatched the
// expected number of messages.
int g_actualMsgCount = 0;
int g_expectedMsgCount = 2;

// Structure using only POD (Plain Old Data) types.
struct MsgType1
{
	int i;
	explicit MsgType1(int i) : i(i) { DEBUG() << "MsgType1 ctor"; }
	~MsgType1() { DEBUG() << "MsgType1 dtor"; }
};
REGISTER_TYPEID(MsgType1)

// Structure using STL container.
struct MsgType2
{
	explicit MsgType2() { DEBUG() << "MsgType2 ctor"; }
	~MsgType2() { DEBUG() << "MsgType2 dtor"; }
	std::vector<int> v;
};
REGISTER_TYPEID(MsgType2)

// Use composition instead of deriving from a base class.
// The SubscriberObj class requires that a class implement a public callback
// with the following format:
//
//    void OnUpdate(const SomeMsgType&, TypeId)
//
// This example shows a class with two callbacks to receive two different
// message types.
class Generic
{
public:
	Generic(Publisher* publisher)
		: m_objType1(publisher, this)
		, m_objType2(publisher, this)
	{
	}

	void OnUpdate(const MsgType1& msg, TypeId typeId)
	{
		++g_actualMsgCount;

		DEBUG() << "MsgType1.i = " << msg.i << "";
	}

	void OnUpdate(const MsgType2& msg, TypeId typeId)
	{
		++g_actualMsgCount;

		std::ostringstream m_oss;
		m_oss << "MsgType2.v.size() = " << msg.v.size() << ": [";

		bool useComma {false};
		for (auto item : msg.v)
		{
			if (useComma) m_oss << ", "; else useComma = true;
			m_oss << item;
		}
		m_oss << "]";

		DEBUG() << m_oss.str();
	}

private:
	SubscriberObj<MsgType1, Generic> m_objType1;
	SubscriberObj<MsgType2, Generic> m_objType2;
};

int main()
{
	Logger::Disable();

	QueuedPublisher publisher;
	Generic generic(&publisher);

	MsgType1* msg1 = new MsgType1{84};
	MsgType2* msg2 = new MsgType2;
	msg2->v = {2025, 2, 19, 42};

	JobManager jobMgr(false);
	jobMgr.Start();

	DEBUG() << "Queueing MsgType1 message...";
	publisher.Update(GetTypeId<MsgType1>(), msg1, sizeof(*msg1));

	DEBUG() << "Queueing MsgType2 message...";
	publisher.Update(GetTypeId<MsgType2>(), msg2, sizeof(*msg2));

	jobMgr.AddJob([pub = &publisher]() { 
		DEBUG() << "Dispatching from thread (first two messages)...";
		pub->Dispatch();
	});

	jobMgr.AddJob([pub = &publisher]() { 
		DEBUG() << "Dispatching from thread (message added during last dispatch)...";
		pub->Dispatch();
	});

	using namespace std::chrono_literals;

	auto naptime = 2s;

	DEBUG() << "Sleep for " << naptime.count() << " seconds";

	std::this_thread::sleep_for(naptime);

	jobMgr.Stop();

	Logger::Enable();

	if (g_actualMsgCount != g_expectedMsgCount)
	{
		DEBUG()
			<< "Expected " << g_expectedMsgCount
			<< " messages to be dispatched, but only " << g_actualMsgCount
			<< " messages were dispatched.\n";
		g_result = 1;
	}
	else
	{
		DEBUG() << "Test PASSED.";
	}

	return g_result;
}
