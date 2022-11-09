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
        if ((_fd = ::socket(family, SOCK_STREAM, IPPROTO_TCP)) == -1)
            throw std::runtime_error("socket() failed");
    }

    void Socket::bind(const sockaddr *addr, socklen_t len) {
        if (::bind(_fd, addr, len) == -1)
            throw std::runtime_error("bind() failed");
        std::memcpy(&_addr, &addr, len);
    }

    int Socket::close() throw() {
        if (_fd == -1)
            return 0;
        int ret = ::close(_fd);
        _fd = -1;
        return ret;
    }

    void Socket::setReuseAddr() {
        int reuse = 1;
        if (::setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
            throw std::runtime_error("setsockopt() failed");
    }

    void Socket::setNonBlocking(bool nonBlocking) {
        if (nonBlocking) {
            if (fcntl(_fd, F_SETFL, O_NONBLOCK) == -1)
                throw std::runtime_error("fcntl() failed");
        } else {
            int oldfl;
            if ((oldfl = fcntl(_fd, F_GETFL)) == -1)
                throw std::runtime_error("fcntl() failed");
            fcntl(_fd, F_SETFL, oldfl & ~O_NONBLOCK);
        }
    }

    void Socket::setIPV6Only() {
        int v6Only = 1;
        if (::setsockopt(_fd, IPPROTO_IPV6, IPV6_V6ONLY, &v6Only, sizeof(v6Only)) == -1)
            throw std::runtime_error("setsockopt() failed");
    }
}