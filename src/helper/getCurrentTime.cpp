// current date and time

#include "getCurrentTime.h"

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
