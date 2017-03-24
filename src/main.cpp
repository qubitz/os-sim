// PROGRAM INFORMATION ////////////////////////////////////////////////////////

/**
 * @file main.cpp
 *
 * @brief main file for simulator CS446 project at University of Nevada, Reno
 *
 * @details This files initializes and runs the simulator
 *
 * @author Tyler Goffinet
 *
 * @version 1.00 (17 September 16)
 */

// PRECOMPILER DIRECTIVES ///////////////////////////////////////////////////

// none

// HEADER FILES /////////////////////////////////////////////////////////////

#include <iostream>
#include "Application.h"
#include "Simulator.h"

using std::printf;

// GLOBAL CONSTANTS ///////////////////////////////////////////////////////////

// none

// IMPLEMENTION //////////////////////////////////////////////////////////////

/**
 * @brief Main function to initialize and run simulator
 */
int main( int argc, const char* argv[] )
{
  if( argc != 2 )
  {
    printf( "Invalid arguments. Please include the config file path.\n" );
    return 1;
  }

  Simulator sim = Simulator( argv[1] );
  printf( "Initializing simulator...\n" );

  try
  {
    if( !sim.initialize() )
    {
      printf( "Simulator failed to initialize: " );
      printf( "%s\n", sim.getError().c_str() );
      return 1;
    }

    printf( "Running simulator...\n\n" );
    if( !sim.run() )
    {
      printf( "Simulator failed to run properly: " );
      printf( "%s\n", sim.getError().c_str() );
      return 1;
    }
  }
  catch( const std::exception e )
  {
    printf( "%s\n", sim.getError().c_str() );
  }

  printf( "Successful execution\n" );
  return 0;
}
