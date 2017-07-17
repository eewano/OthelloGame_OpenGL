#ifndef Utility_h
#define Utility_h

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

std::string GetCurrentWorkingDir()
{
    char buff[FILENAME_MAX];
    GetCurrentDir(buff, FILENAME_MAX);
    std::string currentWorkingDir(buff);
    return currentWorkingDir;
}

#endif /* Utility_h */