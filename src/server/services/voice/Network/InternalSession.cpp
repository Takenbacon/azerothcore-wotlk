#include "ChannelMgr.h"
#include "InternalSession.h"
#include "VoiceChatSharedDefines.h"

InternalSession::InternalSession(tcp::socket&& socket) : TcpSocket(std::move(socket))
{
    _headerBuffer.Resize(sizeof(VoiceChatServerPktHeader));
}

void InternalSession::Start()
{
    std::string ip_address = GetRemoteIpAddress().to_string();
    LOG_TRACE("network", "Accepted connection from {}", ip_address);

    AsyncRead();
}

void InternalSession::OnClose()
{
    LOG_TRACE("network", "InternalSession closed");
}

bool InternalSession::Update()
{
    if (!InternalSocket::Update())
        return false;

    return true;
}

bool InternalSession::ReadHeaderHandler()
{
    ASSERT(_headerBuffer.GetActiveSize() == sizeof(VoiceChatServerPktHeader));

    VoiceChatServerPktHeader* header = reinterpret_cast<VoiceChatServerPktHeader*>(_headerBuffer.GetReadPointer());
    EndianConvertReverse(header->size);
    EndianConvert(header->cmd);

    if (!header->IsValidSize() || !header->IsValidOpcode())
    {
        LOG_ERROR("network", "InternalSession::ReadHeaderHandler(): client {} sent malformed packet (size: {}, cmd: {})",
            GetRemoteIpAddress().to_string(), header->size, header->cmd);

        return false;
    }

    header->size -= sizeof(header->cmd);
    _packetBuffer.Resize(header->size);

    return true;
}

ReadDataHandlerResult InternalSession::ReadDataHandler()
{
    VoiceChatServerPktHeader* header = reinterpret_cast<VoiceChatServerPktHeader*>(_headerBuffer.GetReadPointer());
    VoiceChatServerOpcodes opcode = static_cast<VoiceChatServerOpcodes>(header->cmd);

    LOG_TRACE("network", "Received opcode {} size {}", opcode, header->size);

    VoiceChatServerPacket packet(opcode, std::move(_packetBuffer));

    switch (opcode)
    {
        case VOICECHAT_CMSG_PING:
        {
            VoiceChatServerPacket data(VOICECHAT_SMSG_PONG, 0);
            SendPacket(data);
            break;
        }
        case VOICECHAT_CMSG_CREATE_CHANNEL:
        {
            uint8 type;
            uint32 requestId;

            packet >> type;
            packet >> requestId;

            sChannelMgr.CreateChannel(this, type, requestId);
            break;
        }
        case VOICECHAT_CMSG_ADD_MEMBER:
        {
            uint16 channelId;
            uint8 memberId;

            packet >> channelId;
            packet >> memberId;

            sChannelMgr.AddMember(this, channelId, memberId);
            break;
        }
        case VOICECHAT_CMSG_VOICE_MEMBER:
        {
            uint16 channelId;
            uint8 memberId;

            packet >> channelId;
            packet >> memberId;

            sChannelMgr.VoiceMember(this, channelId, memberId);
            break;
        }
    }

    return ReadDataHandlerResult::Ok;
}

void InternalSession::SendPacket(VoiceChatServerPacket const& packet)
{
    if (!IsOpen())
        return;

    VoiceChatServerPktHeader pktHeader;
    pktHeader.size = sizeof(pktHeader.cmd) + packet.wpos();
    pktHeader.cmd = packet.GetOpcode();

    EndianConvertReverse(pktHeader.size);
    EndianConvert(pktHeader.cmd);

    MessageBuffer buffer(pktHeader.headerSize() + packet.size());
    buffer.Write(pktHeader.data(), pktHeader.headerSize());
    if (!packet.empty())
        buffer.Write(packet.contents(), packet.size());
    QueuePacket(std::move(buffer));

    LOG_TRACE("network", "Sending opcode {} size {}", packet.GetOpcode(), packet.size());
}
