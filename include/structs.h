#ifndef MW_PARALLEL_PROGRAMMING_STRUCTS_H
#define MW_PARALLEL_PROGRAMMING_STRUCTS_H

#define EVENT_NAME_MAXLENGTH 50 /** Maximum event name length **/
#define PLAYER_NAME_MAXLENGTH 50 /** Maximum player name length **/

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
typedef struct event_t {
    unsigned int id;
    char name[EVENT_NAME_MAXLENGTH];
    unsigned long int ts;    //in picoseconds
    unsigned int counter; //different for each unique event name
    //char[] comment: useless
} referee_event;


typedef struct player_t {
    char name[PLAYER_NAME_MAXLENGTH];   //Player name+surname
    char role;                          //Role -> P = Player, G = Goalkeeper, R = Referee
    char team;                          //Either A,B or "" for the referee
    unsigned int sensors[4];            //2 sensors ids for player, 4 for the goalkeeper
} player;

#endif //MW_PARALLEL_PROGRAMMING_STRUCTS_H
