
#include <algorithm>
#include <cstring>
#include <future>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "silkit/SilKit.hpp"
#include "silkit/services/logging/ILogger.hpp"
#include "silkit/services/orchestration/all.hpp"
#include "silkit/services/orchestration/string_utils.hpp"
#include "silkit/services/can/all.hpp"
#include "silkit/services/can/string_utils.hpp"

using namespace SilKit::Services::Orchestration;
using namespace SilKit::Services::Can;
using namespace SilKit::Services::Logging;
using namespace SilKit::Services::Ethernet;

using namespace std::chrono_literals;

using EtherType = uint16_t;
using EthernetMac = std::array<uint8_t, 6>;
std::vector<uint8_t> Can_payload;
std::vector<uint8_t> ETH_HAZARD_paylaod;
std::vector<uint8_t> ETH_TRUNK_paylaod;

bool flagCheckTrunkDataReceived{false};
bool flagCheck_HAZARD_DataReceived{false};

namespace std
{
    namespace chrono
    {
        std::ostream &operator<<(std::ostream &out, nanoseconds timestamp)
        {
            auto seconds = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(timestamp);
            out << seconds.count() << "s";
            return out;
        }
    } // namespace chrono
} // namespace std

std::vector<uint8_t> CreateFrame(const std::vector<uint8_t> &payload)
{

    std::vector<uint8_t> raw;
    EthernetMac sourceAddress = {0x66, 0x55, 0x44, 0x33, 0x22, 0x22};
    EthernetMac destinationAddress = {0x66, 0x55, 0x44, 0x33, 0x22, 0x33};

    std::vector<uint8_t> pre_payload = {0x08, 0x00, 0x45, 0x00,
                                        0x00, 0x64, 0x00, 0x00,
                                        0x40, 0x00, 0x40, 0x11,
                                        0xB7, 0x69, 0xC0, 0xA8,
                                        0x01, 0x02, 0xC0, 0xA8,
                                        0x01, 0x05, 0x13, 0x59,
                                        0x13, 0x58, 0x00, 0x50,
                                        0x00, 0x00, 0x00, 0x00,
                                        0x14, 0x01, 0x00, 0x00, 0x00, 0x0C};

    std::copy(sourceAddress.begin(), sourceAddress.end(), std::back_inserter(raw));
    std::copy(destinationAddress.begin(), destinationAddress.end(), std::back_inserter(raw));
    std::copy(pre_payload.begin(), pre_payload.end(), std::back_inserter(raw));
    std::copy(payload.begin(), payload.end(), std::back_inserter(raw));

    return raw;
}
std::vector<uint8_t> GetPayloadHexFromFrame(const EthernetFrame &frame)
{
    const size_t FrameHeaderSize = 2 * sizeof(EthernetMac) + sizeof(EtherType);

    std::vector<uint8_t> payload;
    payload.insert(payload.end(), frame.raw.begin() + FrameHeaderSize, frame.raw.end());
    return payload;
}

void PrintHexFrame(const std::vector<uint8_t> &frame)
{
    int i = 1;
    for (const auto &byte : frame)
    {
        std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte) << " ";
        if (i % 10 == 0)
            std::cout << "\n";
        i++;
    }
    std::cout << std::dec << std::endl;
}



void ETH_FrameTransmitHandler_HPC_HAZARD(IEthernetController * /*controller*/, const EthernetFrameTransmitEvent &frameTransmitEvent)
{
    if (frameTransmitEvent.status == EthernetTransmitStatus::Transmitted)
    {
        std::cout << ">> ACK for Ethernet frame with userContext=" << frameTransmitEvent.userContext << std::endl;
    }
    else
    {
        std::cout << ">> NACK for Ethernet frame with userContext=" << frameTransmitEvent.userContext;
        switch (frameTransmitEvent.status)
        {
        case EthernetTransmitStatus::Transmitted:
            break;
        case EthernetTransmitStatus::InvalidFrameFormat:
            std::cout << ": InvalidFrameFormat";
            break;
        case EthernetTransmitStatus::ControllerInactive:
            std::cout << ": ControllerInactive";
            break;
        case EthernetTransmitStatus::LinkDown:
            std::cout << ": LinkDown";
            break;
        case EthernetTransmitStatus::Dropped:
            std::cout << ": Dropped";
            break;
        }

        std::cout << std::endl;
    }
}

