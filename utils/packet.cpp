#include "packet.hpp"
#include "network_utils.hpp"
#include <cstring>
#include <array>

using NetworkUtils::Packet;

Packet::Packet(const std::size_t buffer_size, const Type type, const std::byte *data)
{
    m_packet_size = buffer_size + packet_header_size;
    m_buffer      = std::make_unique<DataBuffer>(m_packet_size); 

    auto packet   = m_buffer->data();
    NetworkUtils::serializeUnsignedInt(m_packet_size, packet); // Copy size of packet into a data buffer
    NetworkUtils::serializeUnsignedInt(type, packet + sizeof(uint32_t)); // Copy packet type into a data buffer 
    memcpy(packet + packet_header_size, data, buffer_size); // Copy data to packet
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
        m_buffer         = std::make_unique<DataBuffer>(m_packet_size);

        memcpy(m_buffer->data(), size_of_packet.data(), size_of_packet.size());        
        n_bytes          = recv(connect_fd, m_buffer->data() + sizeof(uint32_t), m_packet_size, 0);

        // Received some data
        printf("Received data: %s\n", asChars());
        printf("Packet size: %d\n", packetSize());
//        printf("Packet type: %d\n", type());
        m_is_empty = false;
    }
}

uint32_t Packet::packetSize()
{
    return NetworkUtils::deserializeUnsignedInt(m_buffer->data());
}

Packet::Type Packet::type()
{
    return static_cast<Type>(NetworkUtils::deserializeUnsignedInt(m_buffer->data() + sizeof(uint32_t)));
}
