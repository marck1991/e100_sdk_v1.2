/**
 * @file src/app/connected/applib/src/format/ApplibFormat_MuxExifPIV.c
 *
 * Implementation of MW format utility
 *
 * History:
 *    2013/09/03 - [Martin Lai] created file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (��Software��) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <applib.h>
#include <fifo/Fifo.h>
#include <format/Muxer.h>
#include <format/ExtMux.h>
#include <format/ExifMux.h>
#include <stream/File.h>
#include <recorder/Encode.h>
#include <recorder/VideoEnc.h>
#include <recorder/StillEnc.h>


//#define DEBUG_APPLIB_FORMAT_MUX_EXIF_PIV
#if defined(DEBUG_APPLIB_FORMAT_MUX_EXIF_PIV)
#define DBGMSG AmbaPrint
#define DBGMSGc(x) AmbaPrintColor(GREEN,x)
#define DBGMSGc2 AmbaPrintColor
#else
#define DBGMSG(...)
#define DBGMSGc(...)
#define DBGMSGc2(...)
#endif

static AMP_STREAM_HDLR_s *MuxExifPIVStream = NULL;
static AMP_MUX_FORMAT_HDLR_s *MuxExifPIVFormat = NULL;
static AMP_IMAGE_INFO_s *MuxExifPIVImage = NULL;
static AMP_MUXER_PIPE_HDLR_s *MuxExifPIVPipe = NULL;


extern UINT8 *H264EncBitsBuf;
static AMP_FIFO_HDLR_s *MuxExifPIVFifoHdlr = NULL;

#ifdef CONFIG_APP_ARD
#define EXIF_PIV_OPENED 0x00000001
static AMP_CFG_TAG_s GpsIfd[GPS_TOTAL_TAGS];
static UINT8 GpsTagCount = 0;
#define EXIF_PHOTO_AMOUT  (10)
static UINT8 CurrentSequenceNum = 0;
static UINT8 RawSequenceNo = 0;
static UINT8 ExifTagCount[EXIF_PHOTO_AMOUT] = {0};
static APPLIB_EXIF_DATA_s ExifIfd[EXIF_PHOTO_AMOUT][EXIF_TOTAL_TAGS];
static UINT8 EndianType = 1;  //1: AMP_FORMAT_EXIF_BIGENDIAN, 0: AMP_FORMAT_EXIF_LITTLEENDIAN
static AMBA_KAL_EVENT_FLAG_t ExifPIVOpen;
static int AppLibFormatMuxExifPiv_SetGpsTag(UINT16 GpsTagNum, AMP_CFG_TAG_s *GpsTag)
{
    UINT8 i = 0, j = 0;

    for (i=0; i<GpsTagNum; i++) {
        for (j=0; j<GpsTagCount; j++) {
            if (GpsTag[i].Tag == GpsIfd[j].Tag) {
                GpsTag[i].Set = GpsIfd[j].Set;
                GpsTag[i].Count = GpsIfd[j].Count;
                GpsTag[i].Data = GpsIfd[j].Data;
                GpsTag[i].Value = GpsIfd[j].Value;
                break;
            }
        }
    }
    return 0;
}

static int AppLibFormatMuxExifPiv_SetExifTag(UINT8 SequenceNo, UINT16 ExifTagNum, AMP_CFG_TAG_s *ExifTag)
{
    UINT8 i = 0;
    UINT8 BytesPerData = 0;

//    AmbaPrint("--- SetExifTag: ExifTagCount[%d] = %d", SequenceNo, ExifTagCount[SequenceNo]);
    for (i = 0; i < ExifTagCount[SequenceNo]; i++) {
        switch (ExifIfd[SequenceNo][i].DataType) {
            case APPLIB_TAG_TYPE_BYTE:
            case APPLIB_TAG_TYPE_ASCII:
            case APPLIB_TAG_TYPE_UNDEFINED:
                BytesPerData = 1;
                break;
            case APPLIB_TAG_TYPE_SHORT:
                BytesPerData = 2;
                break;
            case APPLIB_TAG_TYPE_LONG:
            case APPLIB_TAG_TYPE_SLONG:
                BytesPerData = 4;
                break;
            case APPLIB_TAG_TYPE_RATIONAL:
            case APPLIB_TAG_TYPE_SRATIONAL:
                BytesPerData = 8;
                break;
            default:
                BytesPerData = 1;
                break;
        }

        ExifTag[ExifIfd[SequenceNo][i].TagId].Set = TAG_CONFIGURED;
        ExifTag[ExifIfd[SequenceNo][i].TagId].Count = ExifIfd[SequenceNo][i].DataLength / BytesPerData;
        if (ExifIfd[SequenceNo][i].DataLength > 4) { // > 4 Bytes
            ExifTag[ExifIfd[SequenceNo][i].TagId].Data = ExifIfd[SequenceNo][i].Data;
//            AmbaPrint("--- SetExifTag: Data 0x%X %X %X %X %X %X %X %X", ExifIfd[SequenceNo][i].Data[0], ExifIfd[SequenceNo][i].Data[1],
//                                                          ExifIfd[SequenceNo][i].Data[2], ExifIfd[SequenceNo][i].Data[3],
//                                                          ExifIfd[SequenceNo][i].Data[4], ExifIfd[SequenceNo][i].Data[5],
//                                                          ExifIfd[SequenceNo][i].Data[6], ExifIfd[SequenceNo][i].Data[7]);
        } else {
            ExifTag[ExifIfd[SequenceNo][i].TagId].Value = ExifIfd[SequenceNo][i].Value;
 //           AmbaPrint("--- SetExifTag: Value %d", ExifTag[ExifIfd[SequenceNo][i].TagId].Value);
        }
    }

    return 0;
}

#endif

/**
 *  @brief EXIF muxer's call back function
 *
 *  EXIF muxer's call back function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatMuxExifPIV_MuxCB(void* hdlr, UINT32 event, void* info)
{
    switch (event) {
    case AMP_MUXER_EVENT_START:
        AppLibFormatMuxMgr_MuxStart(hdlr, info);
        DBGMSG("[Applib - Format] AMP_MUXER_EVENT_START");
//        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_START, 0, 0);
        break;
    case AMP_MUXER_EVENT_END:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_END, APPLIB_MUXER_TYPE_EXIF_PIV, 0);
        DBGMSG("[Applib - Format] AMP_MUXER_EVENT_END");
//        AppLibFormatMuxExifPIV_Close();
//        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_END, 0, 0);
        break;
#if 0
    case AMP_MUXER_EVENT_PAUSE:
        AmbaPrint("[Applib - Format] AMP_MUXER_EVENT_PAUSE");
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_PAUSE, 0, 0);
        break;
    case AMP_MUXER_EVENT_RESUME:
        AmbaPrint("[Applib - Format] AMP_MUXER_EVENT_RESUME");
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_RESUME, 0, 0);
        break;
    case AMP_MUXER_EVENT_STOP_ON_PAUSED:
        AmbaPrint("[Applib - Format] AMP_MUXER_EVENT_STOP_ON_PAUSED");
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_STOP_ON_PAUSED, 0, 0);
        break;
#endif
    case AMP_MUXER_EVENT_REACH_LIMIT:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_REACH_LIMIT, 0, 0);
        DBGMSG("[Applib - Format] AMP_MUXER_EVENT_REACH_LIMIT");
        break;
    case AMP_MUXER_EVENT_IO_ERROR:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_IO_ERROR, 0, 0);
        DBGMSG("[Applib - Format] AMP_MUXER_EVENT_IO_ERROR");
        break;
    case AMP_MUXER_EVENT_FIFO_ERROR:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_FIFO_ERROR, 0, 0);
        DBGMSG("[Applib - Format] AMP_MUXER_EVENT_FIFO_ERROR");
        break;
    case AMP_MUXER_EVENT_GENERAL_ERROR:
        AppLibComSvcHcmgr_SendMsgNoWait(HMSG_MUXER_GENERAL_ERROR, 0, 0);
        DBGMSG("[Applib - Format] AMP_MUXER_EVENT_GENERAL_ERROR");
        break;
    default:
        AmbaPrint("[Applib - Format] <ExifMux_MuxCB> Unknown event %X info: %x", event, info);
        break;
    }

    return 0;
}



int AppLibFormatMuxExifPIV_Open(void)
{
    int ReturnValue = 0;
    char PhotoFileName[MAX_FILENAME_LENGTH];
    UINT32 VidEncBitsFifoSize = 0;
    UINT32 VidEncDescSize = 0;
    DBGMSG("[Applib - Format] <MuxExifPIV_Open>");
    // open file stream
    if (MuxExifPIVStream == NULL) {
        AMP_FILE_STREAM_CFG_s FileCfg;

        ReturnValue = AppLibStorageDmf_CreateFile(APPLIB_DCF_MEDIA_IMAGE, "JPG", DCIM_HDLR, PhotoFileName);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] AppLibStorageDmf_CreateFile fail");
            return -1;
        }

        AmpFileStream_GetDefaultCfg(&FileCfg);
        FileCfg.Async = TRUE;
        FileCfg.AsyncParam.MaxBank = 2;
        ReturnValue = AmpFileStream_Create(&FileCfg, &MuxExifPIVStream);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] AmpFileStream_Create fail");
            return -1;
        } else {
            DBGMSG("[Applib - Format] <MuxExifPIV_Open> AmpFileStream_Create");
        }
        MuxExifPIVStream->Func->Open(MuxExifPIVStream, PhotoFileName, AMP_STREAM_MODE_WRONLY);
    }

    if (MuxExifPIVImage == NULL) {
        AMP_MUX_IMAGE_INFO_CFG_s ImageCfg;
        APPLIB_SENSOR_PIV_ENC_CONFIG_s PIVCapConfig;
        APPLIB_SENSOR_VIDEO_ENC_CONFIG_s *VideoEncConfigData;
        UINT64 SeqNum = 0;

        AmpFormat_NewImageInfo(PhotoFileName, &MuxExifPIVImage);
        AppLibSysSensor_GetPIVSize(AppLibVideoEnc_GetSensorVideoRes(), &PIVCapConfig);
        VideoEncConfigData = AppLibSysSensor_GetVideoConfig(AppLibVideoEnc_GetSensorVideoRes());
        {
            AMP_BITS_DESC_s *Desc;
#ifdef CONFIG_APP_ARD
            if (MuxExifPIVFifoHdlr != NULL) {
#endif
            ReturnValue = AmpFifo_PeekEntry(MuxExifPIVFifoHdlr, &Desc, 0);
            if (ReturnValue < 0) {
                SeqNum = 0;
                AmbaPrintColor(RED,"[Applib - Format] AmpFifo_PeekEntry fail");
            } else {
                SeqNum = Desc->SeqNum;
#ifndef CONFIG_APP_ARD
                CurrentSequenceNum = (SeqNum % EXIF_PHOTO_AMOUT);
#endif
#ifdef CONFIG_APP_ARD
             }
#endif
            }
        }
        // set media info
        AmpMuxer_GetDefaultImageInfoCfg(&ImageCfg);
        AppLibRecorderMemMgr_GetBufSize(&VidEncBitsFifoSize, &VidEncDescSize);
        ImageCfg.Fifo = MuxExifPIVFifoHdlr;
        ImageCfg.BufferBase = H264EncBitsBuf;
        ImageCfg.BufferLimit = H264EncBitsBuf + VidEncBitsFifoSize;
        ImageCfg.UsedFrame = ImageCfg.TotalFrame = 3;
        ImageCfg.Frame[0].SeqNum = SeqNum;
        ImageCfg.Frame[0].Type = AMP_FIFO_TYPE_JPEG_FRAME;
        ImageCfg.Frame[0].Width = ALIGN_32(VideoEncConfigData->CaptureWidth);
        ImageCfg.Frame[0].Height = VideoEncConfigData->CaptureHeight;

        /*Test for exifinfo*/
        ImageCfg.Frame[0].ExifInfo.ExposureTimeNum = 100;
        ImageCfg.Frame[0].ExifInfo.ExposureTimeDen = 200;
        ImageCfg.Frame[0].ExifInfo.FNumberNum = 400;
        ImageCfg.Frame[0].ExifInfo.FNumberDen = 500;
        ImageCfg.Frame[0].ExifInfo.ColorSpace = 66;
        ImageCfg.Frame[0].ExifInfo.Contrast = 99;

        ImageCfg.Frame[1].SeqNum = SeqNum;
        ImageCfg.Frame[1].Type = AMP_FIFO_TYPE_THUMBNAIL_FRAME;
        ImageCfg.Frame[1].Width = ALIGN_32(PIVCapConfig.ThumbnailWidth);
        ImageCfg.Frame[1].Height = PIVCapConfig.ThumbnailHeight;
        ImageCfg.Frame[2].SeqNum = SeqNum;
        ImageCfg.Frame[2].Type = AMP_FIFO_TYPE_SCREENNAIL_FRAME;
        ImageCfg.Frame[2].Width = ALIGN_32(PIVCapConfig.ScreennailWidth);
        ImageCfg.Frame[2].Height = PIVCapConfig.ScreennailHeight;
        DBGMSG("[Applib - Format] <MuxExifPIV_Open>: ImageCfg.Fifo = 0x%x",ImageCfg.Fifo);
        ReturnValue = AmpMuxer_InitImageInfo(MuxExifPIVImage, &ImageCfg);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] AmpMuxer_InitImageInfo fail");
            return -1;
        } else {
            DBGMSG("[Applib - Format] <MuxExifPIV_Open> AmpMuxer_InitImageInfo");
        }
    }

    if (MuxExifPIVFormat == NULL) {
        AMP_EXIF_MUX_CFG_s ExifCfg;
        /*Test for set tag*/
        UINT8 nMake[10] = {'I','f','d','0','t','e','s','t','A','\0'};
        UINT8 nModel[10] = {'I','f','d','0','t','e','s','t','B','\0'};
        UINT8 nSoftVer[12] = {'I','f','d','0','t','e','s','t','C','D','E','\0'};
        int i;
        AmpExifMux_GetDefaultCfg(&ExifCfg);

        for (i = 0; i < ExifCfg.SetTagInfo.Ifd0Tags; i++) {
            switch (ExifCfg.SetTagInfo.Ifd0[i].Tag) {
            case TIFF_Software:
                ExifCfg.SetTagInfo.Ifd0[i].Set = TAG_CONFIGURED;
                ExifCfg.SetTagInfo.Ifd0[i].Count = 12;
                ExifCfg.SetTagInfo.Ifd0[i].Data = nSoftVer;
                break;
            case TIFF_Make:
                ExifCfg.SetTagInfo.Ifd0[i].Set = TAG_CONFIGURED;
                ExifCfg.SetTagInfo.Ifd0[i].Count = 10;
                ExifCfg.SetTagInfo.Ifd0[i].Data = nMake;
                break;
            case TIFF_Model:
                ExifCfg.SetTagInfo.Ifd0[i].Set = TAG_CONFIGURED;
                ExifCfg.SetTagInfo.Ifd0[i].Count = 10;
                ExifCfg.SetTagInfo.Ifd0[i].Data = nModel;
                break;
            case TIFF_Orientation:
                ExifCfg.SetTagInfo.Ifd0[i].Set = TAG_DISABLED;
                break;
            case TIFF_XResolution:
                ExifCfg.SetTagInfo.Ifd0[i].Set = TAG_DISABLED;
                break;
            case TIFF_YResolution:
                ExifCfg.SetTagInfo.Ifd0[i].Set = TAG_DISABLED;
                break;
            case TIFF_GPSInfoIFDPointer:
                ExifCfg.SetTagInfo.Ifd0[i].Set = TAG_ENABLED;
                break;
            default:
                break;
            }
        }
#ifdef CONFIG_APP_ARD
        // for GPS
        AppLibFormatMuxExifPiv_SetGpsTag(ExifCfg.SetTagInfo.GpsIfdTags, ExifCfg.SetTagInfo.GpsIfd);

        // for Exif
        AppLibFormatMuxExifPiv_SetExifTag(CurrentSequenceNum, ExifCfg.SetTagInfo.ExifIfdTags, ExifCfg.SetTagInfo.ExifIfd);
#endif
        /*Create*/
        ExifCfg.Stream = MuxExifPIVStream;
#ifdef CONFIG_APP_ARD
        ExifCfg.Endian = EndianType;
#endif
        ReturnValue = AmpExifMux_Create(&ExifCfg, &MuxExifPIVFormat);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] AmpExifMux_Create fail");
            return -1;
        } else {
            DBGMSG("[Applib - Format] <MuxExifPIV_Open> AmpExifMux_Create");
        }
    }

    if (MuxExifPIVPipe == NULL) {
        AMP_MUXER_PIPE_CFG_s MuxPipeCfg;
        // create muxer pipe
        AmpMuxer_GetDefaultCfg(&MuxPipeCfg);
        MuxPipeCfg.FormatCount = 1;
        MuxPipeCfg.Format[0] = MuxExifPIVFormat;
        MuxPipeCfg.Media[0] = (AMP_MEDIA_INFO_s *)MuxExifPIVImage;
        MuxPipeCfg.OnEvent = AppLibFormatMuxExifPIV_MuxCB;
        ReturnValue = AmpMuxer_Create(&MuxPipeCfg, &MuxExifPIVPipe);
#ifdef CONFIG_APP_ARD
        if ((NULL == MuxExifPIVPipe) || (ReturnValue < 0)) {
#else
        if (ReturnValue < 0) {
#endif
            AmbaPrintColor(RED,"[Applib - Format] AmpMuxer_Create fail");
            return -1;
        } else {
            DBGMSG("[Applib - Format] <MuxExifPIV_Open> AmpMuxer_Create");
        }
        ReturnValue = AmpMuxer_Add(MuxExifPIVPipe);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] AmpMuxer_Add fail");
            return -1;
        } else {
            DBGMSG("[Applib - Format] <MuxExifPIV_Open> AmpMuxer_Add");
        }
    }
    ReturnValue = AmpMuxer_Start(MuxExifPIVPipe, 0);

