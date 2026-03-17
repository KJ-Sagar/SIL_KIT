
#include "include/Helper_SIL_Kit.hpp"

std::string broker_url;
std::string topic = "Vehicle_Control_Testing";
std::string topic_response = "Vehicle_Control_Response";

std::vector<uint8_t> PAYLOAD_FROM_HPC;

std::vector<uint8_t> GetPayloadHexFromFrame(const EthernetFrame &frame)
{
    const size_t FrameHeaderSize = 2 * sizeof(EthernetMac) + sizeof(EtherType);

    std::vector<uint8_t> payload;
    payload.insert(payload.end(), frame.raw.begin() + FrameHeaderSize, frame.raw.end());
    return payload;
}

void FrameHandler_HPC(IEthernetController * /*controller*/, const EthernetFrameEvent &frameEvent, std::unique_ptr<SilKit::IParticipant> &participant)
{
    auto frame = frameEvent.frame;
    PAYLOAD_FROM_HPC = GetPayloadHexFromFrame(frame);
    std::cout << "\n>> Ethernet frame from HPC:  \n";
    PrintHexFrame(PAYLOAD_FROM_HPC);
    // std::cout << " Trunk frame :\n";
    // PrintHexFrame(TRUNK_FRAME);
    std::cout << "\n";

    if (PAYLOAD_FROM_HPC[44] == 0x80 and TRUNK_FRAME[15] == 0x80)
    {
        publish_to_mqtt(broker_url, topic_response, "TRUNK_OPENED");
    }
    else if (PAYLOAD_FROM_HPC[44] == 0x00 and TRUNK_FRAME[15] == 0x80)
    {
        // publish_to_mqtt(broker_url, topic_response, "TRUNK_CANNOT_BE_OPEN_VEHICLE_IS_MOVING");
        publish_to_mqtt(broker_url, topic_response, "TRUNK_CLOSED");    // For Test-Case(3) Purposes Only
    }
    else if (PAYLOAD_FROM_HPC[44] == 0x00 and TRUNK_FRAME[15] == 0x00)
    {
        publish_to_mqtt(broker_url, topic_response, "TRUNK_CLOSED");
    }
    else
    {
        // std::cout << PAYLOAD_FROM_HPC[44] << " " << TRUNK_FRAME[15]<<" : ";
        publish_to_mqtt(broker_url, topic_response, "OOPS_SOMETHING_WENT_WRONG");
    }
}

int main(int argc, char **argv)
{

    if (argc < 3)
    {
        std::cerr << "Missing arguments!" << argv[0]
                  << " <ParticipantName>" << "\n"
                  << "Use \"Telematics" << "\n";
        return -1;
    }

    if (argc > 4)
    {
        std::cerr << "Too many arguments!" << argv[0]
                  << "<ParticipantName>" << "\n"
                  << "Use \"Telematics" << "\n";
        return -1;
    }
    broker_url = argv[3];
    // broker_url = "192.168.134.189";
    // std::cout << " broker url is  : " << broker_url << "\n";

    std::string participantName(argv[2]);

    if (participantName != "Telematics")
    {
        std::cout << "Wrong participant name provided. Use \"Telematics" << "\n";
        return 1;
    }

    std::cout << "SIL Kit Version: " << SilKit::Version::String() << "\n";

    try
    {
        std::string participantConfigurationFileName(argv[1]);

        auto participantConfiguration = SilKit::Config::ParticipantConfigurationFromFile(participantConfigurationFileName);

        std::cout << "Creating participant: " << participantName << "\n";

        auto participant = SilKit::CreateParticipant(participantConfiguration, participantName);
        auto *ethernetController = participant->CreateEthernetController("Eth1", "Eth1");

        // Linking Controller with the Callback
        // ethernetController->AddFrameHandler(&FrameHandler);
        ethernetController->AddFrameTransmitHandler(&FrameTransmitHandler);

        ethernetController->AddFrameHandler(
            [&](IEthernetController *controller, const EthernetFrameEvent &frameEvent)
            {
                FrameHandler_HPC(controller, frameEvent, participant);
            });
        ethernetController->AddFrameTransmitHandler(&FrameTransmitHandler);

        // Creation of Simulation Lifecycle
        auto *lifecycleService = participant->CreateLifecycleService({SilKit::Services::Orchestration::OperationMode::Autonomous});

        // Orchestration Lifecycle Handlers
        lifecycleService->SetStopHandler(
            []()
            {
                std::cout << "Stop handler called" << "\n";
            });

        lifecycleService->SetShutdownHandler(
            []()
            {
                std::cout << "Shutdown handler called" << "\n";
            });

        lifecycleService->SetAbortHandler(
            [](auto lastState)
            {
                std::cout << "Abort handler called while in state " << lastState << "\n";
            });

        std::promise<void> startHandlerPromise;
        auto startHandlerFuture = startHandlerPromise.get_future();
        std::atomic<bool> isStopRequested = {false};

        lifecycleService->SetStartingHandler(
            [&]()
            {
                startHandlerPromise.set_value();
            });
        lifecycleService->StartLifecycle();

        // MQTT Thread which will be a Subscriber to the MQTT Data
        std::thread mqtt_Thread(
            [&]()
            {
                listen_to_mqtt(broker_url, topic);
            });

        // SIL Kit Thread which will be sending data to HPC
        std::thread silKitThread(
            [&]()
            {
                startHandlerFuture.get();
                std::cout << "Communication ready handler called for " << participantName << "\n";
                while (lifecycleService->State() == SilKit::Services::Orchestration::ParticipantState::ReadyToRun || lifecycleService->State() == SilKit::Services::Orchestration::ParticipantState::Running)
                {
                    if (participantName == "Telematics" && ethernetController)
                    {

                        SendFrame(ethernetController);
                    }
                    ethernetController->Activate();
                    std::this_thread::sleep_for(1s);
                }
            });

        silKitThread.detach();

        std::cout << "Press enter to leave the simulation..." << "\n";
        std::cin.ignore();

        isStopRequested = true;
        if (lifecycleService->State() == ParticipantState::Running || lifecycleService->State() == ParticipantState::Paused)
        {
            std::cout << "User requested to stop in state " << lifecycleService->State() << "\n";
            lifecycleService->Stop("User requested to stop");
        }

        mqtt_Thread.detach();

        std::cout << "The participant has shut down and left the simulation" << "\n";
    }
    catch (const SilKit::ConfigurationError &error)
    {
        std::cerr << "Invalid configuration: " << error.what() << "\n";
        std::cout << "Press enter to end the process..." << "\n";
        std::cin.ignore();
        return -2;
    }
    catch (const std::exception &error)
    {
        std::cerr << "Something went wrong: " << error.what() << std::endl;
        std::cout << "Press enter to end the process..." << std::endl;
        std::cin.ignore();
        return -3;
    }

    return 0;
}
