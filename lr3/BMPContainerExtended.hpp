#pragma once
#include <mutex>
#include <atomic>
#include <thread>
#include <future>
#include "BMPContainer.hpp"



class BMPContainerExtended : public BMPContainer {
public:
        struct CellExtended : public BMPContainer::Cell {
        public:
            CellExtended(BMP &p_orig, BMP &p, int x, int y, uint8_t color_idx)
                : Cell(p, x, y, color_idx)
                , pic_orig{p_orig}
            {}

            virtual bool get() const override;
            virtual void set(bool v) override;

        private:
            BMP &pic_orig;
        };



        class CellExtIterator : public BMPContainer::CellIterator {
        public:
            void begin() override { itr = cells.begin(); } 
            void next() override { itr++; } 
            bool done() override { return itr == cells.end(); } 
            ICell& item() override { return *itr; }

        private:
            friend class BMPContainerExtended; 

            std::vector<CellExtended> cells;
            decltype(cells)::iterator itr;
        };

public:
        BMPContainerExtended() : BMPContainer() {}

        bool load_original(const std::string &fn);
        virtual bool load(const std::string &fn) override;
        virtual void save(const std::string &fn) override;
        CellExtIterator& cells(void) override { return itr; };

private:
        CellExtIterator itr;
        BMP original_picture;

        static std::mutex pic_mtx;
        static bool is_origin_loaded;
        static std::atomic<int> latch_;
        static std::vector<std::future<void>> futures;
};
