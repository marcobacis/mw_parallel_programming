
#ifndef MW_PARALLEL_PROGRAMMING_GAME_H
#define MW_PARALLEL_PROGRAMMING_GAME_H

#include <vector>
#include <set>
#include <map>
#include "utils.h"
#include "structs.h"


#define START_FIRST   10753295594424116
#define END_FIRST     12557295594424116
#define NO_BALL_START 12398000000000000

#define START_SECOND  13086639146403495
#define END_SECOND    14879639146403495


class game {
public:
    std::vector<std::vector<sensor_record> > game_records; //! Sensor data for the entire game
    std::vector<referee_event> game_events;  //! Referee events for the entire game
    std::vector<player> players;             //! Players data
    std::set<unsigned int> balls[2];         //! For each of the two halves, set of the used balls (might be useless)
    std::vector<interruption> interruptions;
    std::map<unsigned int, unsigned int> sensorPlayerIdx;
    unsigned int referee_idx = 0;

    void load_from_directory(fs::path basepath);
    bool is_interrupted_at_time(sensor_timestamp_t ts);
};


#endif // MW_PARALLEL_PROGRAMMING_GAME_H


