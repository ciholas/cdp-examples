// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*********************
* Public Functions
**********************/
void  Print_ExitOnError(const char *on_what) {
    fputs(strerror(errno),stderr);
    fputs(": ",stderr);
    fputs(on_what,stderr);
    fputc('\n',stderr);
    exit(1);
}

int Print_Line(const char * format, ...){
    va_list args;
    int rv;
    //print
    va_start(args, format);
    rv = vprintf(format, args);
    va_end(args);
    printf("\r\n");
    fflush(stdout);

    return rv;
}

void Print_ExitOnMessage(int exit_code, const char *format, ...){
    FILE * fs = stderr;
    if(exit_code == 0)
        fs = stdout;
    va_list args;
    va_start(args, format);
    vfprintf(fs, format, args);
    va_end(args);
    fflush(fs);
    exit(exit_code);
}
