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

#ifndef __VoiceChatMgr_H__
#define __VoiceChatMgr_H__

#include "TcpAsyncConnector.h"

class Channel;
class VoiceChatSocket;

class VoiceServerConnector : public TcpAsyncConnector
{
public:
    VoiceServerConnector(boost::asio::io_context& context) :
        TcpAsyncConnector(context) { }

    void OnConnectionSuccess(std::unique_ptr<tcp::socket> socket);
};

class VoiceChatMgr
{
public:
    static VoiceChatMgr* Instance()
    {
        static VoiceChatMgr instance;
        return &instance;
    }

    VoiceChatMgr();

    bool IsEnabled() const { return _enabled; }
    void Disable();
    void Enable();

    void Update(uint32 const diff);

    void RegisterVoiceChannel(Channel* channel);
    void UnregisterVoiceChannel(Channel* channel);

    void CreateVoiceSession(Channel* channel);

    uint16 GetNextVoiceChannelId() { return ++_nextVoiceChannelId; }

private:
    void UpdateVoiceServerConnection();

    bool _enabled;
    uint16 _nextVoiceChannelId;

    typedef std::unordered_map<uint16, Channel*> VoiceChannelsMap;
    VoiceChannelsMap _voiceChannels;

    VoiceServerConnector _voiceServerConnector;
    std::shared_ptr<VoiceChatSocket> _voiceServerSocket;
};

#define sVoiceChatMgr VoiceChatMgr::Instance()

#endif
