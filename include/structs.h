#ifndef MW_PARALLEL_PROGRAMMING_STRUCTS_H
#define MW_PARALLEL_PROGRAMMING_STRUCTS_H

#include <math.h>

#define SENSOR_FREQ 5000000000 // 1 sec / 200 Hz = 1e^12 / 200 = 5e^9
#define BALL_FREQ 500000000 // 2kHz

#define MAX_PLAYER_SENSORS 4


typedef unsigned long int sensor_timestamp_t;

/**
 * Single sensor measurement (only relevant fields for this application)
 */
struct sensor_record {
    unsigned int sid;       // sensor id
    sensor_timestamp_t ts;   // timestamp in picoseconds
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
    sensor_timestamp_t ts;    //in picoseconds
    unsigned int counter; //different for each unique event name
};


struct interruption {
    sensor_timestamp_t start;
    sensor_timestamp_t end;
};

struct player {
    std::string name;                   /// Player name+surname
    char role;                          /// Role -> P = Player, G = Goalkeeper, R = Referee
    char team;                          /// Either A,B or "" for the referee
    std::vector<unsigned int> sensors;  /// 2 sensors ids for player, 4 for the goalkeeper
    player(): sensors(4) {}
};


inline float distance(sensor_record a, sensor_record b) {
    return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z));
}
#endif //MW_PARALLEL_PROGRAMMING_STRUCTS_H