#ifdef CONFIG_APP_ARD
    if (AmbaKAL_EventFlagGive(&ExifPIVOpen, EXIF_PIV_OPENED) != OK) {
        AmbaPrint("Fail to give flag EXIF_PIV_OPENED %s, %d", __func__, __LINE__);
    }
#endif

    return ReturnValue;
}


//static UINT32 MuxExifPIVCounter = 0;

/**
 *  @brief Close the Exif muxer
 *
 *  Close the Exif muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxExifPIV_Close(void)
{
    int ReturnValue = 0;
#ifdef CONFIG_APP_ARD
    UINT32 actualFlags;

    if (AmbaKAL_EventFlagTake(&ExifPIVOpen, EXIF_PIV_OPENED, AMBA_KAL_AND_CLEAR, &actualFlags, 5*1000) != OK) {
        AmbaPrint("Fail to take ExifPIVOpen flag EXIF_PIV_OPENED %s, %d", __func__, __LINE__);
        return -1;
    }
#endif

    AmbaPrint("[Applib - Format] <MuxExifPIV_Close> wait complete start");
    ReturnValue = AmpMuxer_WaitComplete(MuxExifPIVPipe, AMBA_KAL_WAIT_FOREVER);
    AmbaPrint("[Applib - Format] <MuxExifPIV_Close> wait complete end");

    if (MuxExifPIVPipe != NULL) {
        ReturnValue = AmpMuxer_Remove(MuxExifPIVPipe);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] AmpMuxer_Remove fail");
        }
        ReturnValue = AmpMuxer_Delete(MuxExifPIVPipe);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] AmpMuxer_Delete fail");
        }
        MuxExifPIVPipe = NULL;
    }

    if (MuxExifPIVFormat != NULL) {
        ReturnValue = AmpExifMux_Delete(MuxExifPIVFormat);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] AmpExifMux_Delete fail");
        }
        MuxExifPIVFormat = NULL;
#ifdef CONFIG_APP_ARD
        GpsTagCount = 0;
        //AmbaPrint("AppLibFormatMuxExif_Close %d", CurrentSequenceNum);
        ExifTagCount[CurrentSequenceNum] = 0;
#endif
    }

    if (MuxExifPIVStream != NULL) {
        MuxExifPIVStream->Func->Close(MuxExifPIVStream);
        ReturnValue = AmpFileStream_Delete(MuxExifPIVStream);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] AmpFileStream_Delete fail");
        }
        MuxExifPIVStream = NULL;
    }

    if (MuxExifPIVImage != NULL) {
        ReturnValue = AmpFormat_RelImageInfo(MuxExifPIVImage, TRUE);
        if (ReturnValue < 0) {
            AmbaPrintColor(RED,"[Applib - Format] AmpFormat_RelImageInfo fail");
        }
        MuxExifPIVImage = NULL;
    }

    if (MuxExifPIVFifoHdlr != NULL) {
        AmpFifo_Delete(MuxExifPIVFifoHdlr);
        MuxExifPIVFifoHdlr = NULL;
    }

    return ReturnValue;
}

/**
 *  @brief EXIF muxer's fifo call back function
 *
 *  EXIF muxer's fifo call back function
 *
 *  @param [in] hdlr Handler
 *  @param [in] event Event
 *  @param [in] info Infomation
 *
 *  @return >=0 success, <0 failure
 */
