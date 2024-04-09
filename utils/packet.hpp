#pragma once
#include "data_buffer.hpp"
#include <memory>
#include <string>

namespace NetworkUtils {
    class Packet final {
    public:
        static constexpr std::size_t  packet_header_size = 8;
        enum Type : uint32_t {
            SERVER = 1,
            CLIENT
        };

        explicit Packet(const size_t buffer_size, const Type type, const std::byte *buffer_data);
        explicit Packet(const int connect_fd);
        explicit Packet(const Packet &other)
        {
            *m_buffer     = *other.m_buffer; 
            m_packet_size = other.m_packet_size;
            m_is_empty     = other.m_is_empty;
        }
        
        Packet(Packet &&other) noexcept
        {
            m_buffer.swap(other.m_buffer);
            m_is_empty    = other.m_is_empty;
            m_packet_size = other.m_packet_size;
        }
        
        ~Packet() = default;

        Packet &operator=(const Packet &rhs) 
        {
            return *this = Packet(rhs);
        }
        
        Packet& operator=(Packet &&rhs) noexcept
        {
            m_buffer.swap(rhs.m_buffer);
            return *this;
        }

        std::byte &operator[](const std::size_t i) noexcept
        {
            return m_buffer->data()[i];
        }
        
        const std::byte &operator[](const std::size_t i) const noexcept
        {
            return m_buffer->data()[i];
        }

        void setData(std::byte *data)
        { memcpy(m_buffer->data(), data, m_buffer->size()); m_is_empty = false; }

        std::byte *asBytes()
        { return m_buffer->data() + packet_header_size; }
        
        std::string asString()
        { return reinterpret_cast<const char*>(m_buffer->data() + packet_header_size); }
        
        const char *asChars()
        {
            return reinterpret_cast<const char*>(m_buffer->data() + packet_header_size);
        }
        
        bool isEmpty()
        { return m_is_empty; }

        uint32_t size() const noexcept
        {
            return m_packet_size;
        }

        uint32_t packetSize();
        Type     type();

    private:
        std::unique_ptr<DataBuffer> m_buffer {};
        std::size_t  m_packet_size  = 0;
        bool         m_is_empty     = true;
    };
}
