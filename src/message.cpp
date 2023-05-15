#include "message.h"

namespace message
{
    std::unique_ptr<Message> Message::parse(std::string line)
    {
        json js = json::parse(line);
        // Mandatory parameters
        MessageType type = js["body"]["type"].get<MessageType>();
        std::string src = js["src"];
        std::string dest = js["dest"];
        json body = js["body"];

        return std::unique_ptr<Message>(new Message(src, dest, type, body));
    }

    std::unique_ptr<Message> Message::generate_response(MessageType &type, size_t msg_id, json &body)
    {
        body["msg_id"] = msg_id;
        body["type"] = type;
        body["in_reply_to"] = get_body()["msg_id"];
        return std::unique_ptr<Message>(new Message(dest, src, type, body));
    }

    void Message::send(std::mutex &write_mtx)
    {
        json js = {
            {"src", src},
            {"dest", dest},
            {"body", body},
        };
        // Synchronize the sending by acquiring a mutex
        // This is necessary because otherwise messages 
        // will overlap.
        std::lock_guard<std::mutex> guard(write_mtx);
        std::cerr << "Sending: " << js << std::endl;
        std::cout << js << std::endl;
    }

    json const& Message::get_body() const
    {
        return body;
    }

    MessageType const& Message::get_type() const
    {
        return type;
    }

    std::string const& Message::get_src() const
    {
        return src;
    }

    std::string const& Message::get_dest() const
    {
        return dest;
    }
}
