#include "QueuedPublisher.h"
#include <gtest/gtest.h>

struct Control
{
	int nofMsgs {0};

	int GetNofMsgs() const { return nofMsgs; }
};

struct Msg1
{
	Control& m_ctrl;

	Msg1(Control& ctrl)
		: m_ctrl(ctrl)
	{
		m_ctrl.nofMsgs++;
	}

	~Msg1()
	{
		m_ctrl.nofMsgs--;
	}
};
REGISTER_TYPEID(Msg1)


TEST(QueueDispatcherTest, MsgFreedAfterPublish)
{
	Control ctrl;
	QueuedPublisher p;

	// Publisher queue is empty, no messages have been allocated.
	EXPECT_EQ(0, p.GetMessageCount());
	EXPECT_EQ(0, ctrl.GetNofMsgs());

	// Allocate memory for a new message.
	auto msg = new Msg1(ctrl);
	EXPECT_EQ(1, ctrl.GetNofMsgs());

	// Publish (i.e. queue) a message (even though there are no subscribers). 
	p.Update(GetTypeId<Msg1>(), msg, sizeof(*msg));
	EXPECT_EQ(1, ctrl.GetNofMsgs());
	EXPECT_EQ(1, p.GetMessageCount());

	// Dispatch message and then free the memory.
	p.Dispatch();
	EXPECT_EQ(0, ctrl.GetNofMsgs());
	EXPECT_EQ(0, p.GetMessageCount());
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
