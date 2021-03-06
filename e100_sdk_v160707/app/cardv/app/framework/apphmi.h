/**
  * @file src/app/framework/apphmi.h
  *
  * User-defined HMI messages
  *
  * History:
  *    2013/07/05 - [Martin Lai] created file
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

#ifndef APP_APPHMI_H_
#define APP_APPHMI_H_

#include <framework/appdefines.h>
#include <applibhmi.h>

/**********************************************************************/
/* MDL_APP_FLOW_ID messsages                                          */
/**********************************************************************/
/**
* Partition: |31 - 27|26 - 24|23 - 16|15 -  8| 7 -  0|
*   |31 - 27|: MDL_APP_FLOW_ID
*   |26 - 24|: MSG_TYPE_HMI
*   |23 - 16|: app flow type ID
*   |15 -  8|: Self-defined
*   | 7 -  0|: Self-defined
* Note:
*   bit 0-15 could be defined in the app itself (individual
*   header files). However, module ID should be defined here
*   for arrangement
**/
#define HMSG_APPFLOW(x)         MSG_ID(MDL_APP_FLOW_ID, MSG_TYPE_HMI, (x))
/** Sub-group:type of app library & interface events */
#define HMSG_APPFLOW_ID_FLOW    (0x01)
#define HMSG_APPFLOW_ID_TEST    (0xFF)

/* App flow events */
#define HMSG_APPFLOW_FLOW(x)    HMSG_APPFLOW(((UINT32)HMSG_APPFLOW_ID_FLOW << 16) | (x))
#define HMSG_EXIT_HDL           HMSG_APPFLOW_FLOW(0x0001)
#define HMSG_STOP_APP_COMPLETE  HMSG_APPFLOW_FLOW(0x0002)
#define HMSG_APP_SET_REF_MQID   HMSG_APPFLOW_FLOW(0x0003)
#define HMSG_APP_SET_CLNTID     HMSG_APPFLOW_FLOW(0x0004)
#define HMSG_APP_COPY_USER_PREF HMSG_APPFLOW_FLOW(0x0005)

/*************************************************************************
 * Application messages ID (|15 -  8| in message)
 ************************************************************************/
#define HMSG_APPFLOW_FLOW_ID_ERROR      (0x01)
#define HMSG_APPFLOW_FLOW_ID_STATE      (0x02)
#define HMSG_APPFLOW_FLOW_ID_CMD        (0x03)

/*************************************************************************
 * Application Error messages
 ************************************************************************/
#define HMSG_APPFLOW_FLOW_ERROR(x)      HMSG_APPFLOW_FLOW(((UINT32)HMSG_APPFLOW_FLOW_ID_ERROR << 8) | (x))
#define AMSG_ERROR_CARD_REMOVED         HMSG_APPFLOW_FLOW_ERROR(0x01)

/*************************************************************************
 * Application state messages
 ************************************************************************/
#define HMSG_APPFLOW_FLOW_STATE(x)      HMSG_APPFLOW_FLOW(((UINT32)HMSG_APPFLOW_FLOW_ID_STATE << 8) | (x))
#define AMSG_STATE_CARD_REMOVED         HMSG_APPFLOW_FLOW_STATE(0x01)
#define AMSG_STATE_CARD_INSERT_ACTIVE   HMSG_APPFLOW_FLOW_STATE(0x05)
#define AMSG_STATE_WIDGET_CLOSED        HMSG_APPFLOW_FLOW_STATE(0x02)
#define AMSG_STATE_BATTERY_STATE        HMSG_APPFLOW_FLOW_STATE(0x03)
#define AMSG_STATE_BOSS_BOOTED          HMSG_APPFLOW_FLOW_STATE(0x04)
#define AMSG_STATE_BOOTMGR_DONE         HMSG_APPFLOW_FLOW_STATE(0x06)
#define AMSG_STATE_PWR_BEEP_INIT_DONE   HMSG_APPFLOW_FLOW_STATE(0x07)
#define AMSG_STATE_AINFRA_INIT_DONE     HMSG_APPFLOW_FLOW_STATE(0x08)
#define AMSG_STATE_ADEC_INIT_DONE       HMSG_APPFLOW_FLOW_STATE(0x09)
#define AMSG_STATE_AENC_INIT_DONE       HMSG_APPFLOW_FLOW_STATE(0x0A)
#define AMSG_STATE_LENS_INIT_DONE       HMSG_APPFLOW_FLOW_STATE(0x0B)
#define AMSG_STATE_EXTLIB_LOAD_DONE     HMSG_APPFLOW_FLOW_STATE(0x0C)
#define AMSG_STATE_ENCODE_LOAD_DONE     HMSG_APPFLOW_FLOW_STATE(0x0D)
#define AMSG_STATE_DECODE_LOAD_DONE     HMSG_APPFLOW_FLOW_STATE(0x0E)

