#include "node.h"
#include <utility>

namespace node
{
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
            m_resp->send();

            break;
        }
        case message::MessageType::Topology:
        {
            this->topology = body["topology"];
            this->neighbors = this->topology.find(this->node_id)->second;

            json body_resp = {};
            auto type = message::MessageType::TopologyOk;
            auto m_resp = m.generate_response(type, get_msg_id(), body_resp);
            m_resp->send();

            break;
        }
        case message::MessageType::Broadcast:
        {
            size_t message = body["message"];
            this->messages.insert(message);
            
            json body_resp = {};
            auto type = message::MessageType::BroadcastOk;
            auto m_resp = m.generate_response(type, get_msg_id(), body_resp);
            m_resp->send();

            break;
        }
        case message::MessageType::Read:
        {
            json body_resp = {{"messages", this->messages}};
            auto type = message::MessageType::ReadOk;
            auto m_resp = m.generate_response(type, get_msg_id(), body_resp);
            m_resp->send();

            break;
        }
        }
    }

    size_t Node::get_msg_id()
    {
        return msg_id++;
    }
}
