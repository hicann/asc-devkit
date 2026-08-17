/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef TS_TSCH_DEFINES_H
#define TS_TSCH_DEFINES_H

#ifndef DAVINCI_MDC_VOS
#if defined(__COMPILER_HUAWEILITEOS__)
#include <los_typedef.h>
#elif defined(__KERNEL__)
#else
#include <stdint.h>
#endif
#else
#include "ee_platform_types.h"
#endif

#ifdef STARS_CTRL_CPU
#include <linux/types.h>
#endif

#include "task_scheduler_error.h"
#include "tsch_defines_profiling.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(WIN32) && !defined(__cplusplus)

#define inline __inline

#endif

#ifndef char_t
typedef char char_t;
#endif

/**
 * @ingroup tsch
 * @brief the size of each task command is 64 byte
 */
#define TS_TASK_COMMAND_SIZE (64U)

/**
 * @ingroup tsch
 * @brief the size of each task report msg to host is 4 byte
 */
#define TS_TASK_REPORT_MSG_SIZE (12U)

/**
 * @ingroup tsch
 * @brief the count of task report queue's slot
 */
#define TS_TASK_REPORT_QUEUE_SLOTS_COUNT (1024U)

/**
* @ingroup tsch
* @brief when the number of report queue's msg node is large than the task report queue's threshold,

*        then trigger the interrupt to the host
*/
#define TS_TASK_REPORT_THRESHOLD (512U)

/**
 * @ingroup tsch
 * @brief  the duration of report timer is x milliseconds
 */
#define TS_TASK_REPORT_TIMER_DURATION (10U)

/**
 * @ingroup tsch
 * @brief there are 8 priority levels
 */
#define TS_TASK_CMD_QUEUE_PRIORITIES_LEVEL (8U)

/**
 * @ingroup tsch
 * @brief the total count of those task command queue's slot is 1024
 */
#define TS_TASK_CMD_QUEUE_SLOTS_COUNT (1024U)

/**
 * @ingroup tsch
 * @brief the size of each task command queue
 */
#define TS_SIZE_OF_PER_TASK_CMD_QUEUE (TS_TASK_CMD_QUEUE_SLOTS_COUNT / TS_TASK_CMD_QUEUE_PRIORITIES_LEVEL)

/**
 * @ingroup tsch
 * @brief the number of task commands for one prefetch from SQ queue
 */
#define ONE_SQ_PREFECH_NUM (4U)

/**
 * @ingroup tsch
 * @brief the report number of one CQ,1024 reports
 */
#define CQ_LEN (1024U)

/**
 * @ingroup tsch
 * @brief cq flow control
 */
#define FLOW_CONTROL_HIGH_LIMITS (900U)
#define FLOW_CONTROL_LOW_LIMITS (700U)
#define FLOW_CONTROL_FREE_LIMITS (CQ_LEN - FLOW_CONTROL_HIGH_LIMITS)
/**
 * @ingroup tsch
 * @brief the initial flag of BS
 */
#define BS_INITIALIZED (0x2U)

/**
 * @ingroup tsch
 * @brief aicore 10 group 16 cycle for bs9sx1a, 8 group 8cycle for pg1
 */
#define CUBE_ACTIVE_CFG_EN_VAL 1U
#define CUBE_DUMMY_START_EN_VAL 1U
#define CUBE_DUMMY_NOP_EN_VAL 1U
#define CUBE_DUMMY_NOP_CYCLE_VAL 1U
#define CUBE_DUMMY_CFG_CYCLE_VAL 1U
#define CUBE_DUMMY_NOP_CYCLE_BS9SX1A_VAL 2U
#define CUBE_DUMMY_CFG_CYCLE_BS9SX1A_VAL 2U
#define AIC_SYS_CNT_16_CYCLE 0x10U
#define AIC_SYS_CNT_1_CYCLE 0x1U
#define AIC_SYS_CNT_OFFSET 16U

#ifdef DAVINCI_MDC_LITE
#define AIC_SYS_CNT_GROUP 0x20U // 4 group * 8 cycle
#define AIC_SYS_CNT_CYCLE 0x8U  // 8 cycle
#else
#define AIC_SYS_CNT_GROUP 0xA0U // 10 group * 16 cycle
#define AIC_SYS_CNT_CYCLE 0x10U // 16 cycle
#endif                          // DAVINCI_MDC_LITE