/*************************************************************************
 * Application flow command messages
 ************************************************************************/
#define HMSG_APPFLOW_FLOW_CMD(x)    HMSG_APPFLOW_FLOW(((UINT32)HMSG_APPFLOW_FLOW_ID_CMD << 8) | (x))
/**
* Note: For easy extension in the future, use enumerator to
* define app flow commands. Please be aware that, since only 8
* bits are arranged for defining flow commands, the definitions
* should be rearranged when the number of flow commands exceeds
* 255 (command ID 0 is not used)
**/
typedef enum _APPFLOW_CMD_MSG_ID_e_ {
    HMSG_USER_FLOW_CMD_DUMMY = HMSG_APPFLOW_FLOW_CMD(0x00),
    AMSG_CMD_SET_VIDEO_RES,
    AMSG_CMD_SET_VIDEO_QUALITY,
    AMSG_CMD_SET_VIDEO_PRE_RECORD,
    AMSG_CMD_SET_VIDEO_TIME_LAPSE,
    AMSG_CMD_SET_VIDEO_DUAL_STREAMS,
    AMSG_CMD_SET_PHOTO_SIZE,
    AMSG_CMD_SET_PHOTO_QUALITY,
    AMSG_CMD_SET_PHOTO_TIME_LAPSE,
    AMSG_CMD_SET_PHOTO_CAPTURE_MODE,
    AMSG_CMD_SET_PHOTO_QUICKVIEW_DELAY,
    AMSG_CMD_SET_RECORD_MODE,
    AMSG_CMD_SET_SELFTIMER,
    AMSG_CMD_SET_APP_ENC_MODE,
    AMSG_CMD_SET_DIGITAL_ZOOM,
    AMSG_CMD_SET_SYSTEM_TYPE,
    AMSG_CMD_SET_DMF_MODE,
    AMSG_CMD_SET_VIDEO_EDITOR,
    AMSG_CMD_SET_DELETE_FILE,
    AMSG_CMD_SET_USB_MODE,
    AMSG_CMD_USB_APP_START,
    AMSG_CMD_USB_APP_STOP,
    AMSG_CMD_STOP_PLAYING,
    AMSG_CMD_SWITCH_APP,
    AMSG_CMD_APP_READY,
    AMSG_CMD_POWERSHUTDOWN,
    AMSG_CMD_UPDATE_TIME_DISPLAY,
    AMSG_CMD_CARD_UPDATE_ACTIVE_CARD,
    AMSG_CMD_CARD_FMT_NONOPTIMUM,
    AMSG_CMD_CARD_MOVIE_RECOVER,
#ifdef CONFIG_APP_ARD
    AMSG_CMD_SENSOR_NEAR_FAR_SWITCH,
    AMSG_CMD_APP_ID_SETTING_READY,
    AMSG_CMD_SW_REBOOT,
    AMSG_CMD_SET_VIDEO_GSENSOR_SENSITIVITY,
    AMSG_CMD_RESET_SPLIT_TIME,
    AMSG_CMD_EVENT_RECORD,
    AMSG_CMD_MOTION_RECORD_STOP,
#endif
    HMSG_USER_FLOW_CMD_LAST = HMSG_APPFLOW_FLOW_CMD(0xFF)
} APPFLOW_CMD_MSG_ID_e;

