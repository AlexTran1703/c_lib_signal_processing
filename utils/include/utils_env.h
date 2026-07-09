#include <stdio.h>


#ifdef _WIN32
    #define POPEN _popen
    #define PCLOSE _pclose
    #include <io.h>
    #include <fcntl.h>
    #define OPEN _open
#else
    #define POPEN popen
    #define PCLOSE pclose
    #include <unistd.h>
    #include <fcntl.h>
    #define OPEN open
#endif
