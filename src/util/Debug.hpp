#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>

/*
 * 0 : no debug,
 * 1 : print types + #leaks
 * 2 : print leak details
 */
#define DEBUG 0

// to disable assert
//#define NDEBUG

#if DEBUG == 0
  #define DBOUT( x )
#else
  #define DBOUT( x )  x
#endif

#endif // DEBUG_HPP
