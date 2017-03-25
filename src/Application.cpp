// PROGRAM INFORMATION /////////////////////////////////////////////////////////

/**
 * @file Application.cpp
 *
 * @brief Application class implementation for simulator project
 *
 * @details This file manages and controls an application
 *
 * @author Tyler Goffinet
 *
 * @version 1.00 (17 September 16)
 */

// HEADER FILES ////////////////////////////////////////////////////////////////

#include "Application.h"

// CLASS IMPLEMENTATION ////////////////////////////////////////////////////////

/**
 * @brief      Thread helper for p_thread call
 *
 * @param      object  Pointer to application class instance
 *
 * @return     Returns given pointer to application class instance
 */
void* threadRunner( void* object )
{
  const Operation* op;
  Application*     app;

  app = (Application*)object;
  op  = app->m_ThreadOp;

  // Mutex lock
  if( op->data.descriptor == M_PRINTER )
  {
    app->m_printers->lock( app->m_appID );
  }
  else if( op->data.descriptor == M_HARD_DRIVE )
  {
    app->m_harddrives->lock( app->m_appID );
  }
  else if( op->data.descriptor == M_KEYBOARD )
  {
    app->m_keyboards->lock( app->m_appID );
  }
  else if( op->data.descriptor == M_MONITOR )
  {
    app->m_monitors->lock( app->m_appID );
  }

  app->sleep( op->data.cycles * op->timePerCycle );

  return object;
}

/**
 * @brief      Constructs the application.
 *
 * @param[in]  m_appID         The application identifier
 * @param[in]  m_logToMonitor  Should log to monitor/terminal
 * @param[in]  sim           Parent simulator
 * @param[in]  printers      Printer resources
 * @param[in]  hdds          Hard disk drive resources
 * @param[in]  kbds          m_keyboards resources
 * @param[in]  screens       Monitor resouces
 */
Application::Application( unsigned int m_appID, bool m_logToMonitor, Simulator* sim,
                          Resource* m_printers, Resource* hdds, Resource* kbds,
                          Resource* screens )
    : m_ourSim( sim )
    , m_printers( m_printers )
    , m_harddrives( hdds )
    , m_keyboards( kbds )
    , m_monitors( screens )
    , m_logToMonitor( m_logToMonitor )
    , m_appID( m_appID )
    , m_logStr( "" )
    , m_errorStr( "" )
{
  m_PCB.pCounter = 0;
  m_PCB.pid      = m_appID;
  m_PCB.pState   = ProcessState::NEW;
}

/**
 * @brief      Executes loaded instructions.
 *
 * @details    Handles all operations in m_opQueue class member; front to back
 *
 * @return     True if operation successful; false otherwise.
 */
bool Application::executeInstructions()
{
  int       resourceOwnerIndex;
  Operation op;

  while( !m_opQueue.empty() )
  {
    op = m_opQueue.front();

    switch( op.data.code )
    {
      case M_PROCESS:
        switch( op.data.descriptor )
        {
          case M_RUN:
            output( "processing action: start" );
            sleep( op.data.cycles * op.timePerCycle );
            output( "processing action: end" );
            break;
          default:
            assert( "Unreachable code reached (3), aborting..." );
            break;
        }
        break;

      case M_INPUT:
        switch( op.data.descriptor )
        {
          case M_KEYBOARD:
            output( "keyboard input: start" );
            resourceOwnerIndex = handleIO( op );
            output( "keyboard input: end - KBD " +
                    std::to_string( resourceOwnerIndex ) );
            break;
          case M_HARD_DRIVE:
            output( "hard drive input: start" );
            resourceOwnerIndex = handleIO( op );
            output( "hard drive input: end - HDD " +
                    std::to_string( resourceOwnerIndex ) );
            break;
          default:
            assert( "Unreachable code reached (4), aborting..." );
            break;
        }
        break;

      case M_OUTPUT:
        switch( op.data.descriptor )
        {
          case M_PRINTER:
            output( "printer output: start" );
            resourceOwnerIndex = handleIO( op );
            output( "printer output: end - PRNT " +
                    std::to_string( resourceOwnerIndex ) );
            break;
          case M_HARD_DRIVE:
            output( "hard drive output: start" );
            resourceOwnerIndex = handleIO( op );
            output( "hard drive output: end - HDD " +
                    std::to_string( resourceOwnerIndex ) );
            break;
          case M_MONITOR:
            output( "monitor output: start" );
            resourceOwnerIndex = handleIO( op );
            output( "monitor output: end - MOTR " +
                    std::to_string( resourceOwnerIndex ) );
            break;
          default:
            assert( "Unreachable code reached (5), aborting..." );
            break;
        }
        break;

      case M_MEMORY:
        switch( op.data.descriptor )
        {
          case M_ALLOC:
            output( "allocating memory" );
            handleAllocate( op );
            break;
          case M_CACHE:
            output( "memory caching: start" );
            sleep( op.data.cycles * op.timePerCycle );
            output( "memory caching: end" );
            break;
          default:
            assert( "Unreachable code reached (6), aborting..." );
            break;
        }
        break;

      default:
        assert( "Unreachable code reached (7), aborting..." );
        break;
    }

    if( !m_errorStr.empty() ) return false;

    m_opQueue.pop();
    ++m_PCB.pCounter;
  }

  return true;
}

