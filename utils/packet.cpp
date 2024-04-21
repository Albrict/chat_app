#include "packet.hpp"
#include "network_utils.hpp"
#include <array>
#include <cstdio>

using NetworkUtils::Packet;

Packet::Packet(const std::vector<std::byte> &buffer, const uint32_t type)
{
    m_packet_size = buffer.size() + packet_header_size;
    m_buffer.resize(m_packet_size);
    
    auto packet   = m_buffer.data();
    NetworkUtils::serializeUnsignedInt(m_packet_size, packet); // Copy size of packet into a data buffer
    NetworkUtils::serializeUnsignedInt(type, packet + packet_header_stride); // Copy packet type into a data buffer 
    std::copy(buffer.begin(), buffer.end(), m_buffer.begin() + packet_header_size);
}

Packet::Packet(const int connect_fd)
{
    std::array<std::byte, sizeof(uint32_t)> size_of_packet;
    int n_bytes = recv(connect_fd, size_of_packet.data(), size_of_packet.size(), 0); 
    if (n_bytes <= 0) {
        // Got connection error or closed by client
        if (n_bytes == 0)
            printf("Client closed connection on socket:%d\n", connect_fd);
        else 
            perror("recv");
    } else {
        m_packet_size    = deserializeUnsignedInt(size_of_packet.data()); 
        m_buffer.resize(m_packet_size); 
        
        std::copy(size_of_packet.begin(), size_of_packet.end(), m_buffer.begin());
        n_bytes          = recv(connect_fd, &m_buffer[packet_header_stride], m_packet_size, 0);
        m_is_empty       = false;
        m_packet_type    = static_cast<Type>(NetworkUtils::deserializeUnsignedInt(m_buffer.data() + packet_header_stride)); 
    }
}

bool Packet::send(const int connection_fd)
{
    int bytes_sent = 0;        
    int result     = 0;
    int bytes_left = m_packet_size;
    while (bytes_sent < m_packet_size) {
        result = ::send(connection_fd, &m_buffer[bytes_sent], bytes_left, 0);
        if (result < -1) {
            return false;
        } else {
            bytes_sent += result;
            bytes_left -= result;
        }
    }
    return true;
}


NetworkUtils::LoginPacket::LoginPacket(std::string &nickname, std::string &password)
{
    const std::string nickname_sym = "/N/";
    const std::string password_sym = "/P/";
    
    nickname.insert(0, nickname_sym);
    password.insert(0, password_sym);
    auto message = nickname + password;
    const auto message_as_bytes  = reinterpret_cast<std::byte*>(message.data());
    const auto packet_data = std::vector<std::byte>(message_as_bytes, message_as_bytes + message.size() + 1);
    m_packet = std::make_unique<Packet>(packet_data, Packet::Type::SERVER_REGISTRATION);
}

NetworkUtils::LoginPacket::LoginPacket(const Packet &packet)
{
    std::string data    = packet.asChars();
    bool meta_data      = false;
    bool nickname_found = false;
    bool password_found = false;

    for (auto &ch : data) {
        if (nickname_found && ch != '/')
            m_nickname.push_back(ch); 
        if (password_found && ch != '/')
            m_password.push_back(ch);

        if (meta_data) {
            if (ch == 'N')
                nickname_found = true; 
            else if (ch == 'P')
                password_found = true;
        }
        if (ch == '/') {
            if (meta_data) {
                meta_data = false;
            } else {
                meta_data = true; 
                nickname_found = false;
                password_found = false;
            }
        };
    } 
}
