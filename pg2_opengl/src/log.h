#pragma once

#include <stdio.h>

#define _LOGGING
#define _LOGLEVEL 2

#ifdef _LOGGING
#if _LOGLEVEL >= 2
#	define warnlog(...) printf(__VA_ARGS__)
#else
#	define warnlog(...) 
#endif
#if _LOGLEVEL >= 1
#	define errlog(...) printf(__VA_ARGS__)
#else
#	define errlog(...) 
#endif
#else
#	define errlog(...) 
#	define warnlog(...) 
#endif
