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

#include "Channel.h"
#include "VoiceChatInternalHandler.h"
#include "VoiceChatMgr.h"
#include "VoiceChatSocket.h"
#include "VoiceChatSocketMgr.h"
#include "WorldSocketMgr.h"
#include <boost/asio/ip/address.hpp>

#define SOCKET_PING_INTERVAL 10 * SECOND * IN_MILLISECONDS
#define SOCKET_NOREPLY_TIME  30 * SECOND * IN_MILLISECONDS

VoiceChatMgr::VoiceChatMgr() : _enabled(sWorld->getBoolConfig(CONFIG_VOICE_CHAT_ENABLED)), _nextVoiceChannelId(1)
{
    internalVoiceChatOpcodeTable.Init();
    sVoiceChatSocketMgr.Init(sWorldSocketMgr.GetIoContext());
    _voiceServerConnector = std::make_shared<VoiceServerConnector>(sWorldSocketMgr.GetIoContext());
    _pingTimer.SetInterval(SOCKET_PING_INTERVAL);
}

void VoiceChatMgr::Disable()
{
    _enabled = false;
    _voiceServerConnector->Stop();
}

void VoiceChatMgr::Enable()
{
    _enabled = true;
}

void VoiceChatMgr::Update(uint32 const diff)
{
    // Check if config changed
    if (_enabled != sWorld->getBoolConfig(CONFIG_VOICE_CHAT_ENABLED))
    {
        if (sWorld->getBoolConfig(CONFIG_VOICE_CHAT_ENABLED))
            Enable();
        else
            Disable();
    }

    if (!_enabled)
        return;

    UpdateVoiceServerConnection(diff);
}

void VoiceChatMgr::QueueIncomingVoiceServerPacket(std::unique_ptr<VoiceChatServerPacket> packet)
{
    _voiceServerPacketQueue.emplace_back(std::move(packet));
}

void VoiceChatMgr::SendPacket(VoiceChatServerPacket const& pkt)
{
    if (!_voiceServerSocket)
        return;

    _voiceServerSocket->SendPacket(pkt);
}

void VoiceChatMgr::HandlePongOpcode(VoiceChatServerPacket const& packet)
{
    // test
}

void VoiceChatMgr::UpdateVoiceServerConnection(uint32 const diff)
{
    // Process incoming packet queue
    std::unique_ptr<VoiceChatServerPacket> packet;
    while (_voiceServerPacketQueue.next(packet))
    {
        VoiceChatInternalOpcodeHandler<VoiceChatMgr> const* opHandle = internalVoiceChatOpcodeTable[packet->GetOpcode()];
        if (!opHandle)
            continue;

        try
        {
            (this->*opHandle->_handler)(*packet.get());
        }
        catch (ByteBufferException const&)
        {
            
        }
    }

    // Check connection status
    if (_voiceServerSocket)
    {
        // Periodically fire a ping packet to check if connection is still alive
        _pingTimer.Update(diff);
        if (_pingTimer.Passed())
        {
            VoiceChatServerPacket pkt(VoiceChatServerOpcodes::CMSG_PING, 0);
            SendPacket(pkt);
            _pingTimer.Reset();
        }

        // Has it been too long since the last received packet? Drop the socket to allow connection of a new one
        if (GetMSTimeDiffToNow(_voiceServerSocket->GetLastPacketReceiveTime()) > SOCKET_NOREPLY_TIME)
            _voiceServerSocket->CloseSocket();

        if (!_voiceServerSocket->IsOpen())
            _voiceServerSocket = nullptr;
    }
    else
    {
        // Connect if needed
        if (!_voiceServerConnector->IsAttemptingConnection())
        {
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), sWorld->getIntConfig(CONFIG_VOICE_CHAT_SERVER_PORT));
            _voiceServerConnector->ConnectRepeat(endpoint);
        }
    }
}

void VoiceChatMgr::RegisterVoiceChannel(Channel* channel)
{
    _voiceChannels[channel->GetVoiceId()] = channel;
}

void VoiceChatMgr::UnregisterVoiceChannel(Channel* channel)
{
    _voiceChannels.erase(channel->GetVoiceId());
}

void VoiceChatMgr::CreateVoiceSession(Channel* channel)
{
    RegisterVoiceChannel(channel);

    VoiceChatServerPacket pkt(VoiceChatServerOpcodes::CMSG_CREATE_VOICE_SESSION, 2);
    pkt << channel->GetVoiceId();
    _voiceServerSocket->SendPacket(pkt);
}

// Warning: Called from boost asio callback, not thread safe
void VoiceServerConnector::OnConnectionSuccess(std::unique_ptr<tcp::socket> socket)
{
    std::shared_ptr<VoiceChatSocket> voiceSocket = sVoiceChatSocketMgr.OnSocketOpen(std::move(*socket));
    if (voiceSocket)
        sVoiceChatMgr->SetVoiceChatServerSocket(voiceSocket->shared_from_this());
}
