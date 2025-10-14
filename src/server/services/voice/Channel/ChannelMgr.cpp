#include "ChannelMgr.h"
#include "InternalSession.h"
#include "VoiceChatSharedDefines.h"

void ChannelMgr::CreateChannel(InternalSession* session, uint8 type, uint32 requestId)
{
    /*VoiceChatServerPacket data(VOICECHAT_SMSG_CHANNEL_CREATED, 4 + 1 + 2);
    data << requestId;
    data << uint8(0); // error
    data << uint16(100); // channelId
    session->SendPacket(data);*/
}

void ChannelMgr::AddMember(InternalSession* session, uint16 channelId, uint8 memberId)
{

}

void ChannelMgr::VoiceMember(InternalSession* session, uint16 channelId, uint8 memberId)
{

}
