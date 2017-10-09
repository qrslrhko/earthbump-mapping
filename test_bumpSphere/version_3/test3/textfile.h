#ifndef macGL_textfile_h
#define macGL_textfile_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *textFileRead(const char *fn);
int textFileWrite(char *fn, char *s);
unsigned char *readDataFromFile(char *fn);
#endif