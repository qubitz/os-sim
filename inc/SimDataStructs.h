// PROGRAM INFORMATION /////////////////////////////////////////////////////////

/**
* @file SimDataStructs.h
*
* @brief Simulator data structure and functions for simulator CS446 project at
*          University of Nevada, Reno.
*
* @details This file contains data structures and functions essential for
*            the simulator.
*
* @author Tyler Goffinet
*
* @version 1.00 (17 September 16)
*/

// PRECOMPILER DIRECTIVES /////////////////////////////////////////////////////

#ifndef SIM_DATA_STRUCTS_H
#define SIM_DATA_STRUCTS_H

// HEADER FILES ////////////////////////////////////////////////////////////////

#include <sys/time.h>
#include <ctime>
#include <iostream>
#include <queue>
#include "Resource.h"

using std::string;

// FILE CONTENTS ///////////////////////////////////////////////////////////

/**
 * @brief      Configuration categories.
 *
 * @details    Ex: "Version", "File Path", etc.
 */
enum ConfigCategory
{
  C_VERSION,        // "Version/Phase"
  C_FILE_PATH,      // "File Path"
  C_PROCESSOR,      // "Processor"
  C_MONITOR,        // "Monitor"
  C_QUAN_MONITOR,   // "Quantity monitor"
  C_HARD_DRIVE,     // "Hard drive"
  C_QUAN_HDD,       // "Hard drive quantity"
  C_PRINTER,        // "Printer"
  C_QUAN_PRINTER,   // "Printer quantity"
  C_KEYBOARD,       // "Keyboard"
  C_QUAN_KEYBOARD,  // "Keyboard quantity"
  C_MEMORY,         // "Memory"
  C_SYS_MEMORY,     // "System memory"
  C_BLOCK_MEMORY,   // "Memory block size"
  C_LOG,            // "Log"
  C_LOG_FILE_PATH   // "Log File Path"
};

/**
 * @brief      Metadata code.
 *
 * @details    Ex: 'S' in "S(start)0".
 */
enum MetaCode
{
  M_OPER_SYS,     // 'S'
  M_PROGRAM_APP,  // 'A'
  M_PROCESS,      // 'P'
  M_INPUT,        // 'I'
  M_OUTPUT,       // 'O'
  M_MEMORY        // 'M'
};

/**
 * @brief      Metadata descriptor.
 *
 * @details    Ex: "start" in "S(start)0".
 */
enum MetaDesc
{
  M_START,
  M_RUN,
  M_END,
  M_ALLOC,
  M_PRINTER,
  M_KEYBOARD,
  M_HARD_DRIVE,
  M_MONITOR,
  M_CACHE
};

/**
 * @brief      Structure to hold configuration data parsed from file.
 */
struct Configdata
{
  ConfigCategory category;
  std::string    units;
  std::string    data;

  Configdata() {}
  Configdata( ConfigCategory cat, std::string units, std::string data )
      : category( cat )
      , units( units )
      , data( data )
  {
  }
};

/**
 * @brief      Structure to hold metadata parsed from file.
 */
struct Metadata
{
  MetaCode code;
  MetaDesc descriptor;
  int      cycles;

  Metadata() {}
  Metadata( MetaCode code, MetaDesc desc, int cyc )
      : code( code )
      , descriptor( desc )
      , cycles( cyc )
  {
  }
};

/**
 * @brief      Structure to hold metadata metrics
 *
 * @details    Used to ouptut metadata metrics to specified output.
 */
struct Operation
{
  Metadata     data;
  unsigned int timePerCycle;

  Operation() {}
  Operation( Metadata data, unsigned int time )
      : data( data )
      , timePerCycle( time )
  {
  }
};

/**
 * @brief      Converts config category enum to std::string.
 *
 * @param[in]  cat   The category
 *
 * @return     String equivalent of config category enum.
 */
