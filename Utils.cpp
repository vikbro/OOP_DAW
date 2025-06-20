#include "Utils.hpp"
#include <cstring>

char *copy(const char *str) {
    if (!str || !*str)
        throw std::invalid_argument("Error: empty string!");

    char *temp = new char[strlen(str) + 1];

    strcpy(temp, str);
    return temp;
}
