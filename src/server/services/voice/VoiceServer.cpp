#include "VoiceServer.h"

bool VoiceServer::Start(Acore::Asio::IoContext& ioContext, std::string const& bindIp, uint16 const port)
{
    //boost::asio::ip::address addr = boost::asio::ip::make_address(bindIp);
    _socket = std::make_unique<UdpSocket>(udp::socket(ioContext, udp::endpoint(udp::v4(), port)));
    // reuse_address?
    return true;
}

void VoiceServer::Run()
{
}