static int AppLibFormatMuxExifPIV_MuxFifoCB(void *hdlr, UINT32 event, void* info)
{
    DBGMSGc2(GREEN,"[Applib - Format] <ExifPIVMux_FifoCB>: hdlr = 0x%x, 0x%x", (UINT32)(hdlr), event);
    switch (event) {
    case AMP_FIFO_EVENT_DATA_READY:
        AmbaPrint("[Applib - Format] <ExifPIVMux_FifoCB>: AMP_FIFO_EVENT_DATA_READY");
#if 0
        if (MuxExifPIVCounter == 0) {
            AppLibFormatMuxExifPIV_Open();
            MuxExifPIVCounter ++;
        }
        AmpMuxer_OnDataReady((AMP_FIFO_HDLR_s *)hdlr);
#else
        AppLibFormatMuxMgr_DataReady(hdlr, info, PIV_MUXER_HANDLER);
#endif
        break;
    default:
        AmbaPrint("[Applib - Format] <ExifPIVMux_FifoCB>: evnet 0x%x", event);
        break;
    }
    return 0;
}


/**
 *  @brief Register the callback function in the EXIF muxer manager.
 *
 *  Register the callback function in the EXIF muxer manager.
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxExifPIV_RegMuxMgr(void)
{
    APPLIB_MUX_MGR_HANDLER_s Handler = {0};

    DBGMSG("[Applib - Format] <MuxExifPIV_RegMuxMgr>");
    memset(&Handler, 0x0, sizeof(APPLIB_MUX_MGR_HANDLER_s));
    Handler.MuxerInit = &AppLibFormat_ExifMuxerInit;
    Handler.MuxerOpen = &AppLibFormatMuxExifPIV_Open;
    Handler.MuxerClose = &AppLibFormatMuxExifPIV_Close;
    Handler.DataReadyNum = 3;
    Handler.Used = 1;
    Handler.Type = PIV_MUXER_HANDLER;

    return AppLibFormatMuxMgr_RegMuxHandler(&Handler);
}

/**
 *  @brief Start the exif muxer for PIV
 *
 *  Start the exif muxer
 *
 *  @return >=0 success, <0 failure
 */
