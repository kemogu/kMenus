#ifndef IMENUITEM_H
#define IMENUITEM_H

#include <string>

class IMenuItem {
    protected:
        std::string title;
    public:
        explicit IMenuItem(const std::string& t) : title(t) {}
        virtual ~IMenuItem() = default;

        std::string get_title() const {
            return this->title;
        }

        virtual bool execute() = 0;
};

#endif //! IMENUITEM_H