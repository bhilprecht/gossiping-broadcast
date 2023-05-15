#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>

#include "message.h"

namespace node
{
    class Node
    {
    private:
        size_t msg_id = 1;
        std::string node_id;
        std::vector<std::string> node_ids;
        std::mutex write_mtx;
        std::unordered_map<std::string, std::vector<std::string>> topology;
        std::vector<std::string> neighbors;

        std::unordered_set<size_t> messages;
        std::unordered_map<std::string, std::unordered_set<size_t>> knows;
        std::unordered_map<std::string, std::unordered_set<size_t>> to_deliver;

    public:
        void handle(message::Message &m);
        void gossip();

        void recv_message(size_t &message);
        void register_knows(std::string const &node, size_t &message);

        void start_gossip_thread(int interval);

        size_t get_msg_id();
    };
}
