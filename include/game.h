/** @file
 * @brief Game-related data structures
 *
 * @author Marco Bacis
 * @author Daniele Cattaneo */

#ifndef MW_PARALLEL_PROGRAMMING_GAME_H
#define MW_PARALLEL_PROGRAMMING_GAME_H

#include <vector>
#include <set>
#include <map>
#include <cmath>
#include "utils.h"
#include "referee.h"


const ps_timestamp_t sensor_sample_period = one_second / 200;       //!< Sensor sample period (the frequency is 200Hz)
const ps_timestamp_t ball_sensor_sample_period = one_second / 2000; //!< Sensor sample period (the frequency is 2kHz)

const ps_timestamp_t first_half_start = 10753295594424116;          //!< Beginning of the game
const ps_timestamp_t no_ball_start = 12398000000000000;             //!< Beginning of the period in which the ball sensor didn't work
const ps_timestamp_t first_half_end = 12557295594424116;            //!< End time of the first half of the game
const ps_timestamp_t second_half_start = 13086639146403495;         //!< Starting time of the second half (and end of no_ball)
const ps_timestamp_t second_half_end = 14879639146403495;           //!< End of the second half (and of the game)


typedef unsigned int sensor_id_t;                                   /// Sensor ID number type

/**
 * Single sensor measurement (only relevant fields for this application)
 */
struct sensor_record {
    sensor_id_t sid;            //!< sensor id
    ps_timestamp_t ts;          //!< timestamp in sensor units
    int x;                      //!< x position (in mm)
    int y;                      //!< y position (in mm)
    int z;                      //!< z position (in mm)

    /** Computes the 3D euclidean distance to the given record
     * @param b     The record from which we are computing the distance
     * @return      the computed distance
     */
    inline double distance_to(sensor_record& b)
    {
        return sqrt((x-b.x)*(x-b.x) + (y-b.y)*(y-b.y) + (z-b.z)*(z-b.z));
    }

    /** Computes the 2D euclidean distance to the given record, on the xy plane
     * @param b     The record from which we are computing the distance
     * @return      the 2D computed distance
     */
    inline double distance_on_ground_to(sensor_record& b)
    {
        return sqrt((x-b.x)*(x-b.x) + (y-b.y)*(y-b.y));
    }
};

/**
 * Player Metadata
 */
struct player {
    std::string name;                   //!< Player name+surname
    char role;                          //!< Role -> P = Player, G = Goalkeeper, R = Referee
    char team;                          //!< Either A,B or "" for the referee
    std::vector<sensor_id_t> sensors;   //!< 2 sensors ids for player, 4 for the goalkeeper
    player(): sensors(4) {}
};

/**
 * Game interruption
 */
struct interruption {
    ps_timestamp_t start;
    ps_timestamp_t end;
};

/**
 * Represents the full game data, including sensor records, events, players etc...
 */
class game {
public:
    std::vector<std::vector<sensor_record> > game_records; //! Sensor data for the entire game
    std::vector<referee_event> game_events;  //!< Referee events for the entire game
    std::vector<player> players;             //!< Players data
    std::set<sensor_id_t> balls;             //!< For each of the two halves, set of the used balls (might be useless)
    std::vector<interruption> interruptions;
    std::map<sensor_id_t, unsigned int> sensorPlayerIdx;
    unsigned int referee_idx = 0;

    /** Loads the game CSVs into the game instance
     *
     * The CSV files must have these names / relative position to the base path:
     * - referee-events/interruptions/
     *      - 1stHalf.csv
     *      - 2ndHalf.csv
     * - players.csv
     * - balls.csv
     * - full-game.csv
     *
     * @param basepath The base path in which all the game related data resides
     */
    void load_from_directory(const std::string& basepath);

    /** Tells if the game is interrupted at the given timestamp
     * @param ts Timestamp for which to check
     * @return true if the game is interrupted, false otherwise
     */
    bool is_interrupted_at_time(ps_timestamp_t ts) const
    {
        for(interruption inter : interruptions) {
            if (ts >= inter.start && ts <= inter.end)
                return true;
        }
        return false;
    }

    /** Tells whether the sensor is attached to a ball or not
     * @param sid   ID of the sensor to check
     * @return      true if the sensor is attached to a ball, false otherwise
     */
    bool is_ball_sensor_id(sensor_id_t sid) const
    {
        return balls.find(sid) != balls.end();
    }

    /** Tells whether the sensor is attached to a player
     * @param sid   ID of the sensor to check
     * @return      true if the sensor is attached to a player, false otherwise
     */
    bool is_player_sensor_id(sensor_id_t sid)
    {
        auto pi = sensorPlayerIdx.find(sid);
        if (pi == sensorPlayerIdx.end())
            return false;
        if (pi->second == referee_idx)
            return false;
        return true;
    }

    /** Tells whether the sensor's ball is inside the soccer field
     *
     * The field size and limiting points are taken from the DEBS2013 challenge paper
     * (approximated to the most inside measures e.g. highest low y, lowest high x etc...)
     *
     * @param ball_record   The record for which to check the position
     * @return              true if the sensor's position is inside the field, faalse otherwise
     */
    bool is_ball_inside_field(sensor_record ball_record)
    {
        int y = ball_record.y;
        int x = ball_record.x;

        return (x > 0 && x < 52477) && (y > -33960 && y < 33941);
    }

    /** Returns the total length in picoseconds of the game data. 
     * @returns The duration of the recording.
     */
    ps_timestamp_t recording_length()
    {
        return game_records.back().back().ts - game_records.front().front().ts;
    }
};


#endif // MW_PARALLEL_PROGRAMMING_GAME_H


