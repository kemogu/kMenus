# kMenu (kemogu's Menu System)

![Licence](https://img.shields.io/badge/license-MIT-blue.svg)
![C++ Version](https://img.shields.io/badge/C%2B%2B-17-green.svg)
![Structure](https://img.shields.io/badge/structure-header--only-brightgreen.svg)

**[English](#kmenu-english) | [Türkçe](#kmenu-türkçe)**

## kMenu (English)
**kMenu is a lightweight, header-only CLI (Command Line Interface) menu system for C++. Built using the Composite Design Pattern, it allows developers to create nested menus and bind actions easily.**

### ✨ Features
* ** Composite Structure: Menus can contain other menus or action items seamlessly.

* ** Modern C++: Utilizes std::function, std::shared_ptr, and variadic templates.

* ** Type-Safe Input: Robust input handling utilities to prevent infinite loops on bad input.

* ** Easy Integration: Just include the headers and start building your CLI.

### 🛠️ Code Structure & Improvements
1. Interface (IMenuItem.h)
* ** Encapsulation: Member variables are moved from protected to private to reduce coupling.

* ** Move Semantics: Constructors use pass-by-value and std::move for string optimization.

* ** Nodiscard: Getters are marked with [[nodiscard]] to encourage usage of return values.

``` cpp

#pragma once

#include <string>
#include <utility>

class IMenuItem {
public:
    explicit IMenuItem(std::string title) : title_(std::move(title)) {}
    virtual ~IMenuItem() = default;

    // Prevents ignoring the return value
    [[nodiscard]] const std::string& get_title() const {
        return title_;
    }

    // Pure virtual method for executing the item
    virtual bool execute() = 0;

private:
    std::string title_;
};
```

2. Action Implementation (ActionItem.h)
Functional Approach: Stores logic as std::function<void()>, allowing lambdas, function pointers, or functors.

Validation: Checks if the action is callable before execution.

```cpp
#pragma once

#include "IMenuItem.h"
#include <functional>
#include <iostream>

class ActionItem : public IMenuItem {
public:
    // Takes title and a callable object (lambda, function ptr, etc.)
    ActionItem(std::string title, std::function<void()> actionFunc) 
        : IMenuItem(std::move(title)), action_(std::move(actionFunc)) {}

    bool execute() override {
        if (action_) {
            action_();
        }
        return true; // Returns true to keep the parent menu loop running
    }

private:
    std::function<void()> action_;
};
```

3. Utilities (MenuUtils.h)
Namespace over Class: Converted class MenuUtils to namespace MenuUtils. In C++, utilities that maintain no state should effectively be free functions or within a namespace, not static class methods.

Robust Input: Improved cin handling to clear error flags and buffers effectively.

```cpp
#pragma once

#include <iostream>
#include <limits>
#include <string>
#include <optional>

namespace MenuUtils {
    
    inline void clear_screen() {
        // ANSI escape code for clearing screen (Works on Linux/Mac/Win10+)
        std::cout << "\033[2J\033[1;1H";
    }

    inline void pause(const std::string& message = "Please press enter to continue...") {
        std::cout << "\n" << message;
        // Check if there is anything in the buffer to ignore
        if (std::cin.rdbuf()->in_avail() > 0) {
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        std::cin.get();
    }

    inline int get_int_option(const std::string& prompt) {
        int choice;
        while (true) {
            std::cout << prompt;
            if (std::cin >> choice) {
                // Clear the newline left in the buffer by cin >>
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return choice;
            } else {
                std::cout << "Invalid input! Please enter a number.\n";
                std::cin.clear(); // Reset error flags
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear bad input
            }
        }
    }

    inline std::string get_string_option(const std::string& prompt) {
        std::string input;
        std::cout << prompt;
        // std::ws removes leading whitespace
        std::getline(std::cin >> std::ws, input);
        return input;
    }
}
```

4. The Menu System (Menu.h)
Variadic Templates: add_action uses forwarding references (Func&&) for perfect forwarding, avoiding unnecessary copies of the callback objects.

Smart Pointers: Usage of std::shared_ptr allows items to be shared or managed automatically.

Error Handling: Wraps execution in try-catch to prevent the application from crashing due to an unhandled exception in a menu item.

```cpp
#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <iostream>

#include "IMenuItem.h"
#include "ActionItem.h"
#include "MenuUtils.h"

class Menu : public IMenuItem {
public:
    Menu(std::string title, bool root = false) 
        : IMenuItem(std::move(title)), is_root_(root) {}

    // Add an existing item (Menu or ActionItem)
    void add_item(std::shared_ptr<IMenuItem> item) {
        items_.push_back(std::move(item));
    }
    
    // Helper to create and add an action inline
    template <typename Func, typename... Args>
    void add_action(const std::string& title, Func&& func, Args&&... args) {
        // Create a bind object or lambda
        auto boundAction = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
        items_.push_back(std::make_shared<ActionItem>(title, boundAction));
    }

    void add_sub_menu(std::shared_ptr<Menu> subMenu) {
        items_.push_back(std::move(subMenu));
    }

    bool execute() override {
        while (true) {
            MenuUtils::clear_screen();
            
            // Print Title
            std::cout << "=== " << get_title() << " ===\n\n";

            // List items
            for (size_t i = 0; i < items_.size(); ++i) {
                std::cout << (i + 1) << ". " << items_[i]->get_title() << "\n";
            }

            // Exit/Back option
            if (is_root_) {
                std::cout << "0. Exit" << "\n";
            } else {
                std::cout << "0. Go back" << "\n";
            }

            int choice = MenuUtils::get_int_option("\nChoice >> ");
            
            if (choice == 0) {
                return false; // Break the loop and return to previous menu
            }

            if (choice > 0 && choice <= static_cast<int>(items_.size())) {
                try {
                    // Execute the chosen item
                    // If it's a submenu, it enters its own loop.
                    // If it's an action, it runs and returns.
                    items_[choice - 1]->execute();
                    
                    // Pause only if it was an action, not a submenu return
                    // (Optional design choice, here we pause generally)
                    MenuUtils::pause();
                }
                catch(const std::exception& err) {
                    std::cerr << "\nError: " << err.what() << '\n';
                    MenuUtils::pause("Error occurred. Press enter to continue...");
                }
            } else {
                std::cout << "Invalid choice!\n";
                MenuUtils::pause();
            }
        }
    }

private:
    std::vector<std::shared_ptr<IMenuItem>> items_;
    bool is_root_;
};
```

## kMenu (Türkçe)
**kMenu, C++ için geliştirilmiş hafif, header-only (sadece başlık dosyası) bir komut satırı menü sistemidir. Composite Tasarım Deseni (Composite Design Pattern) üzerine inşa edilmiştir, bu sayede iç içe menüler ve aksiyonlar kolayca yönetilebilir.**

### ✨ Özellikler
** *Kompozit Yapı: Menüler, başka menüleri veya aksiyon öğelerini içerebilir.

** * Modern C++: std::function, std::shared_ptr, ve move semantics gibi modern özellikler kullanır.

** * Güvenli Girdi: Hatalı kullanıcı girişlerinde sonsuz döngüye girmeyi önleyen sağlam yardımcı fonksiyonlar.

** * Kolay Entegrasyon: Sadece başlık dosyalarını dahil edin ve kullanmaya başlayın.

### 🔍 Kod İncelemesi ve Geliştirmeler
Bu bölüm, orijinal koda yapılan mühendislik dokunuşlarını açıklar.

1. Arayüz (IMenuItem.h)
Kapsülleme (Encapsulation): protected olan title değişkeni private yapıldı. Alt sınıfların ebeveynin verisine doğrudan erişmesi "tight coupling" (sıkı bağ) yaratır, bunun yerine getter kullanmak daha güvenlidir.

Move Semantics: const std::string& yerine değeri alıp std::move kullanmak, geçici nesneler (r-value) için kopyalamayı önler (optimization).

[[nodiscard]]: get_title fonksiyonunun dönüş değerinin göz ardı edilmemesi gerektiğini derleyiciye bildirir.

(Kod bloğu yukarıdaki İngilizce bölümde verilmiştir)

2. Aksiyon Uygulaması (ActionItem.h)
Fonksiyonel Yaklaşım: İş mantığını std::function olarak tutar. Bu sayede lambda ifadeleri, fonksiyon pointerları veya functor'lar kolayca menüye eklenebilir.

Kontrol: Aksiyon çağrılmadan önce geçerli olup olmadığı (if (action_)) kontrol edilir.

(Kod bloğu yukarıdaki İngilizce bölümde verilmiştir)

3. Yardımcı Araçlar (MenuUtils.h)
Namespace vs Class: Orijinal koddaki class MenuUtils yapısı namespace'e çevrildi. Durum (state) tutmayan yardımcı fonksiyonlar için statik sınıf metodları yerine namespace veya serbest fonksiyonlar (free functions) kullanmak C++ Core Guidelines tarafından önerilir.

inline: Fonksiyonlar header-only olduğu için "Multiple Definition" hatalarını önlemek adına inline anahtar kelimesi eklendi.

(Kod bloğu yukarıdaki İngilizce bölümde verilmiştir)

4. Menü Sistemi (Menu.h)
Forwarding References: add_action fonksiyonu artık argümanları kopyalamak yerine std::forward ile iletiyor. Bu, performans artışı sağlar.

Hata Yönetimi: execute içindeki işlemler try-catch bloğuna alındı. Eğer bir menü elemanı çalışırken throw ederse, tüm programın çökmesi yerine hata ekrana yazılır ve menü çalışmaya devam eder.

(Kod bloğu yukarıdaki İngilizce bölümde verilmiştir)

### 🚀 Örnek Kullanım (Example Usage)

```cpp
#include "Menu.h"

void say_hello() {
    std::cout << "Hello World form function!\n";
}

int main() {
    // Kök Menü
    auto mainMenu = std::make_shared<Menu>("Main Menu", true);

    // Basit bir aksiyon ekleme
    mainMenu->add_action("Say Hello", say_hello);

    // Lambda ile aksiyon ekleme
    mainMenu->add_action("Calculate 2+2", [](){
        std::cout << "Result: " << 2 + 2 << "\n";
    });

    // Alt Menü Oluşturma
    auto subMenu = std::make_shared<Menu>("Settings");
    subMenu->add_action("Volume Up", [](){ std::cout << "Volume increased.\n"; });
    subMenu->add_action("Volume Down", [](){ std::cout << "Volume decreased.\n"; });

    // Alt menüyü ana menüye ekleme
    mainMenu->add_sub_menu(subMenu);

    // Menüyü çalıştır
    mainMenu->execute();

    return 0;
}
```