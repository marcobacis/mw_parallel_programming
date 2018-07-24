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


/**
 * Prints a percentage loading bar
 * @param count Current value
 * @param total Total value (= 100%)
 * @param ret   Whether or not to print a carriage return (\r) at the end
 */
void printLoadPerc(unsigned long int count, unsigned long int total, bool ret);

/**
 * Parses a string in the hh:mm:ss.sss format and returns
 * its value in picoseconds from 00:00:00.000.
 * @param field The string representing the datetime to parse
 * @return The datetime timestamp in picoseconds
 */
unsigned long int parse_datetime(std::string field);

#endif //MW_PARALLEL_PROGRAMMING_UTILS_H