/**
 * @brief      Handles input/output operations.
 *
 * @param      op    The input/output operation data
 *
 * @return     Returns the number of the requested resource.
 */
int Application::handleIO( const Operation& op )
{
  int       resourceIndex;
  pthread_t thread;

  m_ThreadOp = &op;

  // Resource semaphore checks
  if( op.data.descriptor == M_PRINTER )
  {
    if( m_printers->available() )
    {
      m_printers->reserve();
    }
    else
    {
      m_errorStr = "Out of m_printers";
    }
  }
  else if( op.data.descriptor == M_HARD_DRIVE )
  {
    if( m_harddrives->available() )
    {
      m_harddrives->reserve();
    }
    else
    {
      m_errorStr = "Out of hard drives";
    }
  }
  else if( op.data.descriptor == M_KEYBOARD )
  {
    if( m_keyboards->available() )
    {
      m_keyboards->reserve();
    }
    else
    {
      m_errorStr = "Out of m_keyboards";
    }
  }
  else if( op.data.descriptor == M_MONITOR )
  {
    if( m_monitors->available() )
    {
      m_monitors->reserve();
    }
    else
    {
      m_errorStr = "Out of m_monitors";
    }
  }

  if( m_errorStr.empty() )  // no errors
  {
    pthread_create( &thread, NULL, threadRunner, this );
    pthread_join( thread, NULL );
  }

  // Free resources
  if( op.data.descriptor == M_PRINTER )
  {
    resourceIndex = m_printers->getResourceIndex( m_appID );
    m_printers->free( m_appID );
  }
  else if( op.data.descriptor == M_HARD_DRIVE )
  {
    resourceIndex = m_harddrives->getResourceIndex( m_appID );
    m_harddrives->free( m_appID );
  }
  else if( op.data.descriptor == M_KEYBOARD )
  {
    resourceIndex = m_keyboards->getResourceIndex( m_appID );
    m_keyboards->free( m_appID );
  }
  else if( op.data.descriptor == M_MONITOR )
  {
    resourceIndex = m_monitors->getResourceIndex( m_appID );
    m_monitors->free( m_appID );
  }
  else
  {
    resourceIndex = -1;
  }

  return resourceIndex;
}

/**
 * @brief      Handles memory allocation operations.
 *
 * @param      op    The memory operation data
 *
 * @return     Returns true if operation was sucessful; false otherwise.
 */
bool Application::handleAllocate( const Operation& op )
{
  uint              address;
  string            addressStr;
  std::stringstream sstream;

  address = m_ourSim->getMemoryAddress();

  if( address == UINT_MAX )
  {
    m_errorStr = "Out of memory";
    return false;
  }

  sstream << "0x" << std::setfill( '0' ) << std::setw( sizeof( uint ) * 2 )
          << std::hex << address;

  sleep( op.data.cycles * op.timePerCycle );
  output( "memory allocated at " + sstream.str() );

  return true;
}

/**
 * @brief      Sleeps for the specified amount of milliseconds
 *
 * @details    Loops until desired time is equal to current time
 *
 * @param[in]  msecs  Millisecs to sleep
 */
void Application::sleep( unsigned int msecs )
{
  float wantedTime;

  wantedTime = getTime( systemStartTime ) + ( (float)msecs / MS_TIME_FACTOR ) +
               TIME_COMPENSATION;

  while( getTime( systemStartTime ) < wantedTime )
    ;
}

/**
 * @brief      Ouputs string to terminal and/or file if allowed.
 *
 * @param[in]  output  String to out
 */
void Application::output( std::string output )
{
  std::string outputStr;

  outputStr = getTimeStamp( systemStartTime ) + "Application " +
              std::to_string( m_appID ) + ": " + output + "\n";

  if( m_logToMonitor ) std::cout << outputStr;

  m_logStr += outputStr;
}
