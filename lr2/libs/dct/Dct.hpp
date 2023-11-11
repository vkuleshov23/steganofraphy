#pragma once
#include <Settings.hpp>
#include <array>
#include <cmath>



#define BLOCK_SIZE 8
constexpr double C[] = {
    1.0 / std::sqrt(BLOCK_SIZE),
    std::sqrt(2) / std::sqrt(BLOCK_SIZE)
};



inline constexpr double clip(const double x);



std::array<std::array<double, BLOCK_SIZE>, BLOCK_SIZE>
dct(const std::array<std::array<double, BLOCK_SIZE>, BLOCK_SIZE> &data)
{
        auto result{data};

        for (size_t i = 0; i < data.size(); ++i)
        {
            double c[2] = {C[(i != 0)]};

            for (size_t j = 0; j < data[0].size(); ++j)
            {
                c[1] = C[(j != 0)]; 

                double sum = 0;
                for (size_t k = 0; k < BLOCK_SIZE; ++k)
                    for (size_t l = 0; l < BLOCK_SIZE; ++l)
                        sum += static_cast<double>(data[k][l]) 
                             * std::cos(((2 * k + 1) * i * M_PI) / (BLOCK_SIZE * 2))
                             * std::cos(((2 * l + 1) * j * M_PI) / (BLOCK_SIZE * 2));

                result[i][j] = c[0] * c[1] * sum;
            }
        }

        return result;
}


std::array<std::array<double, BLOCK_SIZE>, BLOCK_SIZE>
idct(const std::array<std::array<double, BLOCK_SIZE>, BLOCK_SIZE> &data)
{
        auto result{data};

        for (size_t i = 0; i < data.size(); ++i)
            for (size_t j = 0; j < data[0].size(); ++j)
            {
                double sum = 0;
                for (size_t k = 0; k < BLOCK_SIZE; ++k)
                {
                    double c[2] = {C[(k != 0)]};

                    for (size_t l = 0; l < BLOCK_SIZE; ++l)
                    {
                        c[1] = C[(l != 0)]; 
                        sum += c[0] * c[1]
                             * static_cast<double>(data[k][l]) 
                             * std::cos(((2 * i + 1) * k * M_PI) / (BLOCK_SIZE * 2))
                             * std::cos(((2 * j + 1) * l * M_PI) / (BLOCK_SIZE * 2));
                    }
                }

                result[i][j] = clip(sum);
            }

        return result;
}


inline constexpr double 
clip(const double x)
{
        if (x > 255)
            return 255.;
        else if (x < 0)
            return 0.;
        else
            return x;
}
