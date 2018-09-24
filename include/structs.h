#ifndef MW_PARALLEL_PROGRAMMING_STRUCTS_H
#define MW_PARALLEL_PROGRAMMING_STRUCTS_H

#include <math.h>

#define SENSOR_FREQ 5000000000 // 1 sec / 200 Hz = 1e^12 / 200 = 5e^9
#define BALL_FREQ 500000000 // 2kHz

#define EVENT_NAME_MAXLENGTH 50 /** Maximum event name length **/
#define PLAYER_NAME_MAXLENGTH 50 /** Maximum player name length **/
#define MAX_PLAYER_SENSORS 4

/**
 * Single sensor measurement (only relevant fields for this application)
 */
typedef struct game_t {
    unsigned int sid;       // sensor id
    unsigned long int ts;   // timestamp in picoseconds
    int x;         // x position (in mm)
    int y;         // y position (in mm)
    int z;         // z position (in mm)
} sensor_record;

/**
 * Referral event (goal, interruption begin/end) record structure
 */

enum event_type { INT_BEGIN, INT_END , OTHER_EVENT};

typedef struct event_t {
    unsigned int id;
    event_type type;
    unsigned long int ts;    //in picoseconds
    unsigned int counter; //different for each unique event name
    //char[] comment: useless
} referee_event;


typedef struct interruption_t {
    unsigned long int start;
    unsigned long int end;
} interruption;

typedef struct player_t {
    char name[PLAYER_NAME_MAXLENGTH];   //Player name+surname
    char role;                          //Role -> P = Player, G = Goalkeeper, R = Referee
    char team;                          //Either A,B or "" for the referee
    unsigned int sensors[MAX_PLAYER_SENSORS];            //2 sensors ids for player, 4 for the goalkeeper
} player;


inline float distance(sensor_record a, sensor_record b) {
    return sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z));
}
#endif //MW_PARALLEL_PROGRAMMING_STRUCTS_H
