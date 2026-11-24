#include "Logo.h"
#include <iostream>
#include <string>

void mostrarLogo() {
    const std::string NARANJA = "\033[38;2;255;140;0m";
    const std::string RESET = "\033[0m";

    std::cout << NARANJA << "\n";
    std::cout << "    ____    _     _____ _____ _____ _____ _   _ _____ \n";
    std::cout << "   / ___|  / \\   | ___ | ___ | ____|_   _| \\ | | ____|\n";
    std::cout << "  | |     / _ \\  | |_  | |_  |  _|   | | |  \\| |  _|   \n";
    std::cout << "  | |___ / ___ \\ |  _| |  _| | |___  | | | |\\  | |___ \n";
    std::cout << "   \\____/_/   \\_\\|_|   |_|   |____| |___|| |_| \\_|___|\n";
    std::cout << "                                                 ENGINE\n";
    std::cout << "=====================================================\n";
    std::cout << RESET;
}
