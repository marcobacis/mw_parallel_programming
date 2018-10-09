/** 
 * @file
 * Referee events class methods and local functions
 *
 * @author Marco Bacis
 * @author Daniele Cattaneo
 */

#include "referee.h"

#define EVENT_SEP ';'

/** 
 * Parses a single referee event record
 * @param line      The referee event string
 * @param base_ts   Base timestamp from which to start counting
 * @return          The parsed event
 */
referee_event parse_referee_event(std::string line, unsigned long int base_ts)
{
    std::stringstream lineStream(line);
    std::string cell;
    referee_event event;

    //event id
    std::getline(lineStream, cell, EVENT_SEP);
    event.id = std::stoul(cell);

    //event type
    std::getline(lineStream, cell, EVENT_SEP);

    //trim + tolower
    cell.erase(remove_if(cell.begin(), cell.end(), isspace), cell.end());
    std::transform(cell.begin(), cell.end(), cell.begin(), ::tolower);

    if(cell.compare("gameinterruptionbegin") == 0) {
        event.type = referee_event::type::INT_BEGIN;
    } else if (cell.compare("gameinterruptionend") == 0) {
        event.type = referee_event::type::INT_END;
    } else {
        event.type = referee_event::type::OTHER;
    }

    //event timestamp (need to parse hh:mm:ss.sss, or just hh = 0 in the first case)
    std::getline(lineStream, cell, EVENT_SEP);
    event.ts = base_ts + parse_datetime(cell);

    //event counter
    std::getline(lineStream, cell, EVENT_SEP);
    event.counter = std::stoul(cell);

    return event;
}


void load_referee_csv(std::string file_path, std::vector<referee_event> &events_vector,
                    unsigned long int base_ts)
{
    std::ifstream events_file;

    events_file.open(file_path);

    if (!events_file.is_open()) {
        throw std::runtime_error("Events file not found: " + file_path);
    }

    std::string line;

    while(std::getline(events_file, line)) {
        events_vector.push_back(parse_referee_event(line, base_ts));
    }

    events_file.close();
}
