// PROGRAM INFORMATION /////////////////////////////////////////////////////////

/**
 * @file Simulator.cpp
 *
 * @brief Simulator class implementation for simulator CS446 project
 *           at University of Nevada, Reno
 *
 * @details This file manage and controls the simulator
 *
 * @author Tyler Goffinet
 *
 * @version 1.00 (17 September 16)
 */

// HEADER FILES ////////////////////////////////////////////////////////////////

#include "Simulator.h"

using std::ifstream;
using std::ofstream;
using std::vector;
using std::string;

// CLASS IMPLEMENTATION ////////////////////////////////////////////////////////

const boost::char_separator<char> Simulator::CONFIG_TOKEN_DELIMITER(
    " ,;:()\t\n" );
const boost::char_separator<char> Simulator::META_TOKEN_DELIMITER(
    " .,;:\t\n" );
const string Simulator::CONFIG_START_SYNTAX =
    "Start Simulator Configuration File";
const string Simulator::CONFIG_END_SYNTAX = "End Simulator Configuration File";
const string Simulator::META_START_SYNTAX = "Start Program Meta-Data Code";
const string Simulator::META_END_SYNTAX   = "End Program Meta-Data Code";

/**
 * @brief      Contructor for Simulator class.
 *
 * @param[in]  configPath  The configuration file path.
 */
Simulator::Simulator( const char *configPath )
{
  m_configFile  = configPath;
  m_initialized = false;
  m_usedBlocks  = 0;
}

/**
 * @brief      Initializes simulator.
 *
 * @details     Loads and parses m_config and m_metadata files.
 *
 * @return     *true* if simulator successfully m_initialized, *false*
 *              	otherwise.
 */
bool Simulator::initialize()
{
  if( !loadConfig() || !loadMetadata() )
  {
    return false;
  }
  if( !contructOperations() || !loadOutputSettings() )
  {
    return false;
  }

  m_initialized = true;
  return true;
}

/**
 * @brief      Runs simulator.
 *
 * @details    Processes m_metadata and outputs results.
 *
 * @return     *true* if simulator successfully ran, *false* otherwise.
 */
bool Simulator::run()
{
  if( !m_initialized )
  {
    m_error = "Simulator not m_initialized\n";
    return false;
  }
  if( !loadOperations() || !runScheduler() )
  {
    return false;
  }
  if( !outputResultsToFile() )
  {
    return false;
  }

  return true;
}

/**
 * @brief      Gets the next memory address.
 *
 * @details    Increments by block size
 *
 * @return     The memory address.
 *
 * @note       Retruns UINT_MAX if m_error or out of memory
 */
unsigned int Simulator::getMemoryAddress()
{
  uint address;
  uint totalMemory = UINT_MAX;
  uint blockSize   = UINT_MAX;

  try
  {
    for( Configdata data : m_config )
    {
      if( data.category == C_SYS_MEMORY ) totalMemory = std::stoul( data.data );
      if( data.category == C_BLOCK_MEMORY ) blockSize = std::stoul( data.data );
    }
  }
  catch( const std::exception &e )
  {
    m_error = "Invalid configuration data for System memory/block size";
    return -1;
  }

  address = blockSize * m_usedBlocks;

  if( address > totalMemory )
  {
    return UINT_MAX;
  }
  else
  {
    ++m_usedBlocks;
  }

  return address;
}

/**
 * @brief      Loads configuration file into 'm_config' private variable.
 *
 * @return     *true* if operation successful, *false* otherwise.
 */
