
#include <algorithm>
#include <cstring>
#include <future>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "silkit/SilKit.hpp"
#include "silkit/SilKitVersion.hpp"

#include "silkit/services/all.hpp"
#include "silkit/services/orchestration/all.hpp"
#include "silkit/services/orchestration/string_utils.hpp"

using namespace SilKit::Services::Orchestration;
using namespace SilKit::Services::Ethernet;

using namespace std::chrono_literals;
std::mutex mtx;

using EtherType = uint16_t;
using EthernetMac = std::array<uint8_t, 6>;

std::vector<uint8_t> PAYLOAD_FROM_ZONAL_A;
std::vector<uint8_t> PAYLOAD_FROM_HPC_TO_ZONALB;
std::vector<bool> Bits_FROM_ZONAL_A_to_HPC;
std::vector<uint8_t> PAYLOAD_FROM_TELEMATIC;
bool flag_telematic = false;
bool flag_hazard = false;
bool trunk_response =false;
bool ignition_data_available = false;

void Extract_bits(uint8_t info)
{
    Bits_FROM_ZONAL_A_to_HPC.clear(); // Clear any existing Bits_FROM_ZONAL_A_to_HPC
    for (int i = 7; i >= 0; --i)
    {
        Bits_FROM_ZONAL_A_to_HPC.push_back((info >> i) & 1); // Extract each bit
    }
    std::cout << "\nextracted bits from Zonal A to HPC: ";
    for (int i = 0; i < 8; i++)
    {
        std::cout << Bits_FROM_ZONAL_A_to_HPC[i] << " ";
    }
    std::cout << "\n";
    ignition_data_available = true;
}

void Send_HAZARD_2_ZONAL(IEthernetController *controller);

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

std::ostream &operator<<(std::ostream &out, std::chrono::nanoseconds timestamp)
{
    auto seconds = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(timestamp);
    out << seconds.count() << "s";
    return out;
}

std::vector<uint8_t> GetPayloadHexFromFrame(const EthernetFrame &frame)
{
    const size_t FrameHeaderSize = 2 * sizeof(EthernetMac) + sizeof(EtherType);

    std::vector<uint8_t> payload;
    payload.insert(payload.end(), frame.raw.begin() + FrameHeaderSize, frame.raw.end());
    return payload;
}

void FrameTransmitHandler_ZONAL_A(IEthernetController * /*controller*/, const EthernetFrameTransmitEvent &frameTransmitEvent)
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

void FrameHandler_ZONAL_A(IEthernetController * /*controller*/, const EthernetFrameEvent &frameEvent, std::unique_ptr<SilKit::IParticipant> &participant)
{
    auto frame = frameEvent.frame;
    PAYLOAD_FROM_ZONAL_A = GetPayloadHexFromFrame(frame);
    std::cout << "\n>> Ethernet frame from ZONAL A: \n";
    PrintHexFrame(PAYLOAD_FROM_ZONAL_A);

    {
        std::lock_guard<std::mutex> lH(std::mutex);
        Extract_bits(PAYLOAD_FROM_ZONAL_A[44]);
        flag_hazard = true;
    }
}

void FrameTransmitHandler_TELEMATIC(IEthernetController * /*controller*/, const EthernetFrameTransmitEvent &frameTransmitEvent)
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

void FrameHandle_TELEMATIC(IEthernetController * /*controller*/, const EthernetFrameEvent &frameEvent)
{
    auto frame = frameEvent.frame;
    PAYLOAD_FROM_TELEMATIC = GetPayloadHexFromFrame(frame);
    std::cout << "\n>> Ethernet frame from TELEMATIC ECU: \n";
    PrintHexFrame(PAYLOAD_FROM_TELEMATIC);

    if (ignition_data_available)
    {

        std::lock_guard<std::mutex> lock(mtx);
        flag_telematic = true;
    }
    else
    {
        std::cout << "NO Ignition and Vspeed data available" << std::endl;
    }
}

