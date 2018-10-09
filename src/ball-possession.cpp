/** @file
 * @brief Main application source file, contains the main
 *
 * @author Marco Bacis
 * @author Daniele Cattaneo
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
#include <chrono>
#include "utils.h"
#include "game.h"
#include "referee.h"


using namespace std;

/** Computes and accumulates the ball possession for a record
 * @param g          Game from which to compute the possession
 * @param K          Maximum distance to define possession
 * @param i          Record index to consider
 * @param tot_ball   Total number of ball records found (updated by the method)
 * @param tot_rec    Total number of player sensor records found (updated by the method)
 * @param possession List of all players possessions in the time period (updated by the method)
 */
inline void accum_ball_possession(game& g, int K, int i, int& ball, int& rec, vector<double>& possession)
{
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
        
        rec = step_players.size();
        ball += step_balls.size();

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

/** Prints the current partial possession for each player and team
 * @param g             The game state
 * @param possession    Partial cumulative possessions
 */
void print_possession(game& g, vector<double>& possession)
{
    int teams[2] = {0, 0};
    for (unsigned int pl=0; pl<possession.size(); pl++) {
        if (pl == g.referee_idx)
            continue;
        cout << "player " << g.players[pl].name << ", possession = " << possession[pl] << endl;

        int team = 0;
        if(g.players[pl].team == 'B') team = 1;

        teams[team] += possession[pl];
    }

    cout << "team possession: A = " << teams[0] << " , B = " << teams[1] << endl;
}

/** Prints the final possession and game statistics
 * @param basepath  Base path from which to take the expected possession results
 * @param g         The game state
 * @param possession Final total possessions list
 */
void print_final_stats(const string& basepath, game& g, vector<double>& possession)
{
    int teams_exp[2] = {0, 0};
    int teams_act[2] = {0, 0};

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

            int team = 0;
            if(g.players[pl].team == 'B') team = 1;

            teams_exp[team] += player_possession;
            teams_act[team] += possession[pl];

            cout << "player " << g.players[pl].name << ", possession = " << possession[pl] << 
                " (referee = " << player_possession << ")" << endl;
        }
    }

    cout << "team A, possession " << teams_act[0] << " (referee = " << teams_exp[0] << ")" << endl;
    cout << "team B, possession " << teams_act[1] << " (referee = " << teams_exp[1] << ")" << endl;

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

    auto load_time_start = chrono::steady_clock::now();
    game g;
    g.load_from_directory(basepath);
    auto load_time_end = chrono::steady_clock::now();
    
    ps_timestamp_t zero_offset = g.game_records.front().front().ts;
    long int n_periods = (g.recording_length() / (T * one_second)) + 1;
    vector<vector<double> > possession(n_periods, vector<double>(g.players.size(), 0));

    int tot_ball = 0;
    int tot_rec = 0;

    auto compute_time_start = chrono::steady_clock::now();
    int nrecords = g.game_records.size();
    #pragma omp parallel for reduction(+:tot_ball, tot_rec)
    for (int i=1; i<nrecords; i++) {
        vector<sensor_record>& step = g.game_records[i];
        int bucket_i = (step[0].ts - zero_offset) / (T * one_second);
        int ball=0, rec=0;
        accum_ball_possession(g, K, i, ball, rec, possession[bucket_i]);
        tot_ball += ball;
        tot_rec += rec;
    }
    
    for (int i=1; i<n_periods; i++) {
        #pragma omp parallel for simd
        for (unsigned int j=0; j<g.players.size(); j++) {
            possession[i][j] += possession[i-1][j];
        }
    }
    auto compute_time_end = chrono::steady_clock::now();

    for (int i=0; i<n_periods-1; i++) {
        cout << "possession at time " << ((i + 1) * T) + (zero_offset / one_second) << " s" << endl;
        print_possession(g, possession[i]);
        cout << endl;
    }
    cout << "final possession stats: " << endl;
    print_final_stats(basepath, g, possession.back());

    DBOUT << "Total player records " << tot_rec << ", total ball records " << tot_ball << "\n\n";

    chrono::duration<double> load_time = load_time_end - load_time_start;
    chrono::duration<double> compute_time = compute_time_end - compute_time_start;
    DBOUT << "file read time = " << load_time.count() << " s" << endl;
    DBOUT << "computation time = " << compute_time.count() << " s" << endl;
}