/* App test events */
#define HMSG_APPFLOW_TEST(x)    HMSG_APPFLOW(((UINT32) HMSG_APPFLOW_ID_TEST << 16) | (x))
#define HMSG_APPFLOW_TEST_ID_CMD        (0x01)

#define HMSG_APPFLOW_TEST_CMD(x)        HMSG_APPFLOW_TEST(((UINT32)HMSG_APPFLOW_TEST_ID_CMD << 8) | (x))
/**
* Note: For easy extension in the future, use enumerator to
* define app test commands. Please be aware that, since only 8
* bits are arranged for defining test commands, the definitions
* should be rearranged when the number of test commands exceeds
* 255 (command ID 0 is not used)
**/
typedef enum _APPFLOW_TEST_CMD_MSG_ID_e_ {
    HMSG_USER_TEST_CMD_DUMMY = HMSG_APPFLOW_TEST_CMD(0x00),
    HMSG_USER_LOOP_TEST,
    HMSG_USER_MODE_DZSCAN,
    HMSG_USER_MODE_SAVE_OSD,
    HMSG_USER_MODE_STAMP_LOGO,
    HMSG_USER_MODE_STAMP_DEBUG,///< 005
    HMSG_USER_SHUTTER_PROFILING,
    HMSG_USER_DEBUG_DUMP,
    HMSG_USER_MODE_ENC,
    HMSG_USER_MODE_PHOTO,
    HMSG_USER_MODE_DEC,///< 010
    HMSG_USER_MODE_PIV_SKIP,
    HMSG_USER_HDMI_MODE,
    HMSG_USER_CP_MODE,
    HMSG_USER_CS_MODE,
    HMSG_USER_LCD_MODE,///< 015
    HMSG_USER_VOUT_RES_AUTO,
    HMSG_USER_VOUT_RES_1080P,
    HMSG_USER_VOUT_RES_1080P_HALF,
    HMSG_USER_VOUT_RES_1080I,
    HMSG_USER_VOUT_RES_1080P24,///< 020
    HMSG_USER_VOUT_RES_720P,
    HMSG_USER_VOUT_RES_720P_HALF,
    HMSG_USER_VOUT_RES_720P24,
    HMSG_USER_VOUT_RES_SDP,
    HMSG_USER_VOUT_RES_SDI,///< 025
    HMSG_USER_VOUT_RES_LCD,
    HMSG_USER_SWITCH_VIN_COLOR_DOMAIN,
    HMSG_USER_SWITCH_VOUT_COLOR_DOMAIN,
    HMSG_USER_LCD_FCHAN_REPROGRAM,
    HMSG_USER_LCD_DCHAN_REPROGRAM,///< 030
    HMSG_USER_LOAD_CALIB_DATA_ALL,
    HMSG_USER_LOAD_CALIB_DATA_STG3,
    HMSG_USER_LOAD_CALIB_DATA_STG4,
    HMSG_USER_QUICK_VIEW_SET,
    HMSG_USER_SHUTTER_SETTING,    ///< 035
    HMSG_USER_VIDEO_RES_LIMIT,
    HMSG_USER_VIN_ROTATE,
    HMSG_USER_FORMAT,
    HMSG_USER_CP_DIR,
    HMSG_USER_VBR_TUNING,///< 040
    HMSG_USER_VBR_TUNING_FROM_RANGE,
    HMSG_USER_THM_TV_DISP,
    HMSG_USER_THM_CLIP_STOP_UPDATE,
    HMSG_USER_JACK_SWITCH_PB,
    HMSG_USER_APP_EXIT,///< 045
    HMSG_USER_DISP_ROTATE,
    HMSG_USER_SWITCH_PB_STREAM,
    HMSG_USER_TEST_CALIB,
    HMSG_USER_USB_MSC2FIO,
    HMSG_USER_RECORD_CTRL,///< 050
    HMSG_USER_DEBUG_YUV_DUMP,
    HMSG_USER_STILL_PIV_PAUSE_VIDEO,
    HMSG_USER_STILL_PIV_RESUME_VIDEO,
    HMSG_USER_EVENT_FORCE_STOP,
    HMSG_USER_VF_START,
    HMSG_USER_VF_STOP,
    HMSG_USER_VF_SWITCH_TO_RECORD,
    HMSG_USER_PIRNT_REC_CAP_STATE,
    HMSG_USER_TEST_CMD_LAST = HMSG_APPFLOW_TEST_CMD(0xFF)
} APPFLOW_TEST_CMD_MSG_ID_e;