std::vector<uint8_t>
CreateFrame_1(const std::vector<uint8_t> &payload)
{

    std::vector<uint8_t> raw;
    EthernetMac sourceAddress = {0x66, 0x55, 0x44, 0x33, 0x22, 0x33};
    EthernetMac destinationAddress = {0x66, 0x55, 0x44, 0x33, 0x22, 0x11};

    std::vector<uint8_t> pre_payload = {0x08, 0x00};

    PAYLOAD_FROM_HPC_TO_ZONALB[30] = 0x11;
    PAYLOAD_FROM_HPC_TO_ZONALB[31] = 0x01;

    // SRC Address
    PAYLOAD_FROM_HPC_TO_ZONALB[12] = 0xC0;
    PAYLOAD_FROM_HPC_TO_ZONALB[13] = 0xA8;
    PAYLOAD_FROM_HPC_TO_ZONALB[14] = 0x01; // zonalB
    PAYLOAD_FROM_HPC_TO_ZONALB[15] = 0x02;
    // DST Address
    PAYLOAD_FROM_HPC_TO_ZONALB[16] = 0xC0;
    PAYLOAD_FROM_HPC_TO_ZONALB[17] = 0xA8; // qemu
    PAYLOAD_FROM_HPC_TO_ZONALB[18] = 0x01;
    PAYLOAD_FROM_HPC_TO_ZONALB[19] = 0x05;

    std::copy(destinationAddress.begin(), destinationAddress.end(), std::back_inserter(raw));
    std::copy(sourceAddress.begin(), sourceAddress.end(), std::back_inserter(raw));
    std::copy(pre_payload.begin(), pre_payload.end(), std::back_inserter(raw));
    std::copy(payload.begin(), payload.end(), std::back_inserter(raw));

    return raw;
}

