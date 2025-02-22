#include <iostream>

#include "QueuedPublisher.h"

#include <iostream>

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
		// Publisher: Subscriber count: 0
		m_publisher->AddSubscriber(this, GetTypeId<MsgType1>());
		// Publisher: Subscriber count: 1
		m_publisher->AddSubscriber(this, GetTypeId<MsgType2>());
		// Publisher: Subscriber count: 2
	}

	~Sub1()
	{
		// Publisher: Subscriber count: 2
		m_publisher->RemoveSubscriber(this);
		// Publisher: Subscriber count: 0
	}

private:
	void DumpMsgType1_(MsgType1* msg)
	{
		std::cout << "MsgType1.i = " << msg->i << "\n";
	}
	void DumpMsgType2_(MsgType2* msg)
	{
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
			DumpMsgType1_(msg);
			ret = true;
		}
		else if (typeId == GetTypeId<MsgType2>())
		{
			auto msg = reinterpret_cast<MsgType2*>(data);
			std::cout << "Sub1::Update(): ";
			DumpMsgType2_(msg);
			ret = true;
		}
		return ret;
	}

private:
	Publisher* m_publisher {nullptr};
};

int main()
{
	QueuedPublisher publisher;
	Sub1 sub1(&publisher);

	MsgType1* msg1 = new MsgType1{84};
	MsgType2* msg2 = new MsgType2;
	msg2->v = {2025, 2, 19, 42};

	std::cout << "Queueing MsgType1 message...\n";
	publisher.Update(GetTypeId<MsgType1>(), msg1, sizeof(*msg1));

	std::cout << "Queueing MsgType2 message...\n";
	publisher.Update(GetTypeId<MsgType2>(), msg2, sizeof(*msg2));

	std::cout << "Dispatching...\n";
	publisher.Dispatch();

	return 0;
}
