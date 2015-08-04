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

FILE * open_csvlog_file(const char *filename)
{
    static const char* CSVHeader =
        " EncodeOrder,"
        " FrameType,"
        " POC,"
        " AverageQP,"
        " FrameSize,"
        " Y PSNR,"
        " U PSNR,"
        " V PSNR,"
        " YUV PSNR,"
        " SSIM,"
        " SSIM(db),"
        " Average RateFactor,";

    static const char* MBHeader =
        " Intra 4x4 mbCount,"
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
        "AverageChromaDistortion \n";

    FILE *csvfh = NULL;
    csvfh = x264_fopen(filename, "r");
    if (csvfh)
    {
        fclose(csvfh);
        /* file already exist re-open the file with append mode */
        csvfh = x264_fopen(filename, "ab");
    }
    else
    {
        /* open new csv file and write header */
        csvfh = x264_fopen(filename, "wb");
        if (csvfh)
            fprintf(csvfh, "%s%s", CSVHeader, MBHeader);
    }
    return csvfh;
}

void write_framelog_to_csvfile(const x264_t *ht, FILE* csvfh, const x264_picture_t *pic_out, int frame_size)
{
    int i;
    x264_t *h;
    for (i = 0; i < ht->i_thread_frames; i++)
    {
        if (pic_out->poc == ht->thread[i]->fdec->i_poc)
        {
            h = ht->thread[i];
            break;
        }
    }

    if (csvfh)
    {
        fprintf(csvfh, "%4d, %c, %3d, %.2f, %d, ",
            h->i_frame,
            h->sh.i_type == SLICE_TYPE_I ? 'I' : (h->sh.i_type == SLICE_TYPE_P ? 'P' : 'B'),
            h->fdec->i_poc >> 1,
            h->fdec->f_qp_avg_aq,
            frame_size);
        /* if psnr enabled */
        if (h->param.analyse.b_psnr)
            fprintf(csvfh, "%5.2f, %5.2f, %5.2f, %5.2f, ",
            pic_out->prop.f_psnr[0],
            pic_out->prop.f_psnr[1],
            pic_out->prop.f_psnr[2],
            pic_out->prop.f_psnr_avg);
        else
            fputs("-, -, -, -, ", csvfh);
        /* if ssim enabled */
        if (h->param.analyse.b_ssim)
        {
            double inv_ssim = 1 - pic_out->prop.f_ssim;
            double ssim_db;
            if (inv_ssim <= 0.0000000001)
                ssim_db = 100;
            ssim_db = -10.0 * log10(inv_ssim);

            fprintf(csvfh, "%.5f, %5.3f, ",
                pic_out->prop.f_ssim,
                ssim_db);
        }
        else
            fputs("-, -, ", csvfh);

        fprintf(csvfh, "%2.8f,",
            pic_out->prop.f_crf_avg);

        int mbCount = 0;
        for (int j = 0; j < X264_MBTYPE_MAX; j++)
        {
            fprintf(csvfh, "%d,",
                h->stat.frame.i_mb_count[j]);
            mbCount += h->stat.frame.i_mb_count[j];
        }
        fprintf(csvfh, "%d, %d, %d",
            mbCount,
            h->mb.i_mb_luma_satd / mbCount,
            h->mb.i_mb_chroma_satd / mbCount);
        h->mb.i_mb_luma_satd = h->mb.i_mb_chroma_satd = 0;

        fputs("\n", csvfh);
    }
}