#ifndef CRTDBG_HELPER_H
#define CRTDBG_HELPER_H
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG   
#ifndef DBG_NEW      
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )     
#define new DBG_NEW   
#endif
#endif  // _DEBUG
#else
#include <stdlib.h>	
#endif
#endif