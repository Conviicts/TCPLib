#pragma once

#include <sys/socket.h>
#include <stdexcept>

#include "TCPSocket.hpp"

namespace TCP {
    class TCPListener : public Socket {
        public:
            TCPListener(int family);
            virtual ~TCPListener() throw();

            virtual TCPSocket*	accept();
            void listen(int maxQueuLen = 50);

        private:
            TCPListener();
            TCPListener(const TCPListener &);
            TCPListener &operator=(const TCPListener &);
    };
}