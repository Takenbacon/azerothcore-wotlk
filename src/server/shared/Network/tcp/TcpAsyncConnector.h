/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TCPASYNCCONNECTOR_H_
#define __TCPASYNCCONNECTOR_H_

#include <boost/asio.hpp>
#include <memory>

using boost::asio::ip::tcp;

class TcpAsyncConnector : public std::enable_shared_from_this<TcpAsyncConnector>
{
public:
    TcpAsyncConnector(Acore::Asio::IoContext& context): _ioContext(&context), _socket(nullptr),
        _connectTimer(context), _needsReconnect(false), _reconnectTimer(context) { }
    ~TcpAsyncConnector() { Stop(); }

    // Connect once to an endpoint with timeout
    bool ConnectOnce(tcp::endpoint const& endpoint, std::chrono::seconds timeout = std::chrono::seconds(10))
    {
        if (_socket)
            return false; // Connection already in progress, must wait until timed out.

        _endpoint = endpoint;
        _timeout = timeout;

        TryConnect();
        return true;
    }

    // Connect to an endpoint with timeout until successful
    bool ConnectRepeat(tcp::endpoint const& endpoint,
        std::chrono::seconds timeout = std::chrono::seconds(10),
        std::chrono::seconds reconnectTime = std::chrono::seconds(30))
    {
        if (_socket)
            return false; // Connection already in progress, must wait until timed out.

        _endpoint = endpoint;
        _timeout = timeout;

        _needsReconnect = true;
        _reconnectTime = reconnectTime;

        TryConnect();
        return true;
    }

    // Stop attempting any connections
    void Stop()
    {
        _needsReconnect = false;
        _connectTimer.cancel();
        _reconnectTimer.cancel();
        _socket->close();
        _socket = nullptr;
    }

    bool IsAttemptingConnection() const { return _socket.get(); }

private:
    virtual void OnConnectionSuccess(std::unique_ptr<tcp::socket> socket) { }
    virtual void OnConnectionFail() { }
    virtual void OnConnectionTimeout() { }

    void TryConnect()
    {
        // Clean up old socket first
        if (_socket)
            _socket->close();

        // Start timeout timer
        _connectTimer.expires_after(_timeout);
        _connectTimer.async_wait([self = shared_from_this()](boost::system::error_code const& ec)
        {
            self->ConnectionTimeout(ec);
        });

        // Start async connect
        _socket = std::make_unique<tcp::socket>(*_ioContext);
        _socket->async_connect(_endpoint, [self = shared_from_this()](boost::system::error_code const& ec)
        {
            self->SocketConnected(ec);
        });
    }

    void SocketConnected(boost::system::error_code const& ec)
    {
        if (!ec && _socket && _socket->is_open())
            OnConnectionSuccess(std::move(_socket));
        else
        {
            if (_needsReconnect)
                StartReconnectTimer();

            OnConnectionFail();
        }

        // Cancel the connection timeout timer
        _connectTimer.cancel();
    }

    void ConnectionTimeout(boost::system::error_code const& ec)
    {
        if (!ec)
        {
            if (_needsReconnect)
                StartReconnectTimer();

            OnConnectionTimeout();
        }
    }

    void StartReconnectTimer()
    {
        _reconnectTimer.expires_after(_reconnectTime);
        _reconnectTimer.async_wait([self = shared_from_this()](boost::system::error_code const& ec)
        {
            self->TryConnect();
        });
    }

    Acore::Asio::IoContext* _ioContext;
    std::unique_ptr<tcp::socket> _socket;

    tcp::endpoint _endpoint;
    std::chrono::seconds _timeout;

    boost::asio::steady_timer _connectTimer;

    bool _needsReconnect;
    std::chrono::seconds _reconnectTime;
    boost::asio::steady_timer _reconnectTimer;
};

#endif