/**********************************************************************/
/* MDL_APP_KEY_ID messsages                                           */
/**********************************************************************/
/**
* Partition: |31 - 27|26 - 24|23 - 16|15 -  8| 7 -  0|
*   |31 - 27|: MDL_APP_KEY_ID
*   |26 - 24|: MSG_TYPE_HMI
*   |23 - 16|: User input device ID
*   |15 -  8|: Event ID
*   | 7 -  0|: Key ID
**/
#define HMSG_KEY2(x)            MSG_ID(MDL_APP_KEY_ID, MSG_TYPE_HMI, (x))
/* Sub-group:type of input device */
#define HMSG_KEY_ID_BUTTON      (0x01)
#define HMSG_KEY_ID_TOUCH       (0x02)

#if 1
enum ui_button_id_e
{
    UP_BUTTON,/* 0 */
    DOWN_BUTTON,
    LEFT_BUTTON,
    RIGHT_BUTTON,
    VOLUME_UP_BUTTON,
    VOLUME_DOWN_BUTTON,
    FFW_BUTTON,
    REW_BUTTON,
    ZOOM_IN_BUTTON,
    ZOOM_OUT_BUTTON,
    POWER_BUTTON,/* 10*/
    RESET_BUTTON,
    RECORD_BUTTON,
    SNAP1_BUTTON,
    SNAP2_BUTTON,
    MODE_BUTTON,
    MENU_BUTTON,
    SET_BUTTON,
    PANEL_CLOSED_BUTTON,
    PANEL_ROTATED_BUTTON,
    PLAY_BUTTON,/* 20 */
    STOP_BUTTON,
    BLC_BUTTON,
    DISPLAY_BUTTON,
    DEL_BUTTON,
    MIRROR_BUTTON,
    BATT_COVER_DET_BUTTON,
    LCD_ON_BUTTON,
    LCD_OFF_BUTTON,
    VIO_BUTTON,/* 30 */
    F0_BUTTON,
    F1_BUTTON,
    F2_BUTTON,
    F3_BUTTON,
    F4_BUTTON,
    F5_BUTTON,
    F6_BUTTON,
    F7_BUTTON,
    F8_BUTTON,
    F9_BUTTON,

    IR_ZOOM_IN_BUTTON,/* 40 */
    IR_ZOOM_OUT_BUTTON,
    IR_REG_DOWN_BUTTON,
    IR_REG_UP_BUTTON,
    IR_UP_BUTTON,
    IR_DOWN_BUTTON,
    IR_LEFT_BUTTON,
    IR_RIGHT_BUTTON,
    IR_SET_BUTTON,
    IR_HUNDRED_BUTTON,
    IR_AUDIO_BUTTON,/* 50 */
    IR_MODE_BUTTON,
    IR_MENU_BUTTON,
    IR_BASS_LEFT_BUTTON,
    IR_RETURN_BUTTON,
    IR_IMAGE_BUTTON,
    IR_FOUR_THREE_BUTTON,
    IR_STANDARD_BUTTON,
    IR_INDEX_BUTTON,
    IR_MTS_BUTTON,
    IR_C1_BUTTON,/* 60 */
    IR_MUTE_BUTTON,
    IR_DISPLAY_BUTTON,
    IR_DEL_BUTTON,
    IR_N0_BUTTON,
    IR_SNAP1_BUTTON,
    IR_SNAP2_BUTTON,
    IR_RECORD_BUTTON,
    IR_N4_BUTTON,
    IR_N5_BUTTON,
    IR_N6_BUTTON,/* 70 */
    IR_N7_BUTTON,
    IR_N8_BUTTON,
    IR_N9_BUTTON,
};
#endif


