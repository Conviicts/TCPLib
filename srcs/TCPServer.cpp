#include "TCPServer.hpp"
#include "UniquePtr.hpp"

namespace TCP {

    bool alive = true;

    const size_t TCPServer::defaultMaxConnections = 1024;
    const timeval TCPServer::selectTimeout = {5, 0};

    void abortSelect(int sig) throw() {
        (void)sig;
        alive = false;
    }

    TCPServer::TCPServer():_maxConnections(1024) {
        bzero(&_hint, sizeof(_hint));
        _hint.ai_family = AF_UNSPEC;
        _hint.ai_socktype = SOCK_STREAM;
        _hint.ai_protocol = IPPROTO_TCP;
        _hint.ai_flags = AI_PASSIVE;
        signal(SIGINT, abortSelect);
        signal(SIGPIPE, SIG_IGN);
        siginterrupt(SIGINT, 1);
    }

    TCPServer::~TCPServer() throw() {
        while (!_connections.empty())
            disconnect(_connections.front());
        for (std::list<TCPListener *>::iterator it = _listeners.begin(); it != _listeners.end(); it++)
            delete *it;
    }

    size_t TCPServer::connectionsSize() const { return (_connections.size()); }

    void TCPServer::setMaxConnections(size_t maxConnections) { _maxConnections = maxConnections; }

    void TCPServer::listen(const std::string &port, size_t maxQueueLen) {
        addrinfo *head, *node;
        int i = 0;

        if (getaddrinfo(NULL, port.c_str(), &_hint, &head))
            throw std::runtime_error("getaddrinfo() failed");
        node = head;
        try {
            while (node && i < 2) {
                TCP::unique_ptr<TCPListener> listener;
                listener.reset(new TCPListener(node->ai_family));
                listener->setReuseAddr(1);
                listener->setNonBlocking(true);
                if (node->ai_family == AF_INET6)
                    listener->setIPV6Only(1);
                listener->bind(node->ai_addr, node->ai_addrlen);
                listener->listen(maxQueueLen);
                _listeners.push_back(listener.release());
                node = node->ai_next;
                i++;
            }
        } catch (std::exception &e) {
            freeaddrinfo(head);
            throw;
        }
        freeaddrinfo(head);
    }

    void TCPServer::disconnect(TCPSocket *connection) throw() {
        _pending.remove(connection);
        _connections.remove(connection);
        delete connection;
    }

    TCPSocket *TCPServer::getNextNewConnection() throw() {
        if (_incoming.empty())
            return (NULL);
        TCPListener *listener = _incoming.front();
        _incoming.pop_front();
        TCP::unique_ptr<TCPSocket> newConnection;
        try {
            newConnection.reset(listener->accept());
            if (_connections.size() == _maxConnections)
                throw std::exception();
            _connections.push_back(newConnection.get());
        } catch (std::exception &e) {
            return getNextNewConnection();
        }
        return newConnection.release();
    }

    TCPSocket *TCPServer::getNextPendingConnection() throw() {
        if (_pending.empty())
            return (NULL);
        TCPSocket *next = _pending.front();
        _pending.pop_front();
        return (next);
    }

    void TCPServer::select() {
        fd_set readSet;
        fd_set writeSet;
        struct timeval timeout = selectTimeout;

        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        for (std::list<TCPListener *>::iterator it = _listeners.begin(); it != _listeners.end(); it++)
            FD_SET((*it)->fd(), &readSet);
        for (std::list<TCPSocket *>::iterator it = _connections.begin(); it != _connections.end(); it++) {
            TCPSocket *c = *it;
            FD_SET(c->fd(), &readSet);
            if (!c->bufferIsEmpty())
                FD_SET(c->fd(), &writeSet);
        }
        if (alive)
            ::select(FD_SETSIZE + 1, &readSet, &writeSet, NULL, &timeout);
        if (!alive)
            throw std::exception();
        for (std::list<TCPListener *>::iterator it = _listeners.begin(); it != _listeners.end(); it++)
            if (FD_ISSET((*it)->fd(), &readSet))
                _incoming.push_back(*it);
        for (std::list<TCPSocket *>::iterator it = _connections.begin(); it != _connections.end(); it++) {
            TCPSocket *c = *it;
            if (FD_ISSET(c->fd(), &readSet))
                c->_canRead = true;
            if (FD_ISSET(c->fd(), &writeSet))
                c->_canWrite = true;
            if (c->canRead() || c->_canWrite)
                _pending.push_back(c);
        }
    }
}
