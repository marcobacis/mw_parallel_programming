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
#include <limits>
#include <climits>

#include "gameio.h"
#include "utils.h"

#define START_FIRST 10753295594424116
#define END_FIRST 12557295594424116

#define START_SECOND 13086639146403495
#define END_SECOND 14879639146403495

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

    vector<vector<sensor_record> > game_records; //! Sensor data for the entire game
    vector<referee_event> game_events;  //! Referee events for the entire game
    vector<player> players;             //! Players data
    set<unsigned int> balls[2];         //! For each of the two halves, set of the used balls (might be useless)
    vector<interruption> interruptions;

    game_records.reserve(50000000); //we know approximately the number of records

    loadGameCSV(basepath / fs::path("full-game.csv"), game_records);

    game_events.push_back({2010, INT_BEGIN, 0, 0});
    game_events.push_back({2011, INT_END, START_FIRST, 0});
    loadRefereeCSV(basepath / fs::path("referee-events/interruptions/1stHalf.csv"), game_events, START_FIRST);
    //add interruption between games
    game_events.push_back({2010, INT_BEGIN, END_FIRST, 35});
    game_events.push_back({2011, INT_END, START_SECOND, 35});
    loadRefereeCSV(basepath / fs::path("referee-events/interruptions/2ndHalf.csv"), game_events, START_SECOND);
    //add end game interruption
    game_events.push_back({6014, INT_BEGIN, END_SECOND, 39});
    game_events.push_back({6015, INT_END, ULONG_MAX, 39});

    loadPlayers(basepath / fs::path("players.csv"), players);
    loadBalls(basepath / fs::path("balls.csv"), balls);

    //link each sensor to a player, in order to have a fast lookup
    std::map<int, int> sensorPlayerIdx;
    vector<float> possession(players.size());

    int referee_idx = 0;
    for(int p = 0; p < players.size(); p++) {
        for(int s : players[p].sensors) {
            if(s != 0)
                sensorPlayerIdx[s] = p;
        }
        if(players[p].role == 'R') referee_idx = p; //we assume just one referee

        possession[p] = 0;
    }

    //map interruptions, assuming they are in order start-end-start-end...
    for(int in = 0; in < game_events.size(); in += 2) {
        interruptions.push_back({game_events[in].ts / SENSOR_FREQ, game_events[in+1].ts / SENSOR_FREQ});
    }

    //HERE GOES ALGORITHM
    //bruteforce algorithm

    int real = 0;
    for(vector<sensor_record> step : game_records) {

        //check if interrupted
        bool interrupted = false;
        for(interruption inter : interruptions) {
            interrupted &= (step[0].ts >= inter.start && step[0].ts <= inter.end);
        }


        if(!interrupted) {
            //get ball position (the first it finds....wrong but a first attempt)
            sensor_record ball;
            for(sensor_record rec : step) {
                if(balls->find(rec.sid) != balls->end()) {
                    ball = rec;
                    break;
                }
            }

            //get nearest sensor
            float mindist = std::numeric_limits<float>::infinity();
            int nearid = 0;
            for(sensor_record rec : step) {
                if (rec.sid != ball.sid && sensorPlayerIdx[rec.sid] != referee_idx) {
                    float dist = distance(ball, rec);
                    if(dist < mindist) {
                        nearid = rec.sid;
                        mindist = dist;
                    }
                }
            }

            if(mindist != std::numeric_limits<float>::infinity() && mindist < K*1000) {
                real++;
                possession[sensorPlayerIdx[nearid]] += 0.005;
                //DBOUT << "Player: " << players[sensorPlayerIdx[nearid]].name << " distance: " << mindist << "\n";
            }
        }
    }

    float total_possession = 0;
    float total_actual = 0;
    for(int pl = 0; pl < players.size(); pl++) {
        if(pl != referee_idx) {
            string name = players[pl].name;
            std::replace(name.begin(), name.end(), ' ', '_');
            name.append(".csv");

            vector<referee_event> poss_events;
            loadRefereeCSV(
                    basepath / fs::path("referee-events/ball_possession/1stHalf/") / fs::path(name),
                    poss_events, 0);
            loadRefereeCSV(
                    basepath / fs::path("referee-events/ball_possession/2ndHalf/") / fs::path(name),
                    poss_events, 0);

            float player_possession = 0;
            for (int pe = 0; pe < poss_events.size(); pe += 2) {
                float tempposs = poss_events[pe + 1].ts - poss_events[pe].ts;
                player_possession += tempposs > 0 ? tempposs : 0;
            }
            player_possession /= 1000000000000;
            total_possession += player_possession;
            total_actual += possession[pl];

            cout << players[pl].name << " real " << player_possession << " actual " << possession[pl]
                 << endl;
        }
    }

    float tot = 0;
    for(int e = 1; e < game_events.size()-2;e += 2) {
        tot += (game_events[e+1].ts - game_events[e].ts);
    }
    tot /= 1000000000000;

    float interr = 0;
    for(int e = 2; e < game_events.size()-2;e += 2) {
        interr += (game_events[e+1].ts - game_events[e].ts);
    }
    interr /= 1000000000000;

    float tot_time = (END_SECOND - START_FIRST) / 1000000000000;

    DBOUT << "\n" <<  "Effective game time " << tot << "\n";
    DBOUT << "Possession real: " << total_possession << " actual " << total_actual << "\n";
    DBOUT << "Interruptions " << interr << "\n";
    DBOUT << "Total time (included interruptions) " << tot_time << " seconds\n";
}