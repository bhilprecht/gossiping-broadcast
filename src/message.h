#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace message
{
    enum MessageType
    {
        Init,
        InitOk,
        Broadcast,
        BroadcastOk,
        Read,
        ReadOk,
        Topology,
        TopologyOk,
        Gossip,
        GossipOk,
        Invalid
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(MessageType, {
                                                  {Init, "init"},
                                                  {InitOk, "init_ok"},
                                                  {Broadcast, "broadcast"},
                                                  {BroadcastOk, "broadcast_ok"},
                                                  {Read, "read"},
                                                  {ReadOk, "read_ok"},
                                                  {Topology, "topology"},
                                                  {TopologyOk, "topology_ok"},
                                                  {Gossip, "gossip"},
                                                  {GossipOk, "gossip_ok"},
                                                  {Invalid, nullptr},
                                              })

    class Message
    {
    private:
        std::string src;
        std::string dest;
        MessageType type;
        json body;

    public:
        Message(std::string &src, std::string &dest, MessageType type, json &body) : src(src), dest(dest), type(type), body(body){};

        static std::unique_ptr<Message> parse(std::string line);
        std::unique_ptr<Message> generate_response(MessageType &type, size_t msg_id, json &body);

        void send(std::mutex &write_mtx);

        MessageType const &get_type() const;

        json const &get_body() const;

        std::string const& get_src() const;

        std::string const& get_dest() const;
    };
}
