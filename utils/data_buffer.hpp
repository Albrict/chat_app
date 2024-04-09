#pragma once
#include <cstddef>
#include <cstring>
#include <utility>

namespace NetworkUtils {
    class DataBuffer final {
    public:
        explicit DataBuffer(const std::size_t size)
            : m_size(size), m_data(new std::byte[size]()) {}

        DataBuffer(const DataBuffer &other)
            : m_size(other.m_size), m_data(new std::byte[other.m_size]()) 
        { std::memcpy(m_data, other.m_data, other.m_size); }

        DataBuffer(DataBuffer &&other) noexcept
            : m_size(other.m_size), m_data(std::exchange(other.m_data, nullptr)) {}

        ~DataBuffer()
        { delete [] m_data; }
        
        DataBuffer &operator=(const DataBuffer &rhs) 
        {
            return *this = DataBuffer(rhs);
        }
        
        DataBuffer& operator=(DataBuffer &&rhs) noexcept
        {
            std::swap(m_data, rhs.m_data);
            return *this;
        }
        
        [[nodiscard]] std::size_t size() const noexcept
        { return m_size; }

        [[nodiscard]] const std::byte *data() const noexcept
        { return m_data; }

        [[nodiscard]] std::byte *data() noexcept
        { return m_data; }

        void clear() noexcept;
    private:
        std::size_t m_size  = 0;
        std::byte   *m_data = nullptr;
    };
}
