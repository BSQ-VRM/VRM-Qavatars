#include "VMC/VMCServer.hpp"

#include "config/ConfigManager.hpp"
#include "VMC/util.hpp"

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

            /*VRMLogger.info("packet address %s", msg.address());

            if(msg == "/VMC/Ext/T")
            {
                VRMLogger.info("time! %f", args.float32());
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
                VRMLogger.info("Ok! %d, %d, %d", loaded, calibState, calibMode);
            }

            if(msg == "/VMC/Ext/Root/Pos")
            {
                VRMLogger.info("Root Pos! %s {%f %f %f} {%f %f %f %f}", args.string(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32());
            }

            if(msg == "/VMC/Ext/Bone/Pos")
            {
                VRMLogger.info("Bone Pos! %s {%f %f %f} {%f %f %f %f}", args.string(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32());
            }

            if(msg == "/VMC/Ext/Con/Pos")
            {
                VRMLogger.info("Controller Pos! %s {%f %f %f} {%f %f %f %f}", args.string(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32());
            }

            if(msg == "/VMC/Ext/Hmd/Pos")
            {
                VRMLogger.info("Hmd Pos! %s {%f %f %f} {%f %f %f %f}", args.string(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32(), args.float32());
            }*/

            if(msg == "/VMC/Ext/Tra/Pos")
            {
                const std::string name = args.string();
                const UnityEngine::Vector3 pos = {args.float32(), args.float32(), args.float32()};
                UnityEngine::Quaternion rot = {args.float32(), args.float32(), args.float32(), args.float32()};

                availableTrackers.push_back({name, pos, rot});
                const auto euler = rot.get_eulerAngles();
                VRMLogger.info("Tracker Pos! {} [{} {} {}] [{} {} {}]", name.c_str(), pos.x, pos.y, pos.z, euler.x, euler.y, euler.z);
            }
        }
    }

    void VMCServer::InitServer()
    {
        //VRMLogger.info("x1");
        if(socket)
        {
            socket.shutdown();
        }
        const auto config = Config::ConfigManager::GetVMCSettings();
        //VRMLogger.info("x2 %s", config.recvPort.c_str());
        if(config.enableReceiver)
        {
            //VRMLogger.info("x3 %d", IntUtil::to_int(config.recvPort.c_str()));
            socket = kissnet::udp_socket(kissnet::endpoint("0.0.0.0", IntUtil::to_int(config.recvPort.c_str())));
            //VRMLogger.info("x4");
            socket.bind();
            //VRMLogger.info("x5");
            socket.listen();
            //VRMLogger.info("x6");
        }
    }

    void VMCServer::Receive()
    {
        const auto config = Config::ConfigManager::GetVMCSettings();
        if(config.enableReceiver)
        {
            if(socket.get_status() == kissnet::socket_status::valid)
            {
                if(socket.bytes_available() > 0)
                {
                    kissnet::buffer<8192> buff;
                    auto [received_bytes, status] = socket.recv(buff);

                    const auto from = socket.get_recv_endpoint();
                    if(received_bytes > 0)
                    {
                        handlePacket(OSCPP::Server::Packet(buff.data(), received_bytes));
                    }
                    else
                    {
                    }
                }
            }
        }
    }
}
