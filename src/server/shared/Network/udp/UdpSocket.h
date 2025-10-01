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

#pragma pack(push, 1)
struct UdpSocketPacketHeader
{
    uint32 signature;
    uint8 userId;
    uint16 channelId;
    //uint16 frameId;
};
#pragma pack(pop)

#define UDP_SOCKET_PACKET_MAX_SIZE 1024

class UdpSocketPacket
{
public:
    UdpSocketPacket(uint8* data, uint32 length)
    {
        memcpy(&m_data, data, length);
        m_length = length;
        m_header = reinterpret_cast<UdpSocketPacketHeader*>(m_data);
    }

    UdpSocketPacketHeader const& GetHeader() const { return *m_header; }
    UdpSocketPacketHeader& GetHeader() { return *m_header; }
    uint8 const* GetPayload() const { return m_data; }
    uint16 GetPayloadLength() const { return m_length; }

private:
    UdpSocketPacketHeader* m_header;
    uint16 m_length;
    uint8 m_data[UDP_SOCKET_PACKET_MAX_SIZE] = { };
};

#define UDP_SOCKET_BUFFER_SIZE 2048

class UdpSocket
{
public:
    explicit UdpSocket(udp::socket&& socket) : _socket(std::move(socket))
    {
        StartReceive();
    }

    void StartReceive()
    {
        _socket.async_receive_from(boost::asio::buffer(_buffer, UDP_SOCKET_BUFFER_SIZE), _senderEndpoint,
            [this](boost::system::error_code const errorCode, std::size_t const dataReceived)
            {
                if (!errorCode && dataReceived >= sizeof(UdpSocketPacketHeader))
                {
                    UdpSocketPacket const packet(_buffer, dataReceived);
                    ProcessPacket(packet, _senderEndpoint);
                }

                StartReceive();
            });
    }

protected:
    void ProcessPacket(UdpSocketPacket const& packet, boost::asio::ip::udp::endpoint const& remoteEndpoint)
    {
        LOG_TRACE("network", "Received data from UDP socket. Payload size {}. Signature {}. UserID {} ChannelID {}",
            packet.GetPayloadLength(), packet.GetHeader().signature, packet.GetHeader().userId, packet.GetHeader().channelId);
    }

private:
    udp::socket _socket;
    uint8 _buffer[UDP_SOCKET_BUFFER_SIZE] = { };
    udp::endpoint _senderEndpoint;
};

#endif // __UDPSOCKET_H__