typedef struct tag_stream_proc_info {
    uint16_t last_rcv_type;
    uint16_t last_rcv_id;
    uint16_t last_rcv_phase;
    uint16_t last_run_type;
    uint16_t last_run_id;
    uint16_t last_run_phase;
    uint16_t last_snd_type;
    uint16_t last_snd_id;
    uint16_t stream_recyle_flag;
    uint16_t sq_recyle_flag;
    uint8_t stream_phase;
    uint8_t reserved;
} stream_proc_info_t;

// define for error message
typedef struct ts_ttlv_msg {
    uint16_t tag;
    uint16_t type;
    uint16_t length;
    uint8_t value[];
} ts_ttlv_msg_t;

// define for error message
typedef struct ts_ttv_msg {
    uint16_t tag;
    uint16_t type;
    uint8_t value[];
} ts_ttv_msg_t;

/**
* @ingroup tsch
* @brief error code and exception code encoding scheme
Loc information 2 bits (31~30) ------ 0x01:device
Code category 2 bits (29~28) ------------- 0x01:error code
Error level 3 bits (27~25) --------------- 0b100:CRITICAL 0b011:MAJOR 0b010:MINOR 0b001:SUGGESTION 0b000:UNKNOWN
Module ID 8 bits (24~17) ----------------- 0x03:TS
*/
#define TS_STD_ERROR_CODE_BASIC (0x90060000U)
#define TS_STD_ERROR_LVL_CRITICAL (0X08000000U)
#define TS_STD_ERROR_LVL_MAJOR (0X06000000U)
#define TS_STD_ERROR_LVL_MINOR (0X04000000U)
#define TS_STD_ERROR_LVL_SUGGESTION (0X02000000U)
#define TS_STD_ERROR_CODE_CRITICAL(errcode) (TS_STD_ERROR_CODE_BASIC | TS_STD_ERROR_LVL_CRITICAL | (errcode))
#define TS_STD_ERROR_CODE_MAJOR(errcode) (TS_STD_ERROR_CODE_BASIC | TS_STD_ERROR_LVL_MAJOR | (errcode))
#define TS_STD_ERROR_CODE_MINOR(errcode) (TS_STD_ERROR_CODE_BASIC | TS_STD_ERROR_LVL_MINOR | (errcode))
#define TS_STD_ERROR_CODE_SUGGESTION(errcode) (TS_STD_ERROR_CODE_BASIC | TS_STD_ERROR_LVL_SUGGESTION | (errcode))
#define TS_STD_ERROR_CODE_DEFAULT(errcode) (TS_STD_ERROR_CODE_BASIC | (errcode))

// stars model exe result: 4bits, valid value is 0~15
typedef enum tag_ts_stars_model_exe_result {
    TS_STARS_MODEL_EXE_SUCCESS = 0,
    TS_STARS_MODEL_STREAM_EXE_FAILED = 1,
    TS_STARS_MODEL_END_OF_SEQ = 2,
    TS_STARS_MODEL_EXE_ABORT = 3,
    TS_STARS_MODEL_AICPU_TIMEOUT = 4,
    TS_STARS_MODEL_EXE_RES5 = 5,
    TS_STARS_MODEL_EXE_RES6 = 6,
    TS_STARS_MODEL_EXE_RES7 = 7,
    TS_STARS_MODEL_EXE_RES8 = 8,
    TS_STARS_MODEL_EXE_RES9 = 9,
    TS_STARS_MODEL_EXE_RES10 = 10,
    TS_STARS_MODEL_EXE_RES11 = 11,
    TS_STARS_MODEL_EXE_RES12 = 12,
    TS_STARS_MODEL_EXE_RES13 = 13,
    TS_STARS_MODEL_EXE_RES14 = 14,
    TS_STARS_MODEL_EXE_RES15 = 15
} ts_stars_model_exe_result;

