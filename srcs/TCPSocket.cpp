#include "TCPSocket.hpp"

#include <iostream>

namespace TCP {
    TCPSocket::TCPSocket() : _content(std::string::npos), _canRead(false), _canWrite(false) { }

    TCPSocket::TCPSocket(int fd) : Socket(), _content(std::string::npos), _canRead(false), _canWrite(false) {
        socklen_t len = sizeof(_addr);
        if ((_fd = accept(fd, (sockaddr *)&_addr, &len)) == -1)
            throw std::runtime_error("accept() failed");
        if (family() == AF_INET) {
            _port = ntohs(((sockaddr_in *)&_addr)->sin_port);
            _ip = inet_ntoa(((sockaddr_in *)&_addr)->sin_addr);
        } else if (family() == AF_INET6) {
            char buffer[INET6_ADDRSTRLEN];
            _port = ntohs(((sockaddr_in6 *)&_addr)->sin6_port);
            if (!(inet_ntop(AF_INET6, &_addr, buffer, INET6_ADDRSTRLEN))) {
                close();
                throw std::runtime_error("inet_ntop() failed");
            }
            _ip = buffer;
        } else {
            close();
            throw std::runtime_error("unknown address family");
        }
        char buffer[NI_MAXHOST];
        if (!getnameinfo((sockaddr *)&_addr, len, buffer, sizeof(buffer), NULL, 0, NI_NAMEREQD))
            _host = buffer;
        else
            _host = _ip;
    }

    TCPSocket::~TCPSocket() throw() { close(); }

    const std::string &TCPSocket::ip() const { return _ip; }
    const std::string &TCPSocket::host() const { return _host; }

    uint16_t TCPSocket::port() const { return _port; }

    size_t TCPSocket::bufferSize() const {
        return _writeBuffer.size();
    }

    bool TCPSocket::haveData() {
        ByteArray::iterator it = std::find(_readBuffer.begin(), _readBuffer.end(), '\n');
        ByteArray::iterator it2 = std::find(_readBuffer.begin(), _readBuffer.end(), '\r');

        if (_content == 0) {
            if (it == _readBuffer.end() || (_content = (it - _readBuffer.begin())) == 0)
                if (it2 == _readBuffer.end() || (_content = (it2 - _readBuffer.begin())) == 0)
                    return false;
        }
        return true;
    }

    bool TCPSocket::canRead() { return (haveData() || _canRead); }

    bool TCPSocket::read(ByteArray &data) {
        data.clear();
        if (!haveData() && !fill())
            return false;
        if (!haveData())
            return true;
        std::copy(_readBuffer.begin(), _readBuffer.end(), std::back_inserter(data));
        // data = _readBuffer;
        _readBuffer.clear();
        _content = 0;
        return (true);
    }

    void TCPSocket::write(const ByteArray &data) {
        _writeBuffer.insert(_writeBuffer.end(), data.begin(), data.end());
    }

    bool TCPSocket::fill() {
        if (_canRead) {
            char buffer[2048 + 1];
            int nb;
            _canRead = false;
            if (!(nb = recv(buffer, 2048)))
                return false;
            buffer[nb > 0 ? nb : 0] = '\0';
            _readBuffer.insert(_readBuffer.end(), buffer, buffer + strlen(buffer));
        }
        return true;
    }

    void TCPSocket::flush() {
        if (!_canWrite || bufferIsEmpty())
            return ;
        _canWrite = false;
        send(_writeBuffer.data(), _writeBuffer.size());
        _writeBuffer.clear();
    }

    int TCPSocket::recv(void *buffer, size_t size, int flags) {
        int ret;
        if ((ret = ::recv(_fd, buffer, size, flags)) == -1)
            if (errno != EWOULDBLOCK)
                throw std::runtime_error("recv() failed");
        return (ret);
    }

    int TCPSocket::send(const void *buffer, size_t size, int flags) {
        int ret;
        if ((ret = ::send(_fd, buffer, size, flags)) == -1)
            if (errno != EWOULDBLOCK)
                throw std::runtime_error("send() failed");
        return (ret);
    }

    bool TCPSocket::bufferIsEmpty() const { return (_writeBuffer.empty()); }
}