void ETH_FrameHandler_HPC_HAZARD(IEthernetController * /*controller*/, const EthernetFrameEvent &frameEvent)
{
    auto frame = frameEvent.frame;
    auto payload = GetPayloadHexFromFrame(frame);
    std::cout << "\n>> Received Ethernet frame from HPC HAZARD: \n";
    ETH_HAZARD_paylaod = payload;
    PrintHexFrame(payload);

    {
        std::lock_guard<std::mutex> lk(std::mutex);
        flagCheck_HAZARD_DataReceived = true;
    }
}

void ETH_FrameTransmitHandler_HPC_TRUNK(IEthernetController * /*controller*/, const EthernetFrameTransmitEvent &frameTransmitEvent)
{
    if (frameTransmitEvent.status == EthernetTransmitStatus::Transmitted)
    {
        // std::cout << ">> ACK for Ethernet frame with userContext=" << frameTransmitEvent.userContext << std::endl;
    }
    else
    {
        // std::cout << ">> NACK for Ethernet frame with userContext=" << frameTransmitEvent.userContext;
        switch (frameTransmitEvent.status)
        {
        case EthernetTransmitStatus::Transmitted:
            break;
        case EthernetTransmitStatus::InvalidFrameFormat:
            std::cout << ": InvalidFrameFormat";
            break;
        case EthernetTransmitStatus::ControllerInactive:
            std::cout << ": ControllerInactive";
            break;
        case EthernetTransmitStatus::LinkDown:
            std::cout << ": LinkDown";
            break;
        case EthernetTransmitStatus::Dropped:
            std::cout << ": Dropped";
            break;
        }

        std::cout << std::endl;
    }
}

void ETH_FrameHandler_HPC_TRUNK(IEthernetController * /*controller*/, const EthernetFrameEvent &frameEvent)
{
    auto frame = frameEvent.frame;
    auto payload = GetPayloadHexFromFrame(frame);
    std::cout << "\n>> Ethernet frame from HPC TRUNK: \n";
    ETH_TRUNK_paylaod = payload;
    {
        std::lock_guard<std::mutex> lk(std::mutex);
        flagCheckTrunkDataReceived = true;
    }
    PrintHexFrame(payload);
}

void SendFrame_ZONALB_2_REST_HAZARD(ICanController *controller, ILogger *logger)
{
    CanFrame canFrame{};

    canFrame.canId = 0x101;
    std::cout << "SENDING HAZARD CAN frame ID : " << canFrame.canId << "\n";
    canFrame.flags |= static_cast<CanFrameFlagMask>(CanFrameFlag::Fdf)    // FD Format Indicator
                      | static_cast<CanFrameFlagMask>(CanFrameFlag::Brs); // Bit Rate Switch (for FD Format only)

    std::vector<uint8_t> payload(ETH_HAZARD_paylaod.begin() + 36, ETH_HAZARD_paylaod.end());

    canFrame.dataField = payload;
    canFrame.dlc = static_cast<uint16_t>(canFrame.dataField.size());

    controller->SendFrame(std::move(canFrame));
    std::stringstream buffer;
    std::cout << "SENDING CAN PAYLOAD 0X101 OF HAZARD TO PLANT MODEL : \n";
    PrintHexFrame(payload);
    flagCheck_HAZARD_DataReceived = false;
}

void SendFrame_ZONALB_2_REST_TRUNK(ICanController *controller, ILogger *logger)
{
    CanFrame canFrame{};
    canFrame.canId = 0x103;
    canFrame.flags |= static_cast<CanFrameFlagMask>(CanFrameFlag::Fdf)    // FD Format Indicator
                      | static_cast<CanFrameFlagMask>(CanFrameFlag::Brs); // Bit Rate Switch (for FD Format only)

    std::vector<uint8_t> payload(ETH_TRUNK_paylaod.begin() + 36, ETH_TRUNK_paylaod.end());

    canFrame.dataField = payload;
    canFrame.dlc = static_cast<uint16_t>(canFrame.dataField.size());
    std::cout << "Sending TRUNK CAN Frame ID \n" << canFrame.canId << "\n";
    controller->SendFrame(std::move(canFrame));
    std::stringstream buffer;
    logger->Info(buffer.str());

    PrintHexFrame(payload);
    flagCheckTrunkDataReceived = false;
}


void FrameTransmitHandler_TrunkSend(const CanFrameTransmitEvent& ack, ILogger* logger)
{
    std::stringstream buffer;
    buffer << "==> " << ack.status << " for CAN frame with canId= 0x "  
    << std::setw(3) << std::setfill('0') << std::hex 
    << ack.canId;
    logger->Info(buffer.str());
}

