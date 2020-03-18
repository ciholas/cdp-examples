// Ciholas, Inc. - www.ciholas.com
// Licensed under: creativecommons.org/licenses/by/4.0
#ifndef _UTILS__PRINT__H_
#define _UTILS__PRINT__H_


/*********************
* Public Functions
**********************/
// Prints an error based on errno and exits with 1
void Print_ExitOnError(const char *on_what);  // string to print

// Prints line on stdout with new line on the end and flushes
int Print_Line(const char * format, ...);  // same args as a standard printf

// Prints a message (to stdout for code=0, stderr otherwise) and exits with code
void Print_ExitOnMessage(int code, const char *format, ...);  // code, plus printf arguments
#endif
