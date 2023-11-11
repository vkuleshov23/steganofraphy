#include "../BMPContainerExtended.hpp"
#include <Settings.hpp>
#include <EasyBmp/EasyBMP.h>
#include <dct/Dct.hpp>
#include <filesystem>
#include <utility>
#include <cstdlib>
#include <ctime>


namespace fs = std::filesystem;



inline std::pair<int, int> get_max_idx(const std::array<std::array<double, BLOCK_SIZE>, BLOCK_SIZE> &data);


std::mutex BMPContainerExtended::pic_mtx = {};
std::atomic<int> BMPContainerExtended::latch_ = 0;
bool BMPContainerExtended::is_origin_loaded = false;
std::vector<std::future<void>> BMPContainerExtended::futures = {};



bool 
BMPContainerExtended::CellExtended::get()
const 
{
        if (!is_origin_loaded)
            throw std::runtime_error("Can't execute CellExtended::get function without loaded original file.");

        std::array<std::array<double, BLOCK_SIZE>, BLOCK_SIZE> modified{}, origin{};
        for (int i = 0; i < BLOCK_SIZE; ++i)
            for (int j = 0; j < BLOCK_SIZE; ++j)
                switch (color_) {
                case 0:
                    modified[i][j] = pic.GetPixel(y_ + j, x_ + i).Red;
                    origin[i][j] = pic_orig.GetPixel(y_ + j, x_ + i).Red;
                    break;

                case 1:
                    modified[i][j] = pic.GetPixel(y_ + j, x_ + i).Green;
                    origin[i][j] = pic_orig.GetPixel(y_ + j, x_ + i).Green;
                    break;

                case 2:
                    modified[i][j] = pic.GetPixel(y_ + j, x_ + i).Blue;
                    origin[i][j] = pic_orig.GetPixel(y_ + j, x_ + i).Blue;
                }

        auto transformed = dct(modified);
        auto [i_max, j_max] = get_max_idx(transformed);

        return (modified[i_max][j_max] >= origin[i_max][j_max]);
}

void
BMPContainerExtended::CellExtended::set(bool v)
{
        auto func = [this, v] () {
            std::array<std::array<double, BLOCK_SIZE>, BLOCK_SIZE> origin{};
            for (int i = 0; i < BLOCK_SIZE; ++i)
                for (int j = 0; j < BLOCK_SIZE; ++j)
                    switch (color_) {
                    case 0:
                        origin[i][j] = pic.GetPixel(y_ + j, x_ + i).Red;
                        break;

                    case 1:
                        origin[i][j] = pic.GetPixel(y_ + j, x_ + i).Green;
                        break;

                    case 2:
                        origin[i][j] = pic.GetPixel(y_ + j, x_ + i).Blue;
                    }

            auto transformed = dct(origin);
            auto [i_max, j_max] = get_max_idx(transformed);
            transformed[i_max][j_max] *= E_ALPHA[v];

            auto result = idct(transformed);
            for (int i = 0; i < BLOCK_SIZE; ++i)
                for (int j = 0; j < BLOCK_SIZE; ++j)
                {
                    std::lock_guard<std::mutex> lk{pic_mtx};
                    auto pxl = pic.GetPixel(y_ + j, x_ + i);

                    switch (color_) {
                    case 0:
                        pxl.Red = result[i][j];
                        break;

                    case 1:
                        pxl.Green = result[i][j];
                        break;

                    case 2:
                        pxl.Blue = result[i][j];
                    };

                    pic.SetPixel(y_ + j,
                                 x_ + i,
                                 pxl);
                }
        };

        futures.emplace_back(std::async(std::launch::async, func));
}


bool
BMPContainerExtended::load(const std::string &fn)
{
        fs::path p{fn};
        if (!fs::exists(p))
        {
            std::cerr << "Can't load a container file: " << fn << std::endl;
            return false;
        }

        if (!picture.ReadFromFile(p.c_str()))
            return false;

        uint8_t color_idx = 0;
        for (int i = 0; i < picture.TellHeight(); i += BLOCK_SIZE)
            for (int j = 0; j < picture.TellWidth();)
            {
                itr.cells.emplace_back(original_picture, picture, i, j, color_idx++ % 3);
                if ((color_idx % 3) == 0)
                    j += BLOCK_SIZE;
            }

        std::cout << itr.cells.size() << '\n';
        return true;
}


void
BMPContainerExtended::save(const std::string &fn)
{
        for (auto &&f : futures)
            f.wait();

        BMP output;
        output.SetSize(picture.TellWidth(), picture.TellHeight());
        output.SetBitDepth(24);
        RangedPixelToPixelCopy(picture, 0, output.TellWidth() - 1,
                output.TellHeight() - 1, 0,
                output, 0, 0);
        output.WriteToFile(fn.c_str());
}


bool
BMPContainerExtended::load_original(const std::string &fn)
{
        fs::path p{fn};
        if (!fs::exists(p))
        {
            std::cerr << "Can't load an original file: " << fn << std::endl;
            return false;
        }

        if (!original_picture.ReadFromFile(p.c_str()))
            return false;

        return (is_origin_loaded = true);
}



inline std::pair<int, int> 
get_max_idx(const std::array<std::array<double, BLOCK_SIZE>, BLOCK_SIZE> &data)
{
        double max{-1./0};
        std::pair<int, int> result{0, 0};

        for (int i = 0; i < BLOCK_SIZE; ++i)
            for (int j = 0; j < BLOCK_SIZE; ++j)
                if (max < data[i][j])
                {
                    max = data[i][j];
                    result = {i, j};
                }

        return result;
}
