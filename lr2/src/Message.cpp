#include "../Message.hpp"


void
Message::set(const bitstream &bs)
{
        uint8_t tmp{0};

        data.bytes.clear();
        data.bits.clear();
        for (std::size_t i = 0; i < bs.size(); ++i)
        {
            if (i != 0 && i % 8 == 0)
            {
                data.bytes.emplace_back(tmp);
                tmp = 0;
            }

            if (bs[i])
                tmp |= 1 << (7 - (i % 8));
        }

        data.bytes.emplace_back(tmp);
        data.bits = {bs};
}


void
Message::set(const bytestream &bs)
{
        for (std::size_t i = 0; i < bs.size(); ++i)
            for (std::size_t j = 0; j < 8; ++j)
                data.bits.emplace_back(bs[i] & (1 << (7 - j)));
        
        data.bytes = {bs};
}
