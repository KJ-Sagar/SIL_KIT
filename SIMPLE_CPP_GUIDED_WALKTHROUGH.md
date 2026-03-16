# Guided Walkthrough: `simple.cpp`

Source file:
- `docs/code-samples/simple/simple.cpp`

Use this guide while reading the code. Do not rush. Read one section, then explain it in your own words.

## What this sample is teaching

This sample demonstrates:
- how to create SIL-KIT participants,
- how to use coordinated lifecycle,
- how to use time synchronization,
- how to send and receive PubSub messages.

It creates two participants in the same program:
- a publisher participant,
- a subscriber participant.

Both connect to the same registry and communicate over the same PubSub topic.

## Section-by-section explanation

### Includes

The file includes:
- standard C++ headers like `iostream`, `thread`,
- SIL-KIT headers for participant and services.

What this means:
- C++ provides console output and threading.
- SIL-KIT provides the simulation APIs.

## `using namespace std::chrono_literals;`

This lets the code write short time literals like:
- `1s`
- `1ms`

Without this line, the code would need longer `std::chrono` expressions.

## `const auto registryUri = "silkit://localhost:8500";`

This is the address of the SIL-KIT registry.

Meaning:
- participants use this URI to discover the simulation system,
- `localhost` means the registry runs on the same machine,
- `8500` is the port.

If the registry is not reachable here, participant creation or connection will fail.

## `publisher_main(...)`

This function creates and runs the publisher participant.

### `CreateParticipant(...)`

```cpp
auto participant = SilKit::CreateParticipant(config, "PublisherParticipant", registryUri);
```

Meaning:
- create one SIL-KIT participant,
- give it the name `PublisherParticipant`,
- connect it to the registry.

The participant is the root object from which you create services.

### Lifecycle creation

```cpp
auto* lifecycleService =
    participant->CreateLifecycleService({SilKit::Services::Orchestration::OperationMode::Coordinated});
```

Meaning:
- this participant will use SIL-KIT lifecycle management,
- it will run in coordinated mode,
- coordinated mode means it follows the system-wide simulation startup and stop logic.

Why it matters:
- coordinated participants usually work together with the system controller,
- they do not just run whenever they want.

### PubSub setup

```cpp
SilKit::Services::PubSub::PubSubSpec pubSubSpec{"DataService", "text/plain"};
auto* publisher = participant->CreateDataPublisher("PublisherController", pubSubSpec);
```

Meaning:
- the topic name is `DataService`,
- the media type is `text/plain`,
- the publisher controller name is `PublisherController`.

For communication to happen:
- publisher and subscriber must use the same topic specification.

### Time sync service

```cpp
auto* timeSyncService = lifecycleService->CreateTimeSyncService();
```

Meaning:
- this participant will move with virtual simulation time,
- its work happens in simulation steps instead of only real-time loops.

### Simulation step handler

```cpp
timeSyncService->SetSimulationStepHandler(..., 1ms);
```

Meaning:
- every simulation step, the handler runs,
- the configured step size here is 1 millisecond of simulation time.

Inside the handler:
- a message string is created,
- the string is converted into raw bytes,
- the bytes are published,
- the thread sleeps for 1 second in wall-clock time.

Important distinction:
- `1ms` here is simulation-step size,
- `1s` sleep is real-time delay added by the sample.

### Starting lifecycle

```cpp
auto result = lifecycleService->StartLifecycle();
std::cout << "Publisher: result: " << result.get() << std::endl;
```

Meaning:
- the participant enters the SIL-KIT lifecycle,
- `result.get()` waits until the lifecycle finishes,
- when it ends, the final state is printed.

## `subscriber_main(...)`

This function creates and runs the subscriber participant.

### Similar setup

It creates:
- a participant,
- a coordinated lifecycle service,
- the same PubSub spec.

This is intentional:
- both participants must connect to the same simulation system,
- both must agree on the topic.

### Subscriber callback

```cpp
auto receptionHandler = [](auto* subscriber, const auto& dataMessageEvent) {
    std::string message{dataMessageEvent.data.begin(), dataMessageEvent.data.end()};
    std::cout << " <- Received data=\"" << message << "\"" << std::endl;
};
```

Meaning:
- this callback runs whenever a PubSub message arrives,
- the received bytes are converted back into a string,
- the message is printed to the terminal.

This is event-driven programming:
- you do not continuously poll in a loop,
- you register a function to react when data arrives.

### Why the subscriber still has a simulation step handler

The sample creates a time sync service for the subscriber too.

Meaning:
- the subscriber is also part of the synchronized simulation,
- even if it has no periodic work, it still participates in the lifecycle and time model.

That is why the handler body is empty but still present.

## `main(...)`

This is the normal C++ entry point.

### Load configuration

```cpp
auto config = SilKit::Config::ParticipantConfigurationFromFile("simple.yaml");
```

Meaning:
- read the participant configuration from YAML,
- use it to configure the participants.

### Start two threads

```cpp
std::thread publisher{publisher_main, config};
std::thread subscriber{subscriber_main, config};
```

Meaning:
- run publisher and subscriber concurrently,
- each thread creates its own SIL-KIT participant.

Why this is useful:
- one executable can demonstrate a multi-participant system,
- but in real setups, participants are often separate processes.

### Join threads

```cpp
if (subscriber.joinable())
    subscriber.join();
if (publisher.joinable())
    publisher.join();
```

Meaning:
- wait until both participant threads finish,
- avoid exiting the program too early.

## What must match for the sample to work

- The registry URI must be reachable.
- Both participants must use the same topic name.
- Both participants must use compatible lifecycle behavior.
- If coordinated execution is used, the overall system setup must support it.

## Beginner questions for this file

Answer these without looking:
1. What is the participant object used for?
2. Why is coordinated lifecycle used?
3. What does `PubSubSpec` describe?
4. Why does the publisher convert strings into bytes?
5. Why does the subscriber convert bytes back into a string?
6. What is the difference between simulation time and real time in this sample?
7. Why is `.get()` called on the lifecycle result?

## Safe modifications to try

Try one at a time:
- Rename `PublisherParticipant`.
- Rename `SubscriberParticipant`.
- Change topic name from `DataService` to `TrainingTopic` on both sides.
- Change the message text.
- Publish only every tenth simulation step.
- Stop after a fixed number of published messages.

## Common beginner mistakes

- Changing the topic name on one side only.
- Confusing simulation step time with wall-clock sleep time.
- Thinking the subscriber needs a polling loop instead of a callback.
- Reading the code as generic C++ first instead of reading it as a simulation flow.

## One-sentence summary

`simple.cpp` is a minimal example of two SIL-KIT participants using coordinated lifecycle and synchronized time to exchange PubSub messages on a shared topic.
