#pragma once
#include <string>
#include <vector>



class ICell {
public:
        virtual bool get() const = 0;
        virtual void set(bool) = 0;
};


class ICellIterator {
public:
        virtual void begin() = 0;
        virtual void next() = 0;
        virtual bool done() = 0;
        virtual ICell& item() = 0;
};


class IContainer
{
public:
        virtual bool load(const std::string &fn) = 0;
        virtual void save(const std::string &fn) = 0;
        virtual ICellIterator& cells(void) = 0;
};
