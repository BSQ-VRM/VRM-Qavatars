#pragma once

#include <UnityEngine/Quaternion.hpp>
#include <UnityEngine/Vector3.hpp>

#include "kissnet.hpp"
#include "oscpp/client.hpp"

namespace VRMQavatars::VMC
{
    class VMCClient
    {
    public:
        static void InitClient();

        static void SendHeadsetPos(UnityEngine::Vector3 pos, UnityEngine::Quaternion rot);
        //Send Left then Right!!! why? idk
        static void SendControllerPos(UnityEngine::Vector3 pos, UnityEngine::Quaternion rot, int idx);

        static void SetDestination(std::string address, std::string port);
    private:
        static kissnet::udp_socket socket;
    };
}