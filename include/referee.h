/** @file
 * @brief Referee events data structure and loading method
 *
 * @author Marco Bacis
 * @author Daniele Cattaneo */

#ifndef MW_PARALLEL_PROGRAMMING_IO_H
#define MW_PARALLEL_PROGRAMMING_IO_H

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cctype>
#include <locale>
#include <set>
#include "utils.h"

/**
 * Referral event (goal, interruption begin/end) record structure
 */
struct referee_event {
    enum type { 
        INT_BEGIN,
        INT_END, 
        OTHER
    };
    unsigned int id;            //!< Event ID
    type type;                  //!< Event type
    ps_timestamp_t ts;          //!< Event timestamp
    unsigned int counter;       //!< Event counter (different for each unique event name)
};


/** Loads a referee events csv file into a vector of events
 * @param file_path     Events csv file path
 * @param events_vector Vector to fill
 * @param base_ts       Base timestamp from which to start (used to load 2nd half)
 * @param append        Whether to append the read events or not to the vector
 */
void load_referee_csv(std::string file_path, std::vector<referee_event> &events_vector,
                     unsigned long int base_ts = 0);


#endif //MW_PARALLEL_PROGRAMMING_IO_H
