#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "message.h"

namespace node
{
    class Node
    {
    private:
        size_t msg_id = 1;
        std::string node_id;
        std::vector<std::string> node_ids;
        // used for synchronizing writes to the "network", i.e., stdout
        std::mutex write_mtx;
        // used for internal data structures
        std::mutex data_mtx;
        std::unordered_map<std::string, std::vector<std::string>> topology;
        std::vector<std::string> neighbors;

        std::unordered_set<size_t> messages;
        std::unordered_map<std::string, std::unordered_set<size_t>> knows;
        std::unordered_map<std::string, std::unordered_set<size_t>> to_deliver;
        std::shared_ptr<boost::asio::io_service> io_service_;
        std::shared_ptr<boost::asio::io_service::work> work_;
        boost::thread_group threadpool_;

    public:
        Node(size_t num_worker);

        void handle(std::string &line);
        void process_message(std::string &line);
        void gossip();

        void recv_message(size_t &message);
        void register_knows(std::string const &node, size_t &message);

        void start_gossip_thread(int interval);

        size_t get_msg_id();
    };
}
