#include "include/Helper.hpp"

bool onlyOneTestCase{true};

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "Missing arguments! Start demo with: " << argv[0]
                  << " <ParticipantConfiguration.yaml|json> <ParticipantName>" << "\n"
                  << "Use \"PlantModel_Restbus\" as <ParticipantName> and <Test-Case-ID>" << "\n";
        return -1;
    }

    if (argc > 4)
    {
        std::cerr << "Too many arguments! Start demo with: " << argv[0]
                  << " <ParticipantConfiguration.yaml|json> <ParticipantName>" << "\n"
                  << "Use \"PlantModel_Restbus\" as <ParticipantName> and <Test-Case-ID>" << "\n";
        return -1;
    }

    std::string participantName(argv[2]);

    if (participantName != "PlantModel_Restbus")
    {
        std::cout << "Wrong participant name provided. Use either \"PlantModel_Restbus\"." << "\n";
        return -1;
    }

    testCaseId = std::stoi(argv[3]);

    try
    {
        if(testCaseId < 1 || testCaseId > 3)
        {
            std::runtime_error("Invalid Input: [Test Case IDs: 1, 2, 3 Only]\n");   
        }

        std::string participantConfigurationFilename(argv[1]);

        auto participantConfiguration = SilKit::Config::ParticipantConfigurationFromFile(participantConfigurationFilename);

        std::cout << "Creating participant " << participantName << "\n";

        auto participant = SilKit::CreateParticipant(participantConfiguration, participantName);

        auto *logger = participant->GetLogger();

        // Required CAN channels
        auto *canControllerSendHazard = participant->CreateCanController("CAN1", "CAN1");
        auto *canControllerReceiveHazard = participant->CreateCanController("CAN2", "CAN2");
        auto *canControllerReceiveTrunk = participant->CreateCanController("CAN3", "CAN3"); 
        
        canControllerSendHazard->AddFrameTransmitHandler
        (
            [logger](ICanController * /*ctrl*/, const CanFrameTransmitEvent &ack)
            { 
                FrameTransmitHandler_HazardSend(ack, logger); 
            }
        );

        canControllerReceiveHazard->AddFrameHandler
        (
            [logger](ICanController * /*ctrl*/, const CanFrameEvent &frameEvent)
            { 
                FrameHandler_ReceiveHazard(frameEvent, logger); 
            }
        );

        canControllerReceiveTrunk->AddFrameHandler
        (
            [logger](ICanController * /*ctrl*/, const CanFrameEvent &frameEvent)
            { 
                FrameHandler_ReceiveTrunk(frameEvent, logger); 
            }
        );

        auto operationMode = (OperationMode::Autonomous);
        auto *lifecycleService = participant->CreateLifecycleService({operationMode});

        // Lifecycle State Handlers
        lifecycleService->SetStopHandler
        (
            []()
            { std::cout << "Stop handler called" << "\n"; }
        );
        lifecycleService->SetShutdownHandler
        (
            []()
            { std::cout << "Shutdown handler called" << "\n"; }
        );
        lifecycleService->SetAbortHandler
        (
            [](auto lastState)
            { std::cout << "Abort handler called while in state " << lastState << "\n"; }
        );

        std::thread workerThread;
        std::promise<void> promiseObj;
        std::future<void> futureObj = promiseObj.get_future();
        lifecycleService->SetCommunicationReadyHandler
        (
            [&]()
            {
                std::cout << "Communication ready for " << participantName << "\n";

                canControllerSendHazard->SetBaudRate(10'000, 1'000'000, 2'000'000);
                canControllerReceiveHazard->SetBaudRate(10'000, 1'000'000, 2'000'000);
                canControllerReceiveTrunk->SetBaudRate(10'000, 1'000'000, 2'000'000);

                workerThread = std::thread
                {
                    [&]()
                    {
                        futureObj.get();
                
                        while (lifecycleService->State() == ParticipantState::ReadyToRun || lifecycleService->State() == ParticipantState::Running)
                        {
                            // std::cout << "\nEnter the Test Case ID: " << "\n";

                            while(onlyOneTestCase)
                            {
                                onlyOneTestCase = false;

                                if(participantName == "PlantModel_Restbus" && testCaseId >= 1 && testCaseId <= 3)
                                {                                                                                
                                        SendFrame_ZonalA(canControllerSendHazard, logger);                                                                                         
                                }

                                // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                std::this_thread::sleep_for(1s);

                                // std::cout << "\nEnter the Test Case ID: " << "\n";
                            }
                            
                            
                        }
                    }
                };
                canControllerSendHazard->Start();
                canControllerReceiveHazard->Start();
                canControllerReceiveTrunk->Start(); 
            }
        );
        lifecycleService->SetStartingHandler
        (
            [&]()
            { promiseObj.set_value(); }
        );

        lifecycleService->StartLifecycle();

        while (lifecycleService->State() != SilKit::Services::Orchestration::ParticipantState::Stopping)
        {
            if (std::cin.peek() == '\n')
            {
                std::cin.ignore();
            }
            else
            {
                std::this_thread::sleep_for(100ms); 
            }
        }

        if (workerThread.joinable())
        {
            workerThread.join();
        }

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
        std::cerr << "Something went wrong: " << error.what() << "\n";
        std::cout << "Press enter to end the process..." << "\n";
        std::cin.ignore();
        return -3;
    }

    return 0;
}