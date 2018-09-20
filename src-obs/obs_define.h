#pragma once

#ifndef SafeDelete
#define SafeDelete(p)   if(p) { delete (p); (p) = NULL;}  
#endif

#ifndef SafeDelArry
#define SafeDelArry(p)  if(p) { delete []p; (p) = NULL;} 
#endif

#ifndef SafeFree
#define SafeFree(p)   if(p) { free (p); (p) = NULL;}  
#endif

#define NELEMS(arr) (sizeof(arr) / sizeof(arr[0]))

typedef signed char			int8;
typedef signed short		int16;
typedef signed int			int32;
typedef signed long long	int64;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

//inline void IncPtr(void **p, int64 i){ *p = (void*)((int)*p + i); }