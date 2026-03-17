#ifndef HELPER_SIL_KIT_HPP
#define HELPER_SIL_KIT_HPP

// Vector SIL Kit Libraries
#include "silkit/SilKit.hpp"
#include "silkit/SilKitVersion.hpp"
#include "silkit/services/all.hpp"
#include "silkit/services/orchestration/all.hpp"
#include "silkit/services/orchestration/string_utils.hpp"

// Including Helper file for Telematics MQTT Communication
#include "Telematics.hpp"
#include "TrunkData.hpp"

// Vector SIL Kit Namespaces
using namespace SilKit::Services::Orchestration;
using namespace SilKit::Services::Ethernet;
using namespace std::chrono_literals;

// Field in a frame that can indicate the protocol, payload size, or the start of a VLAN tag
using EtherType = uint16_t;
using EthernetMac = std::array<uint8_t, 6>;

// Operator << Overloading for std::chrono library
std::ostream &operator<<(std::ostream &out, std::chrono::nanoseconds timestamp)
{
    auto seconds = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1, 1>>>(timestamp);
    out << seconds.count() << "s";
    return out;
}

// Sending ETH Frames
void SendFrame(IEthernetController *controller)
{

    if (CHECK_MQTT)
    {
        auto frame1 = TRUNK_FRAME;
        controller->SendFrame(EthernetFrame{frame1});

        std::cout << "-- Sending TRUNK Data --\n";
        PrintHexFrame(frame1);
        CHECK_MQTT = false;
    }
}

// Frame Transmit Handler to check the transmitted event status
void FrameTransmitHandler(IEthernetController * /*controller*/, const EthernetFrameTransmitEvent &frameTransmitEvent)
{
    if (frameTransmitEvent.status == EthernetTransmitStatus::Transmitted)
    {
        std::cout << ">> ACK for Ethernet frame" << "\n";
    }
    else
    {
        std::cout << ">> NACK for Ethernet frame: ";
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

        std::cout << "\n";
    }
}

#endif // HELPER_SIL_KIT_HPP
