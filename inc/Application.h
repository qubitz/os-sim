// PROGRAM INFORMATION /////////////////////////////////////////////////////////

/**
* @file Application.h
*
* @brief Application class declaration for simulator project at
*
* @details This file manages and controls an application
*
* @author Tyler Goffinet
*
* @version 1.00 (17 September 16)
*/

// PRECOMPILER DIRECTIVES //////////////////////////////////////////////////////

#ifndef APPLICATION_H
#define APPLICATION_H

// HEADER FILES ////////////////////////////////////////////////////////////////

#include <pthread.h>
#include <sys/time.h>
#include <climits>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>

class Simulator;
#include "Resource.h"
#include "SimDataStructs.h"
#include "Simulator.h"

// CLASS DEFINITION ////////////////////////////////////////////////////////////

/**
 * @brief      Process states
 */
enum ProcessState
{
  NEW,
  READY,
  RUNNING,
  EXIT
};

/**
 * @brief      Process control block
 */
struct ProcessCtrlBlock
{
  unsigned int pid;
  unsigned int pCounter;
  unsigned int pPriority;
  ProcessState pState;
};

/**
 * @brief      Holds and operates on application data.
 */
class Application
{
 public:
  ProcessCtrlBlock      m_PCB;
  std::queue<Operation> m_opQueue;

  // Constructors
  Application( unsigned int appID, bool logToMonitor, Simulator* sim,
               Resource* m_printers, Resource* hdds, Resource* kbd,
               Resource* screens );
  ~Application() {}

  // Accessors
  unsigned int getAppID() { return m_appID; }
  std::string  getLogStr() { return m_logStr; }
  std::string  getErrorStr() { return m_errorStr; }

  // Modifiers
  bool executeInstructions();
  void setStartTime( struct timeval startTime ) { systemStartTime = startTime; }

 private:
  static const int constexpr MS_TIME_FACTOR      = 1000;
  static const float constexpr TIME_COMPENSATION = -0.0008;

  struct timeval systemStartTime;

  // Referenced from parent simulator
  Simulator* m_ourSim;
  Resource*  m_printers;
  Resource*  m_harddrives;
  Resource*  m_keyboards;
  Resource*  m_monitors;
  bool       m_logToMonitor;

  // Class member variables
  unsigned int m_appID;
  std::string  m_logStr;
  std::string  m_errorStr;

  // Threads
  const Operation* m_ThreadOp;

  // Operation handlers
  int handleIO( const Operation& op );
  bool handleAllocate( const Operation& op );
  void sleep( unsigned int time );

  // Output handlers
  void output( std::string output );

  // Thread helpers
  friend void* threadRunner( void* object );
};

#endif  // APPLICATION_H
