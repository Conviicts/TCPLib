#pragma once

#include <string>
#include <cerrno>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdexcept>
#include <algorithm>

#include "Socket.hpp"
#include "Types.hpp"

namespace TCP {
    class TCPSocket : public Socket {
        public:
            TCPSocket();
            TCPSocket(int fd);
            virtual ~TCPSocket() throw();

            const std::string   &ip() const;
            const std::string   &host() const;

            uint16_t            port() const;
            size_t              bufferSize() const;

            bool                haveData();
            bool                canRead();

            bool                read(ByteArray &data);
            void                write(const ByteArray &data);

            virtual bool		fill();
            virtual void		flush();

        protected:
            virtual int		    recv(void *buffer, size_t size, int flags = 0);
            virtual int		    send(const void *buffer, size_t size, int flags = 0);
            virtual bool	    bufferIsEmpty() const;

            std::string         _ip;
            std::string         _host;
            ByteArray           _readBuffer;
            ByteArray           _writeBuffer;

            uint16_t            _port;
            size_t              _content;

            bool                _canRead;
            bool                _canWrite;

        private:
            TCPSocket(const TCPSocket &other);
            TCPSocket &operator=(const TCPSocket &other);

            friend class TCPListener;
            friend class TCPServer;
    };
}