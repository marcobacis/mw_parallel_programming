/**
 * Middleware Technologies course
 * Parallel Programming project
 *
 *
 * Authors: Marco Bacis, Daniele Cattaneo
 */

#include <iostream>
#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <limits>
#include <climits>
#include "utils.h"
#include "game.h"
#include "gameio.h"


using namespace std;


int main(int argc, char **argv)
{
    if(argc < 3) {
        cerr << "Usage: " << argv[0] << " K T files_base_path" << endl;
        return 1;
    }

    double K = stof(argv[1]);
    double T = stof(argv[2]);

    fs::path basepath(argv[3]);

    if(K < 1 || K > 5 || T < 1 || T > 60) {
        cerr << "K must be between 1 and 5" << endl << "T must be between 1 and 60" << endl;
        return 2;
    }

    game g;
    g.load_from_directory(basepath);
    
    vector<double> possession(g.players.size(), 0);

    //HERE GOES ALGORITHM
    //bruteforce algorithm

    int tot_ball = 0;
    int tot_rec = 0;

    int real = 0;

    int nrecords = g.game_records.size();
    #pragma omp parallel for reduction(+:tot_ball, tot_rec, real)
    for (int i=0; i<nrecords; i++) {
        vector<sensor_record>& step = g.game_records[i];

        //check if interrupted
        bool interrupted = g.is_interrupted_at_time(step[0].ts);

        if(!interrupted) {
            //get balls positions
            vector<sensor_record> step_balls;
            for(sensor_record rec : step) {
                if(g.balls->find(rec.sid) != g.balls->end() && g.is_ball_inside_field(rec)) {
                    step_balls.push_back(rec);
                }
            }
            
            tot_rec += step.size() - step_balls.size();
            tot_ball += step_balls.size();

            double toAdd = 0.005 / step_balls.size();

            for(sensor_record ball : step_balls) {
                //get nearest sensor
                float mindist = std::numeric_limits<float>::infinity();
                int nearid = 0;
                for (sensor_record rec : step) {
                    if (g.balls->find(rec.sid) == g.balls->end() &&
                        g.sensorPlayerIdx[rec.sid] != g.referee_idx) {
                        float dist = distance(ball, rec);
                        if (dist < mindist) {
                            nearid = rec.sid;
                            mindist = dist;
                        }
                    }
                }

                if (mindist != std::numeric_limits<double>::infinity() && mindist < K * 1000) {
                    real++;
                    double& this_possession = possession[g.sensorPlayerIdx[nearid]];
                    #pragma omp atomic
                    this_possession += toAdd;
                    //DBOUT << "Player: " << players[sensorPlayerIdx[nearid]].name << " distance: " << mindist << "\n";
                }
            }
        }
    }

    double total_possession = 0;
    double total_actual = 0;
    for(unsigned int pl = 0; pl < g.players.size(); pl++) {
        if(pl != g.referee_idx) {
            string filename = g.players[pl].name;
            std::replace(filename.begin(), filename.end(), ' ', '_');
            filename.append(".csv");

            vector<referee_event> poss_events;
            loadRefereeCSV(
                    basepath / fs::path("referee-events/ball_possession/1stHalf/") / fs::path(filename),
                    poss_events, 0);
            loadRefereeCSV(
                    basepath / fs::path("referee-events/ball_possession/2ndHalf/") / fs::path(filename),
                    poss_events, 0);

            double player_possession = 0;
            for (unsigned int pe = 0; pe < poss_events.size(); pe += 2) {
                player_possession += poss_events[pe + 1].ts - poss_events[pe].ts;
            }
            player_possession /= 1000000000000;
            total_possession += player_possession;
            total_actual += possession[pl];

            cout << g.players[pl].name << " expected " << player_possession << ", got " << possession[pl]
                 << endl;
        }
    }

    float tot = 0;
    for(unsigned int e = 1; e < g.game_events.size()-2;e += 2) {
        tot += (g.game_events[e+1].ts - g.game_events[e].ts);
    }
    tot /= 1000000000000;

    float interr = 0;
    for(unsigned int e = 2; e < g.game_events.size()-2;e += 2) {
        interr += g.game_events[e+1].ts - g.game_events[e].ts;
    }
    interr /= 1000000000000;

    float tot_time = (END_SECOND - START_FIRST) / 1000000000000;

    DBOUT << "\n";
    DBOUT << "Effective game time " << tot << "\n";
    DBOUT << "Possession real: " << total_possession << " actual " << total_actual << "\n";
    DBOUT << "Interruptions " << interr << "\n";
    DBOUT << "Total time (included interruptions) " << tot_time << " seconds\n";
    DBOUT << "Total player records " << tot_rec << ", total ball records " << tot_ball << "\n\n";
}
