/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ACORE_CHANNELMGR_H
#define __ACORE_CHANNELMGR_H

#include "Channel.h"
#include <memory>
#include <string>

class Player;

#define MAX_CHANNEL_NAME_STR 100
#define MAX_CHANNEL_PASS_STR 31

class ChannelMgr
{
    typedef std::unordered_map<std::wstring, std::shared_ptr<Channel>> ChannelsByNameMap;
    typedef std::unordered_map<uint64, std::shared_ptr<Channel>> ChannelsByGUIDMap;

public:
    static ChannelMgr& Instance()
    {
        static ChannelMgr instance;
        return instance;
    }

    ChannelMgr() : _nextChannelGuid(1) { }
    ~ChannelMgr() { }

    Channel* GetOrCreateChannel(TeamId team, std::string const& name, uint32 channelDBCId);
    Channel* GetChannel(TeamId team, std::string const& name);
    Channel* GetChannel(uint64 channelId);

    void LoadChannels();
    void LoadChannelRights();
    void SetChannelRightsFor(uint32 const channelId, uint32 const flags, uint32 const speakDelay, std::string const& joinmessage, std::string const& speakmessage, std::set<uint32> const& moderators);

    uint64 const GetNextChannelGuid() { return ++_nextChannelGuid; }

    static void SendNotOnPacket(Player* player, std::string const& channelName);

private:
    ChannelsByNameMap _channelsByName[MAX_TEAMS];
    ChannelsByGUIDMap _channelsByGuid;

    uint64 _nextChannelGuid;
};

#define sChannelMgr ChannelMgr::Instance()

#endif
