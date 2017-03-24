// PROGRAM INFORMATION /////////////////////////////////////////////////////////

/**
* @file Application.h
*
* @brief Application class declaration for simulator CS446 project at
*					University of Nevada, Reno
*
* @details This file manages and controls an application
*
* @author Tyler Goffinet
*
* @version 1.00 (17 September 16)
*/

// PRECOMPILER DIRECTIVES //////////////////////////////////////////////////////

#ifndef RESOURCE_H
#define RESOURCE_H

// HEADER FILES ////////////////////////////////////////////////////////////////

// None

// CLASS DEFINITION ////////////////////////////////////////////////////////////

/**
 * @brief      Maintains and controls resources
 */
class Resource
{
 public:
  // Constructors
  Resource( int quantity );
  ~Resource();

  // Accesssors
  bool available() { return ( m_semaphore > 0 ); }
  int getResourceIndex( int pid );

  // Modifiers
  bool reserve();
  void lock( int pid );
  void free( int pid );

 private:
  int * m_arrResources;
  bool *m_locks;
  int   m_quantity;
  int   m_semaphore;

  void initMemberArrays();
};

#endif  // RESOURCE_H