bool Simulator::loadConfig()
{
  const string absent        = "N/A";
  const string stdCycleUnit  = "msec";
  const string stdMemoryUnit = "kbytes";

  unsigned int startIndex, endIndex, index, size;
  bool         invalidConfig = false;

  int printerQuantity   = 1;
  int harddriveQuantity = 1;
  int keyboardQuantity  = 1;
  int monitorQuantity   = 1;

  string                  lastToken, lastTwoTokens;
  vector<string>          lineTokens;
  vector<vector<string> > fileTokens;

  if( !getTokens( m_configFile, fileTokens, CONFIG_TOKEN_DELIMITER ) )
  {
    if( m_error.compare( "File not found" ) == 0 )
    {
      m_error = "Config file not found";
      return false;
    }
  }

  // Find start syntax (erase until found)
  for( startIndex = 0; startIndex < fileTokens.size(); startIndex++ )
  {
    if( getTokenString( fileTokens[startIndex] )
            .compare( CONFIG_START_SYNTAX ) == 0 )
    {
      break;  // save startIndex
    }
    else
    {
      fileTokens.erase( fileTokens.begin(), fileTokens.begin() + 1 );
    }
  }

  // Find end syntax (erase until found)
  for( endIndex = fileTokens.size() - 1; endIndex < UINT_MAX; endIndex-- )
  {
    if( getTokenString( fileTokens[endIndex] ).compare( CONFIG_END_SYNTAX ) ==
        0 )
    {
      break;  // save endIndex
    }
    else
    {
      fileTokens.erase( fileTokens.end() - 1, fileTokens.end() );
    }
  }

  // Check for proper m_config start/end syntax
  if( getTokenString( fileTokens[startIndex] ).compare( CONFIG_START_SYNTAX ) !=
          0 ||
      getTokenString( fileTokens[endIndex] ).compare( CONFIG_END_SYNTAX ) != 0 )
  {
    ;
    m_error = "Invalid start/end m_config syntax";
    return false;
  }

  // Remove start/end syntax
  fileTokens.erase( fileTokens.begin(), fileTokens.begin() + 1 );
  fileTokens.erase( fileTokens.end() - 1, fileTokens.end() );

  // Parse tokens
  for( index = 0; index < fileTokens.size(); index++ )
  {
    lineTokens = fileTokens[index];
    size       = lineTokens.size();
    lastToken  = lineTokens[lineTokens.size() - 1];

    switch( size )
    {
      case 2:  // Version/Phase
        if( getTokenString( lineTokens, 1 ).compare( "Version/Phase" ) == 0 )
        {
          m_config.push_back(
              Configdata( ConfigCategory::C_VERSION, absent, lastToken ) );
        }
        else
        {
          invalidConfig = true;
        }
        break;

      case 3:  // File Path - Printer quantity - Keyboard quantity - Monitor
               // quantity
        if( getTokenString( lineTokens, 2 ).compare( "File Path" ) == 0 )
        {
          m_config.push_back(
              Configdata( ConfigCategory::C_FILE_PATH, absent, lastToken ) );
        }
        else if( getTokenString( lineTokens, 2 )
                     .compare( "Printer quantity" ) == 0 )
        {
          m_config.push_back(
              Configdata( ConfigCategory::C_QUAN_PRINTER, absent, lastToken ) );
        }
        else if( getTokenString( lineTokens, 2 )
                     .compare( "Keyboard quantity" ) == 0 )
        {
          m_config.push_back( Configdata( ConfigCategory::C_QUAN_KEYBOARD, absent,
                                        lastToken ) );
        }
        else if( getTokenString( lineTokens, 2 )
                     .compare( "Monitor quantity" ) == 0 )
        {
          m_config.push_back(
              Configdata( ConfigCategory::C_QUAN_MONITOR, absent, lastToken ) );
        }
        else
        {
          invalidConfig = true;
        }
        break;

      case 4:  // System Memory - Hard drive quantity - Log - Log File Path
        if( getTokenString( lineTokens, 2 ).compare( "System memory" ) == 0 )
        {
          m_config.push_back( Configdata( ConfigCategory::C_SYS_MEMORY,
                                        stdMemoryUnit, lastToken ) );
        }
        else if( getTokenString( lineTokens, 3 )
                     .compare( "Hard drive quantity" ) == 0 )
        {
          m_config.push_back(
              Configdata( ConfigCategory::C_QUAN_HDD, absent, lastToken ) );
        }
        else if( getTokenString( lineTokens, 3 ).compare( "Log File Path" ) ==
                 0 )
        {
          m_config.push_back( Configdata( ConfigCategory::C_LOG_FILE_PATH, absent,
                                        lastToken ) );
        }
        else if( getTokenString( lineTokens, 1 ).compare( "Log" ) == 0 )
        {
          m_config.push_back(
              Configdata( ConfigCategory::C_LOG, absent,
                          getTokenString( lineTokens, size - 3, size ) ) );
        }
        else
        {
          invalidConfig = true;
        }
        break;

      case 5:  // Processor - Monitor - Hard Drive - Keyboard - Memory
        if( getTokenString( lineTokens, 3 ).compare( "Processor cycle time" ) ==
            0 )
        {
          m_config.push_back( Configdata( ConfigCategory::C_PROCESSOR,
                                        stdCycleUnit, lastToken ) );
        }
        else if( getTokenString( lineTokens, 3 )
                     .compare( "Monitor display time" ) == 0 )
        {
          m_config.push_back( Configdata( ConfigCategory::C_MONITOR, stdCycleUnit,
                                        lastToken ) );
        }
        else if( getTokenString( lineTokens, 3 )
                     .compare( "Printer cycle time" ) == 0 )
        {
          m_config.push_back( Configdata( ConfigCategory::C_PRINTER, stdCycleUnit,
                                        lastToken ) );
        }
        else if( getTokenString( lineTokens, 3 )
                     .compare( "Keyboard cycle time" ) == 0 )
        {
          m_config.push_back( Configdata( ConfigCategory::C_KEYBOARD,
                                        stdCycleUnit, lastToken ) );
        }
        else if( getTokenString( lineTokens, 3 )
                     .compare( "Memory cycle time" ) == 0 )
        {
          m_config.push_back(
              Configdata( ConfigCategory::C_MEMORY, stdCycleUnit, lastToken ) );
        }
        else if( getTokenString( lineTokens, 3 )
                     .compare( "Memory block size" ) == 0 )
        {
          m_config.push_back( Configdata( ConfigCategory::C_BLOCK_MEMORY,
                                        stdMemoryUnit, lastToken ) );
        }
        else
        {
          invalidConfig = true;
        }
        break;

      case 6:  // Printer
        if( getTokenString( lineTokens, 4 )
                .compare( "Hard drive cycle time" ) == 0 )
        {
          m_config.push_back( Configdata( ConfigCategory::C_HARD_DRIVE,
                                        stdCycleUnit, lastToken ) );
        }
        else
        {
          invalidConfig = true;
        }
        break;

      default:
        invalidConfig = true;
    }

    if( invalidConfig )
    {
      m_error = "Unknown configuration \"" + getTokenString( lineTokens ) +
              "\" -- line " + std::to_string( index + 2 );
      break;
    }
  }

  if( invalidConfig )
  {
    return false;
  }
  if( index != fileTokens.size() )
  {
    m_error = "Failed to parse file";
    return false;
  }

  // Load settings for resources
  for( Configdata data : m_config )
  {
    if( data.category == C_QUAN_PRINTER )
    {
      try
      {
        printerQuantity = std::stoi( data.data );
      }
      catch( std::exception e )
      {
        m_error = "Failed to parse printer quantity: \"" + data.data + "\"";
        return false;
      }
    }
    else if( data.category == C_QUAN_HDD )
    {
      try
      {
        harddriveQuantity = std::stoi( data.data );
      }
      catch( std::exception e )
      {
        m_error = "Failed to parse hard drive quantity: \"" + data.data + "\"";
        return false;
      }
    }
    else if( data.category == C_QUAN_KEYBOARD )
    {
      try
      {
        keyboardQuantity = std::stoi( data.data );
      }
      catch( std::exception e )
      {
        m_error = "Failed to parse keyboard quantity: \"" + data.data + "\"";
        return false;
      }
    }
    else if( data.category == C_QUAN_MONITOR )
    {
      try
      {
        monitorQuantity = std::stoi( data.data );
      }
      catch( std::exception e )
      {
        m_error = "Failed to parse monitor quantity: \"" + data.data + "\"";
        return false;
      }
    }
  }

  // Create resources
  m_printers   = new Resource( printerQuantity );
  m_harddrives = new Resource( harddriveQuantity );
  m_keyboards  = new Resource( keyboardQuantity );
  m_monitors   = new Resource( monitorQuantity );

  // printf("Printer: %i, HDD: %i\n", printerQuantity, harddriveQuantity );

  // for( Configdata &test : m_config )  // DEBUG
  // {
  //   std::printf( "Category: %i\n", test.category );
  //   std::printf( "Units: %s\n", test.units.c_str() );
  //   std::printf( "Data: %s\n", test.data.c_str() );
  //   std::printf( "-------------------------------\n" );
  // }

  return true;
}

