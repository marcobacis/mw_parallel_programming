/**
 * Middleware Technologies course
 * Parallel Programming project
 *
 *
 * Authors: Marco Bacis, Daniele Cattaneo
 */

#include <iostream>
#include <experimental/filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <map>

#include "gameio.h"
#include "utils.h"


using namespace std;

int main(int argc, char **argv) {

    if(argc < 3) {
        cerr << "Usage: " << argv[0] << " K T files_base_path" << endl;
        return 1;
    }

    float K = stof(argv[1]);
    float T = stof(argv[2]);

    fs::path basepath(argv[3]);

    if(K < 1 || K > 5 || T < 1 || T > 60) {
        cerr << "K must be between 1 and 5" << endl << "T must be between 1 and 60" << endl;
        return 2;
    }

    vector<sensor_record> game_records; //! Sensor data for the entire game
    vector<referee_event> game_events;  //! Referee events (TODO choose if one half at a time, or entire game)
    vector<player> players;             //! Players data
    set<unsigned int> balls[2];         //! For each of the two halves, set of the used balls (might be useless)

    loadGameCSV(basepath / fs::path("full-game.csv"), game_records);
    loadRefereeCSV(basepath / fs::path("referee-events/interruptions/1stHalf.csv"), game_events);
    loadPlayers(basepath / fs::path("players.csv"), players);
    loadBalls(basepath / fs::path("balls.csv"), balls);

    //link each sensor to a player, in order to have a fast lookup
    std::map<int, int> sensorPlayerIdx;

    int referee_idx = 0;
    for(int p = 0; p < players.size(); p++) {
        int s = 0;
        while(players[p].sensors[s] != 0) {
            sensorPlayerIdx[s] = p;
            if(players[p].role == 'R') referee_idx = p; //we assume just one referee
        }
    }

    cout << "End" << endl;
}