/*
 * Simple tests for the ImmediatePublisher tests:
 * - Show that messages with/without STL containers can be sent.
 * - Verify that messages are deleted after dispatching.
 * - Verify that an exception is thrown for unregistered message types.
 */
#include <iostream>

#include "TypeId.h"
#include "ImmediatePublisher.h"

#include <iostream>

// Set to 1 if any test fails.
int g_result = 0;

// Use at end of application to verify that the Publisher dispatched the
// expected number of messages.
int g_actualMsgCount = 0;
int g_expectedMsgCount = 2;

// Attempting to use Notify() will throw an exception.
struct MsgUnregistered
{};

// Structure using only POD (Plain Old Data) types.
struct MsgType1
{
	int i;
	explicit MsgType1(int i) : i(i) { std::cout << "MsgType1 ctor\n"; }
	~MsgType1() { std::cout << "MsgType1 dtor\n"; }
};
REGISTER_TYPEID(MsgType1)

// Structure using STL container.
struct MsgType2
{
	explicit MsgType2() { std::cout << "MsgType2 ctor\n"; }
	~MsgType2() { std::cout << "MsgType2 dtor\n"; }
	std::vector<int> v;
};
REGISTER_TYPEID(MsgType2)

// Derive directly from Subscriber class and subscribe to receive two message
// types.
class Sub1 : public Subscriber
{
public:
	explicit Sub1(Publisher* publisher)
		: m_publisher(publisher)
	{
		// Subscribe to receive two different message types.
		m_publisher->AddSubscriber(this, GetTypeId<MsgType1>());
		m_publisher->AddSubscriber(this, GetTypeId<MsgType2>());
	}

	~Sub1()
	{
		// Unsubscribe from all messages.
		m_publisher->RemoveSubscriber(this);
	}

private:
	void ProcessMsgType1_(MsgType1* msg)
	{
		++g_actualMsgCount;

		std::cout << "MsgType1.i = " << msg->i << "\n";
	}
	void ProcessMsgType2_(MsgType2* msg)
	{
		++g_actualMsgCount;

		std::cout << "MsgType2.v.size() = " << msg->v.size() << ": [";

		bool useComma {false};
		for (auto item : msg->v)
		{
			if (useComma) std::cout << ", "; else useComma = true;
			std::cout << item;
		}
		std::cout << "]\n";
	}

	bool Update(TypeId typeId, void* data, size_t size) override
	{
		bool ret {false};
		if (typeId == GetTypeId<MsgType1>())
		{
			auto msg = reinterpret_cast<MsgType1*>(data);
			std::cout << "Sub1::Update(): ";
			ProcessMsgType1_(msg);
			ret = true;
		}
		else if (typeId == GetTypeId<MsgType2>())
		{
			auto msg = reinterpret_cast<MsgType2*>(data);
			std::cout << "Sub1::Update(): ";
			ProcessMsgType2_(msg);
			ret = true;
		}
		return ret;
	}

private:
	Publisher* m_publisher {nullptr};
};

int main()
{
	ImmediatePublisher publisher;
	Sub1 sub1(&publisher);

	auto dummyMsg = new MsgUnregistered;
	try
	{
		publisher.Update(GetTypeId<MsgUnregistered>(), dummyMsg, sizeof(*dummyMsg));
		g_result = 1; // Test failure: Expected exception.
	}
	catch (const std::exception& e)
	{
		std::cout << "MsgUnregistered: Caught: " << e.what() << "\n";
		delete dummyMsg;
	}

	MsgType1* msg1 = new MsgType1{42};
	MsgType2* msg2 = new MsgType2;
	msg2->v = {2025, 2, 19, 42};

	try
	{
		std::cout << "Sending MsgType1...\n";
		publisher.Update(GetTypeId<MsgType1>(), msg1, sizeof(*msg1));
	}
	catch (const std::exception& e)
	{
		std::cout << "Update(MsgType1): Caught: " << e.what();
		g_result = 1;
		delete msg1;
		msg1 = nullptr;
	}

	try
	{
		std::cout << "Sending MsgType2...\n";
		publisher.Update(GetTypeId<MsgType2>(), msg2, sizeof(*msg2));
	}
	catch (const std::exception& e)
	{
		std::cout << "Update(MsgType2): Caught: " << e.what();
		g_result = 1;
		delete msg2;
		msg2 = nullptr;
	}

	try
	{
		std::cout << "Calling Dispatch() - noop for ImmediateDispatcher\n";
		publisher.Dispatch();
	}
	catch (const std::exception& e)
	{
		std::cout << "Dispatch(): Caught: " << e.what();
		g_result = 1;
	}

	if (g_actualMsgCount != g_expectedMsgCount)
	{
		std::cout
			<< "Expected " << g_expectedMsgCount
			<< " messages to be dispatched, but only " << g_actualMsgCount
			<< " messages were dispatched.\n";
		g_result = 1;
	}

	return g_result;
}
