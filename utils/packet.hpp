#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <array>

namespace NetworkUtils {
    class Packet final { public:
        enum Type : uint32_t {
            SERVER = 1,
            SERVER_REGISTRATION,
            SERVER_REGISTRATION_BAD,
            SERVER_REGISTRATION_ALREADY_EXISTS,
            SERVER_REGISTRATION_OK,
            _ALL_MESSAGES
        };

        static constexpr std::size_t packet_header_size   = sizeof(uint32_t) * 2;
        static constexpr std::size_t packet_header_stride = sizeof(uint32_t);
        static constexpr const std::array<const char*, _ALL_MESSAGES> messages = {
            "",
            "Server message",
            "Server: registration",
            "Server: registration invalid",
            "Server: user already exists",
            "Server: registration OK"
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
        
        [[nodiscard]] std::string asString() const noexcept
        { return std::string(asChars()); }

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

    class LoginPacket final {
    public:
        explicit LoginPacket(std::string &nickname, std::string &password);
        explicit LoginPacket(const Packet &packet);
        
        const Packet *getPacket() const noexcept
        { return m_packet.get(); }
        
        const std::string nickname() const noexcept
        { return m_nickname; }

        const std::string password() const noexcept
        { return m_password; }

        std::string nickname() noexcept
        { return m_nickname; }

        std::string password() noexcept
        { return m_password; }

        bool send(const int connection_fd)
        { return m_packet->send(connection_fd); }
    private:
        std::unique_ptr<Packet> m_packet   {};
        std::string             m_nickname {}; 
        std::string             m_password {};
    };
}
