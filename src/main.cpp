#include <iostream>
#include <string>
#include <utility>
#include "message.h"
#include "node.h"
#include <thread>

int main(void)
{
    std::unique_ptr<node::Node> n = std::unique_ptr<node::Node>(new node::Node());

    // spawn the background job
    std::cerr << "Init gossip thread... " << std::endl;
    n->start_gossip_thread(100);

    for (std::string line; std::getline(std::cin, line);)
    {
        // Log message using std::cerr as described in protocol
        std::cerr << "Received msg: " << line << std::endl;

        auto m = message::Message::parse(line);
        n->handle(*m);
    }
    return 0;
}
