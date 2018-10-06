
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
    std::set<sensor_id_t> balls;             //! For each of the two halves, set of the used balls (might be useless)
    std::vector<interruption> interruptions;
    std::map<sensor_id_t, unsigned int> sensorPlayerIdx;
    unsigned int referee_idx = 0;

    void load_from_directory(fs::path basepath);

    bool is_interrupted_at_time(sensor_timestamp_t ts) const
    {
        for(interruption inter : interruptions) {
            if (ts >= inter.start && ts <= inter.end)
                return true;
        }
        return false;
    }

    bool is_ball_sensor_id(sensor_id_t sid) const
    {
        return balls.find(sid) != balls.end();
    }

    bool is_player_sensor_id(sensor_id_t sid)
    {
        auto pi = sensorPlayerIdx.find(sid);
        if (pi == sensorPlayerIdx.end())
            return false;
        if (pi->second == referee_idx)
            return false;
        return true;
    }

    bool is_ball_inside_field(sensor_record ball_record)
    {
        int y = ball_record.y;
        int x = ball_record.x;

        //Field points taken from the DEBS2013 paper
        // (approximated to the most inside measures (e.g 0 instead of -50 on top etc..)

        return (y > 0 && y < 52477) && (x > -33960 && x < 33941);
    }
};


#endif // MW_PARALLEL_PROGRAMMING_GAME_H