int main(int argc, char **argv)
{

    if (argc < 3)
    {
        std::cerr << "Missing arguments! Start demo with: " << argv[0]
                  << " <ParticipantConfiguration.yaml|json> <ParticipantName> [RegistryUri] [--async]" << std::endl
                  << "Use \"ZONAL_B as <ParticipantName>." << std::endl;
        return -1;
    }

    if (argc > 5)
    {
        std::cerr << "Too many arguments! Start demo with: " << argv[0]
                  << " <ParticipantConfiguration.yaml|json> <ParticipantName> [RegistryUri] [--async]" << std::endl
                  << "Use \"ZONAL_B as <ParticipantName>." << std::endl;
        return -1;
    }

    std::string participantName(argv[2]);
    if (participantName != "ZONAL_B")
    {
        std::cout << "Wrong participant name provided. Use either \"ZONAL_B\" "
                  << std::endl;
        return 1;
    }
    // std::cout << "SIL Kit Version: " << SilKit::Version::String() << std::endl;
    try
    {
        std::string participantConfigurationFilename(argv[1]);
        // std::string registryUri{"silkit://localhost:8500"};

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
                // registryUri = arg.c_str();
            }
        }

        using SilKit::CreateParticipant;
        using SilKit::Config::ParticipantConfigurationFromFile;

        auto participantConfiguration = ParticipantConfigurationFromFile(participantConfigurationFilename);

        std::cout << "Creating participant '" << participantName << std::endl;
        auto participant = CreateParticipant(participantConfiguration, participantName);

        auto operationMode = (runSync ? OperationMode::Coordinated : OperationMode::Autonomous);
        auto *lifecycleService = participant->CreateLifecycleService({operationMode});

        // Observe state changes
        lifecycleService->SetStopHandler([]()
                                         { std::cout << "Stop handler called" << std::endl; });
        lifecycleService->SetShutdownHandler([]()
                                             { std::cout << "Shutdown handler called" << std::endl; });

        auto *logger = participant->GetLogger();

        // Receiving HAZARD from HPC => ZonalB
        auto *ethernetController_HPC_HAZARD = participant->CreateEthernetController("Eth2", "Eth2");

        // Receiving TRUNK data from HPC to ZonalB
        auto *ethernetController_HPC_TRUNK = participant->CreateEthernetController("Eth3", "Eth3");

        ethernetController_HPC_HAZARD->AddFrameHandler(&ETH_FrameHandler_HPC_HAZARD);
        ethernetController_HPC_HAZARD->AddFrameTransmitHandler(&ETH_FrameTransmitHandler_HPC_HAZARD);

        ethernetController_HPC_TRUNK->AddFrameHandler(&ETH_FrameHandler_HPC_TRUNK);
        ethernetController_HPC_TRUNK->AddFrameTransmitHandler(&ETH_FrameTransmitHandler_HPC_TRUNK);

        auto *canController1 = participant->CreateCanController("CAN2", "CAN2");
        auto *canController2 = participant->CreateCanController("CAN3", "CAN3");

        canController2->AddFrameTransmitHandler([logger](ICanController* /*ctrl*/, const CanFrameTransmitEvent& ack) {
            FrameTransmitHandler_TrunkSend(ack, logger);
        });

        std::promise<void> startHandlerPromise;
        auto startHandlerFuture = startHandlerPromise.get_future();
        bool isStopped = false;
        std::thread workerThread;
        // Set a CommunicationReady Handler
        lifecycleService->SetCommunicationReadyHandler([&]()
                                                       {
			std::cout << "Communication ready handler called for " << participantName << std::endl;
            canController1->SetBaudRate(10'000, 1'000'000, 2'000'000);
           canController2->SetBaudRate(10'000, 1'000'000, 2'000'000);


           
			workerThread = std::thread{ [&]() {
				startHandlerFuture.get();
				while (!isStopped)
				{
					if (participantName == "ZONAL_B")
					{
                       
						if(flagCheck_HAZARD_DataReceived)
                        {                           					                 
                         SendFrame_ZONALB_2_REST_HAZARD(canController1, logger);                /// CAN sending //hazard
                        }
                         if(flagCheckTrunkDataReceived)
                         {
                             SendFrame_ZONALB_2_REST_TRUNK(canController2, logger);/// CAN sending //trunk
                         }
					}
					std::this_thread::sleep_for(1s);
				}
				lifecycleService->Stop("User requested to Stop");
			} };
			ethernetController_HPC_HAZARD->Activate();
            ethernetController_HPC_TRUNK->Activate();
            canController1->Start();
            canController2->Start(); 
            }); //// can_controller starting

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
        lifecycleService->Stop("User requested to Stop");
        // }
        // }
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