typedef enum tag_debug_exception {
    /* BIU */
    DFX_ERR = 0x00000000ULL,
    L2_WRITE_OOB,
    L2_READ_OOB,
    /* CCU */
    DIV0_CCU = 0x10000000ULL,
    NEG_SQRT_CCU,
    ILLEGAL_INSTR,
    CALL_DEPTH_OVRFLW,
    LOOP_ERR,
    LOOP_CNT_ERR,
    DATA_EXCP_CCU,
    SBUF_ECC = 0x10000009ULL,
    CCU_INF_NAN,
    UB_ECC_CCU,
    /* IFU */
    BUS_ERR = 0X20000000ULL,
    /* MTE */
    UB_ECC_MTE = 0X30000000ULL,
    DATA_EXCP_MTE,
    DECOMP,
    COMP,
    UNZIP,
    CIDX_OVERFLOW,
    ILLEGAL_STRIDE,
    ILLEGAL_FM_SIZE,
    ILLEGAL_L1_3D_SIZE,
    BAS_RADDR_OBOUND,
    FMAP_LESS_KERNEL,
    FPOS_LARGER_FSIZE,
    F1WPOS_LARGER_FSIZE,
    FMAPWH_LARGER_L1SIZE,
    PANDDING_CFG,
    WRITE_3D_OVERFLOW,
    READ_OVERFLOW, /* 0X30000010ULL */
    WRITE_OVERFLOW,
    GDMA_READ_OVERFLOW,
    GDMA_WRITE_OVERFLOW,
    GDMA_ILLEGAL_BURST_NUM,
    GDMA_ILLEGAL_BURST_LEN,
    L1_ECC,
    TLU_ECC,
    ROB_ECC,
    BIU_RDWR_RESP,
    AIPP_ILLEGAL_PARAM,
    KTABLE_RD_ADDR_OVERFLOW,
    TIMEOUT = 0x3000001EULL,
    MTE_ILLEGAL_SCHN_CFG,
    ILLEGAL_SCHN_CFG,
    ATM_ADD,
    KTABLE_WR_ADDR_OVERFLOW,
    /* VEC */
    UB_ECC_VEC = 0X40000000ULL,
    DATA_EXCP_VEC,
    ILLEGAL_MASK,
    SAME_BLK_ADDR,
    NEG_SQRT_VEC,
    NEG_LN,
    INF_NAN,
    DIV0_VEC,
    L0C_ECC_VEC,
    UB_SELF_RDWR_CFLT,
    COL2IMG_ILLEGAL_K_SIZE,
    COL2IMG_ILLEGAL_FETCH_POS,
    COL2IMG_ILLEGAL_1ST_WIN_POS,
    COL2IMG_ILLEGAL_STRIDE,
    COL2IMG_ILLEGAL_FM_SIZE,
    COL2IMG_RD_DFM_ADDR_OVFLOW,
    COL2IMG_RD_FM_ADDR_OVFLOW, /* 0X40000010ULL */
    UB_WRAP_AROUND,
    /* CUBE */
    L0A_ECC = 0X50000001ULL,
    L0B_ECC,
    L0C_ECC_CUBE,
    L0C_SELF_RDWR_CFLT,
    INVLD_INPUT,
    L0A_WRAP_AROUND,
    L0B_WRAP_AROUND,
    L0C_WRAP_AROUND,
    /* CFLT */
    L0A_RDWR_CFLT = 0X60000000ULL,
    L0B_RDWR_CFLT,
    L0C_RDWR_CFLT,
    UB_WR_CFLT,
    UB_RD_CFLT,
    /* unknown */
    UNKNOWN_EXCEPTION = 0xF0000000ULL
} ts_dbg_except_t;
/**
 * @ingroup tsch
 * @brief the bool value definition
 */
#define TS_FALSE ((ts_bool_t)0U)
#define TS_TRUE ((ts_bool_t)1U)

#define TS_DISABLE ((ts_bool_t)0U)
#define TS_ENABLE ((ts_bool_t)1U)

#define TS_EFFECTIVE_LATER ((ts_bool_t)0U)
#define TS_EFFECTIVE_IMMEDIATELY ((ts_bool_t)1U)

#define TS_HUGE_STREAM_FLAG (0x4U)
enum tag_switch_condition { TS_EQUAL = 0, TS_NOT_EQUAL, TS_GREATER, TS_GREATER_OR_EQUAL, TS_LESS, TS_LESS_OR_EQUAL };

enum tag_switch_datatype { TS_SWITCH_INT32 = 0, TS_SWITCH_INT64 = 1 };

