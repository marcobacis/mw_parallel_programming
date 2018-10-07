#ifndef MW_PARALLEL_PROGRAMMING_STRUCTS_H
#define MW_PARALLEL_PROGRAMMING_STRUCTS_H

#include <math.h>

#define      SENSOR_SAMPLE_PERIOD 5000000000  // picoseconds// 1 sec / 200 Hz = 1e^12 ps / 200 = 5e^9 ps
#define BALL_SENSOR_SAMPLE_PERIOD  500000000  // 2 kHz


typedef unsigned long int sensor_timestamp_t;
const sensor_timestamp_t one_second = 1000000000000 / SENSOR_SAMPLE_PERIOD;

typedef unsigned int sensor_id_t;

/**
 * Single sensor measurement (only relevant fields for this application)
 */
struct sensor_record {
    sensor_id_t sid;       // sensor id
    sensor_timestamp_t ts; // timestamp in sensor units
    int x;         // x position (in mm)
    int y;         // y position (in mm)
    int z;         // z position (in mm)
};

/**
 * Referral event (goal, interruption begin/end) record structure
 */

enum event_type { 
    INT_BEGIN, 
    INT_END, 
    OTHER_EVENT
};

struct referee_event {
    unsigned int id;
    event_type type;
    sensor_timestamp_t ts;    //in MAYBE picoseconds????
    unsigned int counter; //different for each unique event name
};


struct interruption {
    sensor_timestamp_t start;   // in sensor units
    sensor_timestamp_t end;     // in sensor units
};

struct player {
    std::string name;                   /// Player name+surname
    char role;                          /// Role -> P = Player, G = Goalkeeper, R = Referee
    char team;                          /// Either A,B or "" for the referee
    std::vector<sensor_id_t> sensors;  /// 2 sensors ids for player, 4 for the goalkeeper
    player(): sensors(4) {}
};


inline double distance(sensor_record& a, sensor_record& b)
{
    return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z));
}


inline double distance_on_ground(sensor_record& a, sensor_record& b)
{
    return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
}


#endif //MW_PARALLEL_PROGRAMMING_STRUCTS_H
