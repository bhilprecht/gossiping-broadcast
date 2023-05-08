#pragma once

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "message.h"

namespace node
{
    class Node {
        private:
            size_t msg_id = 1;
            std::string node_id;
            std::vector<std::string> node_ids;
            std::unordered_map<std::string, std::vector<std::string>> topology;
            std::vector<std::string> neighbors;

            std::unordered_set<size_t> messages;

        public:
            void handle(message::Message &m);

            size_t get_msg_id();
    };
}
