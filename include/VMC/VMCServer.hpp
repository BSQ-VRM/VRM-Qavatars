#pragma once

#include <sombrero/shared/FastQuaternion.hpp>
#include <sombrero/shared/FastVector3.hpp>

#include "kissnet.hpp"
#include "oscpp/server.hpp"

namespace VRMQavatars::VMC
{
    struct Tracker
    {
        std::string name;
        Sombrero::FastVector3 pos;
        Sombrero::FastQuaternion rot;
    };

    class VMCServer
    {
    public:
        static std::vector<Tracker> availableTrackers;
        static void InitServer();
        static void Receive();
    private:
        static kissnet::udp_socket socket;

        static void handlePacket(const OSCPP::Server::Packet& packet);
    };
}
