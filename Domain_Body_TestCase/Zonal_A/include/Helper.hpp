#ifndef HELPER_HPP
#define HELPER_HPP


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
#include "silkit/SilKitVersion.hpp"
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
bool flagCheckCANFrame{false};

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
	} 
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

// ============================================= CAN Frames Logic =============================================
void CAN_FrameTransmitHandler(const CanFrameTransmitEvent &ack, ILogger *logger)
{
	std::stringstream buffer;
	buffer << ">> " << ack.status << " for CAN frame with timestamp=" << ack.timestamp;
	logger->Info(buffer.str());
}

void CAN_FrameHandler(const CanFrameEvent &frameEvent, ILogger *logger)
{
	Can_payload.clear();
	std::vector<uint8_t> payload(frameEvent.frame.dataField.begin(), frameEvent.frame.dataField.end());
	std::stringstream buffer;

	std::cout << "\n=================== Received CAN Payload: ===================\n";
	
    {
		std::lock_guard<std::mutex> lk(std::mutex);
		flagCheckCANFrame = true;
	}

	PrintHexFrame(payload);

	for (int i = 0; i < frameEvent.frame.dlc; i++)
	{

		Can_payload.push_back((frameEvent.frame.dataField[i]) & 0xFF);
	}

	logger->Info(buffer.str());
}


// ============================================= ETH Frames Logic =============================================
void ETH_FrameTransmitHandler(IEthernetController * /*controller*/, const EthernetFrameTransmitEvent &frameTransmitEvent)
{
	if (frameTransmitEvent.status == EthernetTransmitStatus::Transmitted)
	{
		// std::cout << ">> ACK for Ethernet frame" << std::endl;
	}
	else
	{
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

void ETH_FrameHandler(IEthernetController * /*controller*/, const EthernetFrameEvent &frameEvent)
{
	auto frame = frameEvent.frame;
}


std::vector<uint8_t> CreateFrame(const std::vector<uint8_t> &payload)
{
	std::vector<uint8_t> raw;
	EthernetMac sourceAddress = {0x66, 0x55, 0x44, 0x33, 0x22, 0x22};
	EthernetMac destinationAddress = {0x66, 0x55, 0x44, 0x33, 0x22, 0x33};

	std::vector<uint8_t> pre_payload = 
    {
        0x08, 0x00, 0x45, 0x00,
        0x00, 0x64, 0x00, 0x00,
        0x40, 0x00, 0x40, 0x11,
        0xB7, 0x69, 0xC0, 0xA8,
        0x01, 0x08, 0xC0, 0xA8,
        0x01, 0x02, 0x13, 0x59,
        0x13, 0x58, 0x00, 0x50,
        0x00, 0x00, 0x00, 0x00,
        0x14, 0x01, 0x00, 0x00,
        0x00, 0x40
    };

	std::copy(sourceAddress.begin(), sourceAddress.end(), std::back_inserter(raw));
	std::copy(destinationAddress.begin(), destinationAddress.end(), std::back_inserter(raw));
	std::copy(pre_payload.begin(), pre_payload.end(), std::back_inserter(raw));
	std::copy(payload.begin(), payload.end(), std::back_inserter(raw));

	return raw;
}

void ETH_SendFrame(IEthernetController *controller)
{
	static int frameId = 0x1401;
	const auto userContext = reinterpret_cast<void *>(static_cast<intptr_t>(frameId));

	if(flagCheckCANFrame)
	{
		auto frame = CreateFrame(Can_payload);

		controller->SendFrame(EthernetFrame{frame}, userContext);
		std::cout << "\n<< ETH Frame from ZONAL_A sent!" << std::endl;
		PrintHexFrame(frame);
		std::cout << "\n";

		flagCheckCANFrame = false;
	}
}

#endif // HELPER_HPP
