#ifndef MENU_H
#define MENU_H

#include <vector>
#include <memory>

#include "IMenuItem.h"
#include "ActionItem.h"

class Menu : public IMenuItem {
    public:
        Menu(const std::string& title, bool root = false) : IMenuItem(title), isRoot(root) {}

        void add_item(std::shared_ptr<IMenuItem> item) {
            items.push_back(item);
        }
        
        template <typename Func, typename... Args>
        void add_action(const std::string& title, Func&& func, Args&&... args) {
            auto boundAction = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            items.push_back(std::make_shared<ActionItem>(title, boundAction));
        }

        void add_sub_menu(std::shared_ptr<Menu> subMenu) {
            items.push_back(subMenu);
        }

        bool execute() override {
            while (true) {
                #ifdef _WIN32
                    std::system("cls");
                #else
                    std::system("clear");
                #endif
                
                // List items
                for (size_t i = 0; i < items.size(); ++i) {
                    std::cout << (i + 1) << ". " << items[i]->get_title() << "\n";
                }

                if (isRoot) {
                    std::cout << "0. Exit" << "\n";
                }
                else {
                    std::cout << "0. Go back." << "\n";
                }

                std::cout << "\n" << "Choice >> ";

                int choice;
                if (!(std::cin >> choice)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                if (0 == choice) {
                    return false;
                }

                bool isChoiceValid = ((choice > 0) && (choice <= static_cast<int>(this->items.size())));
                if (isChoiceValid) {
                    items[choice - 1]->execute();
                }
                else {
                    std::cout << "Invalid choice" << "\n";
                }
            }
        }
    
    private:
        std::vector<std::shared_ptr<IMenuItem>> items;
        bool isRoot;
};

#endif //! MENU_H