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

#ifndef __UDPSOCKET_H__
#define __UDPSOCKET_H__

#include "Log.h"
#include "MessageBuffer.h"
#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/bind.hpp>
#include <functional>
#include <memory>
#include <queue>
#include <type_traits>

using boost::asio::ip::udp;

class UdpSocket
{
public:
    explicit UdpSocket(udp::socket&& socket) : _socket(std::move(socket))
    {
        StartReceive();
    }

    void StartReceive()
    {
        _socket.async_receive_from(
            boost::asio::buffer(_receiveBuffer), _senderEndpoint,
            boost::bind(&UdpSocket::HandleReceive, this, boost::asio::placeholders::error)
        );
    }

protected:
    void HandleReceive(boost::system::error_code const& error)
    {
        LOG_TRACE("network", "Received data from UDP socket");

        if (!error/* || error == boost::asio::error::message_size*/)
        {
            

            StartReceive();
        }
    }

private:
    udp::socket _socket;
    std::vector<uint8> _receiveBuffer;
    udp::endpoint _senderEndpoint;
};

#endif // __SOCKET_H__
