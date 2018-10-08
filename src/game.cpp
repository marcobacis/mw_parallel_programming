#include "game.h"
#include "gameio.h"

using namespace std;


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



