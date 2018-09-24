#include "gameio.h"

#define GAME_SEP ','
#define EVENT_SEP ';'
#define PLAYER_SEP ','

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
}


void getNextRefereeEvent(std::istream &str, referee_event &event, unsigned long int base_ts) {
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

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
        throw std::runtime_error("Event type not supported!");
    }

    //event timestamp (need to parse hh:mm:ss.sss, or just hh = 0 in the first case)
    std::getline(lineStream, cell, EVENT_SEP);
    event.ts = base_ts + parse_datetime(cell);

    //event counter
    std::getline(lineStream, cell, EVENT_SEP);
    event.counter = std::stoul(cell);

}

player getNextPlayer(std::istream &str) {
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

    player *player_read = new player();

    //player name
    std::getline(lineStream, cell, PLAYER_SEP);
    cell.copy(player_read->name, PLAYER_NAME_MAXLENGTH);

    //! For role and team we assume no spaces in the CSV!! (to not implement trim from scratch)

    //player role
    std::getline(lineStream, cell, PLAYER_SEP);
    player_read->role = cell[0];

    //player team (empty for referee)
    std::getline(lineStream, cell, PLAYER_SEP);
    if(player_read->role == 'R')
        player_read->team = (char) 0;
    else
        player_read->team = cell[0];

    //player sensors
    for (int i = 0; i < MAX_PLAYER_SENSORS && lineStream.rdstate() != std::ios_base::eofbit; i++) {
        std::getline(lineStream, cell, PLAYER_SEP);
        player_read->sensors[i] = std::stoul(cell);
    }

    return *player_read;
}

void loadGameCSV(fs::path file_path, std::vector<sensor_record> &game_vector) {

    std::ifstream game_file;

    //print loading header
    DBOUT << "Loading game file\n";

    //count lines
    game_file.open(file_path);

    if (!game_file.is_open()) {
        throw std::runtime_error("Game file not found: " + file_path.string());
    }

    unsigned long int tot_records = std::count(std::istreambuf_iterator<char>(game_file),
               std::istreambuf_iterator<char>(), '\n');

    DBOUT << tot_records << " total sensor records found.\n";

    //read file and import records

    game_vector.reserve(tot_records);

    game_file.clear();
    game_file.seekg(0);

    for (unsigned long int i = 0; i < tot_records; i++) {
        getNextSensorRecord(game_file, game_vector[i]);
    }

    game_file.close();

    DBOUT << "Sensor read completed\n";
}


void loadRefereeCSV(fs::path file_path, std::vector<referee_event> &events_vector,
                    unsigned long int base_ts, bool append) {
    std::ifstream events_file;

    //print loading header
    DBOUT << "Loading events file\n";

    //count lines
    events_file.open(file_path);

    if (!events_file.is_open()) {
        throw std::runtime_error("Events file not found: " + file_path.string());
    }

    unsigned long int tot_records = std::count(std::istreambuf_iterator<char>(events_file),
                                 std::istreambuf_iterator<char>(), '\n');


    DBOUT << tot_records << " total referee events found.\n";

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


void loadPlayers(fs::path file_path, std::vector<player> &players) {

    //the vector should be small, so we don't count the lines beforehand

    std::ifstream players_file;

    players_file.open(file_path);

    if (!players_file.is_open()) {
        throw std::runtime_error("Players file not found: " + file_path.string());
    }

    while(players_file.rdstate() != std::ios_base::eofbit) {
        players.push_back(getNextPlayer(players_file));
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