int AppLibFormatMuxExifPIV_Start(void)
{
    AppLibFormat_ExifMuxerInit();
    AppLibFormatMuxMgr_Init();
    AmbaPrint("[Applib - Format] <MuxExifPIV_Start>");
    AppLibFormatMuxExifPIV_RegMuxMgr();
    /**delete fifo to clean read/ write point before start encode*/
    if (MuxExifPIVFifoHdlr != NULL) {
        AmpFifo_Delete(MuxExifPIVFifoHdlr);
        MuxExifPIVFifoHdlr = NULL;
    }

    if (MuxExifPIVFifoHdlr == NULL) {
        extern AMP_STLENC_HDLR_s *StillEncPriPIV;

        AMP_FIFO_CFG_s fifoDefCfg;
        // create muxer fifo
        AmpFifo_GetDefaultCfg(&fifoDefCfg);
        fifoDefCfg.hCodec = StillEncPriPIV;
        fifoDefCfg.IsVirtual = 1;
        fifoDefCfg.NumEntries = 128;
        fifoDefCfg.cbEvent = AppLibFormatMuxExifPIV_MuxFifoCB;
        AmpFifo_Create(&fifoDefCfg, &MuxExifPIVFifoHdlr);
        DBGMSG("[Applib - Format] <ExifMux_Start>: MuxExifPIVFifoHdlr = 0x%x codec = 0x%x",MuxExifPIVFifoHdlr,StillEncPriPIV);
    }
#ifdef CONFIG_APP_ARD
    if(AmbaKAL_EventFlagCreate(&ExifPIVOpen) != OK) {
        AmbaPrint("Fail to create ExifPIVOpen flag %s, %d", __func__, __LINE__);
    }
#endif

    return 0;
}
#ifdef CONFIG_APP_ARD

