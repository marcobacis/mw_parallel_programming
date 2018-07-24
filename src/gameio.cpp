#include "utils.h"
#include "gameio.h"

#define GAME_SEP ','
#define EVENT_SEP ';'

void getNextSensorRecord(std::istream &str, sensor_record &result) {

    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;
    //sid
    std::getline(lineStream, cell, GAME_SEP);
    result.sid = std::stoul(cell);

    //ts
    std::getline(lineStream, cell, GAME_SEP);
    result.ts = std::stoull(cell);

    //x
    std::getline(lineStream, cell, GAME_SEP);
    result.x = std::stoi(cell);

    //y
    std::getline(lineStream, cell, GAME_SEP);
    result.y = std::stoi(cell);

    //z
    std::getline(lineStream, cell, GAME_SEP);
    result.z = std::stoi(cell);
}


void getNextRefereeEvent(std::istream &str, referee_event &event, unsigned long int base_ts) {
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

    //event id
    std::getline(lineStream, cell, EVENT_SEP);
    event.id = std::stoul(cell);

    //event name
    std::getline(lineStream, cell, EVENT_SEP);
    cell.copy(event.name, EVENT_NAME_MAXLENGTH);


    //event timestamp (need to parse hh:mm:ss.sss, or just hh = 0 in the first case)
    std::getline(lineStream, cell, EVENT_SEP);
    event.ts = base_ts + parse_datetime(cell);

    //event counter
    std::getline(lineStream, cell, EVENT_SEP);
    event.counter = std::stoul(cell);

}


void loadGameCSV(std::experimental::filesystem::path file_path, std::vector<sensor_record> &game_vector) {

    std::ifstream game_file;

    //print loading header
    DBOUT << "Loading game file... this will take a while" << std::endl << std::flush;

    //count lines
    game_file.open(file_path);

    if (!game_file.is_open()) {
        throw std::runtime_error("Game file not found: " + file_path.string());
    }

    unsigned long int tot_records = std::count(std::istreambuf_iterator<char>(game_file),
               std::istreambuf_iterator<char>(), '\n');

    DBOUT << tot_records << " total sensor records found." << std::endl << std::flush;

    //read file and import records

    game_vector.reserve(tot_records);

    game_file.clear();
    game_file.seekg(0);

    for (unsigned long int i = 0; i < tot_records; i++) {
        getNextSensorRecord(game_file, game_vector[i]);
    }

    game_file.close();
}


void loadRefereeCSV(std::experimental::filesystem::path file_path, std::vector<referee_event> &events_vector,
                    unsigned long int base_ts, bool append) {
    std::ifstream events_file;

    //print loading header
    DBOUT << "Loading events file " << file_path << "... this will take a while" << std::endl << std::flush;

    //count lines
    events_file.open(file_path);

    if (!events_file.is_open()) {
        throw std::runtime_error("Events file not found: " + file_path.string());
    }

    unsigned long int tot_records = std::count(std::istreambuf_iterator<char>(events_file),
                                 std::istreambuf_iterator<char>(), '\n');


    DBOUT << tot_records << " total referee events found." << std::endl << std::flush;

    //read file and import events

    int baseidx = append == true ? events_vector.size() : 0;

    events_vector.resize(baseidx + tot_records);

    events_file.clear();
    events_file.seekg(0);

    for (unsigned long int i = 0; i < tot_records; i++) {
        getNextRefereeEvent(events_file, events_vector[baseidx + i], base_ts);
    }

    events_file.close();
}