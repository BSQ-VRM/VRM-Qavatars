#include "VMC/VMCServer.hpp"

#include "config/ConfigManager.hpp"

namespace VRMQavatars::VMC
{
    kissnet::udp_socket VMCServer::socket;
    std::vector<Tracker> VMCServer::availableTrackers;

    void VMCServer::handlePacket(const OSCPP::Server::Packet& packet)
    {
        if (packet.isBundle()) {
            const OSCPP::Server::Bundle bundle(packet);

            OSCPP::Server::PacketStream packets(bundle.packets());

            availableTrackers.clear();

            while (!packets.atEnd()) {
                handlePacket(packets.next());
            }
        } else {
            const OSCPP::Server::Message msg(packet);

            OSCPP::Server::ArgStream args(msg.args());

            //getLogger().info("packet address %s", msg.address());

            /*if(msg == "/VMC/Ext/T")
            {
                getLogger().info("time! %f", args.float32());
            }

            if(msg == "/VMC/Ext/Ok")
            {
                int loaded = -1;
                int calibState = -1;
                int calibMode = -1;
                if(!args.atEnd())
                {
                    loaded = args.int32();
                    if(!args.atEnd())
                    {
                        calibState = args.int32();
                        if(!args.atEnd())
                        {
                            calibMode = args.int32();
                        }
                    }
                }
                getLogger().info("Ok! %d, %d, %d", loaded, calibState, calibMode);
            }

            if(msg == "/VMC/Ext/Root/Pos")
            {
                getLogger().info("Root Pos! %s {%f %f %f} {%f %f %f %f}", args.string(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32());
            }

            if(msg == "/VMC/Ext/Bone/Pos")
            {
                getLogger().info("Bone Pos! %s {%f %f %f} {%f %f %f %f}", args.string(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32());
            }

            if(msg == "/VMC/Ext/Con/Pos")
            {
                getLogger().info("Controller Pos! %s {%f %f %f} {%f %f %f %f}", args.string(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32());
            }

            if(msg == "/VMC/Ext/Hmd/Pos")
            {
                getLogger().info("Hmd Pos! %s {%f %f %f} {%f %f %f %f}", args.string(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32());
            }*/

            if(msg == "/VMC/Ext/Tra/Pos")
            {
                std::string name = args.string();
                UnityEngine::Vector3 pos = {args.float32(), args.float32(), args.float32()};
                UnityEngine::Quaternion rot = {args.float32(), args.float32(), args.float32(), args.float32()};

                availableTrackers.push_back({name, pos, rot});
                const auto euler = rot.get_eulerAngles();
                //getLogger().info("Tracker Pos! %s {%f %f %f} {%f %f %f}", name.c_str(), pos.x, pos.y, pos.z, euler.x, euler.y, euler.z);
            }
        }
    }

    void VMCServer::InitServer()
    {
        if(socket)
        {
            socket.shutdown();
        }
        const auto config = Config::ConfigManager::GetVMCSettings();
        if(config.enableSender)
        {
            socket = kissnet::udp_socket(kissnet::endpoint("0.0.0.0", 39539));
            socket.bind();
            socket.listen();
        }
    }

    void VMCServer::Receive()
    {
        //getLogger().info("available %zu", socket.bytes_available());
        //if(socket.bytes_available() > 0)
        //{
            kissnet::buffer<8192> buff;
            auto [received_bytes, status] = socket.recv(buff);
            //getLogger().info("received bytes: %zu status: %d", received_bytes, status.value);

            const auto from = socket.get_recv_endpoint();
            //getLogger().info("Receieved! From: %s", from.address.c_str());
            if(received_bytes > 0)
            {
                handlePacket(OSCPP::Server::Packet(buff.data(), received_bytes));
            }
            else
            {
                //getLogger().info("no bytes available");
            }
        //}
    }
}
