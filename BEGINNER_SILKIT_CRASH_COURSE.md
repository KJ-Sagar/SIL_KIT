# SIL-KIT 3-Day Crash Course for a Beginner in C++

This guide turns the study plan into something you can follow directly in this repository.

Primary goal:
- Become comfortable running, understanding, and lightly modifying SIL-KIT demos in 3 days.

Primary code anchors:
- `docs/code-samples/simple/simple.cpp`
- `Demos/api/SimpleCan/SimpleCan.cpp`
- `docs/simulation/simulation.rst`

Recommended daily time budget:
- 6 to 8 focused hours

How to use this guide:
1. Read only the files listed for the current block.
2. Write short notes in plain English after each block.
3. Run or inspect one sample at a time.
4. Change one thing at a time and predict the result before you test it.

## Outcomes

By the end of Day 3, you should be able to:
- Explain registry, participant, service, lifecycle, time sync, and system controller.
- Read `simple.cpp` without feeling lost.
- Read `SimpleCan.cpp` and identify setup, handlers, and simulation flow.
- Run at least one PubSub flow and one CAN flow.
- Make small safe changes: names, payloads, timings, logging, network names.
- Write or modify one tiny participant program without copying blindly.

## Fast C++ Survival Guide

You do not need all of C++ for SIL-KIT. For this crash course, learn only these:

- `#include`
  Brings declarations from headers into the file.

- `auto`
  Lets the compiler infer the type.

- `std::shared_ptr<T>`
  Shared ownership of an object.

- `std::unique_ptr<T>`
  Single-owner pointer, often returned by factory functions.

- `const`
  Means the value should not be modified.

- `->`
  Access a member through a pointer.

- Lambda
  Inline function used for callbacks.
  Example:
  ```cpp
  [](int value) { std::cout << value << std::endl; }
  ```

- Lambda capture
  `[x]` copies `x` into the lambda.
  `[&x]` references the existing `x`.

- `std::thread`
  Runs work in another thread.

- `std::chrono`
  Used for durations like milliseconds and nanoseconds.

- `try/catch`
  Handles exceptions and prevents abrupt termination.

- Future-like result
  If you see `.get()`, the code is often waiting for an async result to finish.

## Day 1

### Objective

Build the SIL-KIT mental model and learn enough C++ to read the smallest sample.

### Files to read

Read in this order:
- `README.rst`
- `docs/overview/overview.rst`
- `docs/api/participant.rst`
- `docs/code-samples/simple/simple.cpp`
- `docs/code-samples/simple/SIMPLE_CPP_GUIDED_WALKTHROUGH.md`

### Concepts to learn

- A registry helps participants discover each other.
- A participant is one SIL-KIT application/process.
- Services provide communication or orchestration features.
- Lifecycle controls when a participant starts, runs, and stops.
- Time synchronization lets multiple participants move through the same virtual time.
- PubSub lets participants exchange data using a shared topic name.

### What to notice in `simple.cpp`

- Configuration is loaded from a YAML file.
- A participant is created with a name and registry URI.
- A lifecycle service is created in coordinated mode.
- A publisher and subscriber use the same `PubSubSpec`.
- The publisher sends raw bytes every simulation step.
- The subscriber receives data through a callback.
- A time sync service drives the periodic work.
- `StartLifecycle().get()` waits until the participant stops.

### Day 1 exercises

Write one line for each:
- Registry
- Participant
- Lifecycle
- Time sync
- PubSub

Answer from memory:
1. What does the registry do?
2. Why are there two threads in `simple.cpp`?
3. What makes the publisher and subscriber communicate?
4. What would happen if the topic names did not match?
5. Why does the subscriber still create a simulation step handler?

### Day 1 checkpoint

You are ready for Day 2 if you can explain:
- Why `CreateParticipant(...)` is the entry into the SIL-KIT world.
- Why coordinated mode is being used in the sample.
- Why publisher and subscriber are different services on the same topic.
- What the simulation step handler does.

If this still feels fuzzy, repeat:
- `docs/api/participant.rst`
- `docs/simulation/simulation.rst`
- `docs/code-samples/simple/simple.cpp`

## Day 2

### Objective

Move from reading to operating. Learn CAN and the difference between coordinated and autonomous execution.

### Files to read

