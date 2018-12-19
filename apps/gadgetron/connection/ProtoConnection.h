#pragma once

#include "Connection.h"
#include "Config.h"
#include "Channel.h"
#include "Context.h"

namespace Gadgetron::Server::Connection {

    class ProtoConnection {
    public:
        static boost::optional<Config> process(
                std::iostream &stream,
                const Core::Context::Paths &paths
        );

    private:
        ProtoConnection(Gadgetron::Core::Context::Paths paths, std::iostream &stream);
        ~ProtoConnection();

        using MessageChannel = Gadgetron::Core::MessageChannel;

        void process_input();
        void process_output();

        std::promise<boost::optional<Config>> promise;

        std::shared_ptr<MessageChannel> channel;
        std::iostream &stream;

        struct {
            std::thread input, output;
        } threads;

        const Gadgetron::Core::Context::Paths paths;
    };

}