/* Button device event */
#define HMSG_KEY2_BUTTON(x)             HMSG_KEY2(((UINT32)HMSG_KEY_ID_BUTTON << 16) | (x))
#define HMSG_USER_UP_BUTTON             HMSG_KEY2_BUTTON(UP_BUTTON)
#define HMSG_USER_DOWN_BUTTON           HMSG_KEY2_BUTTON(DOWN_BUTTON)
#define HMSG_USER_LEFT_BUTTON           HMSG_KEY2_BUTTON(LEFT_BUTTON)
#define HMSG_USER_RIGHT_BUTTON          HMSG_KEY2_BUTTON(RIGHT_BUTTON)
#define HMSG_USER_VOLUME_UP_BUTTON      HMSG_KEY2_BUTTON(VOLUME_UP_BUTTON)
#define HMSG_USER_VOLUME_DOWN_BUTTON    HMSG_KEY2_BUTTON(VOLUME_DOWN_BUTTON)
#define HMSG_USER_FFW_BUTTON            HMSG_KEY2_BUTTON(FFW_BUTTON)
#define HMSG_USER_REW_BUTTON            HMSG_KEY2_BUTTON(REW_BUTTON)
#define HMSG_USER_ZOOM_IN_BUTTON        HMSG_KEY2_BUTTON(ZOOM_IN_BUTTON)
#define HMSG_USER_ZOOM_OUT_BUTTON       HMSG_KEY2_BUTTON(ZOOM_OUT_BUTTON)
#define HMSG_USER_POWER_BUTTON          HMSG_KEY2_BUTTON(POWER_BUTTON)
#define HMSG_USER_RESET_BUTTON          HMSG_KEY2_BUTTON(RESET_BUTTON)
#define HMSG_USER_RECORD_BUTTON         HMSG_KEY2_BUTTON(RECORD_BUTTON)
#define HMSG_USER_SNAP1_BUTTON          HMSG_KEY2_BUTTON(SNAP1_BUTTON)
#define HMSG_USER_SNAP2_BUTTON          HMSG_KEY2_BUTTON(SNAP2_BUTTON)
#define HMSG_USER_MODE_BUTTON           HMSG_KEY2_BUTTON(MODE_BUTTON)
#define HMSG_USER_MENU_BUTTON           HMSG_KEY2_BUTTON(MENU_BUTTON)
#define HMSG_USER_SET_BUTTON            HMSG_KEY2_BUTTON(SET_BUTTON)
#define HMSG_USER_PANEL_CLOSED_BUTTON   HMSG_KEY2_BUTTON(PANEL_CLOSED_BUTTON)
#define HMSG_USER_PANEL_ROTATED_BUTTON  HMSG_KEY2_BUTTON(PANEL_ROTATED_BUTTON)
#define HMSG_USER_PLAY_BUTTON           HMSG_KEY2_BUTTON(PLAY_BUTTON)
#define HMSG_USER_STOP_BUTTON           HMSG_KEY2_BUTTON(STOP_BUTTON)
#define HMSG_USER_BLC_BUTTON            HMSG_KEY2_BUTTON(BLC_BUTTON)
#define HMSG_USER_DISPLAY_BUTTON        HMSG_KEY2_BUTTON(DISPLAY_BUTTON)
#define HMSG_USER_DEL_BUTTON            HMSG_KEY2_BUTTON(DEL_BUTTON)
#define HMSG_USER_MIRROR_BUTTON         HMSG_KEY2_BUTTON(MIRROR_BUTTON)
#define HMSG_USER_BATT_COVER_DET_BUTTON HMSG_KEY2_BUTTON(BATT_COVER_DET_BUTTON)
#define HMSG_USER_LCD_ON                HMSG_KEY2_BUTTON(LCD_ON_BUTTON)
#define HMSG_USER_LCD_OFF               HMSG_KEY2_BUTTON(LCD_OFF_BUTTON)
#define HMSG_USER_VIO_BUTTON            HMSG_KEY2_BUTTON(VIO_BUTTON)
#define HMSG_USER_F0_BUTTON             HMSG_KEY2_BUTTON(F0_BUTTON)
#define HMSG_USER_F1_BUTTON             HMSG_KEY2_BUTTON(F1_BUTTON)
#define HMSG_USER_F2_BUTTON             HMSG_KEY2_BUTTON(F2_BUTTON)
#define HMSG_USER_F3_BUTTON             HMSG_KEY2_BUTTON(F3_BUTTON)
#define HMSG_USER_F4_BUTTON             HMSG_KEY2_BUTTON(F4_BUTTON)
#define HMSG_USER_F5_BUTTON             HMSG_KEY2_BUTTON(F5_BUTTON)
#define HMSG_USER_F6_BUTTON             HMSG_KEY2_BUTTON(F6_BUTTON)
#define HMSG_USER_F7_BUTTON             HMSG_KEY2_BUTTON(F7_BUTTON)
#define HMSG_USER_F8_BUTTON             HMSG_KEY2_BUTTON(F8_BUTTON)
#define HMSG_USER_F9_BUTTON             HMSG_KEY2_BUTTON(F9_BUTTON)

