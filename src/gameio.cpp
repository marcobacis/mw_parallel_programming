#include "gameio.h"

#define GAME_SEP ','
#define EVENT_SEP ';'

void getNextSensorRecord(std::istream& str, sensor_record &result) {

    std::string line;
    std::getline(str,line);

    std::stringstream lineStream(line);
    std::string cell;
    //sid
    std::getline(lineStream,cell, GAME_SEP);
    result.sid = std::stoul(cell);

    //ts
    std::getline(lineStream,cell, GAME_SEP);
    result.ts = std::stoull(cell);

    //x
    std::getline(lineStream,cell, GAME_SEP);
    result.x = std::stoi(cell);

    //y
    std::getline(lineStream,cell, GAME_SEP);
    result.y = std::stoi(cell);

    //z
    std::getline(lineStream,cell, GAME_SEP);
    result.z = std::stoi(cell);

}