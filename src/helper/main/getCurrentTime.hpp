// current date and time

#ifndef GETCURRENTTIME_H

#define GETCURRENTTIME_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getCurrentTime()
{
    time_t rawtime;
    struct tm *timeinfo;
    char *buffer = (char *)malloc(80 * sizeof(char));

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
    return buffer;
}

#endif // GETCURRENTTIME_H