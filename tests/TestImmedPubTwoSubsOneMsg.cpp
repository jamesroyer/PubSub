#include <iostream>

#include "ImmediatePublisher.h"
#include "SimpleClient.h"

int main()
{
	ImmediatePublisher publisher;
	SimpleClient test1(&publisher);
	SimpleClient test2(&publisher);

	MsgType1* msg = new MsgType1{42};

	std::cout << "Notifying...\n";
	publisher.Update(GetTypeId<MsgType1>(), msg, sizeof(MsgType1));

	return 0;
}
