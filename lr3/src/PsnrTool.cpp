#include <vector>
#include <cassert>
#include <iostream>
#include <EasyBmp/EasyBMP.h>




template <std::size_t L>
double
psnr(const std::vector<std::vector<uint8_t>> &orig, const std::vector<std::vector<uint8_t>> &mod)
{
        assert(orig.size() == mod.size());
        assert(orig[0].size() == mod[0].size());
        constexpr std::size_t const_ = ((1 << (L - 1)) - 1) * ((1 << (L - 1)) - 1);

        double result = 0;
        for (std::size_t i = 0; i < orig.size(); ++i)
            for (std::size_t j = 0; j < orig[0].size(); ++j)
            {
                auto tmp = (orig[i][j] - mod[i][j]) ;
                result += tmp * tmp;
            }

        result /= orig.size() * orig[0].size();
        result = static_cast<double>(const_) / result;

        return 10 * std::log10(result);
}


int
main(int argc, char **argv)
{
        BMP p_original{}, p_modified{};

        p_original.ReadFromFile(argv[1]);
        p_modified.ReadFromFile(argv[2]);

        std::vector<std::vector<uint8_t>> original[3] = {{}}, modified[3] = {{}};

        for (size_t i = 0; i < p_original.TellHeight(); ++i)
        {
            std::vector<uint8_t> orig_row[3] = {{}}, mod_row[3] = {{}};

            for (size_t j = 0; j < p_original.TellWidth(); ++j)
                for (size_t k = 0; k < 3; ++k)
                    switch (k) {
                        case 0:
                            orig_row[k].emplace_back(p_original.GetPixel(j, i).Red);
                            mod_row[k].emplace_back(p_modified.GetPixel(j, i).Red);
                            break;

                        case 1:
                            orig_row[k].emplace_back(p_original.GetPixel(j, i).Green);
                            mod_row[k].emplace_back(p_modified.GetPixel(j, i).Green);
                            break;

                        case 2:
                            orig_row[k].emplace_back(p_original.GetPixel(j, i).Blue);
                            mod_row[k].emplace_back(p_modified.GetPixel(j, i).Blue);
                    }

            for (size_t k = 0; k < 3; ++k)
            {
                modified[k].emplace_back(mod_row[k]);
                original[k].emplace_back(orig_row[k]);
            }
        }

        double sum = 0;
        for (size_t i = 0; i < 3; ++i)
            sum += psnr<8>(original[i], modified[i]);

        std::cout << (sum / 3.) << std::endl;
}