/**
 * @brief     Loads m_metadata file into 'm_metadata' private variable.
 *
 * @return    *true* if operation successful, *false* otherwise.
 */
bool Simulator::loadMetadata()
{
  unsigned int            fileIndex, lineIndex, startIndex, endIndex;
  string                  metadataFile, metaToken;
  Metadata                data;
  vector<vector<string> > fileTokens;

  // Find m_metadata file in m_config
  for( Configdata setting : m_config )
  {
    if( setting.category == ConfigCategory::C_FILE_PATH )
    {
      metadataFile = setting.data;
      break;  // if found, exit
    }
  }

  if( metadataFile.empty() )
  {
    m_error = "No file path specified in configuration file";
    return false;
  }

  if( !getTokens( metadataFile, fileTokens, META_TOKEN_DELIMITER ) )
  {
    if( m_error.compare( "File not found" ) == 0 )
    {
      m_error = "Metadata file not found";
      return false;
    }
  }

  // Find start syntax (erase until found)
  for( startIndex = 0; startIndex < fileTokens.size(); startIndex++ )
  {
    if( getTokenString( fileTokens[startIndex] ).compare( META_START_SYNTAX ) ==
        0 )
    {
      break;  // save startIndex
    }
    else
    {
      fileTokens.erase( fileTokens.begin(), fileTokens.begin() + 1 );
    }
  }

  // Find end syntax (erase until found)
  for( endIndex = fileTokens.size() - 1; endIndex < UINT_MAX; endIndex-- )
  {
    if( getTokenString( fileTokens[endIndex] ).compare( META_END_SYNTAX ) == 0 )
    {
      break;  // save endIndex
    }
    else
    {
      fileTokens.erase( fileTokens.end() - 1, fileTokens.end() );
    }
  }

  try
  {
    // Check for proper m_config start/end syntax
    if( getTokenString( fileTokens[startIndex] ).compare( META_START_SYNTAX ) !=
            0 ||
        getTokenString( fileTokens[endIndex] ).compare( META_END_SYNTAX ) != 0 )
    {
      m_error = "Invalid start/end m_metadata syntax";
      return false;
    }
  }
  catch( const std::invalid_argument ia )
  {
    m_error = "Invalid start/end m_metadata syntax";
    throw ia;
  }

  // Remove start/end syntax
  fileTokens.erase( fileTokens.begin(), fileTokens.begin() + 1 );
  fileTokens.erase( fileTokens.end() - 1, fileTokens.end() );

  // Load m_metadata
  for( fileIndex = 0; fileIndex < fileTokens.size(); fileIndex++ )
  {
    for( lineIndex = 0; lineIndex < fileTokens[fileIndex].size(); lineIndex++ )
    {
      metaToken = fileTokens[fileIndex][lineIndex];

      // Case where descriptor has ' ' (space)
      if( ( lineIndex + 1 ) < fileTokens[fileIndex].size() )
      {
        // If next token does not contain '('
        if( fileTokens[fileIndex][lineIndex + 1].find( "(" ) == string::npos )
        {
          metaToken += fileTokens[fileIndex][++lineIndex];  // combine tokens
        }
      }

      if( !parseMetaToken( metaToken, data ) )
      {
        m_error += ( " at line " + std::to_string( fileIndex + 2 ) );
        return false;
      }

      m_metadata.push_back( data );
    }
  }

  // for( Metadata &test : m_metadata )  // DEBUG
  // {
  // 	std::printf( "Code:  %i\n", test.code );
  // 	std::printf( "Desc:  %i\n", test.descriptor );
  // 	std::printf( "Cycle: %i\n", test.cycles );
  // 	std::printf( "-------------------------------\n" );
  // }

  return true;
}