// defined for error message
// stream status
enum tag_ts_stream_state {
    STREAM_STATE_INIT = 0,
    STREAM_STATE_CREATE = 1,
    STREAM_STATE_BIND_MODEL = 2,
    STREAM_STATE_ADD_TASK = 3,
    STREAM_STATE_ACTIVE = 4,
    STREAM_STATE_AICPU_ACTIVE = 5,
    STREAM_STATE_SCHEDULE = 6,
    STREAM_STATE_DEACTIVE = 7, // ignore 1910, reset hwts sq
    STREAM_STATE_UNBIND_MODEL = 8,
    STREAM_STATE_DESTROY = 9,  // get maintenance task, and clean data
    STREAM_STATE_RECYCLE = 10, // send driver for recycling
    STREAM_STATE_RESERVED = 0XFF,
};

typedef enum tag_ts_task_phase {
    TASK_PHASE_INIT = 0,
    TASK_PHASE_WAIT_EXEC = 1, // task is wait to exec
    TASK_PHASE_RUN = 2,
    TASK_PHASE_COMPLETE = 3,
    TASK_PHASE_PENDING = 4,         // task process stream wait event
    TASK_PHASE_AICPU_TASK_WAIT = 5, // aicpu second phase
    TASK_PHASE_TASK_PROCESS_MEMCPY = 6,
    TASK_PHASE_AICORE_DONE = 7,
    TASK_PHASE_AIV_DONE = 8,
    TASK_PHASE_AICPU_TIMEOUT_PROC = 9,
    TASK_PHASE_RESERVED = 0XFF,
} ts_task_phase_t;

enum ts_error_type {
    TS_ERROR_TYPE_SYSTEM,
    TS_ERROR_TYPE_TASK,
    TS_ERROR_TYPE_STREAM,
    TS_ERROR_TYPE_MODEL,
    TS_ERROR_TYPE_PID,
    // can not modify
    TS_ERROR_TYPE_RESERVED = 0xff
};

// define for error msg encode&decode
enum ts_err_msg_data_type {
    TS_ERR_MSG_UINT8 = 0,
    TS_ERR_MSG_INT8 = 1,
    TS_ERR_MSG_UINT16 = 2,
    TS_ERR_MSG_INT16 = 3,
    TS_ERR_MSG_UINT32 = 4,
    TS_ERR_MSG_INT32 = 5,
    TS_ERR_MSG_UINT64 = 6,
    TS_ERR_MSG_INT64 = 7,
    TS_ERR_MSG_BASIC_TYPE_MAX_ID = 0x7E,
    TS_ERR_MSG_STRUCT = 0x7F,
    TS_ERR_MSG_UINT8_ARRAY = 0x80, // set high bit means array
    TS_ERR_MSG_CHAR_ARRAY = 0x81,
    TS_ERR_MSG_UINT16_ARRAY = 0x82,
    TS_ERR_MSG_INT16_ARRAY = 0x83,
    TS_ERR_MSG_UINT32_ARRAY = 0x84,
    TS_ERR_MSG_INT32_ARRAY = 0x85,
    TS_ERR_MSG_UINT64_ARRAY = 0x86,
    TS_ERR_MSG_INT64_ARRAY = 0x87,
    // can not modify
    TS_ERR_MSG_DATA_TYPE_RESERVED,
};

enum ts_err_msg_code_tag {
    // error message basic information
    TAG_TS_ERR_MSG_FUNC_CODE = 0,
    TAG_TS_ERR_MSG_LINE_CODE = 1,
    TAG_TS_ERR_MSG_TS_ERR_CODE = 2,
    // define for ts_err_code_msg_t
    TAG_TS_ERR_MSG_CODE_STRUCT = 3, // ts_err_code_t
    TAG_TS_ERR_MSG_TIMESTAMP_SEC = 4,
    TAG_TS_ERR_MSG_TIMESTAMP_USEC = 5,
    TAG_TS_ERR_MSG_CURRENT_TIME = 6,
    TAG_TS_ERR_MSG_STRING = 7,
    TAG_TS_ERR_MSG_ERROR_CODE = 8,
    TAG_TS_ERR_MSG_MAX_ID = 0x100,