#define HMSG_USER_IR_ZOOM_IN_BUTTON     HMSG_KEY2_BUTTON(IR_ZOOM_IN_BUTTON)
#define HMSG_USER_IR_ZOOM_OUT_BUTTON    HMSG_KEY2_BUTTON(IR_ZOOM_OUT_BUTTON)
#define HMSG_USER_IR_REG_DOWN_BUTTON    HMSG_KEY2_BUTTON(IR_REG_DOWN_BUTTON)
#define HMSG_USER_IR_REG_UP_BUTTON      HMSG_KEY2_BUTTON(IR_REG_UP_BUTTON)
#define HMSG_USER_IR_UP_BUTTON          HMSG_KEY2_BUTTON(IR_UP_BUTTON)
#define HMSG_USER_IR_DOWN_BUTTON        HMSG_KEY2_BUTTON(IR_DOWN_BUTTON)
#define HMSG_USER_IR_LEFT_BUTTON        HMSG_KEY2_BUTTON(IR_LEFT_BUTTON)
#define HMSG_USER_IR_RIGHT_BUTTON       HMSG_KEY2_BUTTON(IR_RIGHT_BUTTON)
#define HMSG_USER_IR_SET_BUTTON         HMSG_KEY2_BUTTON(IR_SET_BUTTON)
#define HMSG_USER_IR_HUNDRED_BUTTON     HMSG_KEY2_BUTTON(IR_HUNDRED_BUTTON)
#define HMSG_USER_IR_AUDIO_BUTTON       HMSG_KEY2_BUTTON(IR_AUDIO_BUTTON)
#define HMSG_USER_IR_MODE_BUTTON        HMSG_KEY2_BUTTON(IR_MODE_BUTTON)
#define HMSG_USER_IR_MENU_BUTTON        HMSG_KEY2_BUTTON(IR_MENU_BUTTON)
#define HMSG_USER_IR_BASE_LEFT_BUTTON   HMSG_KEY2_BUTTON(IR_BASS_LEFT_BUTTON)
#define HMSG_USER_IR_RETURN_BUTTON      HMSG_KEY2_BUTTON(IR_RETURN_BUTTON)
#define HMSG_USER_IR_IMAGE_BUTTON       HMSG_KEY2_BUTTON(IR_IMAGE_BUTTON)
#define HMSG_USER_IR_FOUR_THREE_BUTTON  HMSG_KEY2_BUTTON(IR_FOUR_THREE_BUTTON)
#define HMSG_USER_IR_STANDARD_BUTTON    HMSG_KEY2_BUTTON(IR_STANDARD_BUTTON)
#define HMSG_USER_IR_INDEX_BUTTON       HMSG_KEY2_BUTTON(IR_INDEX_BUTTON)
#define HMSG_USER_IR_MTS_BUTTON         HMSG_KEY2_BUTTON(IR_MTS_BUTTON)
#define HMSG_USER_IR_C1_BUTTON          HMSG_KEY2_BUTTON(IR_C1_BUTTON)
#define HMSG_USER_IR_MUTE_BUTTON        HMSG_KEY2_BUTTON(IR_MUTE_BUTTON)
#define HMSG_USER_IR_DISPLAY_BUTTON     HMSG_KEY2_BUTTON(IR_DISPLAY_BUTTON)
#define HMSG_USER_IR_DEL_BUTTON         HMSG_KEY2_BUTTON(IR_DEL_BUTTON)
#define HMSG_USER_IR_N0_BUTTON          HMSG_KEY2_BUTTON(IR_N0_BUTTON)
#define HMSG_USER_IR_SNAP1_BUTTON       HMSG_KEY2_BUTTON(IR_SNAP1_BUTTON)
#define HMSG_USER_IR_SNAP2_BUTTON       HMSG_KEY2_BUTTON(IR_SNAP2_BUTTON)
#define HMSG_USER_IR_RECORD_BUTTON      HMSG_KEY2_BUTTON(IR_RECORD_BUTTON)
#define HMSG_USER_IR_N4_BUTTON          HMSG_KEY2_BUTTON(IR_N4_BUTTON)
#define HMSG_USER_IR_N5_BUTTON          HMSG_KEY2_BUTTON(IR_N5_BUTTON)
#define HMSG_USER_IR_N6_BUTTON          HMSG_KEY2_BUTTON(IR_N6_BUTTON)
#define HMSG_USER_IR_N7_BUTTON          HMSG_KEY2_BUTTON(IR_N7_BUTTON)
#define HMSG_USER_IR_N8_BUTTON          HMSG_KEY2_BUTTON(IR_N8_BUTTON)
#define HMSG_USER_IR_N9_BUTTON          HMSG_KEY2_BUTTON(IR_N9_BUTTON)

