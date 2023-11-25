#pragma once

#include <UnityEngine/Quaternion.hpp>
#include <UnityEngine/Vector3.hpp>

#include "kissnet.hpp"
#include "oscpp/client.hpp"
#include "oscpp/server.hpp"

namespace VRMQavatars::VMC
{
    struct Tracker
    {
        std::string name;
        UnityEngine::Vector3 pos;
        UnityEngine::Quaternion rot;
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
