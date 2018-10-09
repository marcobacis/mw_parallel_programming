/** 
 * @file
 * @brief Utlity functions
 *
 * @author Marco Bacis
 * @author Daniele Cattaneo 
 */

#ifndef MW_PARALLEL_PROGRAMMING_UTILS_H
#define MW_PARALLEL_PROGRAMMING_UTILS_H

#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>
#include <climits>

#define _DEBUG 1

#ifdef _DEBUG
#define DBOUT std::cout // or any other ostream
#else
#define DBOUT 0 && std::cout
#endif

typedef unsigned long int ps_timestamp_t;           //!< Timestamp data type
const ps_timestamp_t one_second = 1000000000000;    //!< One second in picoseconds (1s = 10^12 ps)
const ps_timestamp_t timestamp_max = ULONG_MAX;     //!< Maximum timestamp available


/** Parses a string in the hh:mm:ss.sss format and returns
 * its value in picoseconds from 00:00:00.000.
 * @param field The string representing the datetime to parse
 * @return The datetime timestamp in picoseconds
 */
ps_timestamp_t parse_datetime(std::string field);


#endif //MW_PARALLEL_PROGRAMMING_UTILS_H