std::vector<uint8_t> CreateFrame_2(const std::vector<uint8_t> &payload)
{

    std::vector<uint8_t> raw;
    EthernetMac sourceAddress = {0x66, 0x55, 0x44, 0x33, 0x22, 0x33};
    EthernetMac destinationAddress = {0x66, 0x55, 0x44, 0x33, 0x22, 0x11};

    std::vector<uint8_t> pre_payload = {0x08, 0x00, 0x45, 0x00,
                                        0x00, 0x64, 0x00, 0x00,
                                        0x40, 0x00, 0x40, 0x11,
                                        0xB7, 0x69, 0xC0, 0xA8,
                                        0x01, 0x02, 0xC0, 0xA8,
                                        0x01, 0x05, 0x13, 0x59,
                                        0x13, 0x58, 0x00, 0x50,
                                        0x00, 0x00, 0x00, 0x00,
                                        0x13, 0x01, 0x00, 0x00, 0x00, 0x40,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    std::copy(destinationAddress.begin(), destinationAddress.end(), std::back_inserter(raw));
    std::copy(sourceAddress.begin(), sourceAddress.end(), std::back_inserter(raw));

    std::copy(pre_payload.begin(), pre_payload.end(), std::back_inserter(raw));
    std::copy(payload.begin(), payload.end(), std::back_inserter(raw));

    return raw;
}

std::vector<uint8_t> CreateFrame_2_tele(const std::vector<uint8_t> &payload)
{

    std::vector<uint8_t> raw;
    EthernetMac sourceAddress = {0x66, 0x55, 0x44, 0x33, 0x22, 0x33};
    EthernetMac destinationAddress = {0x66, 0x55, 0x44, 0x33, 0x22, 0x55};

    std::vector<uint8_t> pre_payload = {0x08, 0x00, 0x45, 0x00,
                                        0x00, 0x64, 0x00, 0x00,
                                        0x40, 0x00, 0x40, 0x11,
                                        0xB7, 0x69, 
                                        0xC0, 0xA8,0x01, 0x02,  //ip of hpc
                                        0xC0, 0xA8,0x01, 0x07,  //ip of telematic
                                        0x13, 0x59,
                                        0x13, 0x58, 0x00, 0x50,
                                        0x00, 0x00, 0x00, 0x00,
                                        0x13, 0x01, 0x00, 0x00, 0x00, 0x40,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    std::copy(destinationAddress.begin(), destinationAddress.end(), std::back_inserter(raw));
    std::copy(sourceAddress.begin(), sourceAddress.end(), std::back_inserter(raw));

    std::copy(pre_payload.begin(), pre_payload.end(), std::back_inserter(raw));
    std::copy(payload.begin(), payload.end(), std::back_inserter(raw));

    return raw;
}

void Send_TELEMATIC_2_ZONAL(IEthernetController *controller)// LOGIC TO CHECK SPEED NAD TRUNK
{
   
        if (PAYLOAD_FROM_TELEMATIC[1] == 0x80 and Bits_FROM_ZONAL_A_to_HPC[2] == 0)
        {
            std::cout << "================= TRUNK CAN BE OPEN Vechile IS STILL =================\n";
            trunk_response =true;

            
        }
        else if (PAYLOAD_FROM_TELEMATIC[1] == 0x80 and Bits_FROM_ZONAL_A_to_HPC[2] == 1)
        {
            std::cout << "================= TRUNK CANNOT OPEN Vechile IS MOVING  =================\n";
            PAYLOAD_FROM_TELEMATIC[1]=0x00;
            trunk_response =true;

            
        }
        else if (PAYLOAD_FROM_TELEMATIC[1] == 0x00 )
        {
            std::cout << "================= CLOSING TRUNK =================\n";
            trunk_response =true;

        }

        auto frame2 = CreateFrame_2(PAYLOAD_FROM_TELEMATIC);
        std::cout << "\nSENDING TRUNK FRAME TO ZONAL B : \n";
        controller->SendFrame(EthernetFrame{frame2});
        
        PrintHexFrame(frame2);

        flag_telematic = false;
    
}

void Send_HAZARD_2_ZONAL(IEthernetController *controller)
{
    static int frameId1 = 0x1101;

    const auto userContext1 = reinterpret_cast<void *>(static_cast<intptr_t>(frameId1));

    while (Bits_FROM_ZONAL_A_to_HPC[0] == 1 and Bits_FROM_ZONAL_A_to_HPC[4] == 1)
    {
        PAYLOAD_FROM_HPC_TO_ZONALB = PAYLOAD_FROM_ZONAL_A;

        std::cout << "\n++BLINKING!!++\n";
        PAYLOAD_FROM_HPC_TO_ZONALB[44] = 0xFE; // on
        auto frameON = CreateFrame_1(PAYLOAD_FROM_HPC_TO_ZONALB);
        controller->SendFrame(EthernetFrame{frameON}, userContext1); // sent
        std::this_thread::sleep_for(std::chrono::milliseconds(333)); // sleep
        std::cout << "<< ETH FrameON sent" << std::endl;
        PrintHexFrame(frameON);

        PAYLOAD_FROM_HPC_TO_ZONALB[44] = 0x00; // off
        auto frameOFF = CreateFrame_1(PAYLOAD_FROM_HPC_TO_ZONALB);
        controller->SendFrame(EthernetFrame{frameOFF}, userContext1); // sent
        std::this_thread::sleep_for(std::chrono::milliseconds(333));  // sleep
        std::cout << "<< ETH FrameOFF sent" << std::endl;
        PrintHexFrame(frameOFF);
    }
    std::cout << "\n-- STOP BLINKING!! -- \n";
    PAYLOAD_FROM_HPC_TO_ZONALB = PAYLOAD_FROM_ZONAL_A;
    auto frame1 = CreateFrame_1(PAYLOAD_FROM_HPC_TO_ZONALB);
    controller->SendFrame(EthernetFrame{frame1}, userContext1);
    std::cout << "\nSENDING HAZARD FRAME : \n";
    flag_hazard = false;

    PrintHexFrame(frame1);
}

void Send_HPC_2_TELEMATIC(IEthernetController *controller)
{
    if (trunk_response)
    {
        auto frame = CreateFrame_2_tele(PAYLOAD_FROM_TELEMATIC);
        // if (PAYLOAD_FROM_TELEMATIC[0] == 0x80)
        // {
        //     std::cout << "================= TRUNK ON =================\n";
        // }
        // else if (PAYLOAD_FROM_TELEMATIC[0] == 0x00)
        // {
        //     std::cout << "================= TRUNK OFF =================\n";
        // }

        std::cout << "\nSENDING TRUNK RESPONSE FRAME TO  TELEMATIC : \n";
        controller->SendFrame(EthernetFrame{frame});

        PrintHexFrame(frame);

        trunk_response = false;
    }
}

int main(int argc, char **argv)
{

    if (argc < 3)
    {
        std::cerr << "Missing arguments! Start demo with: " << argv[0]
                  << " <ParticipantConfiguration.yaml|json> <ParticipantName> [RegistryUri] [--async]" << std::endl
                  << "Use \"HPC" << std::endl;
        return -1;
    }

    if (argc > 5)
    {
        std::cerr << "Too many arguments! Start demo with: " << argv[0]
                  << " <ParticipantConfiguration.yaml|json> <ParticipantName> [RegistryUri] [--async]" << std::endl
                  << "Use \"HPC" << std::endl;
        return -1;
    }

    std::string participantName(argv[2]);
    if (participantName != "HPC")
    {
        std::cout << "Wrong participant name provided. Use either \"HPC"
                  << std::endl;
        return 1;
    }
    std::cout << "SIL Kit Version: " << SilKit::Version::String() << std::endl;
    try
    {
        std::string participantConfigurationFilename(argv[1]);

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
        }

        using SilKit::CreateParticipant;
        using SilKit::Config::ParticipantConfigurationFromFile;

        auto participantConfiguration = ParticipantConfigurationFromFile(participantConfigurationFilename);

        std::cout << "Creating participant '" << participantName << std::endl;
        auto participant = CreateParticipant(participantConfiguration, participantName);
        auto *ethernetController = participant->CreateEthernetController("Eth4", "Eth4"); // listen from zonal A
        auto *ethernetController_TELEMATIC = participant->CreateEthernetController("Eth1", "Eth1");

        auto *ethernetController_HPC_2_ZONAL_1 = participant->CreateEthernetController("Eth2", "Eth2");

        auto *ethernetController_HPC_2_ZONAL_2 = participant->CreateEthernetController("Eth3", "Eth3");

        ethernetController->AddFrameHandler(
            [&](IEthernetController *controller, const EthernetFrameEvent &frameEvent)
            {
                FrameHandler_ZONAL_A(controller, frameEvent, participant);
            });
        ethernetController->AddFrameTransmitHandler(&FrameTransmitHandler_ZONAL_A);

        ethernetController_TELEMATIC->AddFrameHandler(
            [&](IEthernetController *controller, const EthernetFrameEvent &frameEvent)
            {
                FrameHandle_TELEMATIC(controller, frameEvent);
            });
        ethernetController_TELEMATIC->AddFrameTransmitHandler(&FrameTransmitHandler_TELEMATIC);

        auto operationMode = (runSync ? OperationMode::Coordinated : OperationMode::Autonomous);
        auto *lifecycleService = participant->CreateLifecycleService({operationMode});

        // Observe state changes
        lifecycleService->SetStopHandler([]()
                                         { std::cout << "Stop handler called" << std::endl; });
        lifecycleService->SetShutdownHandler([]()
                                             { std::cout << "Shutdown handler called" << std::endl; });
        lifecycleService->SetAbortHandler(
            [](auto lastState)
            { std::cout << "Abort handler called while in state " << lastState << std::endl; });

        std::promise<void> startHandlerPromise;
        auto startHandlerFuture = startHandlerPromise.get_future();
        std::atomic<bool> isStopRequested = {false};
        std::thread workerThread;
        std::thread workerThread_telematic;

        lifecycleService->SetCommunicationReadyHandler([&]()
                                                       {
            std::cout << "Communication ready handler called for " << participantName << std::endl;
            workerThread = std::thread{[&]()
                                        {
                                            startHandlerFuture.get();
                                            while (lifecycleService->State() == ParticipantState::ReadyToRun || lifecycleService->State() == ParticipantState::Running)
                                            {
                                                if (participantName == "HPC" and flag_hazard == true)
                                                {
                                                  Send_HAZARD_2_ZONAL(ethernetController_HPC_2_ZONAL_1);
                                                }
                                                std::this_thread::sleep_for(1s);
                                            }
                                            if (!isStopRequested)
                                            {
                                                std::cout << "Press enter to end the process..." << std::endl;
                                            }
                                        }};

                                         workerThread_telematic = std::thread{[&]()
                                        {
                                            startHandlerFuture.get();
                                            while (lifecycleService->State() == ParticipantState::ReadyToRun || lifecycleService->State() == ParticipantState::Running)
                                            {
                                                
                                                 if (participantName == "HPC" and flag_telematic == true)
                                                {
                                                  Send_TELEMATIC_2_ZONAL(ethernetController_HPC_2_ZONAL_2);
                                                  Send_HPC_2_TELEMATIC(ethernetController_TELEMATIC);
                                                 
                                                }
                                                std::this_thread::sleep_for(1s);
                                            }
                                            if (!isStopRequested)
                                            {
                                                std::cout << "Press enter to end the process..." << std::endl;
                                            }
                                        }};
            ethernetController->Activate();
            ethernetController_HPC_2_ZONAL_1->Activate();
            ethernetController_HPC_2_ZONAL_2->Activate();
            ethernetController_TELEMATIC->Activate(); });

        lifecycleService->SetStartingHandler([&]()
                                             { startHandlerPromise.set_value(); });

        lifecycleService->StartLifecycle();
        std::cout << "Press enter to leave the simulation..." << std::endl;
        std::cin.ignore();

        isStopRequested = true;
        if (lifecycleService->State() == ParticipantState::Running || lifecycleService->State() == ParticipantState::Paused)
        {
            std::cout << "User requested to stop in state " << lifecycleService->State() << std::endl;
            lifecycleService->Stop("User requested to stop");
        }
        if (workerThread.joinable())
        {
            workerThread.join();
        }
        if (workerThread_telematic.joinable())
        {
            workerThread_telematic.join();
        }
        std::cout << "The participant has shut down and left the simulation" << std::endl;
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
