#include "IContainer.hpp"
#include <EasyBmp/EasyBMP.h>
#include <filesystem>
#include <cstdlib>
#include <ctime>


namespace fs = std::filesystem;


class BMPContainer : public IContainer {
public:
        struct Cell : public ICell {
        protected:
            int x_, y_;
            uint8_t color_;
            BMP &pic;

        public:
            Cell(BMP& pic_, int x, int y, uint8_t color)
                : x_{x}, y_{y}
                , color_{color}
                , pic{pic_}
            {
                srand(time(nullptr));
            }

            virtual bool get() const override;
            virtual void set(bool v) override;
        };    


        struct CellIterator : public ICellIterator {
            friend class BMPContainer;

            void
            begin()
            override
            {
                itr = cells.begin();
            }

            void
            next()
            override
            {
                itr++;
            }

            bool
            done()
            override
            {
                return itr == cells.end();
            }

            ICell&
            item()
            override
            {
                return *itr;
            }

        private:
            std::vector<Cell> cells;
            decltype(cells)::iterator itr;
        };

public:
        virtual bool load(const std::string &fn) override;
        virtual void save(const std::string &fn) override;
        virtual ICellIterator& cells(void) override { return itr; };

protected:
        BMP picture;

private:
        CellIterator itr;
};
