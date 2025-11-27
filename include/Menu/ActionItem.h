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
            if (action) {
                action();
            }
            return true;
        }
    private:
        std::function<void()> action;
};


#endif //! ACTIONITEM_H