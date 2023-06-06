#include "node.h"

namespace node
{
    Node::Node(size_t num_worker)
    {
        // creates the threadpool to handle incoming messages
        io_service_ = std::make_shared<boost::asio::io_service>();
        work_ = std::make_shared<boost::asio::io_service::work>(*io_service_);
        for (std::size_t i = 0; i < num_worker; ++i)
        {
            threadpool_.create_thread(boost::bind(&boost::asio::io_service::run, io_service_));
        }
    }

    void Node::gossip()
    {
        // obtain a lock to internal data structures
        std::lock_guard<std::mutex> guard(data_mtx);
        for (auto &n : neighbors)
        {
            // send to every neighbor what it we know but it does
            // not know potentially
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

    void Node::handle(std::string &line)
    {
        // delegate message handling to a thread
        io_service_->post(boost::bind(&Node::process_message, this, line));
    }

    void Node::process_message(std::string &line)
    {
        auto m = message::Message::parse(line);
        json body = m->get_body();

        // obtain a lock to internal data structures
        // to avoid deadlocks with the other mutex for sending 
        // data, this lock is always acquired first.
        std::lock_guard<std::mutex> guard(data_mtx);


        switch (m->get_type())
        {
        case message::MessageType::Init:
        {
            // process init message and set node_ids
            this->node_id = body["node_id"];
            this->node_ids = body["node_ids"];

            // acknowledge the message
            json body_resp = {};
            auto type = message::MessageType::InitOk;
            auto m_resp = m->generate_response(type, get_msg_id(), body_resp);
            m_resp->send(write_mtx);

            break;
        }
        case message::MessageType::Topology:
        {
            // process topology message and set neighbor nodes
            this->topology = body["topology"];
            this->neighbors = this->topology.find(this->node_id)->second;

            // acknowledge the message
            json body_resp = {};
            auto type = message::MessageType::TopologyOk;
            auto m_resp = m->generate_response(type, get_msg_id(), body_resp);
            m_resp->send(write_mtx);

            break;
        }
        case message::MessageType::Broadcast:
        {
            // a new message has arrived and has to be acknowledged
            size_t message = body["message"];
            recv_message(message);

            json body_resp = {};
            auto type = message::MessageType::BroadcastOk;
            auto m_resp = m->generate_response(type, get_msg_id(), body_resp);
            m_resp->send(write_mtx);

            break;
        }
        case message::MessageType::Read:
        {
            // send back all known messages to the requesting node
            json body_resp = {{"messages", this->messages}};
            auto type = message::MessageType::ReadOk;
            auto m_resp = m->generate_response(type, get_msg_id(), body_resp);
            m_resp->send(write_mtx);

            break;
        }
        case message::MessageType::Gossip:
        {
            // just send the content back to make sure the other
            // node knows we know all messages
            auto type = message::MessageType::GossipOk;
            std::vector<size_t> messages = body["messages"];
            for (auto &message : messages)
            {
                // the sending node knows the message
                register_knows(m->get_src(), message);
                recv_message(message);
            }

            json body_resp = {
                {"messages", messages}};
            auto m_resp = m->generate_response(type, get_msg_id(), body_resp);
            m_resp->send(write_mtx);

            break;
        }
        case message::MessageType::GossipOk:
        {
            // this is a reply to a gossip message. Just
            // mark that the sending node has seen all these
            // messages
            std::vector<size_t> messages = body["messages"];
            for (auto &message : messages)
            {
                // the sending node knows the message
                register_knows(m->get_src(), message);
            }
            break;
        }
        // ignored
        case message::MessageType::ReadOk:
        {
            break;
        }
        case message::MessageType::BroadcastOk:
        {
            break;
        }
        case message::MessageType::InitOk:
        {
            break;
        }
        case message::MessageType::TopologyOk:
        {
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
