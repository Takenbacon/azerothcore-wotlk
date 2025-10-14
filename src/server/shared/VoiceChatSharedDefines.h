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

enum class VoiceChatServerOpcodes : uint8
{
    NULL_OPCODE,

    CMSG_PING,
    SMSG_PONG,
    CMSG_CREATE_VOICE_SESSION,
    SMSG_CREATE_VOICE_SESSION_RESPONSE,

    NUM_OPCODES,
};

struct VoiceChatServerPktHeader
{
    uint16 size;
    uint8 cmd;

    char const* data() const { return reinterpret_cast<const char*>(this); }
    std::size_t headerSize() const { return sizeof(VoiceChatServerPktHeader); }

    bool IsValidSize() const { return size >= 1 && size < 10240; } //@todo
    bool IsValidOpcode() const { return cmd < static_cast<uint8>(VoiceChatServerOpcodes::NUM_OPCODES); }
};

class VoiceChatServerPacket : public ByteBuffer
{
public:
    // just container for later use
    VoiceChatServerPacket() : ByteBuffer(0), _opcode(VoiceChatServerOpcodes::NULL_OPCODE)
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
