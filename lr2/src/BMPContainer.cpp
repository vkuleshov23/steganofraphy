#include "../BMPContainer.hpp"
#include <Settings.hpp>
#include <EasyBmp/EasyBMP.h>
#include <filesystem>
#include <cstdlib>
#include <ctime>


namespace fs = std::filesystem;


bool 
BMPContainer::Cell::get()
const 
{
        bool sum = false;

        for (int i = 0; i < BLOCK_SIZE; ++i)
            for (int j = 0; j < BLOCK_SIZE; ++j)
                switch (color_) {
                case 0:
                    sum ^= pic.GetPixel(y_ + j, x_ + i).Red & 1;
                    break;

                case 1:
                    sum ^= pic.GetPixel(y_ + j, x_ + i).Green & 1;
                    break;

                case 2:
                    sum ^= pic.GetPixel(y_ + j, x_ + i).Blue & 1;
                }

        return sum;
}

void
BMPContainer::Cell::set(bool v)
{
        bool sum = false;

        for (int i = 0; i < BLOCK_SIZE; ++i)
            for (int j = 0; j < BLOCK_SIZE; ++j)
                switch (color_) {
                case 0:
                    sum ^= pic.GetPixel(y_ + j, x_ + i).Red & 1;
                    break;

                case 1:
                    sum ^= pic.GetPixel(y_ + j, x_ + i).Green & 1;
                    break;

                case 2:
                    sum ^= pic.GetPixel(y_ + j, x_ + i).Blue & 1;
                }

        if (sum != v)
        {
            struct {
                int x, y;
            } coord = {(rand() % BLOCK_SIZE), (rand() % BLOCK_SIZE)};

            auto pxl = pic.GetPixel(y_ + coord.y,
                                    x_ + coord.x);

            switch (color_) {
            case 0:
                pxl.Red ^= 1;
                break;

            case 1:
                pxl.Green ^= 1;
                break;

            case 2:
                pxl.Blue ^= 1;
                break;
            }

            pic.SetPixel(y_ + coord.y,
                         x_ + coord.x,
                         pxl);
        }
}


bool
BMPContainer::load(const std::string &fn)
{
        fs::path p{fn};
        if (!fs::exists(p))
        {
            std::cerr << "Can't load a container file: " << fn << std::endl;
            return false;
        }

        if (!picture.ReadFromFile(p.c_str()))
            return false;

        uint8_t color = 0;
        for (int i = 0; i < picture.TellHeight(); i += BLOCK_SIZE)
            for (int j = 0; j < picture.TellWidth(); )
            {
                itr.cells.emplace_back(picture, i, j, color++ % 3);

                if ((color % 3) == 0)
                    j += BLOCK_SIZE;
            }
        std::cout << itr.cells.size() << '\n';
        return true;
}


void
BMPContainer::save(const std::string &fn)
{
        BMP output;
        output.SetSize(picture.TellWidth(), picture.TellHeight());
        output.SetBitDepth(24);
        RangedPixelToPixelCopy(picture, 0, output.TellWidth() - 1,
                output.TellHeight() - 1, 0,
                output, 0, 0);
        output.WriteToFile(fn.c_str());
}
