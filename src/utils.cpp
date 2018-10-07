/*
 * Utility methods
 */

#include "utils.h"


sensor_timestamp_t parse_datetime(std::string str)
{
    std::string field;
    std::stringstream dateStream(str);

    const unsigned int multipliers[] = {3600, 60, 1};
    const char sep = ':';

    double ts = 0.0;
    int i = 0;

    do {
        std::getline(dateStream, field, sep);
        ts += std::stof(field) * multipliers[i];
        i++;
    } while (dateStream.rdstate() != std::ios_base::eofbit);

    return ts * one_second; // convert to picoseconds
}
