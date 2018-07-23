/**
 * Middleware Technologies course
 * Parallel Programming project
 *
 *
 * Authors: Marco Bacis, Daniele Cattaneo
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "gameio.h"
#include "utils.h"

using namespace std;

#define TOT_RECORDS 49576080 // number taken by doing wc -l full-game.csv
#define TOT_UNIT 500000


int main(int argc, char **argv) {

    if(argc < 3) {
        cerr << "Usage: " << argv[0] << " K T game_file_path" << endl;
        return 1;
    }

    float K = stof(argv[1]);
    float T = stof(argv[2]);

    if(K < 1 || K > 5 || T < 1 || T > 60) {
        cerr << "K must be between 1 and 5" << endl << "T must be between 1 and 60" << endl;
        return 2;
    }

    ifstream game_file;

    vector<sensor_record> game_records(TOT_RECORDS);

    game_file.open(argv[3]);

    if(!game_file.is_open()) {
        cerr << "Wrong game file path" << endl;
        return 3;
    }

    cout << "Loading game file... this will take a while" << endl;
    for (unsigned long int i = 0; i < TOT_RECORDS; i++) {
        getNextSensorRecord(game_file, game_records[i]);
        if(i % TOT_UNIT == 0) printLoadPerc(i, TOT_RECORDS, true);
    }
    printLoadPerc(TOT_RECORDS, TOT_RECORDS, false);

    cout << endl;

    game_file.close();

    cout << "End" << endl;
}