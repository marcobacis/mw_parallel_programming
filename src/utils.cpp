/*
 * Utility methods
 */

#include "utils.h"


unsigned long int parse_datetime(std::string str)
{
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
