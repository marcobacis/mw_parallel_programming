#include <climits>
#include "game.h"
#include "gameio.h"

using namespace std;


void game::load_from_directory(fs::path basepath)
{
    game_records.reserve(50000000); //we know approximately the number of records

    loadGameCSV(basepath / fs::path("full-game.csv"), game_records);

    game_events.push_back({2010, INT_BEGIN, 0, 0});
    game_events.push_back({2011, INT_END, START_FIRST, 0});
    loadRefereeCSV(basepath / fs::path("referee-events/interruptions/1stHalf.csv"), game_events, START_FIRST);
    //add interruption between games + no ball time
    game_events.push_back({2010, INT_BEGIN, NO_BALL_START, 35});
    game_events.push_back({2011, INT_END, START_SECOND, 35});
    loadRefereeCSV(basepath / fs::path("referee-events/interruptions/2ndHalf.csv"), game_events, START_SECOND);
    //add end game interruption
    game_events.push_back({6014, INT_BEGIN, END_SECOND, 39});
    game_events.push_back({6015, INT_END, ULONG_MAX, 39});

    loadPlayers(basepath / fs::path("players.csv"), players);
    loadBalls(basepath / fs::path("balls.csv"), balls);

    for(unsigned int p = 0; p < players.size(); p++) {
        for(unsigned int s : players[p].sensors) {
            if(s != 0)
                sensorPlayerIdx[s] = p;
        }
        if(players[p].role == 'R') referee_idx = p; //we assume just one referee
    }

    //map interruptions, assuming they are in order start-end-start-end...
    for (unsigned int in = 0; in < game_events.size(); in += 2) {
        interruptions.push_back({game_events[in].ts / SENSOR_FREQ, game_events[in+1].ts / SENSOR_FREQ});
    }
}



