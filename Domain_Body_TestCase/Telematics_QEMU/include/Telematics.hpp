#ifndef TELEMATICS_HPP
#define TELEMATICS_HPP
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <array>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/ethernet.h> // For Ethernet header
#include <netinet/ip.h>   // For IP header
#include <netinet/udp.h>  // For UDP header
#include <sys/ioctl.h>
#include <net/if.h>
#include <netpacket/packet.h> // For sockaddr_ll
#include <vector>

#include "TrunkData.hpp"

#define UDP_ADDRESS "192.168.1.2" // Replace with your actual UDP destination IP

// Functions defined for Debugging Purposes
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
    std::cout << std::dec << "\n";
}

int get_interface_index(const char *ifname)
{
    int sockfd;
    struct ifreq ifr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "Socket creation failed" << "\n";
        return -1;
    }

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0)
    {
        std::cerr << "Failed to get interface index for " << ifname << "\n";
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return ifr.ifr_ifindex;
}
void send_udp_message(const std::string &message)
{
    char buffer[60]; // Buffer to hold the complete packet

    std::vector<uint8_t> byte_sequence;
    if (message.compare("Trunk: Open") == 1)
    {
        byte_sequence =
            {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F,
                0x80, 0x00, 0x57, 0x38, 0x4F, 0xA8, 0xFB, 0x0D,
                0x06, 0x00, 0x2B, 0x00, 0xB8, 0x38, 0x4F, 0xA8,
                0xFB, 0x0D, 0x06, 0x00, 0x1D, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    }
    else if (message.compare("Trunk: Closed") == 1)
    {
        byte_sequence =
            {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F,
                0x00, 0x00, 0x57, 0x38, 0x4F, 0xA8, 0xFB, 0x0D,
                0x06, 0x00, 0x2B, 0x00, 0xB8, 0x38, 0x4F, 0xA8,
                0xFB, 0x0D, 0x06, 0x00, 0x1D, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    }

    TRUNK_FRAME = byte_sequence;
}

void listen_to_mqtt(const std::string &broker_url, const std::string &topic)
{
    std::string command = "mosquitto_sub -h " + broker_url + " -t " + topic;
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    std::cout << "Subscribed to topic: " << topic << " on broker: " << broker_url << "\n";

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        std::string output(buffer.data());
        std::cout << "\n=============================\nReceived payload: " << output;
        {
            std::lock_guard<std::mutex> lk(std::mutex);
            CHECK_MQTT = true;
        }
        send_udp_message(output);
    }
}

void publish_to_mqtt(const std::string &broker_url, const std::string &topic_response, const std::string &Pub_message)
{
    std::string command = "mosquitto_pub -h " + broker_url + " -t " + topic_response + " -m " + Pub_message;
    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    std::cout << "Published the message to topic: " << topic_response << " on broker: " << broker_url << "\n";
}

#endif // TELEMATICS_HPP