/**
 * @brief      Parse a m_metadata token.
 *
 * @param[in]  metaToken  The m_metadata token
 * @param[out] data       The Metadata struct
 *
 * @return     *true* if operation successful, *false* otherwise
 */
bool Simulator::parseMetaToken( string metaToken, Metadata &data )
{
  char         tokenCode;
  unsigned int descIndex;
  unsigned int paraOpenIndex, paraClosedIndex;
  string       tokenDesc, tokenCycles;

  // Get m_metadata code from token
  tokenCode = metaToken.front();
  tokenCode = toupper( tokenCode );

  switch( tokenCode )
  {
    case 'A':
      data.code = MetaCode::M_PROGRAM_APP;
      break;
    case 'I':
      data.code = MetaCode::M_INPUT;
      break;
    case 'M':
      data.code = MetaCode::M_MEMORY;
      break;
    case 'O':
      data.code = MetaCode::M_OUTPUT;
      break;
    case 'P':
      data.code = MetaCode::M_PROCESS;
      break;
    case 'S':
      data.code = MetaCode::M_OPER_SYS;
      break;
    default:
      m_error = "Unknown m_metadata code in entry \"" + metaToken + "\"";
      return false;
      break;
  }

  // Get m_metadata descriptor from token
  paraOpenIndex   = metaToken.find_first_of( "(" );
  paraClosedIndex = metaToken.find_last_of( ")" );
  tokenDesc       = metaToken.substr( paraOpenIndex + 1,
                                paraClosedIndex - ( paraOpenIndex + 1 ) );

  // Convert to all caps
  for( descIndex = 0; descIndex < tokenDesc.size(); descIndex++ )
  {
    tokenDesc[descIndex] = toupper( tokenDesc[descIndex] );
  }

  if( tokenDesc.compare( "START" ) == 0 )
  {
    data.descriptor = MetaDesc::M_START;
  }
  else if( tokenDesc.compare( "RUN" ) == 0 )
  {
    data.descriptor = MetaDesc::M_RUN;
  }
  else if( tokenDesc.compare( "END" ) == 0 )
  {
    data.descriptor = MetaDesc::M_END;
  }
  else if( tokenDesc.compare( "ALLOCATE" ) == 0 )
  {
    data.descriptor = MetaDesc::M_ALLOC;
  }
  else if( tokenDesc.compare( "PRINTER" ) == 0 )
  {
    data.descriptor = MetaDesc::M_PRINTER;
  }
  else if( tokenDesc.compare( "KEYBOARD" ) == 0 )
  {
    data.descriptor = MetaDesc::M_KEYBOARD;
  }
  else if( tokenDesc.compare( "HARDDRIVE" ) == 0 )
  {
    data.descriptor = MetaDesc::M_HARD_DRIVE;
  }
  else if( tokenDesc.compare( "MONITOR" ) == 0 )
  {
    data.descriptor = MetaDesc::M_MONITOR;
  }
  else if( tokenDesc.compare( "CACHE" ) == 0 )
  {
    data.descriptor = MetaDesc::M_CACHE;
  }
  else
  {
    m_error = "Unknown m_metadata descriptor in entry \"" + metaToken + "\"";
    return false;
  }

  // Get m_metadata cycles from token
  tokenCycles = metaToken.substr( paraClosedIndex + 1,
                                  metaToken.size() - ( paraClosedIndex + 1 ) );

  try
  {
    data.cycles = stoi( tokenCycles );  // attempt to convert from string to int
  }
  catch( const std::out_of_range &oor )
  {
    m_error = "Invalid m_metadata cycle number in entry \"" + metaToken + "\"";
    return false;
  }
  catch( const std::invalid_argument ia )
  {
    m_error = "Invalid m_metadata cycle number in entry \"" + metaToken + "\"";
    return false;
  }

  return true;
}

