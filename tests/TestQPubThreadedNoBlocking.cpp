#include "JobManager.h"
#include "Publisher.h"
#include "QueuedPublisher.h"

#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

// Set to 1 if any test fails.
int g_result = 0;

// These structs are all the same - only the name has changed.
struct MsgType1
{
	int i;
	explicit MsgType1(int i) : i(i) {}
};
REGISTER_TYPEID(MsgType1)


struct MsgType2
{
	int i;
	explicit MsgType2(int i) : i(i) {}
};
REGISTER_TYPEID(MsgType2)


struct MsgType3
{
	int i;
	explicit MsgType3(int i) : i(i) {}
};
REGISTER_TYPEID(MsgType3)

class Sub1 : public Subscriber
{
public:
	explicit Sub1(Publisher* publisher)
		: m_publisher(publisher)
	{
		m_publisher->AddSubscriber(this, GetTypeId<MsgType1>());
		m_publisher->AddSubscriber(this, GetTypeId<MsgType2>());
		m_publisher->AddSubscriber(this, GetTypeId<MsgType3>());
	}

	~Sub1()
	{
		m_publisher->RemoveSubscriber(this);
	}

private:
	// Message 1:
	// - Dispatch() makes a copy of all pending messages and then clears the
	//   queue.
	// - When this callback is first entered, the publisher's queue will be
	//   empty.
	// - The DelayNotify thread will add new messages to the publisher's queue
	//   after a brief delay.
	// - After sleep, there should be two messages.
	// - Other threads should be able to queue messages without blocking inside
	//   the thread that called Dispatch().
	void ProcessMsgType1_(MsgType1* msg)
	{
		if (m_publisher->GetMessageCount() != 0)
		{
			std::cout << "MsgType1: Expected Publisher to have 0 messages\n";
			g_result = 1;
		}

		// Timer could be reduced or replaced with signal (i.e. condition
		// variable or semaphore) to speed up this test.
		auto naptime = 4s;
		std::cout << "    Sleep for " << naptime.count() << " seconds" << std::endl;
		std::this_thread::sleep_for(naptime);

		if (m_publisher->GetMessageCount() != 2)
		{
			std::cout << "MsgType1: Expected Publisher to have 2 messages\n";
			g_result = 1;
		}
	}

	void ProcessMsgType2_(MsgType2* msg)
	{
		std::cout << "MsgType2.i = " << msg->i << "\n";
	}

	void ProcessMsgType3_(MsgType3* msg)
	{
		std::cout << "MsgType3.i = " << msg->i << "\n";
	}

	bool Update(TypeId typeId, void* data, size_t size) override
	{
		bool ret {false};
		if (typeId == GetTypeId<MsgType1>())
		{
			auto msg = reinterpret_cast<MsgType1*>(data);
			ProcessMsgType1_(msg);
			ret = true;

			if (m_counter > 0)
			{
				// This message should be added to pending list
				MsgType1* msg1 = new MsgType1{m_counter};
				m_publisher->Update(GetTypeId<MsgType1>(), msg1, sizeof(*msg1));

				--m_counter;
			}
		}
		else if (typeId == GetTypeId<MsgType2>())
		{
			auto msg = reinterpret_cast<MsgType2*>(data);
			ProcessMsgType2_(msg);
			ret = true;
		}
		else if (typeId == GetTypeId<MsgType3>())
		{
			auto msg = reinterpret_cast<MsgType3*>(data);
			ProcessMsgType3_(msg);
			ret = true;
		}

		return ret;
	}

private:
	Publisher* m_publisher {nullptr};
	int m_counter {1};
};

class DelayedNotify : public BaseThread
{
public:
	DelayedNotify(Publisher* publisher, std::chrono::seconds delay)
		: BaseThread("DelayNotify", true)
		, m_publisher(publisher) , m_delay(delay)
	{}

protected:
	void Run_() override
	{
		std::this_thread::sleep_for(m_delay);

		if (m_publisher->GetMessageCount() != 0)
		{
			std::cout << m_name << ": Expected Publisher to have 0 messages\n";
			g_result = 1;
		}

		auto msg2 = new MsgType2{2};
		m_publisher->Update(GetTypeId<MsgType2>(), msg2, sizeof(*msg2));
		if (m_publisher->GetMessageCount() != 1)
		{
			std::cout << m_name << ": MsgType2: Expected Publisher to have 1 message\n";
			g_result = 1;
		}

		auto msg3 = new MsgType3{3};
		m_publisher->Update(GetTypeId<MsgType3>(), msg3, sizeof(*msg3));

		if (m_publisher->GetMessageCount() != 2)
		{
			std::cout << m_name << ": MsgType3: Expected Publisher to have 2 messages\n";
			g_result = 1;
		}
	}

private:
	Publisher* m_publisher;
	std::chrono::seconds m_delay;
};

int main()
{
	QueuedPublisher publisher;
	Sub1 sub1(&publisher);
	DelayedNotify delayThread(&publisher, 1s);

	auto msg1 = new MsgType1{1};

	JobManager jobMgr;

	std::cout << "Queueing MsgType1 message...\n";
	publisher.Update(GetTypeId<MsgType1>(), msg1, sizeof(*msg1));

	jobMgr.AddJob([pub = &publisher]() { 
		std::cout << "JobManager: Dispatching (slow callback)...\n";
		pub->Dispatch();
	});

	auto naptime = 5s;
	std::cout << "Sleep for " << naptime.count() << " second(s)" << std::endl;
	std::this_thread::sleep_for(naptime);

	std::cout << "Tests " << (g_result == 0 ? "PASSED" : "FAILED") << "\n";

	return g_result;
}
