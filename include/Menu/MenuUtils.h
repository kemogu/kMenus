#pragma once

#include <iostream>
#include <limits>
#include <string>

class MenuUtils {
public:
    static void clear_screen() {
        std::cout << "\033[2J\033[1;1H";
    }

    static void pause(const std::string& message = "Please press enter to continue...") {
        std::cout << "\n" << message;
        if (std::cin.rdbuf()->in_avail() > 0) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.get();
    }

    static int get_int_option(const std::string& prompt) {
        int choice;
        while (true) {
            std::cout << prompt;
            if (std::cin >> choice) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return choice;
            } else {
                std::cout << "Invalid choice! Please enter a number.\n";
                std::cin.clear(); 
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    }
};
