#include "VMC/VMCClient.hpp"

#include <UnityEngine/Quaternion.hpp>
#include <UnityEngine/Vector3.hpp>

#include "config/ConfigManager.hpp"
#include "VMC/util.hpp"

namespace VRMQavatars::VMC
{
    size_t makePacket(void* buffer, const size_t size, const std::string& command, const std::string& name, const UnityEngine::Vector3 pos, const UnityEngine::Quaternion rot)
    {
        OSCPP::Client::Packet packet(buffer, size);
        packet
            .openMessage(command.c_str(), 8)
                //name
                .string(name.c_str())
                //pos
                .float32(pos.x)
                .float32(pos.y)
                .float32(pos.z)
                //rot
                .float32(rot.x)
                .float32(rot.y)
                .float32(rot.z)
                .float32(rot.w)
            .closeMessage();
        return packet.size();
    }

    kissnet::udp_socket VMCClient::socket;

    void VMCClient::InitClient()
    {
        //VRMLogger.info("x1");
        if(socket)
        {
            //VRMLogger.info("x2");
            socket.shutdown();
        }
        const auto config = Config::ConfigManager::GetVMCSettings();
        //VRMLogger.info("x3");
        if(config.enableSender)
        {
            //VRMLogger.info("x4, {}, {}", config.destIP.c_str(), to_int(config.destPort.c_str()));
            socket = kissnet::udp_socket(kissnet::endpoint(config.destIP, IntUtil::to_int(config.destPort.c_str())));
        }
    }

    void VMCClient::SendHeadsetPos(const UnityEngine::Vector3 pos, const UnityEngine::Quaternion rot)
    {
        //VRMLogger.info("Send hmd pos");
        const auto config = Config::ConfigManager::GetVMCSettings();
        if(config.enableSender)
        {
            //VRMLogger.info("hmd enabled");
            if(!socket || socket.get_status() != kissnet::socket_status::valid) return;
            //VRMLogger.info("hmd active socket");
            kissnet::buffer<8192> sendBuffer;
            const size_t packetSize = makePacket(sendBuffer.data(),
                                                 sendBuffer.size(),
                                                 "/VMC/Ext/Hmd/Pos",
                                                 "Head",
                                                 pos,
                                                 rot);
            socket.send(sendBuffer.data(), packetSize);
        }
    }

    void VMCClient::SendControllerPos(const UnityEngine::Vector3 pos, const UnityEngine::Quaternion rot, const int idx)
    {
        //VRMLogger.info("Send con pos");
        const auto config = Config::ConfigManager::GetVMCSettings();
        if(config.enableSender)
        {
            //VRMLogger.info("con enabled");
            if(!socket || socket.get_status() != kissnet::socket_status::valid) return;
            //VRMLogger.info("con active socket");
            kissnet::buffer<8192> sendBuffer;
            const size_t packetSize = makePacket(sendBuffer.data(),
                                                 sendBuffer.size(),
                                                 "/VMC/Ext/Con/Pos",
                                                 "con" + std::to_string(idx),
                                                 pos,
                                                 rot);
            socket.send(sendBuffer.data(), packetSize);
        }
    }
}
