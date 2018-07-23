/*
 * Utilitity methods
 */

#include "utils.h"

void printLoadPerc(unsigned long int count, unsigned long int total, bool ret) {
    unsigned int perc = count * 100 / total;

    std::cout << "[";

    for(unsigned int i = 0; i < 100; i++) {
        if(i < perc) std::cout << "=";
        else if (i == perc) std::cout << ">";
        else std::cout << " ";
    }

    std::cout << "] " << perc << "%";

    if(ret) std::cout << "\r";
    else std::cout << std::endl;

    std::cout.flush();
}