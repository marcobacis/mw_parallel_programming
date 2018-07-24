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

    vector<sensor_record> game_records;
    vector<referee_event> game_events;
    vector<player> players;
    set<unsigned int> balls[2];

    loadGameCSV(basepath / fs::path("full-game.csv"), game_records);
    loadRefereeCSV(basepath / fs::path("referee-events/interruptions/1stHalf.csv"), game_events);
    loadPlayers(basepath / fs::path("players.csv"), players);
    loadBalls(basepath / fs::path("balls.csv"), balls);

    cout << "End" << endl;
}