#pragma once
#include <cstdint>
#include <vector>

namespace NetworkUtils {
    class Packet final {
    public:
        static constexpr std::size_t  packet_header_size   = sizeof(uint32_t) * 2;
        static constexpr std::size_t  packet_header_stride = sizeof(uint32_t);

        enum Type : uint32_t {
            SERVER = 1,
        };
        
        explicit Packet(const std::vector<std::byte> &buffer, const uint32_t type);
        explicit Packet(const int connect_fd);
        explicit Packet(const Packet &other)
        {
            m_buffer       = other.m_buffer; 
            m_packet_size  = other.m_packet_size;
            m_is_empty     = other.m_is_empty;
        }
        
        Packet(Packet &&other) noexcept
        {
            m_buffer      = std::move(other.m_buffer);
            m_is_empty    = other.m_is_empty;
            m_packet_size = other.m_packet_size;
        }
        
        ~Packet() = default;

        Packet &operator=(const Packet &rhs) 
        { return *this = Packet(rhs); }
        
        Packet& operator=(Packet &&rhs) noexcept
        { return *this = Packet(std::move(rhs)); }

        [[nodiscard]] std::byte &operator[](const std::size_t i) noexcept
        { return m_buffer[i]; }
        
        [[nodiscard]] const std::byte &operator[](const std::size_t i) const noexcept
        { return m_buffer[i]; }

        [[nodiscard]] const std::byte *asBytes() const noexcept
        { return m_buffer.data() + packet_header_size; }
        
        [[nodiscard]] const char *asChars() const noexcept
        { return reinterpret_cast<const char*>(m_buffer.data() + packet_header_size); }
        
        [[nodiscard]] bool isEmpty() const noexcept
        { return m_is_empty; }

        [[nodiscard]] uint32_t size() const noexcept
        { return m_packet_size; }
        
        [[nodiscard]] Type type() const noexcept
        { return m_packet_type; }
        

        bool send(const int connection_fd);
    private:
        std::vector<std::byte>      m_buffer {};
        std::size_t  m_packet_size  = 0;
        Type         m_packet_type  = Type::SERVER;
        bool         m_is_empty     = true;
    };
}