/**
 * @brief      Contructs the m_metadata metrics.
 *
 * @return     *true* if operation successful, *false* otherwise.
 */
bool Simulator::contructOperations()
{
  unsigned int timePerCycle;

  for( Metadata data : m_metadata )
  {
    switch( data.code )
    {
      case M_OPER_SYS:
        switch( data.descriptor )
        {
          case M_START:
          case M_END:
            timePerCycle = 0;
            break;
          default:
            timePerCycle = 0;
            m_error        = "Metadata descriptor \"" +
                    metaDescToString( data.descriptor ) +
                    "\" is not valid for m_metadata code \"" +
                    metaCodeToString( data.code ) + "\"";
            break;
        }
        break;

      case M_PROGRAM_APP:
        switch( data.descriptor )
        {
          case M_START:
          case M_END:
            timePerCycle = 0;
            break;
          default:
            timePerCycle = 0;
            m_error        = "Metadata descriptor \"" +
                    metaDescToString( data.descriptor ) +
                    "\" is not valid for m_metadata code \"" +
                    metaCodeToString( data.code ) + "\"";
            break;
        }
        break;

      case M_PROCESS:
        switch( data.descriptor )  // get input cycle times
        {
          case M_RUN:
            timePerCycle = getConfigData( C_PROCESSOR );
            break;
          default:
            timePerCycle = 0;
            m_error        = "Metadata descriptor \"" +
                    metaDescToString( data.descriptor ) +
                    "\" is not valid for m_metadata code \"" +
                    metaCodeToString( data.code ) + "\"";
            break;
        }
        break;

      case M_INPUT:
        switch( data.descriptor )  // get input cycle times
        {
          case M_KEYBOARD:
            timePerCycle = getConfigData( C_KEYBOARD );
            break;
          case M_HARD_DRIVE:
            timePerCycle = getConfigData( C_HARD_DRIVE );
            break;
          default:
            timePerCycle = 0;
            printf( "%i\n", data.cycles );
            m_error = "Metadata descriptor \"" +
                    metaDescToString( data.descriptor ) +
                    "\" is not valid for metadate code \"" +
                    metaCodeToString( data.code ) + "\"";
            break;
        }
        break;

      case M_OUTPUT:
        switch( data.descriptor )
        {
          case M_PRINTER:
            timePerCycle = getConfigData( C_PRINTER );
            break;
          case M_HARD_DRIVE:
            timePerCycle = getConfigData( C_HARD_DRIVE );
            break;
          case M_MONITOR:
            timePerCycle = getConfigData( C_KEYBOARD );
            break;
          default:
            timePerCycle = 0;
            m_error        = "Metadata descriptor \"" +
                    metaDescToString( data.descriptor ) +
                    "\" is not valid for m_metadata code \"" +
                    metaCodeToString( data.code ) + "\"";
            break;
        }
        break;

      case M_MEMORY:
        switch( data.descriptor )
        {
          case M_ALLOC:
          case M_CACHE:
            timePerCycle = getConfigData( C_MEMORY );
            break;
          default:
            timePerCycle = 0;
            m_error        = "Metadata descriptor \"" +
                    metaDescToString( data.descriptor ) +
                    "\" is not valid for m_metadata code \"" +
                    metaCodeToString( data.code ) + "\"";
            break;
        }
        break;
    }

    if( !m_error.empty() )
    {
      return false;
    }

    m_operations.push_back( Operation( data, timePerCycle ) );
    timePerCycle = 0;  // reset
  }

  return true;
}

