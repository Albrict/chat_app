#include "data_buffer.hpp"

void NetworkUtils::DataBuffer::clear() noexcept
{
    for (std::size_t i = 0; i < m_size; ++i)
        m_data[i] = {};
}
