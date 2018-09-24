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

/* Methods */

/**
 * Grabs the next available sensor record from a input stream and store it in the given result reference
 * @param str       Input sensor records stream (usually from a file)
 * @param result    Reference where to set the read parameters
 */
bool getNextSensorRecord(std::istream& str, sensor_record &result);

/**
 * Grabs the next available event record from a input stream and store it in the given result reference
 * @param str       Input event records stream (usually from a file)
 * @param result    Reference where to set the read parameters
 * @param base_ts   Base timestamp from which to start (used when reading 2nd half of the match)
 */
void getNextRefereeEvent(std::istream &str, referee_event &event, unsigned long int base_ts = 0);

/**
 * Grabs the next available player record from the input stream
 * @param str
 * @return the new player got from the stream
 */
player getNextPlayer(std::istream &str);

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
void loadBalls(fs::path file_path, std::set<unsigned int> balls[]);

#endif //MW_PARALLEL_PROGRAMMING_IO_H