static std::string configCatToString( ConfigCategory cat )
{
  std::string catStr;

  switch( cat )
  {
    case C_VERSION:
      catStr = "Version/Phase";
      break;
    case C_FILE_PATH:
      catStr = "File Path";
      break;
    case C_PROCESSOR:
      catStr = "Processor";
      break;
    case C_MONITOR:
      catStr = "Monitor";
      break;
    case C_HARD_DRIVE:
      catStr = "Hard drive";
      break;
    case C_PRINTER:
      catStr = "Printer";
      break;
    case C_KEYBOARD:
      catStr = "Keyboard";
      break;
    case C_MEMORY:
      catStr = "Memory";
      break;
    case C_SYS_MEMORY:
      catStr = "System memory";
      break;
    case C_QUAN_PRINTER:
      catStr = "Printer quantity";
      break;
    case C_QUAN_HDD:
      catStr = "Hard drive quantity";
      break;
    case C_QUAN_MONITOR:
      catStr = "Printer quantity";
      break;
    case C_QUAN_KEYBOARD:
      catStr = "Keyboard quantity";
      break;
    case C_BLOCK_MEMORY:
      catStr = "Memory block size";
      break;
    case C_LOG:
      catStr = "Log";
      break;
    case C_LOG_FILE_PATH:
      catStr = "Log File Path";
      break;
  }

  return catStr;
}

/**
 * @brief      Converts metadata code enum to std::string.
 *
 * @param[in]  code   The code
 *
 * @return     String equivalent of metadata code enum.
 */
static std::string metaCodeToString( MetaCode code )
{
  std::string codeStr;

  switch( code )
  {
    case M_OPER_SYS:
      codeStr = "S";
      break;
    case M_PROGRAM_APP:
      codeStr = "A";
      break;
    case M_PROCESS:
      codeStr = "P";
      break;
    case M_INPUT:
      codeStr = "I";
      break;
    case M_OUTPUT:
      codeStr = "O";
      break;
    case M_MEMORY:
      codeStr = "M";
      break;
  }

  return codeStr;
}

/**
 * @brief      Converts metadata description enum to std::string.
 *
 * @param[in]  desc   The description
 *
 * @return     String equivalent of metadata description enum.
 */
static std::string metaDescToString( MetaDesc desc )
{
  std::string descStr;

  switch( desc )
  {
    case M_START:
      descStr = "start";
      break;
    case M_RUN:
      descStr = "run";
      break;
    case M_END:
      descStr = "end";
      break;
    case M_ALLOC:
      descStr = "allocate";
      break;
    case M_PRINTER:
      descStr = "printer";
      break;
    case M_KEYBOARD:
      descStr = "keyboard";
      break;
    case M_HARD_DRIVE:
      descStr = "hard drive";
      break;
    case M_MONITOR:
      descStr = "monitor";
      break;
    case M_CACHE:
      descStr = "cache";
      break;
  }

  return descStr;
}

/**
 * @brief      Converts metadta struct to std::string
 *
 * @param[in]  meta  The metadata
 *
 * @return     String equivalent of metadata struct
 */
static std::string metadataToString( Metadata meta )
{
  return ( metaCodeToString( meta.code ) + "(" +
           metaDescToString( meta.descriptor ) + ")" +
           std::to_string( meta.cycles ) );
}

/**
 * @brief      Gets the time.
 *
 * @param[in]  refTime  Time of reference
 *
 * @return     The current time minus the referenced time
 */
static float getTime( struct timeval refTime )
{
  struct timeval currentTime;
  int            microsec, seconds;
  float          time;

  gettimeofday( &currentTime, NULL );
  seconds  = currentTime.tv_sec - refTime.tv_sec;
  microsec = currentTime.tv_usec - refTime.tv_usec;

  time = (float)seconds + ( (float)microsec / 1000000 );
  // printf("getTime: %f\n", time );

  return time;
}

/**
 * @brief      Gets a time stamp.
 *
 * @param[in]  refTime  Time of reference
 *
 * @return     The time stamp of the current time minus the referenced time.
 */
static std::string getTimeStamp( struct timeval refTime )
{
  return ( std::to_string( getTime( refTime ) ) + " - " );
}

#endif  // SIM_DATA_STRUCTS_H