/**
 * @brief      Gets the configuration data of specified category.
 *
 * @param[in]  category  The configuration category
 *
 * @return     The configuration data. -1 if category not found
 * 						   or invalid m_config data.
 */
int Simulator::getConfigData( ConfigCategory category )
{
  size_t     configIndex;
  Configdata configData;

  for( configIndex = 0; configIndex < m_config.size(); configIndex++ )
  {
    configData = m_config[configIndex];

    if( configData.category == category )
    {
      if( configData.units.compare( "msec" ) == 0 ||
          configData.units.compare( "kbytes" ) == 0 )
      {
        return std::stoi( configData.data );
      }
      else
        break;  // save configData
    }
  }

  m_error = "Invalid units \"" + configData.units + "\"" +
          "not recognized for \"" + configCatToString( configData.category ) +
          "\"";
  return -1;  // category not found OR not valid data
}

/**
 * @brief      Retrieves and stores simulator output configurations.
 *
 * @return     *true* if operation successful, *false* otherwise.
 */
bool Simulator::loadOutputSettings()
{
  string logInstructions;

  m_logToFile = m_logToMonitor = false;

  for( Configdata tempConfig : m_config )
  {
    if( tempConfig.category == C_LOG )
    {
      logInstructions = tempConfig.data;
    }
    else if( tempConfig.category == C_LOG_FILE_PATH )
    {
      m_logFilePath = tempConfig.data;
    }
  }

  if( logInstructions.compare( "Log to Both" ) == 0 )
  {
    m_logToFile    = true;
    m_logToMonitor = true;
  }
  else if( logInstructions.compare( "Log to File" ) == 0 )
  {
    m_logToFile = true;
  }
  else if( logInstructions.compare( "Log to Monitor" ) == 0 )
  {
    m_logToMonitor = true;
  }
  else
  {
    m_error = "Unknown logging instructions \"" + logInstructions + "\"";
    return false;
  }

  return true;
}

/**
 * @brief      Outputs results to file if specified.
 *
 * @return     *true* if operation successful, *false* otherwise.
 */
bool Simulator::outputResultsToFile()
{
  ofstream fout;

  if( m_logToFile && !m_logFilePath.empty() )
  {
    fout.open( m_logFilePath.c_str() );

    if( !fout.good() )
    {
      m_error = "Output file not found";
      return false;
    }

    fout << m_fileOutput;
  }

  if( m_logToFile )
  {
    fout.close();
  }

  return true;
}

/**
 * @brief      Outputs simulator configuration results.
 *
 * @return     *true* if operation successful, *false* otherwise.
 */
