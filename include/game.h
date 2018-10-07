
#ifndef MW_PARALLEL_PROGRAMMING_GAME_H
#define MW_PARALLEL_PROGRAMMING_GAME_H

#include <vector>
#include <set>
#include <map>
#include <cmath>
#include "utils.h"


#define START_FIRST   10753295594424116
#define END_FIRST     12557295594424116
#define NO_BALL_START 12398000000000000

#define START_SECOND  13086639146403495
#define END_SECOND    14879639146403495


const ps_timestamp_t sensor_sample_period = 5000000000;  // picoseconds// 1 sec / 200 Hz = 1e^12 ps / 200 = 5e^9 ps
const ps_timestamp_t ball_sensor_sample_period = 500000000; // 2 kHz


typedef unsigned int sensor_id_t;

/**
 * Single sensor measurement (only relevant fields for this application)
 */
struct sensor_record {
    sensor_id_t sid;       // sensor id
    ps_timestamp_t ts; // timestamp in sensor units
    int x;         // x position (in mm)
    int y;         // y position (in mm)
    int z;         // z position (in mm)

    inline double distance_to(sensor_record& b)
    {
        return sqrt((x-b.x)*(x-b.x) + (y-b.y)*(y-b.y) + (z-b.z)*(z-b.z));
    }

    inline double distance_on_ground_to(sensor_record& b)
    {
        return sqrt((x-b.x)*(x-b.x) + (y-b.y)*(y-b.y));
    }
};

/**
 * Referral event (goal, interruption begin/end) record structure
 */
struct referee_event {
    enum type { 
        INT_BEGIN,
        INT_END, 
        OTHER
    };
    unsigned int id;
    type type;
    ps_timestamp_t ts;    //in picoseconds
    unsigned int counter;     //different for each unique event name
};


struct interruption {
    ps_timestamp_t start;   // in sensor units
    ps_timestamp_t end;     // in sensor units
};


struct player {
    std::string name;                   /// Player name+surname
    char role;                          /// Role -> P = Player, G = Goalkeeper, R = Referee
    char team;                          /// Either A,B or "" for the referee
    std::vector<sensor_id_t> sensors;  /// 2 sensors ids for player, 4 for the goalkeeper
    player(): sensors(4) {}
};


class game {
public:
    std::vector<std::vector<sensor_record> > game_records; //! Sensor data for the entire game
    std::vector<referee_event> game_events;  //! Referee events for the entire game
    std::vector<player> players;             //! Players data
    std::set<sensor_id_t> balls;             //! For each of the two halves, set of the used balls (might be useless)
    std::vector<interruption> interruptions;
    std::map<sensor_id_t, unsigned int> sensorPlayerIdx;
    unsigned int referee_idx = 0;

    void load_from_directory(const fs::path& basepath);

    bool is_interrupted_at_time(ps_timestamp_t ts) const
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

        return (x > 0 && x < 52477) && (y > -33960 && y < 33941);
    }
};


#endif // MW_PARALLEL_PROGRAMMING_GAME_H


