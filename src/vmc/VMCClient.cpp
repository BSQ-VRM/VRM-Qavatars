#include "VMC/VMCClient.hpp"

#include <UnityEngine/Quaternion.hpp>
#include <UnityEngine/Vector3.hpp>

#include "config/ConfigManager.hpp"

namespace VRMQavatars::VMC
{
    //Safe int conversion function, returns -1 on empty, skips non numerical chars
    int to_int(char const *s)
    {
        if ( s == NULL || *s == '\0' )
            return -1;

        bool negate = (s[0] == '-');
        if ( *s == '+' || *s == '-' )
            ++s;

        if ( *s == '\0')
            return -1;

        if ( *s == ' ')
            return -1;

        int result = 0;
        while(*s)
        {
            if ( *s < '0' || *s > '9' )
            {
                ++s;
                continue;
            }
            result = result * 10  - (*s - '0');  //assume negative number
            ++s;
        }
        return negate ? result : -result; //-result is positive!
    }

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
        if(socket)
        {
            socket.shutdown();
        }
        const auto config = Config::ConfigManager::GetVMCSettings();
        if(config.enableSender)
        {
            socket = kissnet::udp_socket(kissnet::endpoint(config.destIP, to_int(config.destPort.c_str())));
        }
    }

    void VMCClient::SendHeadsetPos(const UnityEngine::Vector3 pos, const UnityEngine::Quaternion rot)
    {
        const auto config = Config::ConfigManager::GetVMCSettings();
        if(config.enableSender)
        {
            if(!socket || socket.get_status() != kissnet::socket_status::valid) return;
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
        const auto config = Config::ConfigManager::GetVMCSettings();
        if(config.enableSender)
        {
            if(!socket || socket.get_status() != kissnet::socket_status::valid) return;
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
