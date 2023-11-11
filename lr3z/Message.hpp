#pragma once
#include <vector>
#include <cstdint>

using bitstream = std::vector<bool>;
using bytestream = std::vector<std::uint8_t>;



class Message {
public:
        void set(const bitstream &bs);
        void set(const bytestream &bs);

        std::size_t bsize(void) const { return data.bits.size(); }

        bytestream get() const { return data.bytes; };

        bitstream::iterator begin() { return data.bits.begin(); }
        bitstream::const_iterator begin() const { return data.bits.begin(); }

        bitstream::iterator end() { return data.bits.end(); }
        bitstream::const_iterator end() const { return data.bits.end(); }

private:
        struct {
            bytestream bytes;
            bitstream bits;
        } data;
};
