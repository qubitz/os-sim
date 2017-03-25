// PROGRAM INFORMATION /////////////////////////////////////////////////////////

/**
 * @file Resource.cpp
 *
 * @brief Resource class implementation for simulator project
 *
 * @details This file manages and controls a resource
 *
 * @author Tyler Goffinet
 *
 * @version 1.00 (26 October 16)
 */

// HEADER FILES ////////////////////////////////////////////////////////////////

#include "Resource.h"
#include "iostream"  // DEBUG: Remove

// CLASS IMPLEMENTATION ////////////////////////////////////////////////////////

/**
 * @brief      Contructor for resource instance.
 *
 * @param[in]  quantity  Amount of resource
 */
Resource::Resource( int quantity )
    : m_quantity( quantity )
    , m_semaphore( quantity )
{
  m_arrResources = new int[quantity];
  m_locks        = new bool[quantity];

  initMemberArrays();
}

/**
 * @brief      Destroys resource instance.
 */
Resource::~Resource()
{
  delete[] m_arrResources;
  delete[] m_locks;
}

/**
 * @brief      Gets the resource index.
 *
 * @param[in]  pid   The process id given in lock call
 *
 * @return     The resource index.
 */
int Resource::getResourceIndex( int pid )
{
  for( int i = 0; i < m_quantity; ++i )
  {
    if( m_arrResources[i] == pid )
    {
      return i;
    }
  }

  return -1;
}

/**
 * @brief      Reserves resource.
 *
 * @details    Decrements m_semaphore if a resource is available.
 *
 * @return     Returns true if resource can be reserved, false otherwise.
 */
bool Resource::reserve()
{
  if( available() )
  {
    --m_semaphore;
    return true;
  }

  return false;
}

/**
 * @brief      Assigns and locks resource.
 *
 * @param[in]  pid   The process id
 */
void Resource::lock( int pid )
{
  int index;

  index = getResourceIndex( pid );

  // If m_arrResources not found assign first available; otherwise ignore
  if( index == -1 )
  {
    for( int i = 0; i < m_quantity; ++i )
    {
      if( m_arrResources[i] == -1 )
      {
        m_arrResources[i] = pid;
        m_locks[i]        = true;
      }
    }
  }
}

/**
 * @brief      Frees resource.
 *
 * @details    Unassigns the increments semaphore.
 *
 * @param[in]  pid   The process id given in lock call
 */
void Resource::free( int pid )
{
  int index;

  index = getResourceIndex( pid );

  if( index >= 0 )
  {
    // Unassign/free
    m_locks[index]        = false;
    m_arrResources[index] = -1;

    ++m_semaphore;
  }
}

/**
 * @brief      Initialize resouce and lock arrays.
 *
 * @details    Assign resource array with -1's and locks with false.
 */
void Resource::initMemberArrays()
{
  // Fill m_arrResources with -1's and m_locks with false
  for( int i = 0; i < m_quantity; ++i )
  {
    m_arrResources[i] = -1;
    m_locks[i]        = false;
  }
}
