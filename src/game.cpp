#include "game.h"
#include "gameio.h"

using namespace std;


#define GAME_SEP ','
#define PLAYER_SEP ','


/**
 * Parses and return a sensor record from a string
 * @param line      Input sensor record line
 * @return          the parsed sensor record
 */
sensor_record parse_sensor_record(std::string line)
{
    sensor_record result;
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

    return result;
}


/**
 * Parses and return a player record from a string
 * @param line      Player record string
 * @return          the parsed player record
 */
player parse_player(std::string line)
{
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
    for (int i = 0; std::getline(lineStream, cell, PLAYER_SEP); i++) {
        player_read.sensors.push_back(std::stoul(cell));
    }

    return player_read;
}


/**
 * Loads the game sensors csv file into a vector of sensor records
 * @param file_path     Full game file path
 * @param game_vector   Vector to fill
 */
void load_game_csv(string file_path, std::vector<std::vector<sensor_record> > &game_vector)
{
    std::ifstream game_file;

    DBOUT << "Loading game file\n";

    game_file.open(file_path);

    if (!game_file.is_open()) {
        throw std::runtime_error("Game file not found: " + file_path);
    }

    game_file.clear();
    game_file.seekg(0);

    std::vector<sensor_record> game_step;
    std::string line;

    std::getline(game_file, line);
    sensor_record temp = parse_sensor_record(line);
    game_step.push_back(temp);

    while(std::getline(game_file, line)) {
        temp = parse_sensor_record(line);

        if((temp.ts / sensor_sample_period) != (game_step[0].ts / sensor_sample_period)) {
            game_vector.push_back(game_step);
            game_step.clear();
        }

        game_step.push_back(temp);
    }

    game_file.close();

    DBOUT << "Sensor read completed\n";
    DBOUT << game_vector.size() << " steps recorded\n";
}


/**
 * Loads the players from the given file
 * @param file_path The players' file path
 * @param players  The vector in which to store the players
 */
void load_players(string file_path, std::vector<player> &players)
{
    std::ifstream players_file;

    players_file.open(file_path);

    if (!players_file.is_open()) {
        throw std::runtime_error("Players file not found: " + file_path);
    }

    std::string line;

    while(std::getline(players_file, line)) {
        players.push_back(parse_player(line));
    }
}


/**
 * Loads the balls in the sets (one set for each half)
 * @param file_path Balls' file path
 * @param balls the loaded balls
 */
void load_balls(string file_path, std::set<unsigned int>& balls)
{
    std::ifstream balls_file;

    balls_file.open(file_path);

    if(!balls_file.is_open()) {
        throw new std::runtime_error("Balls file not found: " + file_path);
    }

    for(int i = 0; i < 2; i++) {
        std::string line;
        std::getline(balls_file, line);
        std::stringstream lineStream(line);
        std::string ball;
        while(lineStream.rdstate() != std::ios_base::eofbit) {
            std::getline(lineStream, ball, ',');
            balls.insert(std::stoul(ball));
        }
    }
}


void game::load_from_directory(const string& basepath)
{
    game_records.reserve(50000000); //we know approximately the number of records

    load_game_csv(basepath + "full-game.csv", game_records);

    game_events.push_back({2010, referee_event::type::INT_BEGIN, 0, 0});
    game_events.push_back({2011, referee_event::type::INT_END, first_half_start, 0});
    load_referee_csv(basepath + "referee-events/interruptions/1stHalf.csv", game_events, first_half_start);
    //add interruption between games + no ball time
    game_events.push_back({2010, referee_event::type::INT_BEGIN, no_ball_start, 35});
    game_events.push_back({2011, referee_event::type::INT_END, second_half_start, 35});
    load_referee_csv(basepath + "referee-events/interruptions/2ndHalf.csv", game_events, second_half_start);
    //add end game interruption
    game_events.push_back({6014, referee_event::type::INT_BEGIN, second_half_end, 39});
    game_events.push_back({6015, referee_event::type::INT_END, timestamp_max, 39});

    load_players(basepath + "players.csv", players);
    load_balls(basepath + "balls.csv", balls);

    for(unsigned int p = 0; p < players.size(); p++) {
        for(sensor_id_t s : players[p].sensors) {
            if(s != 0)
                sensorPlayerIdx[s] = p;
        }
        if(players[p].role == 'R') referee_idx = p; //we assume just one referee
    }

    //map interruptions, assuming they are in order start-end-start-end...
    for (unsigned int in = 0; in < game_events.size(); in += 2) {
        interruptions.push_back({game_events[in].ts, game_events[in+1].ts});
    }
}