Read in this order:
- `docs/demos/build.rst`
- `docs/demos/api.rst`
- `docs/demos/communication.rst`
- `Demos/api/SimpleCan/SimpleCan.cpp`
- `Demos/api/SimpleCan/SIMPLECAN_GUIDED_WALKTHROUGH.md`
- `docs/simulation/simulation.rst`

### Concepts to lock in

- Coordinated mode waits for the simulation system to be ready.
- Autonomous mode runs independently.
- The system controller coordinates required participants.
- CAN communication depends on matching network names.
- Handlers are callbacks that react to events like received frames.
- SIL-KIT services are usually created before the lifecycle is started.

### Hands-on tasks

1. Build SIL-KIT or the demos if not already built.
2. Start the registry.
3. Run the SimpleCan example with two participants.
4. Run a coordinated example with the system controller.
5. Run an async or autonomous variant and compare the behavior.
6. Change participant names and confirm the system still works.
7. Change the CAN network name on both sides and confirm it still works.
8. Intentionally mismatch network names and observe that communication stops.
9. Change the payload contents and make the logs easier to read.

### Day 2 questions

1. Why does coordinated mode often need a system controller?
2. What does a CAN controller do in SIL-KIT?
3. Why must network names match?
4. What is the difference between a PubSub topic and a CAN network name?
5. Why are handlers important in event-driven code?

### Day 2 checkpoint

You are ready for Day 3 if you can:
- Explain coordinated vs autonomous mode in plain English.
- Explain why a mismatched network name breaks communication.
- Make a small code change and predict the result before running it.

## Day 3

### Objective

Turn understanding into transferable skill by making your own small changes and reading one more demo style.

### Files to read

Read in this order:
- `docs/code-samples/simple/simple.cpp`
- `Demos/api/SimpleCan/SimpleCan.cpp`
- `Demos/api/Orchestration/Coordinated.cpp` or `Demos/api/Orchestration/SimStep.cpp`
- One of:
  - `Demos/communication/PubSub/PublisherDemo.cpp`
  - `Demos/communication/Rpc/RpcClientDemo.cpp`

### Implementation exercises

#### Exercise A: Modify `simple.cpp`

- Change participant names.
- Change topic name.
- Publish every Nth simulation step instead of every step.
- Stop after a fixed number of messages.
- Print the current simulation time with each message.

#### Exercise B: Modify `SimpleCan.cpp`

- Change the CAN ID.
- Change the payload format.
- Add clearer logging around startup and shutdown.
- Add a receive counter and print it.
- Change the timing or pacing if the demo supports it.

#### Exercise C: Write one tiny participant app

Choose one:
- A tiny PubSub sender and receiver
- A tiny CAN writer and reader

Requirements:
- Connect both to the same registry.
- Use matching topic or network names.
- Print timestamps and message counts.
- Keep it small and understandable.

### Expert reading method

When you open an unfamiliar SIL-KIT file:
1. Find object creation first.
2. Find lifecycle creation and mode.
3. Find service creation next.
4. Find callbacks and handlers.
5. Find the point where the lifecycle starts.
6. Ask what exact names must match for communication to happen.
7. Change one small value and predict the effect.

## Practice Checklist

You should be able to explain these without notes:
- Participant
- Service
- Lifecycle
- Coordinated mode
- Autonomous mode
- Time synchronization
- System controller
- Topic matching
- Network matching

You should be able to do these operationally:
- Start the registry and explain why it is needed.
- Run one simple sample and one CAN demo.
- Describe what each terminal/process is doing.
- Diagnose whether a failure is caused by startup order, naming mismatch, or lifecycle mode.

You should be able to do these code changes:
- Change one participant name safely.
- Change one topic or network name on both sides.
- Change one payload.
- Add one useful handler log.
- Change one timing parameter and explain its effect.

## If You Only Have 2 Days

Compress the plan like this:
- Day 1: Read all Day 1 material and the first half of Day 2.
- Day 2: Do Day 2 hands-on tasks and only Exercise A plus one small CAN change from Exercise B.

Skip for now:
- Extra communication families
- Deep orchestration details
- Network simulation

## Suggested Note Template

Use this exact template in your notebook or editor:

```txt
Concept:
My explanation:
Where I saw it in code:
What names/values must match:
What would break it:
One change I want to try:
Prediction:
Actual result:
```

## Final Advice

Do not try to master all of C++ first.

For this repo, the winning approach is:
- understand the simulation behavior first,
- understand the code structure second,
- understand the C++ details only as needed.

Small accurate understanding beats broad shallow reading.
