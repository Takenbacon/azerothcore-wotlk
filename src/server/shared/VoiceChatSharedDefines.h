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

#ifndef _VOICECHATSHAREDDEFINES_H
#define _VOICECHATSHAREDDEFINES_H

#include <Define.h>

enum VoiceChatServerOpcodes
{
    VOICECHAT_NULL_ACTION          = 0,
    VOICECHAT_CMSG_CREATE_CHANNEL  = 1,
    VOICECHAT_SMSG_CHANNEL_CREATED = 2,
    VOICECHAT_CMSG_ADD_MEMBER      = 3,
    VOICECHAT_CMSG_REMOVE_MEMBER   = 4,
    VOICECHAT_CMSG_VOICE_MEMBER    = 5,
    VOICECHAT_CMSG_DEVOICE_MEMBER  = 6,
    VOICECHAT_CMSG_MUTE_MEMBER     = 7,
    VOICECHAT_CMSG_UNMUTE_MEMBER   = 8,
    VOICECHAT_CMSG_DELETE_CHANNEL  = 9,
    VOICECHAT_CMSG_PING            = 10,
    VOICECHAT_SMSG_PONG            = 11,

    VOICECHAT_NUM_OPCODES,
};

struct VoiceChatServerPktHeader
{
    uint16 size;
    uint16 cmd;

    char const* data() const { return reinterpret_cast<const char*>(this); }
    std::size_t headerSize() const { return sizeof(VoiceChatServerPktHeader); }

    bool IsValidSize() const { return size >= 2 && size < 10240; } //@todo
    bool IsValidOpcode() const { return cmd < VOICECHAT_NUM_OPCODES; }
};

class VoiceChatServerPacket : public ByteBuffer
{
public:
    // just container for later use
    VoiceChatServerPacket() : ByteBuffer(0), _opcode(VOICECHAT_NULL_ACTION)
    {
    }
    explicit VoiceChatServerPacket(VoiceChatServerOpcodes opcode, size_t res = 200) : ByteBuffer(res), _opcode(opcode) { }
    // copy constructor
    VoiceChatServerPacket(const VoiceChatServerPacket& packet) : ByteBuffer(packet), _opcode(packet._opcode)
    {
    }

    VoiceChatServerPacket(VoiceChatServerOpcodes opcode, MessageBuffer&& buffer) :
        ByteBuffer(std::move(buffer)), _opcode(opcode) {
    }

    void Initialize(VoiceChatServerOpcodes opcode, size_t newres = 200)
    {
        clear();
        reserve(newres);
        _opcode = opcode;
    }

    VoiceChatServerOpcodes GetOpcode() const { return _opcode; }
    void SetOpcode(VoiceChatServerOpcodes opcode) { _opcode = opcode; }

protected:
    VoiceChatServerOpcodes _opcode;
};

#endif