bool Simulator::outputConfig()
{
  string configOutput;

  configOutput += "Configuration File Data\n";
  for( Configdata tempConfig : m_config )
  {
    if( tempConfig.units.compare( "msec" ) == 0 )
    {
      // Construct: "<category> = <ms per cycle> ms/cycle"
      configOutput += configCatToString( tempConfig.category );
      configOutput += " = " + tempConfig.data + " ms/cycle\n";
    }
    else if( tempConfig.category == C_QUAN_PRINTER ||
             tempConfig.category == C_QUAN_HDD ||
             tempConfig.category == C_QUAN_MONITOR ||
             tempConfig.category == C_QUAN_KEYBOARD )
    {
      configOutput += configCatToString( tempConfig.category );
      configOutput += " = " + tempConfig.data + "\n";
    }
    else if( tempConfig.category == C_LOG_FILE_PATH )
    {
      m_logFilePath = tempConfig.data;
    }
  }

  configOutput += "Logged to: ";
  if( m_logToFile && m_logToMonitor )
  {
    configOutput += "monitor and " + m_logFilePath + "\n";
  }
  else if( m_logToMonitor )
  {
    configOutput += "monitor\n";
  }
  else if( m_logToFile )
  {
    configOutput += m_logFilePath + "\n";
  }
  configOutput += "\n";

  if( m_logToMonitor )
  {
    std::cout << configOutput;
  }

  m_fileOutput += configOutput;

  return true;
}

/**
 * @brief      Outputs simulator metrics results.
 *
 * @return     *true* if operation successful, *false* otherwise.
 */
bool Simulator::outputMetrics()
{
  string metricOutput;

  metricOutput += "Meta-Data Metrics\n";
  for( Operation tempMetric : m_operations )
  {
    if( tempMetric.data.cycles != 0 )
    {
      // Construct: "<code>(<desc>)<cycle> - <time>"
      metricOutput += metadataToString( tempMetric.data );
      metricOutput += " - ";
      metricOutput +=
          std::to_string( tempMetric.data.cycles * tempMetric.timePerCycle ) +
          " ms";
      metricOutput += "\n";
    }
  }
  metricOutput += "\n";

  if( m_logToMonitor )
  {
    std::cout << metricOutput;
  }

  m_fileOutput += metricOutput;

  return true;
}

/**
 * @brief      Runs m_metadata
 *
 * @return     *true* if operation successful, *false* otherwise.
 */
bool Simulator::loadOperations()
{
  unsigned int opIndex, appIndex = 0;
  Operation    op;
  Application *app;

  outputConfig();
  outputMetrics();

  // Verify first operation: S(start)0
  if( !( m_operations.front().data.code == M_OPER_SYS &&
         m_operations.front().data.descriptor == M_START ) )
  {
    m_error = "Operating system not started in m_metadata";
    return false;
  }

  // Cycle through all m_operations; skipping first op; stopping before last op
  for( opIndex = 1; opIndex < m_operations.size() - 1; opIndex++ )
  {
    op = m_operations[opIndex];

    switch( op.data.code )
    {
      case M_OPER_SYS:
        switch( op.data.descriptor )
        {
          case M_START:
            m_error =
                "Operating system can not be started again in m_metadata. "
                "Aborting...";
            break;
          case M_END:
            m_error =
                "Operating system can not be ended until end of m_metadata. "
                "Aborting...";
            break;
          default:
            assert( "Unreachable code reached (1), aborting..." );
            break;
        }
        break;

      case M_PROGRAM_APP:
        switch( op.data.descriptor )
        {
          case M_START:
            app = new Application( ++appIndex, m_logToMonitor, this, m_printers,
                                   m_harddrives, m_keyboards, m_monitors );
            m_applications.push_back( app );
            outputSysOperation( appIndex, "loading application", false );
            break;
          case M_END:
            app = NULL;
            break;
          default:
            assert( "Unreachable code reached (2), aborting..." );
            break;
        }
        break;

      case M_PROCESS:
      case M_INPUT:
      case M_OUTPUT:
      case M_MEMORY:
        if( app == NULL )
        {
          m_error =
              "No application to assign operation "
              "\"" +
              metadataToString( op.data ) + "\"";
        }
        else
        {
          app->m_opQueue.push( op );
        }
        break;
    }

    if( !m_error.empty() )
    {
      return false;
    }
  }

  // Verify last operation: S(end)0
  if( !( m_operations.back().data.code == M_OPER_SYS &&
         m_operations.back().data.descriptor == M_END ) )
  {
    m_error = "Operating system not ended in m_metadata";
    return false;
  }

  return true;
}

/**
 * @brief      Runs the application scheduler.
 *
 * @return     *true* if operation successful, *false* otherwise.
 */
