//
// Created by Marco Bacis on 23/07/18.
//

#ifndef MW_PARALLEL_PROGRAMMING_UTILS_H
#define MW_PARALLEL_PROGRAMMING_UTILS_H

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <experimental/filesystem>

#define _DEBUG 1

#ifdef _DEBUG
#define DBOUT std::cout // or any other ostream
#else
#define DBOUT 0 && std::cout
#endif

//! Filesystem namespace shortcut
namespace fs = std::experimental::filesystem;


typedef unsigned long int ps_timestamp_t;
const ps_timestamp_t one_second = 1000000000000;


/**
 * Parses a string in the hh:mm:ss.sss format and returns
 * its value in picoseconds from 00:00:00.000.
 * @param field The string representing the datetime to parse
 * @return The datetime timestamp in picoseconds
 */
ps_timestamp_t parse_datetime(std::string field);


#endif //MW_PARALLEL_PROGRAMMING_UTILS_H