/**
 *  @brief Set Exif endian type
 *
 *  Set Exif endian type
 *
 *  @return 0
 */
int AppLibFormatMuxExifPiv_SetEndianType(APPLIB_EXIF_ENDIAN_TYPE_e Endian)
{
    if (Endian == APPLIB_EXIF_TYPE_BIG_ENDIAN) {
        EndianType = 0;
    } else {
        EndianType = 1;
    }
    return 0;
}

/**
 *  @brief Get Exif endian type
 *
 *  Get Exif endian type
 *
 *  @return 0
 */
UINT8 AppLibFormatMuxExifPiv_GetEndianType(void)
{
    return EndianType;
}


/**
 *  @brief Confige GPS information
 *
 *  Confige GPS information
 *
 *  @return 0
 */
int AppLibFormatMuxExifPiv_ConfigGpsTag(APPLIB_GPS_DATA_s *GpsData)
{
    UINT8 BytesPerData = 0;
    GpsIfd[GpsTagCount].Tag = (UINT16)GpsData->GpsTagId;
    GpsIfd[GpsTagCount].Set = TAG_CONFIGURED;
    switch (GpsData->GpsDataType) {
        case APPLIB_TAG_TYPE_BYTE:
        case APPLIB_TAG_TYPE_ASCII:
        case APPLIB_TAG_TYPE_UNDEFINED:
            BytesPerData = 1;
            break;
        case APPLIB_TAG_TYPE_SHORT:
            BytesPerData = 2;
            break;
        case APPLIB_TAG_TYPE_LONG:
        case APPLIB_TAG_TYPE_SLONG:
            BytesPerData = 4;
            break;
        case APPLIB_TAG_TYPE_RATIONAL:
        case APPLIB_TAG_TYPE_SRATIONAL:
            BytesPerData = 8;
            break;
        default:
            BytesPerData = 1;
            break;
    }
    GpsIfd[GpsTagCount].Count = GpsData->GpsDataLength / BytesPerData;
    if (GpsData->GpsDataLength > 4) {  // length > 4Byte
        GpsIfd[GpsTagCount].Data = GpsData->GpsData;
    } else {
        GpsIfd[GpsTagCount].Value = ((GpsData->GpsData)[0] << 8) | ((GpsData->GpsData)[1]);
    }
    //AmbaPrint("%d, %d, %d", GpsData->GpsDataLength, BytesPerData, GpsIfd[GpsTagCount].Count);
    //AmbaPrint("0x%X or 0x%X", GpsIfd[GpsTagCount].Value, GpsIfd[GpsTagCount].Data);

    GpsTagCount++;

    return 0;
}

