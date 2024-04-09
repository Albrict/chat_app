#pragma once
#include "data_buffer.hpp"
#include <memory>
#include <string>

namespace NetworkUtils {
    class Packet final {
    public:
        Packet(const std::size_t size)
        { m_buffer = std::make_unique<DataBuffer>(size); }
        
        Packet(const Packet &other)
        { 
            *m_buffer = *other.m_buffer; 
            m_isEmpty = other.m_isEmpty;
        }
        
        Packet(Packet &&other) noexcept
        {
            m_buffer.swap(other.m_buffer);
            m_isEmpty = other.m_isEmpty;
        }
        
        Packet &operator=(const Packet &rhs) 
        {
            return *this = Packet(rhs);
        }
        
        Packet& operator=(Packet &&rhs) noexcept
        {
            m_buffer.swap(rhs.m_buffer);
            return *this;
        }

        ~Packet() = default;

        void setData(std::byte *data)
        { memcpy(m_buffer->data(), data, m_buffer->size()); m_isEmpty = false; }

        std::byte *asBytes()
        { return m_buffer->data(); }
        
        std::string asString()
        { return reinterpret_cast<const char*>(m_buffer->data()); }
        
        const char *asChars()
        {
            return reinterpret_cast<char*>(m_buffer->data());
        }

        bool isEmpty()
        { return m_isEmpty; }
    private:
        std::unique_ptr<DataBuffer> m_buffer {};
        bool m_isEmpty = true;
    };
}
