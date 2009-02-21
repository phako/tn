/*  Copyright (c) 2008 Jens Georg.
 
    This file is part of tn.

    tn is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    tn is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with tn.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "util.h"
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>

void 
logging(enum LogLevel level, 
        const char* file, 
        int line, 
        const char* format, ...)
{
    char buffer[2048] = {0};
    char *color = NULL;
    char *level_string = NULL;

    va_list ap;
    va_start(ap, format);
    vsnprintf(buffer, 2047, format, ap);
    va_end(ap);

    switch (level)
    {
        case DEBUG:
            color = "[0m";
            level_string = "DEBUG";
            break;
        case INFO:
            color = "[32m";
            level_string = "INFO";
            break;
        case WARNING:
            color = "[33m";
            level_string = "WARNING";
            break;
        case ERROR:
            color = "[31m";
            level_string = "ERROR";
            break;
    }
    if (level == ERROR)
    {
        fprintf(stderr, "\033%s[%s]\033[0m\t: %s(%d): %s\n", color, level_string, file, line, buffer);
    }
    else
    {
        fprintf(stdout, "\033%s[%s]\033[0m\t: %s(%d): %s\n", color, level_string, file, line, buffer);
    }
}

