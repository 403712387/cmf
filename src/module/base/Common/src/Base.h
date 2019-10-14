#ifndef BASE_H
#define BASE_H

#ifdef WIN32
#define CMF_EXPORT __declspec(dllexport)
#else
#define CMF_EXPORT
#endif

#endif
