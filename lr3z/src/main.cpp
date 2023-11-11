#include <iostream>
#include "../Message.hpp"
#include <filesystem>
#include <fstream>

#if LABNUM == 3
#include "../BMPContainerExtended.hpp"
#else
#include "../BMPContainer.hpp"
#endif




bool
pack(IContainer &c, const Message &msg)
{
        auto &cell = c.cells(); 
        auto bit = msg.begin();
        const std::size_t bit_count = msg.bsize();

        cell.begin();
        for (std::size_t i = 0; !cell.done() && i < 64; ++i, cell.next())
            cell.item().set(!!(bit_count & (static_cast<size_t>(1) << (63 - i))));

        for (; !cell.done()
            && bit != msg.end(); cell.next(), ++bit)
            cell.item().set(*bit);

        if (!cell.done() && bit != msg.end())
        {
            std::puts("Container too small for this message");
            return false;
        }

        return true;
}


Message
unpack(IContainer &c)
{
        Message result{};
        bitstream tmp{};
        auto &cell = c.cells();
        std::size_t stream_size{0};
            
        cell.begin();
        for (std::size_t i = 0; !cell.done() && i < 64; ++i, cell.next()) 
            if (cell.item().get())
                stream_size |= static_cast<size_t>(1) << (63 - i);

        for (std::size_t i = 0; i < stream_size; ++i, cell.next())
        {
            if (cell.done())
            {
                std::puts("Corrupted container. Can't extract data from this.");
                return {};
            }

            tmp.emplace_back(cell.item().get());
        }

        result.set(tmp);

        return result;
}


inline void
print_help()
{
        const std::string msg = std::string("Usage: ./") 
                                + PROGNAME 
#if LABNUM == 2
#ifdef IS_PACKER
                                + " container payload";
#else
                                + " container";
#endif
#else
#ifdef IS_PACKER
                                + " container payload";
#else
                                + " closed_container original_container";
#endif
#endif
        std::puts(msg.c_str());
}


inline bytestream
read_file(const std::string &fn)
{
        bytestream result{};
        std::filesystem::path p{fn};

        if (!std::filesystem::exists(p))
        {
            std::cerr << "The payload file doesn't exists: " << fn << std::endl;
            return {};
        }

        std::ifstream ifs{fn, std::ios::binary};
        ifs >> std::noskipws;

        std::uint8_t tmp;
        while (ifs >> tmp)
            result.emplace_back(tmp);

        return result;
}



int 
main(int argc, char **argv) 
{
        Message msg;
#if LABNUM == 3
        BMPContainerExtended c;
#else
        BMPContainer c;
#endif

        if (argc != ARGC)
        {
            std::puts("Incorrect number of arguments.");
            print_help();
            return 1;
        }

        SetEasyBMPwarningsOff();
        if (!c.load(argv[1]))
        {
            std::puts("Can't load a container.");
            return 1;
        }

#ifdef IS_PACKER
        auto payload = read_file(argv[2]);
        if (payload.empty())
        {
            std::puts("Try to pack a payload with zero size.");
            return 1;
        }

        msg.set(payload);

        if (!pack(c, msg))
        {
            std::puts("Error was accured awhile a packing operation.");
            return 1;
        }

        c.save("./result.bmp");
        
#else
#if LABNUM == 3
        if (!c.load_original(argv[2]))
            return 1;
#endif

        msg = unpack(c);
        if (msg.bsize() == 0)
        {
            std::puts("Got empty message from container.");
            return 1;
        }

        for (const auto b : msg.get())
            std::cout << b;
        std::cout << std::endl;
#endif
}