#define HMSG_USER_AN_HOME_BUTTON        HMSG_KEY2_BUTTON(AN_HOME_BUTTON)
#define HMSG_USER_AN_MENU_BUTTON        HMSG_KEY2_BUTTON(AN_MENU_BUTTON)
#define HMSG_USER_AN_BACK_BUTTON        HMSG_KEY2_BUTTON(AN_BACK_BUTTON)
#define HMSG_USER_AN_SEARCH_BUTTON      HMSG_KEY2_BUTTON(AN_SEARCH_BUTTON)

/** button release events */
#define HMSG_KEY_BUTTON_ID_RELEASE      (0x08)
#define HMSG_KEY2_BUTTON_RELEASE(x)     HMSG_KEY2_BUTTON(((UINT32)HMSG_KEY_BUTTON_ID_RELEASE << 8) | (x))

#ifdef CONFIG_APP_ARD
#define HMSG_USER_MENU_BUTTON_CLR      HMSG_KEY2_BUTTON_RELEASE(MENU_BUTTON)    /*For Long key*/
#endif
#define HMSG_USER_UP_BUTTON_CLR         HMSG_KEY2_BUTTON_RELEASE(UP_BUTTON)
#define HMSG_USER_DOWN_BUTTON_CLR       HMSG_KEY2_BUTTON_RELEASE(DOWN_BUTTON)
#define HMSG_USER_LEFT_BUTTON_CLR       HMSG_KEY2_BUTTON_RELEASE(LEFT_BUTTON)
#define HMSG_USER_RIGHT_BUTTON_CLR      HMSG_KEY2_BUTTON_RELEASE(RIGHT_BUTTON)
#define HMSG_USER_SNAP1_BUTTON_CLR      HMSG_KEY2_BUTTON_RELEASE(SNAP1_BUTTON)
#define HMSG_USER_SNAP2_BUTTON_CLR      HMSG_KEY2_BUTTON_RELEASE(SNAP2_BUTTON)
#define HMSG_USER_POWER_BUTTON_CLR      HMSG_KEY2_BUTTON_RELEASE(POWER_BUTTON)
#define HMSG_USER_ZOOM_IN_BUTTON_CLR    HMSG_KEY2_BUTTON_RELEASE(ZOOM_IN_BUTTON)
#define HMSG_USER_ZOOM_OUT_BUTTON_CLR   HMSG_KEY2_BUTTON_RELEASE(ZOOM_OUT_BUTTON)
#define HMSG_USER_RECORD_BUTTON_CLR     HMSG_KEY2_BUTTON_RELEASE(RECORD_BUTTON)

