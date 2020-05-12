#pragma once

#include <stdio.h>

#define _LOGGING

#ifdef _LOGGING
#	define errlog(...) printf(__VA_ARGS__)
#else
#	define errlog(...) 
#endif
