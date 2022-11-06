#include "TCPListener.hpp"

namespace TCP {
    TCPListener::TCPListener(int family) : Socket() { socket(family); }

    TCPListener::~TCPListener() throw() { close(); }

    void TCPListener::listen(int maxQueueLen) {
        if (::listen(_fd, maxQueueLen) == -1)
            throw std::runtime_error("listen() failed");
    }

    TCPSocket *TCPListener::accept() { return (new TCPSocket(_fd)); }
}
