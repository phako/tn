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


#ifndef __UTIL_H
#define __UTIL_H

#define return_if(arg, retval) do {if (arg) return retval; } while (0);
#ifndef MAX
#   define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

#define LOG(level, format, ...) \
    logging(level, __FILE__, __LINE__, format, __VA_ARGS__)

void 
logging(enum LogLevel level, const char* file, int line, const char* format, ...);

#endif /* __UTIL_H */
