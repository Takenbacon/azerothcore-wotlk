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

#include "ChannelMgr.h"
#include "Group.h"
#include "Log.h"
#include "Player.h"
#include "SocialMgr.h"
#include "VoiceChatPackets.h"
#include "WorldSession.h"

void WorldSession::HandleVoiceSessionEnableOpcode(WorldPackets::VoiceChat::VoiceSessionEnable& packet)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_VOICE_SESSION_ENABLE");

    _micEnabled = packet.MicrophoneEnabled;
    _voiceEnabled = packet.VoiceEnabled;
}

void WorldSession::HandleChannelVoiceOnOpcode(WorldPackets::VoiceChat::ChannelVoiceOn& packet)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_CHANNEL_VOICE_ON");

}

void WorldSession::HandleSetActiveVoiceChannelOpcode(WorldPackets::VoiceChat::SetActiveVoiceChannel& packet)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_SET_ACTIVE_VOICE_CHANNEL");

}

void WorldSession::HandleChannelVoiceOffOpcode(WorldPackets::VoiceChat::ChannelVoiceOff& /*packet*/)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_CHANNEL_VOICE_OFF");

    // todo check if possible to send with chat commands
}

void WorldSession::HandleAddVoiceIgnoreOpcode(WorldPackets::VoiceChat::AddVoiceIgnore& packet)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_ADD_VOICE_IGNORE");


}

void WorldSession::HandleDeleteVoiceIgnoreOpcode(WorldPackets::VoiceChat::DeleteVoiceIgnore& packet)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_DEL_VOICE_IGNORE");

}

void WorldSession::HandlePartySilenceOpcode(WorldPackets::VoiceChat::PartySilence& packet)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_PARTY_SILENCE");

}

void WorldSession::HandlePartyUnsilenceOpcode(WorldPackets::VoiceChat::PartyUnsilence& packet)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_PARTY_UNSILENCE");

}

void WorldSession::HandleChannelSilenceOpcode(WorldPackets::VoiceChat::ChannelSilence& packet)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_CHANNEL_SILENCE_VOICE");

}

void WorldSession::HandleChannelUnsilenceOpcode(WorldPackets::VoiceChat::ChannelUnsilence& packet)
{
    LOG_DEBUG("network", "WORLD: Received CMSG_CHANNEL_UNSILENCE_VOICE");


}

void WorldSession::SetActiveVoiceChannel(VoiceChatChannel* voiceChannel, VoiceChatChannel* currentChannel, Player* player)
{

}
