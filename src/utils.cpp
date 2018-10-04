/*
 * Utilitity methods
 */

#include "utils.h"

void printLoadPerc(unsigned long int count, unsigned long int total, bool ret) {
    unsigned int perc = count * 100 / total;

    DBOUT << "[";

    for(unsigned int i = 0; i < 100; i++) {
        if(i < perc) DBOUT << "=";
        else if (i == perc) DBOUT << ">";
        else DBOUT << " ";
    }

    DBOUT << "] " << perc << "%";

    if(ret) DBOUT << "\r";
    else DBOUT << std::endl;

    DBOUT.flush();
}


unsigned long int parse_datetime(std::string str) {
    std::string field;
    std::stringstream dateStream(str);

    const unsigned int multipliers[] = {3600000, 60000, 1000};
    const char sep = ':';

    unsigned long int ts = 0;
    int i = 0;

    do {
        std::getline(dateStream, field, sep);
        ts += std::stof(field) * multipliers[i];
        i++;
    } while (dateStream.rdstate() != std::ios_base::eofbit);

    return ts * 1000000000; // milliseconds -> picoseconds
}
