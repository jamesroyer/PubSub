find_package(GTest REQUIRED)

# --- TestImmedPubTwoSubsOneMsg ---

add_executable(TestImmedPubTwoSubsOneMsg TestImmedPubTwoSubsOneMsg.cpp)

target_include_directories(TestImmedPubTwoSubsOneMsg
   PRIVATE
   ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(TestImmedPubTwoSubsOneMsg PRIVATE PubSub) # GTest::GTest)

add_test(
	NAME ImmedPubTwoSubsOneMsg
	COMMAND TestImmedPubTwoSubsOneMsg
)

# --- TestQueuedDispatcher ---

add_executable(TestQueuedDispatcher testQueuedDispatcher.cpp)

target_include_directories(TestQueuedDispatcher
   PRIVATE
   ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(TestQueuedDispatcher PRIVATE PubSub GTest::GTest)

add_test(
	NAME QDTwoSubs
	COMMAND TestQueuedDispatcher
)

# --- TestImmedPubOneSubTwoMsgs ---

add_executable(TestImmedPubOneSubTwoMsgs TestImmedPubOneSubTwoMsgs.cpp)

target_include_directories(TestImmedPubOneSubTwoMsgs
   PRIVATE
   ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(TestImmedPubOneSubTwoMsgs PRIVATE PubSub) # GTest::GTest)

add_test(
	NAME ImmedPubOneSubTwoMsgs
	COMMAND TestImmedPubOneSubTwoMsgs
)

# --- TestQPubOneSubTwoMsgs ---

add_executable(TestQPubOneSubTwoMsgs TestQPubOneSubTwoMsgs.cpp)

target_include_directories(TestQPubOneSubTwoMsgs
   PRIVATE
   ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(TestQPubOneSubTwoMsgs PRIVATE PubSub) # GTest::GTest)

add_test(
	NAME QPubOneSubTwoMsgs
	COMMAND TestQPubOneSubTwoMsgs
)

# --- TestQPubSubObjTwoMsgs ---

add_executable(TestQPubSubObjTwoMsgs TestQPubSubObjTwoMsgs.cpp JobManager.cpp)

target_include_directories(TestQPubSubObjTwoMsgs
   PRIVATE
   ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(TestQPubSubObjTwoMsgs PRIVATE PubSub GTest::GTest)

add_test(
	NAME TestQPubSubObjTwoMsgs
	COMMAND TestQPubSubObjTwoMsgs
)

# --- TestQPubThreadedNoBlocking ---

add_executable(TestQPubThreadedNoBlocking TestQPubThreadedNoBlocking.cpp JobManager.cpp)

target_include_directories(TestQPubThreadedNoBlocking
   PRIVATE
   ${CMAKE_SOURCE_DIR}/src
)
target_link_libraries(TestQPubThreadedNoBlocking PRIVATE PubSub GTest::GTest)

add_test(
	NAME TestQPubThreadedNoBlocking
	COMMAND TestQPubThreadedNoBlocking
)

# --- TestPublisher ---

add_executable(TestPublisher TestPublisher.cpp)

target_include_directories(TestPublisher
   PRIVATE
   ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(TestPublisher
	PRIVATE
	PubSub
	GTest::gtest_main
#	GTest::GTest
)

gtest_discover_tests(TestPublisher)
