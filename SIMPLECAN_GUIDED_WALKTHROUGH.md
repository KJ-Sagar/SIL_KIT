# Guided Walkthrough: `SimpleCan.cpp`

Source file:
- `Demos/api/SimpleCan/SimpleCan.cpp`

This guide helps you read the SimpleCan demo as a SIL-KIT learner, not just as a C++ reader.

## What this demo is teaching

This demo shows how to:
- create a SIL-KIT participant,
- create a CAN controller,
- register CAN receive and transmit handlers,
- use lifecycle and time synchronization,
- exchange CAN frames between participants connected to the same CAN network.

## Read the file with this mindset

While reading, always ask:
1. Which object is being created?
2. Which names must match for communication to happen?
3. Which code runs immediately?
4. Which code runs later as a callback?
5. What starts the simulation?

## Core objects to identify

When you read `SimpleCan.cpp`, locate these first:
- participant creation,
- lifecycle service creation,
- time sync service creation,
- CAN controller creation,
- communication-ready handler,
- frame receive handler,
- transmit handler,
- simulation step handler,
- lifecycle start.

## Mental model

The flow is usually:
1. Create participant
2. Create lifecycle service
3. Create time sync service
4. Create CAN controller
5. Register handlers
6. Configure the controller when communication is ready
7. Start the lifecycle
8. Send or receive frames during simulation execution

That order matters because SIL-KIT services are expected to be created before lifecycle execution begins.

## What a CAN controller is

A CAN controller in SIL-KIT is the service object that lets a participant interact with a CAN network.

It can:
- start and stop,
- set baud rate,
- send frames,
- receive frames through handlers,
- report transmit acknowledgements.

## What must match

For CAN communication to happen:
- participants must connect to the same registry,
- controllers must use the same CAN network name,
- the lifecycle/system setup must let them run together.

Unlike PubSub:
- PubSub matches on a topic name,
- CAN matches on a bus network name.

## Handlers in this demo

You should find two important callback styles:

### Frame receive handler

This runs when a CAN frame is received.

Use it to understand:
- how incoming frames are exposed,
- where payload and CAN ID are read,
- how event-driven reception works.

### Frame transmit handler

This runs when transmit status is reported back to the sender.

Use it to understand:
- acknowledgement of sending,
- how the sender knows a frame was processed,
- the difference between "I tried to send" and "the system reported a transmit event".

## Lifecycle and communication-ready setup

SimpleCan usually configures the CAN controller inside a lifecycle callback such as a communication-ready handler.

Why this matters:
- lifecycle tells you when the participant has reached a state where setup should continue,
- this is part of the simulation orchestration model,
- startup is not just "main starts, so everything is ready".

When you see code that starts the CAN controller, ask:
- is this happening before the simulation starts running?
- is this happening in the appropriate lifecycle phase?

## Time synchronization

If the demo uses a simulation step handler, that means:
- periodic work is tied to simulation time,
- sending may happen on each simulation tick,
- the demo is easier to reason about deterministically.

Always distinguish:
- virtual simulation time,
- real wall-clock time.

## What to look for in outgoing frames

When the writer sends a frame, inspect:
- the CAN ID,
- the payload bytes,
- any flags,
- when in the simulation it sends,
- whether it prints status logs.

Ask yourself:
- If I change the CAN ID, what changes?
- If I change only the payload, what stays the same?
- If I change the network name on only one participant, what happens?

## What to look for in incoming frames

When the reader receives a frame, inspect:
- how the payload is decoded or printed,
- which fields are logged,
- whether timestamps are shown,
- whether counters are tracked.

## Beginner-safe modifications

Make only one change at a time.

Good first changes:
- Change the participant name.
- Change the CAN network name on all matching participants.
- Change the CAN ID.
- Change the payload text or bytes.
- Add one log line in the receive handler.
- Add one log line in the transmit handler.
- Add a simple receive counter.

## Debug questions to answer while reading

1. Where is the participant created?
2. Where is the CAN controller created?
3. What exact network name is used?
4. Where is the controller started?
5. Which code sends a frame?
6. Which code reacts to a received frame?
7. Which code reacts to a transmit acknowledgement?
8. What starts the lifecycle?

## Practice tasks for this file

### Task 1: Explain the startup path

Write a 5-line summary:
- participant created,
- lifecycle created,
- CAN controller created,
- handlers registered,
- lifecycle started.

### Task 2: Change names

Change the CAN network name consistently across matching participants and predict:
- what will still work,
- what will break if only one side is changed.

### Task 3: Add observability

Add logs for:
- participant startup,
- each sent frame,
- each received frame,
- lifecycle completion.

### Task 4: Add a counter

Track:
- number of sent frames,
- number of received frames.

This helps you learn where state belongs in event-driven code.

## Common beginner mistakes

- Mixing up participant names and network names.
- Thinking the CAN ID must match between sender and receiver for all communication.
- Forgetting that handlers run later, not immediately when declared.
- Not distinguishing setup code from runtime event code.
- Changing several variables at once and then not knowing what caused the result.

## One-sentence summary

`SimpleCan.cpp` teaches how a SIL-KIT participant uses lifecycle, time sync, and a CAN controller to exchange CAN frames over a shared named network.