    // other extension information
    TAG_TS_ERR_MSG_TASK_ID = 0x101,
    TAG_TS_ERR_MSG_TASK_TYPE = 0x102,
    TAG_TS_ERR_MSG_TASK_PHASE = 0x103,
    TAG_TS_ERR_MSG_LAST_RECEIVE_TASK_ID = 0x104,
    TAG_TS_ERR_MSG_LAST_SEND_TASK_ID = 0x105,
    TAG_TS_ERR_MSG_STREAM_ID = 0x106,
    TAG_TS_ERR_MSG_STREAM_PHASE = 0x107,
    TAG_TS_ERR_MSG_MODEL_ID = 0x108,
    TAG_TS_ERR_MSG_PID = 0x109,

    // if tag > TAG_TS_ERR_MSG_FIRST_SENTENCE_ID, means add a new sentence
    TAG_TS_ERR_MSG_FIRST_SENTENCE_ID = 0xF000,
    // define for task_track_msg_t
    TAG_TS_ERR_MSG_TASK_TRACK_MSG = 0xF001, // highest bit means sentence
    // define for stream_control_msg_t
    TAG_TS_ERR_MSG_STREAM_DESC_MSG = 0xF002,
    // define for model_desc_msg_t
    TAG_TS_ERR_MSG_MODEL_DESC_MSG = 0xF003,
    // define for pid_desc_msg_t
    TAG_TS_ERR_MSG_PID_DESC_MSG = 0xF004,
    // define for ts_sys_err_desc_msg_t
    TAG_TS_ERR_MSG_SYS_MSG = 0xF005,
    TAG_TS_ERR_MSG_UNKNOWN_SENTENCE = 0xF006,
    // can not modify
    TAG_TS_ERR_MSG_CODE_RESERVED = 0xFFFF,
};

typedef uint8_t ts_bool_t;

enum RECOVER_ABORT_STAUTS {
    RECOVER_INIT = 0X0U,
    RECOVER_SUCC,
    RECOVER_FAIL,
    RECOVER_STATUS_INVALID,
};

enum ts_app_abort_status {
    APP_ABORT_TERMINATE_FAIL = 0x0U,
    APP_ABORT_INIT,
    APP_ABORT_KILL_FINISH,
    APP_ABORT_TERMINATE_FINISH,
    APP_ABORT_STATUS_INVALID,
};

enum ts_app_abort_sts_query_choice {
    APP_ABORT_STS_QUERY_BY_SQ = 0x0U,
    APP_ABORT_STS_QUERY_BY_PID,
    APP_ABORT_STS_QUERY_BY_MODELID,
    APP_ABORT_STS_QUERY_INVALID,
};

enum DAVID_ABORT_STAUTS {
    DAVID_ABORT_INIT = 0x1U,
    DAVID_ABORT_KILL_FINISH,
    DAVID_ABORT_TERMINATE_FAIL,
    DAVID_ABORT_STOP_FINISH,
    DAVID_ABORT_SUBACC_TERMINATE_FINISH,
    DAVID_ABORT_TERMINATE_SUCC,
    DAVID_ABORT_MODIFY_SWAPBUFFER_INIT,
    DAVID_ABORT_MODIFY_SWAPBUFFER_KILL_FINISH,
    DAVID_ABORT_MODIFY_SWAPBUFFER_FINISH,
    DAVID_ABORT_STATUS_INVALID,
};

enum RECOVER_STS_QUERY_CHOICE {
    RECOVER_STS_QUERY_BY_SQID = 0x0U,
    RECOVER_STS_QUERY_BY_PID,
    RECOVER_STS_QUERY_BY_MODELID,
    RECOVER_STS_QUERY_INVALID,
};

enum OPERATION_TYPE {
    OP_ABORT_APP = 0x0U,
    OP_QUERY_ABORT_STATUS,
    OP_ABORT_STREAM,
    OP_QUERY_STREAM_ABORT_STATUS,
    OP_QUERY_DCACHE_LOCK_STATUS,
    OP_QUERY_STARS_REG_BASE_ADDR,
    OP_QUERY_TSFW_VERSION,
    OP_UPDATE_SIMT_STACK_INFO,
    OP_QUERY_SIMT_STACK_INFO,
    OP_ABORT_MODEL,
    OP_RECOVER_STREAM,
    OP_RECOVER_APP,
    OP_QUERY_RECOVER_STATUS,
    OP_INVALID
};
#pragma pack(push)
#pragma pack(1)
typedef struct {
    volatile uint32_t sq_id;
    volatile uint8_t resv[36];
} ts_kill_task_info_t;