#define HMSG_USER_IR_UP_BUTTON_CLR      HMSG_KEY2_BUTTON_RELEASE(IR_UP_BUTTON)
#define HMSG_USER_IR_DOWN_BUTTON_CLR    HMSG_KEY2_BUTTON_RELEASE(IR_DOWN_BUTTON)
#define HMSG_USER_IR_LEFT_BUTTON_CLR    HMSG_KEY2_BUTTON_RELEASE(IR_LEFT_BUTTON)
#define HMSG_USER_IR_RIGHT_BUTTON_CLR   HMSG_KEY2_BUTTON_RELEASE(IR_RIGHT_BUTTON)
#define HMSG_USER_IR_ZOOM_IN_BUTTON_CLR HMSG_KEY2_BUTTON_RELEASE(IR_ZOOM_IN_BUTTON)
#define HMSG_USER_IR_ZOOM_OUT_BUTTON_CLR        HMSG_KEY2_BUTTON_RELEASE(IR_ZOOM_OUT_BUTTON)
#define HMSG_USER_IR_SNAP1_BUTTON_CLR   HMSG_KEY2_BUTTON_RELEASE(IR_SNAP1_BUTTON)
#define HMSG_USER_IR_SNAP2_BUTTON_CLR   HMSG_KEY2_BUTTON_RELEASE(IR_SNAP2_BUTTON)
#define HMSG_USER_IR_RECORD_BUTTON_CLR  HMSG_KEY2_BUTTON_RELEASE(IR_RECORD_BUTTON)

#define HMSG_USER_AN_HOME_BUTTON_CLR    HMSG_KEY2_BUTTON_RELEASE(AN_HOME_BUTTON)
#define HMSG_USER_AN_MENU_BUTTON_CLR    HMSG_KEY2_BUTTON_RELEASE(AN_MENU_BUTTON)
#define HMSG_USER_AN_BACK_BUTTON_CLR    HMSG_KEY2_BUTTON_RELEASE(AN_BACK_BUTTON)
#define HMSG_USER_AN_SEARCH_BUTTON_CLR  HMSG_KEY2_BUTTON_RELEASE(AN_SEARCH_BUTTON)

/** Touchpanel device event */
#define HMSG_KEY2_TOUCH(x)              HMSG_KEY2(((UINT32)HMSG_KEY_ID_TOUCH << 16) | (x))
#define HMSG_USER_TOUCHPANEL_PRESSED    HMSG_KEY2_TOUCH(0x01)
#define HMSG_USER_FAKE_TOUCHPANEL_PRESSED       HMSG_KEY2_TOUCH(0x02)

/** Touchpanel release event */
#define HMSG_KEY_TOUCH_ID_RELEASE       (0x08)
#define HMSG_KEY2_TOUCH_RELEASE(x)      HMSG_KEY2_TOUCH(((UINT32)HMSG_KEY_TOUCH_ID_RELEASE << 8) | (x))
#define HMSG_KEY2_FAKE_TOUCH_RELEASE(x) HMSG_KEY2_TOUCH(((UINT32)HMSG_KEY_TOUCH_ID_RELEASE << 8) | (x))

#define HMSG_USER_TOUCHPANEL_RELEASED   HMSG_KEY2_TOUCH_RELEASE(0x01)

/* Retrieve app flow ID */
#define MSG_APP_FLOW_TYPE(x) ((x >> 16) & 0xFF)

#endif /* APP_APPHMI_H_ */
