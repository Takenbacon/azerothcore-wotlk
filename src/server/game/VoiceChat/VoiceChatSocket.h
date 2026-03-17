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

#ifndef __VoiceChatSocket_H__
#define __VoiceChatSocket_H__

#include "TcpSocket.h"
#include "VoiceChatSharedDefines.h"
#include <boost/asio/ip/tcp.hpp>

using boost::asio::ip::tcp;

class VoiceChatSocket : public TcpSocket<VoiceChatSocket>
{
public:
    explicit VoiceChatSocket(IoContextTcpSocket&& socket);

    void Start() override;
    bool Update() override;
    void SendPacket(VoiceChatServerPacket const& pkt);

    uint32 GetLastPacketReceiveTime() const { return _lastPacketReceiveTime; }

protected:
    bool ReadHeaderHandler();
    ReadDataHandlerResult ReadDataHandler();

private:
    uint32 _lastPacketReceiveTime;
};

#endif
