//
// Created by Marco Bacis on 23/07/18.
//

#ifndef MW_PARALLEL_PROGRAMMING_IO_H
#define MW_PARALLEL_PROGRAMMING_IO_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

/* Data Structures */

/*
 * Game record structure. It has 13 fields,
 * of which only 5 are interesting for us.
 */
typedef struct game_t {
    unsigned int sid;       // sensor id
    unsigned long int ts;   // timestamp in picoseconds
    int x;         // x position (in mm)
    int y;         // y position (in mm)
    int z;         // z position (in mm)
} sensor_record;

/*
 * Referral event (goal, interruption begin/end) record structure
 * TODO: Use a unsigned long timestamp instead of all the numbers
 */
typedef struct event_t {
    unsigned int id;
    char event_name[50];
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond;
    unsigned int event_counter;
    //char[] comment: useless
} referee_event;

/* Methods */

void getNextSensorRecord(std::istream& str, sensor_record &result);




#endif //MW_PARALLEL_PROGRAMMING_IO_H
