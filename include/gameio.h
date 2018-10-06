#ifndef MW_PARALLEL_PROGRAMMING_IO_H
#define MW_PARALLEL_PROGRAMMING_IO_H

#include <algorithm>
#include <iostream>
#include <experimental/filesystem>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <cctype>
#include <locale>
#include <set>

#include "utils.h"
#include "structs.h"


/**
 * Parses and return a sensor record from a string
 * @param line      Input sensor record line
 * @return          the parsed sensor record
 */
sensor_record parseSensorRecord(std::string line);

/**
 * Parses and return a event record from a string
 * @param line      Input event record line
 * @param base_ts   Base timestamp from which to start (used when reading 2nd half of the match)
 * @return          the parsed event
 */
referee_event parseRefereeEvent(std::string line, unsigned long int base_ts);

/**
 * Parses and return a player record from a string
 * @param line      Player record string
 * @return          the parsed player record
 */
player parsePlayer(std::string line);

/**
 * Loads the game sensors csv file into a vector of sensor records
 * @param file_path     Full game file path
 * @param game_vector   Vector to fill
 */
void loadGameCSV(std::experimental::filesystem::path file_path, std::vector<std::vector<sensor_record> > &game_vector);

/**
 * Loads a referee events csv file into a vector of events
 * @param file_path     Events csv file path
 * @param events_vector Vector to fill
 * @param base_ts       Base timestamp from which to start (used to load 2nd half)
 * @param append        Whether to append the read events or not to the vector
 */
void loadRefereeCSV(std::experimental::filesystem::path file_path, std::vector<referee_event> &events_vector,
                     unsigned long int base_ts = 0);


/**
 * Loads the players from the given file
 * @param file_path The players' file path
 * @param players  The vector in which to store the players
 */
void loadPlayers(std::experimental::filesystem::path file_path, std::vector<player> &players);


/**
 * Loads the balls in the sets (one set for each half)
 * @param file_path Balls' file path
 * @param balls the loaded balls
 */
void loadBalls(fs::path file_path, std::set<unsigned int>& balls);

#endif //MW_PARALLEL_PROGRAMMING_IO_H