typedef struct {
    volatile uint32_t choice; // APP_ABORT_STS_QUERY_CHOICE
    volatile uint32_t sq_id;
    volatile uint8_t resv[32];
} ts_query_task_info_t;

typedef struct {
    volatile uint32_t stack_phy_base_h;
    volatile uint32_t stack_phy_base_l;
    volatile uint8_t resv[32];
} ts_query_dcache_lock_info_t;

typedef struct {
    volatile uint32_t status;
    volatile uint32_t stack_phy_base_h;
    volatile uint32_t stack_phy_base_l;
    volatile uint8_t resv[28];
} ts_query_dcache_lock_ack_info_t;

typedef struct {
    volatile uint32_t status; // APP_ABORT_STAUTS
    volatile uint8_t resv[36];
} ts_query_task_ack_info_t;

typedef struct {
    volatile uint64_t reg_base_addr;
    volatile uint32_t resv[8];
} ts_query_stars_ack_info_t;

typedef struct {
    volatile uint32_t tsfw_version;
    volatile uint32_t resv[9];
} ts_query_tsfw_ack_info_t;

typedef struct {
    volatile uint64_t updated_simt_stack_addr;
    volatile uint32_t sq_id;
    volatile uint32_t result;
    volatile uint32_t simt_warp_stack_size;
    volatile uint32_t simt_dvg_warp_stack_size;
    volatile uint32_t resv[4];
} ts_update_stack_info_t;

typedef struct {
    volatile uint32_t sq_id;
    volatile uint32_t status;
    volatile uint32_t resv[8];
} ts_query_stack_info_t;

typedef struct {
    volatile uint32_t result;
    volatile uint32_t resv[9];
} ts_kill_app_info_t;

typedef struct {
    volatile uint32_t sq_id;
    volatile uint32_t result;
    volatile uint32_t resv[8];
} ts_kill_stream_info_t;

typedef struct {
    volatile uint32_t model_id;
    volatile uint32_t result;
    volatile uint32_t resv[8];
} ts_kill_model_info_t;

typedef struct {
    volatile uint32_t choice;
    volatile uint32_t target_id;
    volatile uint32_t resv[8];
} ts_query_abort_info_t;

typedef struct {
    volatile uint32_t status; // ABORT_STAUTS
    volatile uint32_t resv[9];
} ts_query_abort_ack_info_t;

typedef struct {
    volatile uint32_t sq_id;
    volatile uint32_t result;
    volatile uint32_t resv[8];
} ts_recover_stream_info_t;

typedef struct {
    volatile uint32_t result;
    volatile uint32_t resv[9];
} ts_recover_app_info_t;

typedef struct {
    volatile uint32_t app_pid;
    volatile uint8_t app_flag;
    volatile uint8_t vf_id;
    volatile uint8_t len;
    volatile uint8_t reserv;
    volatile uint32_t rev;
} ts_ctrl_msg_head_t;

typedef struct {
    volatile uint32_t type;
    union {
        ts_kill_task_info_t kill_task_info;
        ts_kill_stream_info_t kill_stream_info;
        ts_query_task_info_t query_task_info;
        ts_query_task_ack_info_t query_task_ack_info;
        ts_kill_app_info_t kill_app_info;     // device abort
        ts_kill_model_info_t kill_model_info; // model abort
        ts_recover_app_info_t recover_app_info;
        ts_recover_stream_info_t recover_stream_info;
        ts_query_abort_info_t query_abort_info;
        ts_query_abort_ack_info_t query_abort_ack_info;
        ts_query_stars_ack_info_t query_ack_info;
        ts_query_tsfw_ack_info_t query_tsfw_info;
        ts_update_stack_info_t update_stack_info;
        ts_query_stack_info_t query_stack_info;
        ts_query_dcache_lock_info_t query_dcache_lock_info;
        ts_query_dcache_lock_ack_info_t query_dcache_lock_ack_info;
    } u;              // 40 bytes
} ts_ctrl_msg_body_t; // 44 bytes
#pragma pack(pop)
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TS_TSCH_DEFINES_H */
