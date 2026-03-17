#include "include/Helper.hpp"
int main(int argc, char **argv)
{

	if (argc < 3)
	{
		std::cerr << "Missing arguments! Start demo with: " << argv[0]
				  << " <ParticipantConfiguration.yaml|json> <ParticipantName> [RegistryUri] [--async]" << std::endl
				  << "Use \"ZONAL1\" or \"EthernetReader\" as <ParticipantName>." << std::endl;
		return -1;
	}

	if (argc > 5)
	{
		std::cerr << "Too many arguments! Start demo with: " << argv[0]
				  << " <ParticipantConfiguration.yaml|json> <ParticipantName> [RegistryUri] [--async]" << std::endl
				  << "Use \"ZONAL1\" or \"EthernetReader\" as <ParticipantName>." << std::endl;
		return -1;
	}

	std::string participantName(argv[2]);
	if (participantName != "ZONAL1")
	{
		std::cout << "Wrong participant name provided. Use either \"ZONAL1\" "
				  << std::endl;
		return 1;
	}
	// std::cout << "SIL Kit Version: " << SilKit::Version::String() << std::endl;
	try
	{
		std::string participantConfigurationFilename(argv[1]);
		std::string registryUri{"silkit://localhost:8500"};

		bool runSync = true;

		// skip argv[0] and collect all arguments
		std::vector<std::string> args;
		std::copy((argv + 3), (argv + argc), std::back_inserter(args));

		for (auto arg : args)
		{
			if (arg == "--async")
			{
				runSync = false;
			}
			else
			{
				registryUri = arg.c_str();
			}
		}

		using SilKit::CreateParticipant;
		using SilKit::Config::ParticipantConfigurationFromFile;

		auto participantConfiguration = ParticipantConfigurationFromFile(participantConfigurationFilename);

		std::cout << "Creating participant '" << participantName << "' with registry " << registryUri << std::endl;
		auto participant = CreateParticipant(participantConfiguration, participantName);

		// Sending Hazard data from ZonalA to HPC
		auto *ethernetController = participant->CreateEthernetController("Eth4", "Eth4");

		ethernetController->AddFrameHandler(&ETH_FrameHandler);
		ethernetController->AddFrameTransmitHandler(&ETH_FrameTransmitHandler);

		auto operationMode = (runSync ? OperationMode::Coordinated : OperationMode::Autonomous);
		auto *lifecycleService = participant->CreateLifecycleService({operationMode});

		// Observe state changes
		lifecycleService->SetStopHandler([]()
										 { std::cout << "Stop handler called" << std::endl; });
		lifecycleService->SetShutdownHandler([]()
											 { std::cout << "Shutdown handler called" << std::endl; });
		auto *logger = participant->GetLogger();

		// Receiving CAN data from Plant Model => ZonalA
		auto *canController = participant->CreateCanController("CAN1", "CAN1");

		canController->AddFrameTransmitHandler([logger](ICanController * /*ctrl*/, const CanFrameTransmitEvent &ack)
											   { CAN_FrameTransmitHandler(ack, logger); });
		canController->AddFrameHandler(
			[logger](ICanController * /*ctrl*/, const CanFrameEvent &frameEvent)
			{ CAN_FrameHandler(frameEvent, logger); });

		std::promise<void> startHandlerPromise;
		auto startHandlerFuture = startHandlerPromise.get_future();
		bool isStopped = false;
		std::thread workerThread;
		// Set a CommunicationReady Handler
		lifecycleService->SetCommunicationReadyHandler([&]()
													   {
			std::cout << "Communication ready handler called for " << participantName << std::endl;
			workerThread = std::thread{ [&]() {
				startHandlerFuture.get();
				while (!isStopped)
				{
					if (participantName == "ZONAL1")
					{
						ETH_SendFrame(ethernetController);             /// ethnernet sending 
					}
					std::this_thread::sleep_for(1s);
				}
				lifecycleService->Stop("User requested to Stop");
			} };
			ethernetController->Activate();
			canController->Start(); }); //// can_controller starting

		lifecycleService->SetStartingHandler([&]()
											 { startHandlerPromise.set_value(); });

		auto finalStateFuture = lifecycleService->StartLifecycle();
		std::cout << "Press enter to stop the process..." << std::endl;
		std::cin.ignore();

		isStopped = true;
		if (workerThread.joinable())
		{
			workerThread.join();
		}
		auto finalState = finalStateFuture.get();
		std::cout << "Simulation stopped. Final State: " << finalState << std::endl;
	}
	catch (const SilKit::ConfigurationError &error)
	{
		std::cerr << "Invalid configuration: " << error.what() << std::endl;
		std::cout << "Press enter to end the process..." << std::endl;
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