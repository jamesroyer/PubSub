list(APPEND PUBSUB_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/ImmediatePublisher.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/Publisher.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/QueuedPublisher.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/SubscriberList.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/TypeId.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

#add_executable(PubSub ${PUBSUB_SOURCES})
add_library(PubSub ${PUBSUB_SOURCES})

target_include_directories(PubSub
   PRIVATE
   ${CMAKE_SOURCE_DIR}/src
)
