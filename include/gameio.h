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
#include "game.h"


/**
 * Parses and return a event record from a string
 * @param line      Input event record line
 * @param base_ts   Base timestamp from which to start (used when reading 2nd half of the match)
 * @return          the parsed event
 */
referee_event parse_referee_event(std::string line, unsigned long int base_ts);

/**
 * Loads a referee events csv file into a vector of events
 * @param file_path     Events csv file path
 * @param events_vector Vector to fill
 * @param base_ts       Base timestamp from which to start (used to load 2nd half)
 * @param append        Whether to append the read events or not to the vector
 */
void load_referee_csv(std::string file_path, std::vector<referee_event> &events_vector,
                     unsigned long int base_ts = 0);

#endif //MW_PARALLEL_PROGRAMMING_IO_H
