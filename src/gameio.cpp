#include "gameio.h"

#define GAME_SEP ','
#define EVENT_SEP ';'
#define PLAYER_SEP ','

sensor_record parseSensorRecord(std::string line) {

    sensor_record result;
    std::stringstream lineStream(line);
    std::string cell;
    //sid
    std::getline(lineStream, cell, GAME_SEP);

    result.sid = std::stoul(cell);

    //ts
    std::getline(lineStream, cell, GAME_SEP);
    result.ts = std::stoull(cell) / SENSOR_FREQ;

    //x
    std::getline(lineStream, cell, GAME_SEP);
    result.x = std::stoi(cell);

    //y
    std::getline(lineStream, cell, GAME_SEP);
    result.y = std::stoi(cell);

    //z
    std::getline(lineStream, cell, GAME_SEP);
    result.z = std::stoi(cell);

    return result;
}


referee_event parseRefereeEvent(std::string line, unsigned long int base_ts) {

    std::stringstream lineStream(line);

    std::string cell;

    referee_event  event;

    //event id
    std::getline(lineStream, cell, EVENT_SEP);
    event.id = std::stoul(cell);

    //event type
    std::getline(lineStream, cell, EVENT_SEP);

    //trim + tolower
    cell.erase(remove_if(cell.begin(), cell.end(), isspace), cell.end());
    std::transform(cell.begin(), cell.end(), cell.begin(), ::tolower);


    if(cell.compare("gameinterruptionbegin") == 0) {
        event.type = INT_BEGIN;
    } else if (cell.compare("gameinterruptionend") == 0) {
        event.type = INT_END;
    } else {
        event.type = OTHER_EVENT;
    }

    //event timestamp (need to parse hh:mm:ss.sss, or just hh = 0 in the first case)
    std::getline(lineStream, cell, EVENT_SEP);
    event.ts = base_ts + parse_datetime(cell);

    //event counter
    std::getline(lineStream, cell, EVENT_SEP);
    event.counter = std::stoul(cell);

    return event;
}

player parsePlayer(std::string line) {

    std::stringstream lineStream(line);
    std::string cell;

    player player_read;

    //player name
    std::getline(lineStream, player_read.name, PLAYER_SEP);

    //! For role and team we assume no spaces in the CSV

    //player role
    std::getline(lineStream, cell, PLAYER_SEP);
    player_read.role = cell[0];

    //player team (empty for referee)
    std::getline(lineStream, cell, PLAYER_SEP);
    if(player_read.role == 'R')
        player_read.team = (char) 0;
    else
        player_read.team = cell[0];

    //player sensors
    for (int i = 0; i < MAX_PLAYER_SENSORS && std::getline(lineStream, cell, PLAYER_SEP); i++) {
        player_read.sensors[i] = std::stoul(cell);
    }

    return player_read;
}

void loadGameCSV(fs::path file_path, std::vector<std::vector<sensor_record> > &game_vector) {

    std::ifstream game_file;

    DBOUT << "Loading game file\n";

    game_file.open(file_path);

    if (!game_file.is_open()) {
        throw std::runtime_error("Game file not found: " + file_path.string());
    }

    game_file.clear();
    game_file.seekg(0);

    std::vector<sensor_record> game_step;
    std::string line;

    std::getline(game_file, line);
    sensor_record temp = parseSensorRecord(line);
    game_step.push_back(temp);

    while(std::getline(game_file, line)) {

        temp = parseSensorRecord(line);

        if(temp.ts != game_step[0].ts) {
            game_vector.push_back(game_step);
            game_step.clear();
        }

        game_step.push_back(temp);
    }

    game_file.close();

    DBOUT << "Sensor read completed\n";
    DBOUT << game_vector.size() << " steps recorded\n";
}


void loadRefereeCSV(fs::path file_path, std::vector<referee_event> &events_vector,
                    unsigned long int base_ts) {
    std::ifstream events_file;

    events_file.open(file_path);

    if (!events_file.is_open()) {
        throw std::runtime_error("Events file not found: " + file_path.string());
    }

    std::string line;

    while(std::getline(events_file, line)) {
        events_vector.push_back(parseRefereeEvent(line, base_ts));
    }

    events_file.close();
}


void loadPlayers(fs::path file_path, std::vector<player> &players) {

    std::ifstream players_file;

    players_file.open(file_path);

    if (!players_file.is_open()) {
        throw std::runtime_error("Players file not found: " + file_path.string());
    }

    std::string line;

    while(std::getline(players_file, line)) {
        players.push_back(parsePlayer(line));
    }
}

void loadBalls(fs::path file_path, std::set<unsigned int> balls[]) {

    std::ifstream balls_file;

    balls_file.open(file_path);

    if(!balls_file.is_open()) {
        throw new std::runtime_error("Balls file not found: " + file_path.string());
    }

    for(int i = 0; i < 2; i++) {
        std::string line;
        std::getline(balls_file, line);
        std::stringstream lineStream(line);
        std::string ball;
        while(lineStream.rdstate() != std::ios_base::eofbit) {
            std::getline(lineStream, ball, ',');
            balls[i].insert(std::stoul(ball));
        }
    }
}