/**
 *  @brief Confige Exif information
 *
 *  Confige Exif information
 *
 *  @return 0
 */
int AppLibFormatMuxExifPiv_ConfigExifTag(APPLIB_EXIF_DATA_s *ExifData)
{
    UINT8 SeqNo = RawSequenceNo % EXIF_PHOTO_AMOUT;
    UINT64 Mask = 0x000000FF00000000;
    ExifIfd[SeqNo][ExifTagCount[SeqNo]].TagId = ExifData->TagId;
    ExifIfd[SeqNo][ExifTagCount[SeqNo]].DataType = ExifData->DataType;
    ExifIfd[SeqNo][ExifTagCount[SeqNo]].DataLength = ExifData->DataLength;
    if (ExifData->DataLength > 4) { // > 4 Bytes
        UINT8 i = 0;
        for (i = 0; i < 8; i++) {
            if (EndianType == APPLIB_EXIF_TYPE_BIG_ENDIAN) {
                ExifIfd[SeqNo][ExifTagCount[SeqNo]].Data[i] = (ExifData->Value & 0xFF00000000000000) >> 56;
                ExifData->Value <<= 8;
            } else if (EndianType == APPLIB_EXIF_TYPE_LITTLE_ENDIAN){
                if (i < 4) {
                    ExifIfd[SeqNo][ExifTagCount[SeqNo]].Data[i] = (UINT8)((ExifData->Value & Mask) >> (8*(4+i)));
                    Mask<<= 8;
                    if (Mask == 0) {
                        Mask = 0x00000000000000FF;
                    }
                } else {
                    ExifIfd[SeqNo][ExifTagCount[SeqNo]].Data[i] = (UINT8)(ExifData->Value & Mask);
                    ExifData->Value >>= 8;
                }
            } else {
                AmbaPrintColor(RED,"[Applib - Format] Endian type not exist!");
            }
        }
    } else {
        ExifIfd[SeqNo][ExifTagCount[SeqNo]].Value = ExifData->Value;
    }

    ExifTagCount[SeqNo]++;

    return 0;
}

int ApplibFormatMuxExifPiv_IncreaseRawCount(void)
{
#ifdef CONFIG_APP_ARD
    CurrentSequenceNum = RawSequenceNo;
#endif
    RawSequenceNo++;
    RawSequenceNo %= EXIF_PHOTO_AMOUT;
    return 0;
}
#endif
