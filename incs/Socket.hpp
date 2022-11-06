#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace TCP {
    class Socket {
        public:
            Socket();
            virtual ~Socket() throw();

            int                     fd();
            const sockaddr_storage  &addr() const;
            sa_family_t             family() const;
            void                    socket(int family);
            void                    bind(const sockaddr *addr, socklen_t len);
            int                     close() throw();

            void                    setReuseAddr(bool reuse);
            void                    setNonBlocking(bool nonBlocking);
            void                    setIPV6Only(bool ipv6Only);

            protected:
                int                 _fd;
                sockaddr_storage    _addr;
            
            private:
                Socket(const Socket &other);
                Socket &operator=(const Socket &other);
    };
}