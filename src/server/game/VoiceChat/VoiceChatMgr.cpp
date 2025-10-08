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
#include "VoiceChatMgr.h"
#include "VoiceChatSocket.h"
#include "WorldSocketMgr.h"
#include <boost/asio/ip/address.hpp>

VoiceChatMgr::VoiceChatMgr() : _voiceServerConnector(sWorldSocketMgr.GetIoContext()), _enabled(sWorld->getBoolConfig(CONFIG_VOICE_CHAT_ENABLED)), _nextVoiceChannelId(1)
{
}

void VoiceChatMgr::Disable()
{
    _enabled = false;
    _voiceServerConnector.Stop();
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

    UpdateVoiceServerConnection();
}

void VoiceChatMgr::UpdateVoiceServerConnection()
{
    if (_voiceServerSocket && !_voiceServerSocket->IsOpen())
        _voiceServerSocket = nullptr;

    if (!_voiceServerSocket && !_voiceServerConnector.IsAttemptingConnection())
    {
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), sWorld->getIntConfig(CONFIG_VOICE_CHAT_SERVER_PORT));
        _voiceServerConnector.ConnectRepeat(endpoint);
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

    VoiceChatServerPacket pkt(VoiceChatServerOpcodes::CMSG_CREATE_VOICE_SESSION, 100);
    pkt << channel->GetVoiceId();
    _voiceServerSocket->SendPacket(pkt);
}

void VoiceServerConnector::OnConnectionSuccess(std::unique_ptr<tcp::socket> socket)
{

}
