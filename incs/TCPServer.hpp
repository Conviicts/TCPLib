#pragma once
#include <list>
#include <netdb.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdexcept>
#include <memory>

#include "TCPListener.hpp"
#include "TCPSocket.hpp"

namespace TCP {
    class TCPServer {
        public:
            TCPServer();
            virtual ~TCPServer() throw();

            size_t				connectionsSize() const;
            void				setMaxConnections(size_t MaxConnections);


            void				listen(const std::string &port, size_t maxQueueLen = 5);
            void				disconnect(TCPSocket *client) throw();

            TCPSocket			*getNextNewConnection() throw();
            TCPSocket			*getNextPendingConnection() throw();

            void				select();

        private:

            static const size_t     defaultMaxConnections;
            static const timeval    selectTimeout;

            addrinfo				_hint;
            std::list<TCPListener*>	_listeners;
            std::list<TCPListener*>	_incoming;
            std::list<TCPSocket*>	_connections;
            std::list<TCPSocket*>	_pending;
            size_t					_maxConnections;

            TCPServer(const TCPServer &);
            TCPServer &operator=(const TCPServer &);
    };
}
