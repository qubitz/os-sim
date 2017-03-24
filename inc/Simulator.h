// PROGRAM INFORMATION /////////////////////////////////////////////////////////

/**
 * @file Simulator.h
 *
 * @brief Simulator class for simulator CS446 project at
 *					University of Nevada, Reno
 *
 * @details This file manages and controls the simulator
 *
 * @author Tyler Goffinet
 *
 * @version 1.00 (17 September 16)
 */

// PRECOMPILER DIRECTIVES //////////////////////////////////////////////////////

#ifndef SIMULATOR_H
#define SIMULATOR_H

// HEADER FILES ////////////////////////////////////////////////////////////////

#include <sys/time.h>
#include <boost/tokenizer.hpp>
#include <climits>
#include <fstream>
#include <iomanip>
#include <iostream>  // DEBUG
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class Application;
#include "Application.h"
#include "Resource.h"
#include "SimDataStructs.h"

// CLASS DEFINITION ///////////////////////////////////////////////////////////

/**
 * @brief      Prepares and executes operating system simulation.
 */
class Simulator
{
 public:
  // Constructors
  Simulator( const char *configPath );
  ~Simulator() {}

  // Controls
  bool initialize();
  bool run();

  // Accessors
  string       getError() { return m_error; }
  unsigned int getMemoryAddress();

 private:
  const static boost::char_separator<char> CONFIG_TOKEN_DELIMITER;
  const static boost::char_separator<char> META_TOKEN_DELIMITER;

  const static std::string CONFIG_START_SYNTAX;
  const static std::string CONFIG_END_SYNTAX;
  const static std::string META_START_SYNTAX;
  const static std::string META_END_SYNTAX;

  struct timeval m_systemStartTime;

  // Simulator data members
  std::vector<Application *> m_applications;
  std::vector<Configdata>    m_config;
  std::vector<Metadata>      m_metadata;
  std::vector<Operation>     m_operations;

  // Resources
  Resource *m_printers;
  Resource *m_harddrives;
  Resource *m_keyboards;
  Resource *m_monitors;

  std::string m_configFile;   // name of config file
  bool        m_initialized;  // simulator has been initialzed

  std::string m_fileOutput;    // holds all file output
  std::string m_logFilePath;   // file path the specified log file
  bool        m_logToFile;     // should log to file or not?
  bool        m_logToMonitor;  // should log to monitor or not?

  int m_usedBlocks;  // amount of blocks of memory in use

  std::string m_error;  // simulator error string

  // Simulator preparation
  bool loadConfig();
  bool loadMetadata();
  bool parseMetaToken( std::string metaToken, Metadata &data );
  bool contructOperations();
  int getConfigData( ConfigCategory category );
  bool loadOutputSettings();

  // Simulator execution
  bool loadOperations();
  bool runScheduler();

  // Output functions
  bool outputResultsToFile();
  bool outputConfig();
  bool outputMetrics();
  void outputSysOperation( unsigned int pid, std::string output,
                           bool outputTime );

  // Token tools
  bool getTokens( std::string                             fileName,
                  std::vector<std::vector<std::string> > &fileTokens,
                  boost::char_separator<char>             delimiter );

  string getTokenString( const std::vector<std::string> &tokens );
  string getTokenString( const std::vector<std::string> &tokens,
                         unsigned int                    endIndex );
  string getTokenString( const std::vector<std::string> &tokens,
                         unsigned int startIndex, unsigned int endIndex );
};

#endif  // SIMULATOR_H
