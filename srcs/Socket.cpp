#include "Socket.hpp"

namespace TCP {
    Socket::Socket() : _fd(-1) {
        memset(&_addr, 0, sizeof(_addr));
    }

    Socket::~Socket() throw() { }

    int Socket::fd() { return _fd; }

    const sockaddr_storage &Socket::addr() const {
        return _addr;
    }

    sa_family_t Socket::family() const {
        return _addr.ss_family;
    }

    void Socket::socket(int family) {
        if (_fd != -1)
            throw std::runtime_error("Socket already created");
        _fd = ::socket(family, SOCK_STREAM, 0);
        if (_fd == -1)
            throw std::runtime_error("Socket creation failed");
    }

    void Socket::bind(const sockaddr *addr, socklen_t len) {
        if (_fd == -1)
            throw std::runtime_error("Socket not created");
        if (::bind(_fd, addr, len) == -1)
            throw std::runtime_error("Socket binding failed");
        std::memcpy(&_addr, addr, len);
    }

    int Socket::close() throw() {
        if (_fd == -1)
            return 0;
        int ret = ::close(_fd);
        _fd = -1;
        return ret;
    }

    void Socket::setReuseAddr(bool reuse) {
        if (_fd == -1)
            throw std::runtime_error("Socket not created");
        int opt = reuse ? 1 : 0;
        if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
            throw std::runtime_error("Socket option SO_REUSEADDR failed");
    }

    void Socket::setNonBlocking(bool nonBlocking) {
        if (_fd == -1)
            throw std::runtime_error("Socket not created");
        int flags = fcntl(_fd, F_GETFL, 0);
        if (flags == -1)
            throw std::runtime_error("Socket fcntl F_GETFL failed");
        if (nonBlocking)
            flags |= O_NONBLOCK;
        else
            flags &= ~O_NONBLOCK;
        if (fcntl(_fd, F_SETFL, flags) == -1)
            throw std::runtime_error("Socket fcntl F_SETFL failed");
    }

    void Socket::setIPV6Only(bool ipv6Only) {
        if (_fd == -1)
            throw std::runtime_error("Socket not created");
        // if (family() != AF_INET6)
        //     throw std::runtime_error("Socket is not IPV6");
        int opt = ipv6Only ? 1 : 0;
        if (setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) == -1)
            throw std::runtime_error("Socket option IPV6_V6ONLY failed");
    }
}