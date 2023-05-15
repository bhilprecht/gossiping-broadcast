#include "node.h"

namespace node
{
    void Node::gossip()
    {
        for (auto &n : neighbors)
        {
            auto to_be_delivered_msgs = this->to_deliver[n];
            if (to_be_delivered_msgs.empty())
            {
                continue;
            }

            auto type = message::MessageType::Gossip;
            json body = {
                {"messages", to_be_delivered_msgs},
                {"type", type},
                {"msg_id", msg_id}};

            auto m = message::Message(node_id, n, type, body);
            m.send(write_mtx);
        }
    }

    void Node::handle(message::Message &m)
    {
        json body = m.get_body();
        switch (m.get_type())
        {
        case message::MessageType::Init:
        {
            this->node_id = body["node_id"];
            this->node_ids = body["node_ids"];

            json body_resp = {};
            auto type = message::MessageType::InitOk;
            auto m_resp = m.generate_response(type, get_msg_id(), body_resp);
            m_resp->send(write_mtx);

            break;
        }
        case message::MessageType::InitOk:
        {
            // ignored 
            break;
        }
        case message::MessageType::Topology:
        {
            this->topology = body["topology"];
            this->neighbors = this->topology.find(this->node_id)->second;

            json body_resp = {};
            auto type = message::MessageType::TopologyOk;
            auto m_resp = m.generate_response(type, get_msg_id(), body_resp);
            m_resp->send(write_mtx);

            break;
        }
        case message::MessageType::TopologyOk:
        {
            // ignored 
            break;
        }
        case message::MessageType::Broadcast:
        {
            size_t message = body["message"];
            recv_message(message);

            json body_resp = {};
            auto type = message::MessageType::BroadcastOk;
            auto m_resp = m.generate_response(type, get_msg_id(), body_resp);
            m_resp->send(write_mtx);

            break;
        }
        case message::MessageType::BroadcastOk:
        {
            // ignored 
            break;
        }
        case message::MessageType::Read:
        {
            json body_resp = {{"messages", this->messages}};
            auto type = message::MessageType::ReadOk;
            auto m_resp = m.generate_response(type, get_msg_id(), body_resp);
            m_resp->send(write_mtx);

            break;
        }
        case message::MessageType::ReadOk:
        {
            // ignored 
            break;
        }
        case message::MessageType::Gossip:
        {
            // just send the content back
            auto type = message::MessageType::GossipOk;
            std::vector<size_t> messages = body["messages"];
            for (auto &message : messages)
            {
                // the sending node knows the message
                register_knows(m.get_src(), message);
                recv_message(message);
            }

            json body_resp = {
                {"messages", messages}};
            auto m_resp = m.generate_response(type, get_msg_id(), body_resp);
            m_resp->send(write_mtx);

            break;
        }
        case message::MessageType::GossipOk:
        {
            std::vector<size_t> messages = body["messages"];
            for (auto &message : messages)
            {
                // the sending node knows the message
                register_knows(m.get_src(), message);
            }
            break;
        }
        case message::MessageType::Invalid:
        {
            std::cerr << "Received invalid message" << std::endl;
            break;
        }
        }
    }

    void Node::recv_message(size_t &message)
    {
        this->messages.insert(message);

        // other nodes might not know it
        for (auto &n : neighbors)
        {
            if (this->knows[n].find(message) == this->knows[n].end())
            {
                this->to_deliver[n].insert(message);
            }
        }
    }

    void Node::start_gossip_thread(int interval)
    {
        std::thread([this, interval]()
                    {
        while (true) {
            gossip();                   
            std::this_thread::sleep_for(
            std::chrono::milliseconds(interval));
        } })
            .detach();
    }

    void Node::register_knows(std::string const &node, size_t &message)
    {
        this->knows[node].insert(message);
        this->to_deliver[node].erase(message);
    }

    size_t Node::get_msg_id()
    {
        return msg_id++;
    }
}
