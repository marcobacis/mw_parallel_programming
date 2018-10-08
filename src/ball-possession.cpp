/**
 * Middleware Technologies course
 * Parallel Programming project
 *
 *
 * Authors: Marco Bacis, Daniele Cattaneo
 */

#include <iostream>
#include <algorithm>
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


void accum_ball_possession(game& g, int K, int i_start, int i_end, int& tot_ball, int& tot_rec, vector<double>& possession)
{
    #pragma omp parallel for reduction(+:tot_ball, tot_rec)
    for (int i=i_start; i<i_end; i++) {
        vector<sensor_record>& step = g.game_records[i];

        //check if interrupted
        bool interrupted = g.is_interrupted_at_time(step[0].ts);

        if(!interrupted) {
            //get balls positions
            vector<sensor_record *> step_balls;
            vector<sensor_record *> step_players;
            for(sensor_record& rec: step) {
                if (g.is_ball_sensor_id(rec.sid) && g.is_ball_inside_field(rec)) {
                    step_balls.push_back(&rec);
                } else if (g.is_player_sensor_id(rec.sid)) {
                    step_players.push_back(&rec);
                }
            }
            
            tot_rec += step_players.size();
            tot_ball += step_balls.size();

            double toAdd = ((double)(sensor_sample_period) / (double)one_second) / (double)(step_balls.size());

            for(sensor_record *ball: step_balls) {
                //get nearest sensor
                double mindist = std::numeric_limits<double>::infinity();
                int nearid = 0;
                for (sensor_record *rec: step_players) {
                    double dist = ball->distance_on_ground_to(*rec);
                    if (dist < mindist) {
                        nearid = rec->sid;
                        mindist = dist;
                    }
                }

                if (mindist != std::numeric_limits<double>::infinity() && mindist < (K * 1000)) {
                    double& this_possession = possession[g.sensorPlayerIdx[nearid]];
                    #pragma omp atomic
                    this_possession += toAdd;
                    //DBOUT << "Player: " << players[sensorPlayerIdx[nearid]].name << " distance: " << mindist << "\n";
                }
            }
        }
    }
}


void print_possession(game& g, vector<double>& possession)
{
    for (unsigned int pl=0; pl<possession.size(); pl++) {
        if (pl == g.referee_idx)
            continue;
        cout << "player " << g.players[pl].name << ", possession = " << possession[pl] << endl;
    }
}


void print_final_stats(const string& basepath, game& g, vector<double>& possession)
{
    double total_possession = 0;
    double total_actual = 0;
    for(unsigned int pl = 0; pl < g.players.size(); pl++) {
        if(pl != g.referee_idx) {
            string filename = g.players[pl].name;
            std::replace(filename.begin(), filename.end(), ' ', '_');
            filename.append(".csv");

            vector<referee_event> poss_events;
            load_referee_csv(
                    basepath + "referee-events/ball_possession/1stHalf/" + filename,
                    poss_events, 0);
            load_referee_csv(
                    basepath + "referee-events/ball_possession/2ndHalf/" + filename,
                    poss_events, 0);

            double player_possession = 0;
            for (unsigned int pe = 0; pe < poss_events.size(); pe += 2) {
                player_possession += poss_events[pe + 1].ts - poss_events[pe].ts;
            }
            player_possession /= one_second;
            total_possession += player_possession;
            total_actual += possession[pl];

            cout << "player " << g.players[pl].name << ", possession = " << possession[pl] << 
                " (referee = " << player_possession << ")" << endl;
        }
    }

    double tot = 0;
    for(unsigned int e = 1; e < g.game_events.size()-2;e += 2) {
        tot += (g.game_events[e+1].ts - g.game_events[e].ts);
    }
    tot /= one_second;

    double interr = 0;
    for(unsigned int e = 2; e < g.game_events.size()-2;e += 2) {
        interr += g.game_events[e+1].ts - g.game_events[e].ts;
    }
    interr /= one_second;

    double tot_time = (second_half_end - first_half_start) / one_second;

    DBOUT << "\n";
    DBOUT << "Effective game time " << tot << "\n";
    DBOUT << "Possession real: " << total_possession << " actual " << total_actual << "\n";
    DBOUT << "Interruptions " << interr << "\n";
    DBOUT << "Total time (included interruptions) " << tot_time << " seconds\n";
}


int main(int argc, char **argv)
{
    if(argc < 3) {
        cerr << "Usage: " << argv[0] << " K T files_base_path" << endl;
        return 1;
    }

    double K = stof(argv[1]);
    double T = stof(argv[2]);

    string basepath(argv[3]);

    if(K < 1 || K > 5 || T < 1 || T > 60) {
        cerr << "K must be between 1 and 5" << endl << "T must be between 1 and 60" << endl;
        return 2;
    }

    game g;
    g.load_from_directory(basepath);
    
    vector<double> possession(g.players.size(), 0);

    int tot_ball = 0;
    int tot_rec = 0;

    int interval_start = 0;
    int interval_end = 0;
    ps_timestamp_t interval_start_time = g.game_records[0][0].ts;
    ps_timestamp_t interval_end_time = interval_start_time;

    int nrecords = g.game_records.size();
    for (int i=1; i<nrecords; i++) {
        /* The data is in g.game_records but we simulate getting it in real time
         * by scanning through it linearly */
        vector<sensor_record>& step = g.game_records[i];

        interval_end = i;
        interval_end_time = step[0].ts;
        if (interval_end_time - interval_start_time > T * one_second) {
            accum_ball_possession(g, K, interval_start, interval_end, tot_ball, tot_rec, possession);
            cout << "possession at time " << interval_end_time / one_second << " s" << endl;
            print_possession(g, possession);
            cout << endl;
            interval_start = interval_end;
            interval_start_time = interval_end_time;
        }
    }
    
    accum_ball_possession(g, K, interval_start, interval_end, tot_ball, tot_rec, possession);

    cout << "final possession stats: " << endl;
    print_final_stats(basepath, g, possession);

    DBOUT << "Total player records " << tot_rec << ", total ball records " << tot_ball << "\n\n";
}