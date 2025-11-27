#ifndef ACTIONITEM_H
#define ACTIONITEM_H

#include <functional>

#include "IMenuItem.h"

class ActionItem : public IMenuItem {
    public:
        ActionItem(const std::string& title, std::function<void()> actionFunc) 
        : IMenuItem(title), action(actionFunc) {}
    private:
        std::function<void()> action;
    
};


#endif //! ACTIONITEM_H