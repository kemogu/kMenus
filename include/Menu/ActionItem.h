#ifndef ACTIONITEM_H
#define ACTIONITEM_H

#include <iostream>
#include <functional>
#include <limits>

#include "IMenuItem.h"

class ActionItem : public IMenuItem {
    public:
        ActionItem(const std::string& title, std::function<void()> actionFunc) 
        : IMenuItem(title), action(actionFunc) {}

        bool execute() override {
            try {
                action();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
            }
            catch(const std::exception& e) {
                std::cerr << e.what() << '\n';
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
            }
            return true;
        }
    private:
        std::function<void()> action;
};


#endif //! ACTIONITEM_H