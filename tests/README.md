# Test

These are simple tests written using CTest (CMake tests) and GTest (Google Test).

These tests can be improved. One change is to remove timers and replace them
with other primatives like condition variables or semaphores.


## TestPublisher.cpp - MsgFreedAfterPublish

Simple test using QTest to verify message allocated on the stack is freed
after calling QueuedPublisher::Dispatch().

## ImmedPubTwoSubsOneMsg

## QDTwoSubs

## ImmedPubOneSubTwoMsgs

## QPubOneSubTwoMsgs

## TestQPubSubObjTwoMsgs

## TestQPubThreadedNoBlocking

This CTest uses multi-threading to verify that while
`QueuedPublisher::Dispatch()` is executing, other threads are able to call
`QueuedPublisher::Notify()` without blocking.

```mermaid
sequenceDiagram
participant c as Client
participant qp as Queued<br/>Producer
participant s as Subscriber
participant sys as System


c->>qp: Notify(Msg1)
activate qp
deactivate qp

c->>qp: Dispatch()
activate qp
    qp->>s: Notify(Msg1)
    activate s
        s->>sys: Sleep(5s)
        activate sys
        note over c,qp: Notify(Msg2) should not block<br/>here while dispatching.
        c->>qp: Notify(Msg2)
        note over c,qp: Use flag and timer to verify.
        c->>qp: Notify(Msg3)
        deactivate sys
    deactivate s
deactivate qp
c->>qp: Dispatch()
activate qp
    qp->>s: Notify(Msg2)
    activate s
    deactivate s
    qp->>s: Notify(Msg3)
    activate s
    deactivate s
deactivate qp
```
