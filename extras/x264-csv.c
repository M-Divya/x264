/*****************************************************************************
* x264-csv.c: csv functions
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

#include <stdint.h>
#include "x264.h"
#include "x264-csv.h"
#include "common/common.h"

FILE * x264_csvlog_open( const x264_param_t* param, const char* filename, int level )
{
    static const char* CSVHeader =
        " EncodeOrder,"
        " FrameType,"
        " POC,"
        " AverageQP,"
        " FrameSize";
    static const char* PSNRHeader =
        ", Y PSNR,"
        " U PSNR,"
        " V PSNR,"
        " YUV PSNR";
    static const char* SSIMHeader =
        ", SSIM,"
        " SSIM(db)";
    static const char* MBHeader =
        ", Intra 4x4 mbCount,"
        " Intra 8x8 mbCount,"
        " Intra 16x16 mbCount,"
        " Intra PCM mbCount,"
        " InterP 16x16 16x8 and 8x16 mbCount,"
        " InterP 8x8 mbCount,"
        " InterP SKIP mbCount,"
        " Inter BDIRECT mbCount,"
        " B_L0_L0,"
        " B_L0_L1,"
        " B_L0_BI,"
        " B_L1_L0,"
        " B_L1_L1,"
        " B_L1_BI,"
        " B_BI_L0,"
        " B_BI_L1,"
        " B_BI_BI,"
        " InterB 8x8 mbCount,"
        " InterB SKIP mbCount,"
        " Total MB Count,"
        " AverageLumaDistortion,"
        " AverageChromaDistortion,"
        " Average Luma Level,"
        " Maximum Luma Level,"
        " Minimum Luma Level \n";

    FILE *csvfh = NULL;
    csvfh = x264_fopen( filename, "r" );
    if ( csvfh )
    {
        fclose( csvfh );
        /* file already exist re-open the file with append mode */
        csvfh = x264_fopen( filename, "ab" );
    }
    else
    {
        /* open new csv file and write header */
        csvfh = x264_fopen( filename, "wb" );
        if ( csvfh )
        {
            if ( level )
            {
                fprintf( csvfh, "%s", CSVHeader );
                if ( param->rc.i_rc_method == X264_RC_CRF )
                    fprintf( csvfh, ", Average RateFactor" );
                if ( param->analyse.b_psnr )
                    fprintf( csvfh, "%s", PSNRHeader );
                if ( param->analyse.b_ssim )
                    fprintf( csvfh, "%s", SSIMHeader );
                fprintf( csvfh, "%s", MBHeader );
            }
        }
    }
    return csvfh;
}

void x264_csvlog_frame( FILE* csvfh, const x264_param_t* param, const x264_picture_t* pic, int level )
{
    if ( csvfh )
    {
        fprintf( csvfh, "%4d, %c, %3d, %.2f, %d, ",
                 pic->frameData.i_frame,
                 pic->frameData.i_type == SLICE_TYPE_I ? 'I' : (pic->frameData.i_type == SLICE_TYPE_P ? 'P' : 'B'),
                 pic->frameData.i_poc,
                 pic->frameData.f_qp_avg_aq,
                 pic->frameData.frame_size );
        if ( param->rc.i_rc_method == X264_RC_CRF )
            fprintf( csvfh, "%2.8f,", pic->frameData.f_crf_avg );
        /* if psnr enabled */
        if ( param->analyse.b_psnr )
            fprintf( csvfh, "%5.2f, %5.2f, %5.2f, %5.2f, ",
                     pic->frameData.f_psnr_y,
                     pic->frameData.f_psnr_u,
                     pic->frameData.f_psnr_v,
                     pic->frameData.f_psnr );
        /* if ssim enabled */
        if ( param->analyse.b_ssim )
        {
            double inv_ssim = 1 - pic->frameData.f_ssim;
            double ssim_db;
            if ( inv_ssim <= 0.0000000001 )
                ssim_db = 100;
            ssim_db = -10.0 * log10( inv_ssim );

            fprintf( csvfh, "%.5f, %5.3f, ",
                     pic->frameData.f_ssim,
                     ssim_db );
        }

        int mbCount = 0;
        for ( int j = 0; j < X264_MBTYPE_MAX; j++ )
        {
            fprintf( csvfh, "%d,", pic->frameData.i_mb_count[j] );
            mbCount += pic->frameData.i_mb_count[j];
        }
        fprintf( csvfh, "%d, %.2lf, %.2lf, %.2lf, %u, %u",
                 mbCount,
                 ( double )( pic->frameData.f_luma_satd ) / mbCount,
                 ( double )( pic->frameData.f_chroma_satd ) / mbCount,
                 pic->frameData.f_luma_level / mbCount,
                 pic->frameData.f_max_luma_level,
                 pic->frameData.f_min_luma_level );

        fputs( "\n", csvfh );
    }
}
