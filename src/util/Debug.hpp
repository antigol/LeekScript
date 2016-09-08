#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>

/*
 * 0 : no debug,
 * 1 : print types
 * 2 : print analyse + amount of leeks
 * 3 : print debug messages
 * 4 : print leeks details
 */
#define DEBUG 1

// to disable assert
//#define NDEBUG

#if DEBUG >= 3
#define DBOUT( x )  x
#else
#define DBOUT( x )
#endif

#endif // DEBUG_HPP
