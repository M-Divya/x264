/*****************************************************************************
* x264-csv.h: csv functions
*****************************************************************************
* Copyright (C) 2003-2015 x264 project
*
* Authors: Gopu Govindaswamy <gopu@multicorewareinc.com>
*          Divya Manivannan <divya@multicorewareinc.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
*
* This program is also available under a commercial proprietary license.
* For more information, contact us at licensing@x264.com.
*****************************************************************************/

#ifndef X264_X264_CSV_H
#define X264_X264_CSV_H

#include "x264.h"
#include "stdio.h"
#include <stdint.h>

/* Open a CSV log file. On success it returns a file handle which must be passed
 * to x264_csvlog_frame(). The file handle must be closed by the caller using
 * fclose(). If csv log level is 0, then no frame logging header is written to the
 * file. This function will return NULL if it is unable to open the file for write */
FILE *x264_csvlog_open( const x264_param_t* param, const char* filename, int level );

/* Log frame statistics to the CSV file handle. If csv log level is 0, then no
 * frame logging is written to the file. */
void x264_csvlog_frame( FILE* csvfh, const x264_param_t* param, const x264_picture_t* pic, int level );

#endif
