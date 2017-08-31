// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef _STDAFX_H_PROTECT_
#define _STDAFX_H_PROTECT_


#include <stdio.h>


#ifdef _WIN32
#include <tchar.h>
#else
#include <sys/stat.h>
#endif

#define ASSERT(x)


#include <stdlib.h>
#include <memory.h>
#include <string>
#include <math.h>
#include <algorithm>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <string.h>
#include <map>

//#ifdef _WIN32
#define _SUPPORT_SQS  1
//#endif

//#ifdef _WIN32
#define _SUPPORT_CL_
//#endif


#ifdef _SUPPORT_CL_
#include <CL/cl.h>

#ifdef _WIN32
#include <d3d9.h>
#endif

#endif

#ifndef _WIN32
typedef  unsigned char BYTE;
#endif

// TODO: reference additional headers your program requires here
#endif //_STDAFX_H_PROTECT_

