#include <iostream>

#include "QueuedPublisher.h"
#include "SimpleClient.h"

int main()
{
	QueuedPublisher publisher;

	// Two different clients registering for the same message type.
	SimpleClient test1(&publisher);
	SimpleClient test2(&publisher);

	MsgType1* msg = new MsgType1{84};

	std::cout << "Queueing...\n";
	publisher.Update(GetTypeId<MsgType1>(), msg, sizeof(MsgType1));

	std::cout << "Dispatching...\n";
	publisher.Dispatch();

	return 0;
}