bool Simulator::runScheduler()
{
  string schOutput;

  gettimeofday( &m_systemStartTime, NULL );

  for( size_t i = 0; i < m_applications.size(); i++ )
  {
    m_applications[i]->setStartTime( m_systemStartTime );
  }

  schOutput = getTimeStamp( m_systemStartTime ) + "Simulator program starting\n";
  if( m_logToMonitor ) std::cout << schOutput;
  m_fileOutput += schOutput;

  for( Application *app : m_applications )
  {
    app->m_PCB.pState = ProcessState::READY;
    outputSysOperation( app->getAppID(), "starting application", true );

    app->m_PCB.pState = ProcessState::RUNNING;
    if( !app->executeInstructions() )
    {
      if( !m_error.empty() ) return false;

      m_error = "Application " + std::to_string( app->getAppID() ) +
              " failed to execute instructions " + "\"" + app->getErrorStr() +
              "\"";
      return false;
    }

    outputSysOperation( app->getAppID(), "terminating application", true );
    app->m_PCB.pState = ProcessState::EXIT;
    m_usedBlocks      = 0;  // free memeory

    m_fileOutput += app->getLogStr();
  }

  return true;
}

/**
 * @brief      Log system operation output to the file/monirotr as specified
 *
 * @param[in]  pid         Process id
 * @param[in]  output      String to output
 * @param[in]  outputTime  Whether to output time stamp
 */
void Simulator::outputSysOperation( uint pid, string output, bool outputTime )
{
  string sysOutput;

  if( outputTime ) sysOutput += getTimeStamp( m_systemStartTime );

  sysOutput += "OS: " + output + " " + std::to_string( pid ) + "\n";

  if( m_logToMonitor ) std::cout << sysOutput;

  m_fileOutput += sysOutput;
}

// Token tools ---------------------------------------------------

/**
 * @brief      Gets tokens from the given file
 *
 * @param[in]  fileName    The file name
 * @param[in]  fileTokens  The 'vector' of line tokens
 * @param[in]  delimiter   The token delimiter
 *
 * @return     *true* if operation successful, *false* otherwise
 */
bool Simulator::getTokens( string fileName, vector<vector<string> > &fileTokens,
                           boost::char_separator<char> delimiter )
{
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

  ifstream            fin;
  string              buffer;
  tokenizer *         tok;
  tokenizer::iterator it;
  vector<string>      lineTokens;
  char                temp[500];

  fin.open( fileName.c_str() );
  if( !fin.good() )
  {
    m_error = "File not found";
    return false;
  }

  while( !fin.eof() )
  {
    lineTokens = vector<string>();  // reinitialize
    fin.getline( temp, 500 );
    buffer = temp;

    // Tokenize line
    tok = new tokenizer( buffer, delimiter );
    for( it = tok->begin(); it != tok->end(); it++ )
    {
      lineTokens.push_back( *it );
    }

    fileTokens.push_back( lineTokens );
  }

  // for( vector<string> &t : fileTokens )              // DEBUG
  // {
  // 	std::cout << "- ";

  // 	for( string &t1 : t )
  // 	{
  // 		std::cout << t1 << " ";
  // 	}
  // 	std::cout << std::endl;
  // }

  return true;
}

/**
 * @brief      Gets the token string.
 *
 * @param[in]  tokens  The tokens
 *
 * @return     The string form of the selected tokens.
 */
string Simulator::getTokenString( const std::vector<std::string> &tokens )
{
  return getTokenString( tokens, 0, tokens.size() );
}

/**
 * @brief      Gets the token string.
 *
 * @param[in]  tokens    The tokens
 * @param[in]  endIndex  The end index
 *
 * @return     The string form of the selected tokens.
 */
string Simulator::getTokenString( const std::vector<std::string> &tokens,
                                  unsigned int                    endIndex )
{
  return getTokenString( tokens, 0, endIndex );
}

/**
 * @brief      Gets the token string.
 *
 * @param[in]  tokens      The tokens
 * @param[in]  startIndex  The start index
 * @param[in]  endIndex    The end index
 *
 * @return     The string form of the selected tokens.
 */
string Simulator::getTokenString( const std::vector<std::string> &tokens,
                                  unsigned int                    startIndex,
                                  unsigned int                    endIndex )
{
  unsigned int index;
  string       result;

  if( startIndex > tokens.size() )
  {
    throw std::invalid_argument( "Invalid start index" );
  }
  if( endIndex < startIndex || endIndex > tokens.size() )
  {
    throw std::invalid_argument( "Invalid end index" );
  }

  for( index = startIndex; index < endIndex; index++ )
  {
    result += tokens[index];

    // No trailing spaces at end of line
    if( index < ( endIndex - 1 ) )
    {
      result += " ";
    }
  }

  return result;
}
