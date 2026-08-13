/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef TS_TASK_STRUCT_H
#define TS_TASK_STRUCT_H
#include "tsch_defines.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup tsch
 * @brief the type definition of task
 */
typedef enum tag_ts_task_type {
    TS_TASK_TYPE_KERNEL_AICORE = 0,     /**< AI core task */
    TS_TASK_TYPE_KERNEL_AICPU = 1,      /**< AI cpu task */
    TS_TASK_TYPE_EVENT_RECORD = 2,      /**< event record task */
    TS_TASK_TYPE_STREAM_WAIT_EVENT = 3, /**< stream wait event  task */
    TS_TASK_TYPE_FUSION_ISSUE = 4,      /**< fusion issue task */
    TS_TASK_TYPE_MEMCPY = 5,            /**< memory copy task */
    TS_TASK_TYPE_MAINTENANCE = 6,       /**< such as destroy the event or stream */
    TS_TASK_TYPE_CREATE_STREAM = 7,     /**< create stream task */
    TS_TASK_TYPE_DATA_DUMP = 8,         /**< kernel data dump configure */
    TS_TASK_TYPE_REMOTE_EVENT_WAIT = 9, /* * wait for event on another device */
    TS_TASK_TYPE_PCTRACE_ENABLE = 10,
    TS_TASK_TYPE_CREATE_L2_ADDR = 11, /**< create L2 addr info for aicpu kernel */
    TS_TASK_TYPE_MODEL_MAINTAINCE = 12,
    TS_TASK_TYPE_MODEL_EXECUTE = 13,
    TS_TASK_TYPE_NOTIFY_WAIT = 14,
    TS_TASK_TYPE_NOTIFY_RECORD = 15,
    TS_TASK_TYPE_RDMA_SEND = 16,           /**< hccl rdma send task */
    TS_TASK_TYPE_L2_SDMA_TASK_MEMCPY = 17, /**< test l2 task memory copy task */
    TS_TASK_TYPE_STREAM_SWITCH = 18,
    TS_TASK_TYPE_STREAM_ACTIVE = 19,
    TS_TASK_TYPE_LABEL_SET = 20,                /**< set label for control flow ops */
    TS_TASK_TYPE_LABEL_SWITCH = 21,             /**< switch label for control flow ops */
    TS_TASK_TYPE_LABEL_GOTO = 22,               /**< goto label for control flow ops */
    TS_TASK_TYPE_PROFILER_TRACE = 23,           /**< goto label for profiler trace */
    TS_TASK_TYPE_EVENT_RESET = 24,              /**< event reset task */
    TS_TASK_TYPE_RDMA_DB_SEND = 25,             /**< HCCL rdma db cpy task*/
    TS_TASK_TYPE_PROFILER_TRACE_EX = 26,        /**< profiler trace task*/
    TS_TASK_TYPE_PROFILER_DYNAMIC_ENABLE = 27,  /* profiler dynamic task */
    TS_TASK_TYPE_PROFILER_DYNAMIC_DISABLE = 28, /* profiler dynamic task */
    TS_TASK_TYPE_ALLOC_DSA_ADDR = 29,           /* alloc dsa addr task */
    TS_TASK_TYPE_CCU_LAUNCH = 30,               /* HCCL CCU Launch task */
    TS_TASK_TYPE_PROFILING_ENABLE = 64,
    TS_TASK_TYPE_PROFILING_DISABLE = 65,
    TS_TASK_TYPE_KERNEL_AIVEC = 66,        /**< AI vector task */
    TS_TASK_TYPE_MODEL_END_GRAPH = 67,     /**< add model end graph task */
    TS_TASK_TYPE_MODEL_TO_AICPU = 68,      /**< AICPU schedule task */
    TS_TASK_TYPE_ACTIVE_AICPU_STREAM = 69, /**< active stream task */
    TS_TASK_TYPE_DATADUMP_LOADINFO = 70,   /**< load data dump info task */
    TS_TASK_TYPE_STREAM_SWITCH_N = 71,
    TS_TASK_TYPE_HOSTFUNC_CALLBACK = 72,            /**<  Host func Callback task */
    TS_TASK_TYPE_ONLINEPROF_START = 73,             /**< start online profiling task */
    TS_TASK_TYPE_ONLINEPROF_STOP = 74,              /**< stop online profiling task */
    TS_TASK_TYPE_STREAM_LABEL_SWITCH_BY_INDEX = 75, /**< index switch stream label for control flow ops */
    TS_TASK_TYPE_STREAM_LABEL_GOTO = 77,            /**< goto stream label for control flow ops */
    TS_TASK_TYPE_DEBUG_REGISTER = 78,               /* kernel exception overflow debug register */
    TS_TASK_TYPE_DEBUG_UNREGISTER = 79,             /* kernel exception overflow debug unregister */
    TS_TASK_TYPE_MODEL_EXIT = 81,                   /**< add model exit task */
    TS_TASK_TYPE_MDCPROF = 82,                      /**< add mdc prof task */
    TS_TASK_TYPE_DEVICE_RINGBUFFER_CONTROL = 83,
    TS_TASK_TYPE_DEBUG_REGISTER_WITH_STREAM = 84,   /* kernel exception overflow debug register with stream */
    TS_TASK_TYPE_DEBUG_UNREGISTER_WITH_STREAM = 85, /* kernel exception overflow debug unregister with stream */
    TS_TASK_TYPE_TASK_TIMEOUT_SET = 86,
    TS_TASK_TYPE_GET_DEVICE_MSG = 87,            /* Get device message */
    TS_TASK_TYPE_GET_FLOAT_STATUS = 88,          /* NPUGetFloatStatus */
    TS_TASK_TYPE_CLEAR_FLOAT_STATUS = 89,        /* NPUClearFloatStatus */
    TS_TASK_TYPE_MEMCPY_ASYNC_WITHOUT_SDMA = 90, /* mem cpy async by place holder sqe */
    TS_TASK_TYPE_SET_OVERFLOW_SWITCH = 91,       /* Set overflow switch flag */
    TS_TASK_TYPE_REDUCE_ASYNC_V2 = 92,           /* reduce async v2 */
    TS_TASK_TYPE_SET_STREAM_GE_OP_TAG = 93,      /* Set stream geOpTag */
    TS_TASK_TYPE_SET_STREAM_MODE = 94,           /* set stream mode */
    TS_TASK_TYPE_IPCINT_NOTICE = 95,             /* ipc interrupt D2H notice */
    TS_TASK_TYPE_MODEL_LOAD = 96,                /* ModelLoad task for lhisi */
    TS_TASK_TYPE_GET_STARS_VERSION = 97,
    TS_TASK_TYPE_TASK_FLIP = 98,          /* flip task interrupt */
    TS_TASK_TYPE_MODEL_TASK_UPDATE = 101, /* model task update */
    TS_TASK_TYPE_AICPU_INFO_LOAD = 102,
    TS_TASK_TYPE_NOP = 103,
    TS_TASK_TYPE_COMMON_CMD = 104,
    TS_TASK_TYPE_UB_DB_SEND = 105,    /* HCCL ub db cpy task */
    TS_TASK_TYPE_DIRECT_SEND = 106,   /* HCCL direct db cpy task */
    TS_TASK_TYPE_KERNEL_FUSION = 107, /* fusion kernel task */
    TS_TASK_TYPE_RESERVED,
} ts_task_type_t;

typedef enum tag_ts_task_state {
    TASK_STATE_INIT = 0,
    TASK_STATE_WAIT = 1,
    TASK_STATE_RUN = 2,
    TASK_STATE_COMPLETE = 3,
    TASK_STATE_PENDING = 4,
    TASK_STATE_SDMA_PROCESS_FAILED = 5,
    TASK_STATE_SDMA_PROCESS_SUCCESS = 6,
    TASK_STATE_AICORE_PROCESS_START = 7,
    TASK_STATE_AICORE_DONE = 8,
    TASK_STATE_RUN_PHASE2 = 9,
} ts_task_state_t;

typedef enum tag_ts_report_type {
    TS_REPORT_TYPE_TASK = 0,                    /** task command report */
    TS_REPORT_TYPE_ERROR_REPORT = 1,            /** used for error report */
    TS_REPORT_TYPE_RECYCLE_SQ_FINISHED = 3,     /* recycle sq report */
    TS_REPORT_TYPE_RECYCLE_STREAM_FINISHED = 4, /* recycle streamid report */
    TS_REPORT_TYPE_RECYCLE_NOTIFY_FINISHED = 5, /* recycle notifyid report */
    TS_REPORT_TYPE_RECYCLE_EVENT_FINISHED = 6,  /* recycle eventid report */
    TS_REPORT_TYPE_RECYCLE_CQ_FINISHED = 7,     /* recycle eventid report */
    TS_REPORT_TYPE_UPDATE_SQ_HEAD = 8,          /* notify driver update sq head */
} ts_report_type_t;

typedef enum tag_ts_conds_sub_type {
    CONDS_SUB_TYPE_MODEL_EXEC = 1,
    CONDS_SUB_TYPE_RDMA_1 = 2,
    CONDS_SUB_TYPE_RDMA_2 = 3,
    CONDS_SUB_TYPE_STREAM_SWITCH = 4,
    CONDS_SUB_TYPE_STREAM_SWITCH_EX = 5,
    CONDS_SUB_TYPE_STREAM_ACTIVE = 6,
    CONDS_SUB_TYPE_LABEL_SWITCH_BY_INDEX = 7,
    CONDS_SUB_TYPE_GET_FLOAT_STATUS = 8,
    CONDS_SUB_TYPE_MAX = 9,
} ts_conds_sub_type_t;

typedef enum tag_ts_common_cmd {
    CMD_STREAM_CLEAR = 0,
    CMD_NOTIFY_RESET = 1,
} ts_ph_common_cmd_t;

/**
 * @ingroup tsch
 * @brief the struct define of report msg when task is completed
 */
typedef struct tag_ts_task_report_msg {
    volatile uint16_t SOP : 1; /* start of packet, indicates this is the first 32bit return payload */
    volatile uint16_t MOP : 1; /* middle of packet, indicates the payload is a continuation of previous task return
                                  payload */
    volatile uint16_t EOP : 1; /* end of packet, indicates this is the last 32bit return payload.
                               SOP & EOP can appear in the same packet, MOP & EOP can also appear on the same packet. */
    volatile uint16_t report_type : 3;
    volatile uint16_t streamID : 10;
    volatile uint16_t taskID;
    volatile uint32_t pay_load; /* error code<bit 0--11>; streamid<bit 12--21>; taskid low<bit 22--31>+(reserved) */
    volatile uint16_t SQ_id : 9;
    volatile uint16_t reserved : 6; /* taskid high> */
    volatile uint16_t phase : 1;
    volatile uint16_t SQ_head : 14;
    volatile uint16_t stream_id_ex : 1;       /* streamID high bit */
    volatile uint16_t fault_stream_id_ex : 1; /* fault streamID high bit */
} ts_task_report_msg_t;

typedef struct tag_ts_driver_msg {
    volatile uint16_t phase : 1;
    volatile uint16_t report_type : 15;
    volatile uint16_t sq_id;
    volatile uint16_t sq_head;
    volatile uint16_t recycle_id;
    volatile uint64_t reserved;
} ts_driver_msg_t;

typedef struct tag_ts_logic_cq_report_msg {
    volatile uint16_t phase : 1;
    volatile uint16_t SOP : 1; /* start of packet, indicates this is the first 32bit return payload */
    volatile uint16_t MOP : 1; /* middle of packet, indicates the payload is a continuation of previous task
                               return payload */
    volatile uint16_t EOP : 1; /* end of packet, indicates this is the last 32bit return payload. SOP & EOP
                               can appear in the same packet, MOP & EOP can also appear on the same packet. */
    volatile uint16_t logic_cq_id : 12;
    volatile uint16_t stream_id;
    volatile uint16_t task_id;
    volatile uint8_t error_type;
    volatile uint8_t need_sorting; /* drv need sorting cqe data to user thread */
    volatile uint32_t error_code;
    volatile uint32_t pay_load;
} ts_logic_cq_report_msg_t;

/**
 * @ingroup tsch
 * @brief the struct define of callback report msg
 */

typedef struct tag_ts_host_func_cq_report_msg {
    volatile uint16_t phase : 1;
    volatile uint16_t SOP : 1; /* start of packet, indicates this is the first 32bit return payload */
    volatile uint16_t MOP : 1; /* middle of packet, indicates the payload is a continuation of previous task
                               return payload */
    volatile uint16_t EOP : 1; /* end of packet, indicates this is the last 32bit return payload. SOP & EOP
                               can appear in the same packet, MOP & EOP can also appear on the same packet. */
    volatile uint16_t cq_id : 12;
    volatile uint16_t stream_id;
    volatile uint16_t task_id;
    volatile uint16_t sq_id;
    volatile uint16_t sq_head;
    volatile uint16_t sequence_id; /* for match */
    volatile uint8_t is_block;
    volatile uint8_t reserved1[2];
    volatile uint8_t cqe_type;
    volatile uint64_t host_func_cb_ptr;
    volatile uint64_t fn_data_ptr;
} ts_host_func_cq_report_msg_t;

#define IPCINT_MSGLEN_MAX (0x8U)

#pragma pack(1) // single-byte alignment
struct ts_cb_task {
    uint16_t cq_id;
    uint8_t is_block;
    uint8_t reserved1[3];
    uint64_t host_func_cb_ptr;
    uint64_t fn_data_ptr;
};

struct ts_ipcint_notice_task {
    uint32_t ntc_pid;
    uint16_t ntc_grpId;
    uint16_t ntc_tid;
    uint16_t ntc_subEventId;
    uint8_t ntc_eventId;
    uint8_t msg_len;
    uint8_t msg[IPCINT_MSGLEN_MAX];
    uint16_t phy_devId;
};

typedef struct tag_ts_callback_report_msg {
    uint8_t phase : 1;
    uint8_t reserved0 : 7;
    uint8_t task_type; /* 0:cb task; 1:ipcint notice task */
    uint16_t sq_id;
    uint16_t sq_head;
    uint16_t stream_id;
    uint16_t task_id;
    union {
        struct ts_cb_task cb_task;
        struct ts_ipcint_notice_task ipcint_notice_task;
    } u;
} ts_callback_report_msg_t;
#pragma pack() // Cancels single-byte alignment

typedef struct tag_ts_host_func_sq_send_msg {
    uint16_t phase : 1;
    uint16_t SOP : 1; /* start of packet, indicates this is the first 32bit return payload */
    uint16_t MOP : 1; /* middle of packet, indicates the payload is a continuation of previous task return payload */
    uint16_t EOP : 1; /* end of packet, indicates this is the last 32bit return payload.
                     SOP & EOP can appear in the same packet, MOP & EOP can also appear on the same packet. */
    uint16_t reserved : 12;
    uint16_t stream_id;
    uint16_t task_id;
    uint16_t cq_id;
    uint16_t cq_tail;
    uint16_t sequence_id;
    uint32_t reserved1[13];
} ts_host_func_sq_send_msg_t;

/**
 * @ingroup tsch
 * @brief the struct define of kernel type task
 */
typedef struct tag_ts_kernel_task {
    uint64_t PC_start;
    uint64_t param_base;
    uint64_t l2_preload_ctrl;
    uint64_t literal_src_addr;
    uint32_t literal_dst_base;
    uint32_t literal_size;
    uint16_t num_blocks;
    uint8_t L2_size;
    uint8_t l2_in_main;
    uint32_t priority : 3;
    uint32_t l2PreloadVirAddr : 26; // preserve the offset of l2_preload_ctrl's phy addr, not greater than 50M now
    uint32_t flag : 3;              // 1:TS_KERNEL_CONVERT, 2:TS_KERNEL_DUMPFLAG, 4:FUSION_KERNEL_DUMPFLAG
} ts_kernel_task_t;

/**
 * @ingroup tsch
 * @brief the struct define of event record type task
 */
typedef struct tag_ts_event_record_task {
    uint16_t eventID;
    uint8_t reserved0[6];
    uint64_t timeline_base;
    uint32_t offset;
    uint32_t thread_id;
    uint32_t vir_addr;
    uint8_t flag;
    uint8_t wait_cq_flag;
    uint16_t wait_cq_id;
    uint8_t reserved[16];
} ts_event_record_task_t;

/**
 * @ingroup tsch
 * @brief the struct define of event reset type task
 */
typedef struct tag_ts_event_reset_task {
    uint16_t eventID;     /**< offset 8 */
    uint16_t isNotify;    /**< event to notify */
    uint8_t reserved[44]; // reserved 44 bytes
} ts_event_reset_task_t;
/**
 * @ingroup tsch
 * @brief the struct define of stream wait event type task
 */
typedef struct tag_ts_stream_wait_event_task {
    uint16_t eventID;         /**< offset 8 */
    uint16_t nextStreamIdx;   /**< offset 10 */
    uint16_t isNotify;        /**< event to notify */
    uint16_t ret_code;        // using ts_error_t, only use by ts
    uint16_t fault_task_id;   // using report error of operator, only use by ts
    uint16_t fault_stream_id; // using report error of operator, only use by ts
    uint32_t wait_timeout;    /* used for 1910p */
    uint8_t reserved[28];     /**< offset 16, reserved 40 bytes */
    uint16_t task_id;
    int16_t timewheel_slot; /**< used only by ts */
} ts_stream_wait_event_task_t;

/**
 * @ingroup tsch
 * @brief the struct define of notify record type task
 */
typedef struct tag_ts_notify_record_task {
    uint16_t soc_id;
    uint16_t notify_id;
    uint8_t reserved[44];
} ts_notify_record_task_t;

/**
 * @ingroup tsch
 * @brief the struct define of ipc interrupt notice type task
 */
typedef struct tag_ts_ipc_int_notice_task {
    uint32_t ntc_pid;
    uint16_t ntc_grpId;
    uint16_t ntc_tid;
    uint16_t ntc_subEventId;
    uint8_t ntc_eventId;
    uint8_t msg_len;
    uint8_t msg[IPCINT_MSGLEN_MAX];
    uint16_t phy_devId;
    uint8_t reserved[26];
} ts_ipc_int_notice_task_t;

/**
 * @ingroup tsch
 * @brief the struct define of notify wait type task
 */
typedef struct tag_ts_notify_wait_task {
    uint16_t notify_id;
    uint16_t reserved_field;
    uint32_t time_out;
    uint16_t task_id;
    uint8_t reserved[38];
} ts_notify_wait_task_t;
/**
 * @ingroup tsch
 * @brief the struct define of fusion type task
 */
typedef struct tag_ts_fusion_task {
    uint16_t flag; /**< offset 8 */
    uint8_t reserved[46];
} ts_fusion_task_t;

enum tag_ts_fusion_flag {
    TS_FUSION_BEGIN = 0,
    TS_FUSION_END = 1,
};

#define TS_KERNEL_CONVERT (0x01U)
#define TS_KERNEL_DUMPFLAG (0x02U)
#define TS_ENDGRAPH_DUMPFLAG (0x02U)
#define TS_ENDGRAPH_INFOFLAG (0x04U)
#define TS_TASK_UNSINKFLAG (0x08U)
#define TS_TASK_OVERFLOW_DUMP_FLAG_OFFSET 0x04U
#define TS_TASK_INVALID_FLAG 0x20U /* bit 5: The task is invalid. */
#define TS_TASK_FAST_CQ_FLAG 0x40U /* bit 6: fast cq flag */

/**
 * @ingroup tsch
 * @brief the struct define of memory copy type task
 */
enum rtMemcpyAddrConvertType { TS_NOT_NEED_CONVERT, TS_DMA_ADDR_CONVERT, TS_NO_DMA_ADDR_CONVERT };

struct TS_DMA_OFFSET_ADDR {
    uint64_t offset;
    uint32_t devid;
};

struct TS_NO_DMA_OFFSET_ADDR {
    uint32_t srcVirAddr;
    uint32_t dstVirAddr;
    uint32_t dstOffsetLow; // only TS used for save d2dAddrCpy dstOffset
};

struct TS_D2D_ADDR_OFFSET {
    uint32_t srcOffsetLow;
    uint32_t dstOffsetLow;
    uint16_t srcOffsetHigh;
    uint16_t dstOffsetHigh;
};

typedef struct tag_ts_memcpy_task {
    uint64_t src_base_addr;
    uint64_t dst_base_addr;
    uint64_t length;
    uint16_t memcpy_type;
    uint8_t dir;
    uint8_t isAddrConvert;
    uint8_t copy_data_type;
    uint8_t d2d_offset_flag : 1;
    uint8_t reserved2 : 7;
    int16_t timewheel_slot; /**< used only by ts */
    union {
        struct TS_DMA_OFFSET_ADDR dmaOffsetAddr;      // call MemConvertAddr
        struct TS_NO_DMA_OFFSET_ADDR noDmaOffsetAddr; // call MemAddressTranslate
        struct TS_D2D_ADDR_OFFSET d2dAddrOffset;
    };
} ts_memcpy_task_t;

typedef struct tag_ts_reduce_async_v2_task {
    uint64_t src_base_addr;
    uint64_t dst_base_addr;
    uint64_t overflow_addr;
    uint32_t length;
    uint16_t memcpy_type;
    uint8_t dir;
    uint8_t is_addr_convert;
    uint32_t vir_overflow_addr;
    uint8_t copy_data_type;
    uint8_t reserved[8];
    uint8_t isOverFlowReport : 1; /** mark need to report overflow, used only by ts */
    uint8_t reserved1 : 7;
    int16_t timewheel_slot; /**< used only by ts */
} ts_reduce_async_v2_task_t;

/**
 * @ingroup tsch
 * @brief the struct define of maintenance type task
 */
typedef struct tag_ts_maintenance_task {
    uint16_t goal;     /**< offset 8, 0:stream, 1:event; */
    uint16_t targetID; /**< offset 10 */
    uint8_t terminal;
    uint8_t reserved0;
    uint16_t wait_cq_id;
    uint32_t thread_id;
    uint8_t force_recycle;
    uint8_t reserved[33]; // reserve 33 bytes
    uint16_t v_target_id; // use by vm mode, runtime not use it.
} ts_maintenance_task_t;

#define MAINTENANCE_GOAL_STREAM (0U)

#define MAINTENANCE_GOAL_EVENT (1U)

#define MAINTENANCE_GOAL_STREAM_TASK_RECYCLE (2U)
#define MAINTENANCE_FORCE_RECYCLE_STREAM (0x67) // magicword to avoid dirty data from other module
typedef struct tag_ts_create_stream {
    uint64_t pid;
    uint64_t l2_base_vaddr;
    uint64_t asid_baddr;
    uint16_t vf_id;
    uint16_t v_stream_id;
    uint16_t runtime_version;
    uint16_t reserved0; // split tcr to vf_id/v_stream_id/runtime_version/reserved0,reserved0 used to be extend.
    uint32_t thread;
    uint16_t asid;
    uint16_t SMMU_sub_streamID;
    uint16_t SQ_id;
    uint8_t priority;
    uint8_t stream_attr;
    uint8_t group_id;
    uint8_t device_id; // runtime: the size is
    uint8_t support_log_to_host;
    uint8_t reserved;
} ts_create_stream_t;

typedef struct tag_ts_create_l2_addr {
    uint64_t l2_base_vaddr_for_sdma; /**< statis page table virtual address for sdma */
    uint64_t pte_pa;                 /**< page_4k_table_base[16] physical address */
    uint64_t pid;                    /* *profiling for process CreateStream and L2 mistiming */
    uint32_t virAddr;
    uint8_t reserved[20];
} ts_create_l2_addr_t;

typedef struct tag_ts_profile_enable {
    uint64_t pid;
    uint8_t pmu_event_id[8]; // should use AI_CORE_MAX_PMU_NUM, but it's bad to include profiling_agent.h
    uint64_t global_start_count_cycle;
    uint64_t global_stop_count_cycle;
    uint8_t user_defined_profiling_enable;
    uint8_t is_timeline_prof_en;
    uint8_t is_taskbased_prof_en;
    uint8_t is_prof_log_en;
    uint8_t is_hwts_log_en;
    uint8_t reserved[11];
} ts_profile_enable_t;

typedef struct tag_ts_profile_disable {
    uint64_t pid;
    uint8_t is_timeline_prof_dis;
    uint8_t is_taskbased_prof_dis;
    uint8_t is_prof_log_dis;
    uint8_t is_hwts_log_dis;
    uint8_t reserved[36];
} ts_profile_disable_t;

typedef struct tag_ts_onlineprof_start {
    uint64_t online_prof_addr;
    uint32_t virAddr;
    uint8_t reserved[36]; // reserved 36 bytes
} ts_onlineprof_start_t;

typedef struct tag_ts_onlinerof_stop {
    uint64_t online_prof_addr;
    uint8_t reserved[40]; // reserved 40 bytes
} ts_onlineprof_stop_t;

typedef struct tag_ts_mdcprof {
    uint64_t mdc_prof_addr;
    uint32_t length;
    uint8_t reserved[36]; // reserved 36 bytes
} ts_mdcprof_t;

/* *BEGIN    ADD HCCL multi_device feature 2018-01-24    ******** */
typedef struct tag_ts_remote_event_wait_task {
    uint64_t src_mailbox_pa;  /* offset 0 */
    uint64_t src_doorbell_pa; /* offset 8 */
    uint64_t dst_doorbell_pa; /* offset 16 */
    uint16_t src_event_id;    /* offset 24 */
    uint16_t src_device_id;   /* offset 26 */
    uint16_t dst_device_id;   /* offset 28 */
    uint8_t channel_type;     /* offset 30 */
    uint8_t reserved[17];     /* offset 31 */
} ts_remote_event_wait_t;     /* *END      ADD HCCL multi_device feature 2018-01-24    ******** */

typedef struct tag_tsPCTrace_task {
    uint64_t trace_buf_addr;
    uint16_t taskID;
    uint16_t coreDim;
    uint32_t virAddr;
    uint8_t reserved[32];
} tsPCTrace_task_t;

typedef struct tag_ts_data_dump_task {
    uint64_t dumpBaseAddr;
    uint32_t dumpKind;
    uint32_t dumpBlockSize;
    uint32_t dumpNumBlocks;
    uint16_t dumptaskID;
    uint8_t reserved[22];
    uint32_t virAddr;
} ts_data_dump_task_t;

typedef struct tag_ts_model_maintaince_task {
    uint16_t model_id;
    uint16_t stream_id;
    uint16_t operation_type;
    uint16_t stream_type;
    uint16_t first_task_id;
    uint16_t endgraph_id;
    uint32_t executor_flag;
    uint64_t exec_times_offset;
    uint8_t reserved[22];
    uint16_t v_model_id; // use by vm mode, runtime not use it.
} ts_model_maintaince_task_t;

typedef struct tag_ts_maintaince_task {
    uint8_t sub_type; // force recycle
    uint8_t rsv;
    uint16_t target_id;
    uint8_t reserved[44];
} ts_maintaince_task_t;

typedef struct tag_ts_dynamic_profile_enable {
    uint32_t pid;
    uint8_t eventMuxConfig[8];
    uint64_t startCycle;
    uint64_t stopCycle;
    uint8_t userDefinedEnable;
    uint8_t isTimelineProfEn;
    uint8_t isTaskBasedProfEn;
    uint8_t isProfLogEn;
    uint8_t isSocLogEn;
    uint8_t reserved[15]; // reserved 15 bytes
} ts_dynamic_profile_enable;

typedef struct tag_ts_alloc_dsa_addr_task {
    uint16_t dsa_sq_id;
    uint8_t reserved[46];
} ts_alloc_dsa_addr_task_t;

typedef struct tag_ts_debug_status_t {
    uint8_t debug_flag;
    uint8_t reserved[3];
} ts_debug_status_t;

enum tag_ts_mmt_operation_type {
    TS_MMT_STREAM_BIND = 0,   /**< model stream bind */
    TS_MMT_STREAM_UNBIND = 1, /**< model stream unbind */
    TS_MMT_MODEL_CREATE = 2,  /**< model create by task pool */
    TS_MMT_MODEL_DESTROY = 3, /**< model destroy */
    TS_MMT_MODEL_PRE_PROC = 4,
    TS_MMT_STREAM_LOAD_COMPLETE = 5,
    TS_MMT_MODEL_ABORT = 6, /**< model abort */
    TS_MMT_RESERVED
};

enum tag_ts_mmt_stream_type {
    TS_MMT_HEAD_STREAM = 0,        /**< model first stream  */
    TS_MMT_WAIT_ACTIVE_STREAM = 1, /**< model non-first stream  */
};
typedef struct tag_ts_model_execute_task {
    uint16_t model_id;
    uint16_t first_task_id;
    int16_t sch_group_id;
    uint8_t reserved0[2];
    uint64_t asid_baddr;
    uint64_t tcr;
    uint16_t asid;
    uint16_t SMMU_subStreamID;
    uint8_t reserved[20];
} ts_model_execute_task_t;

typedef struct tag_ts_rdma_send_task {
    uint32_t sq_index;
    uint32_t wqe_index;
    uint8_t reserved[40];
} ts_rdma_send_task_t;

typedef struct tag_ts_rdma_db_send_task {
    uint64_t dbInfo;
    uint32_t dbIndex;
    uint8_t reserved[36]; // offset 36
} ts_rdma_db_send_task_t;

typedef struct tag_ts_stream_switch_task {
    int64_t value;
    uint64_t pptr;
    uint64_t pValuePtr;
    uint32_t condition;
    uint16_t trueStreamId;
    uint8_t isCondEx;
    uint8_t dataType;
    uint32_t pptrVirAddr;
    uint32_t pValuePtrVirAddr;
    uint8_t reserved[8];
} ts_stream_switch_task_t;

typedef struct tag_ts_stream_switchN_task {
    uint64_t pptr;
    uint64_t pValuePtr;
    uint64_t pTrueStreamIdPtr;
    uint32_t size;
    uint32_t elementSize;
    uint32_t pptrVirAddr;
    uint32_t pValuePtrVirAddr;
    uint32_t pTrueVirAddr;
    uint8_t dataType;
    uint8_t isTransAddr;
    uint8_t reserved[2];
} ts_stream_switchN_task_t;

typedef struct tag_ts_stream_active_task {
    uint16_t activeStreamId;
    uint8_t reserved[46];
} ts_stream_active_task_t;

typedef struct tag_ts_profiler_notify_task {
    uint64_t profilerTraceId;
    uint8_t notify;
    uint8_t reserved[39];
} ts_profiler_notify_task_t;

typedef struct tag_ts_keypoint_task {
    uint64_t profilerTraceId;
    uint64_t modelId;
    uint16_t tagId;
    uint8_t reserved[30];
} ts_keypoint_task_t;

typedef struct tag_ts_label_set_task {
    uint16_t label_id;
    uint16_t reserved[3];
    uint64_t label_ptr;
    uint8_t reserved1[28];
    uint32_t vir_addr;
} ts_label_set_task_t;

typedef struct tag_ts_label_switch_task {
    uint64_t pptr; // PA
    uint32_t condition;
    uint32_t value;
    uint16_t true_label_id;
    uint16_t true_label_id_to_node_idx;
    uint16_t next_task_idx_bak; // backup next_task_idx in persistent_task_pool
    uint16_t true_label_id_to_stream_id;
    uint8_t reserved[20];
    uint32_t virAddr;
} ts_label_switch_task_t;

typedef struct tag_ts_label_goto_task {
    uint16_t label_id;
    uint16_t label_id_to_node_idx;
    uint16_t next_task_idx_bak; // backup next_task_idx in persistent_task_pool
    uint16_t label_id_to_stream_id;
    uint8_t reserved[40];
} ts_label_goto_task_t;

typedef struct tag_ts_datadump_load_info_task {
    uint64_t dumpInfoPtr;
    uint32_t length;
    uint8_t reserved[36];
} ts_datadump_load_info_task_t;

typedef struct tag_ts_aicpu_load_info_task {
    uint64_t aicpuInfoPtr;
    uint32_t length;
    uint8_t reserved[36];
} ts_aicpu_load_info_task_t;

/**
 * @ingroup ts
 * @brief the struct define of debug register task
 */
typedef struct tag_ts_debug_register_task {
    uint64_t addr;
    uint32_t model_id;
    uint32_t flag;
    uint32_t virAddr;     /* for addr translation */
    uint8_t reserved[20]; /* reserved 20 bytes */
} ts_debug_register_task_t;

/**
 * @ingroup ts
 * @brief the struct define of debug unregister task
 */
typedef struct tag_ts_debug_unregister_task {
    uint32_t model_id;
    uint8_t reserved[44]; // reserved 44 bytes
} ts_debug_unregister_task_t;

/**
 * @ingroup ts
 * @brief the struct define of debug register with stream task
 */
typedef struct tag_ts_debug_register_with_stream_task {
    uint64_t addr;
    uint32_t stream_id;
    uint32_t flag;
    uint32_t virAddr;     /* for addr translation */
    uint8_t reserved[28]; /* reserved 28 bytes */
} ts_debug_register_with_stream_task_t;

/**
 * @ingroup ts
 * @brief the struct define of debug unregister with stream task
 */
typedef struct tag_ts_debug_unregister_with_stream_task {
    uint32_t stream_id;
    uint8_t reserved[44]; // reserved 44 bytes
} ts_debug_unregister_with_stream_task_t;

typedef struct tag_ts_end_graph {
    uint64_t end_graph_name_ptr;
    uint64_t arg_ptr;
    uint32_t model_id;
    uint32_t executor_flag;
    uint8_t priority;
    uint8_t flag;
    uint8_t reserved[22]; // reserve 22 Bytes
} ts_end_graph_t;

typedef struct tag_ts_model_exit {
    uint32_t model_id;
    uint32_t stream_id;
    uint8_t reserved[40]; // reserve 40 Bytes
} ts_model_exit_t;

typedef struct tag_ts_aicpu_task {
    uint64_t arg_ptr;
    uint32_t model_id;
    uint32_t cmd_type;
    uint32_t executor_flag;
    int16_t timewheel_slot; // used only by ts
    uint8_t reserved[26];
} ts_aicpu_task_t;

typedef struct tag_ts_host_func_cb_task {
    uint64_t host_func_cb_ptr;
    uint64_t fn_data_ptr;
    uint32_t cb_rpt_cqid;
    uint8_t is_block;
    uint8_t reserved[27];
} ts_host_func_cb_task_t;

typedef struct tag_node_info {
    uint32_t node_idx;
    uint32_t reserved[1];
} rt_node_info_t;

typedef struct tag_hwts_info {
    uint16_t sq_exe_head;
    uint16_t stream_exe_head;
    uint16_t reserved1;
    uint16_t task_id;
    uint16_t reserved2;
} rt_hwts_info_t;

typedef struct tag_ts_label_dev_info_v2 {
    uint16_t label_id;
    uint16_t stream_id;
    uint16_t model_id;
    union {
        rt_node_info_t node_info;
        rt_hwts_info_t hwts_info;
        uint16_t reserved[5];
    } u;
} ts_label_dev_info_v2_t;

typedef struct tag_ts_label_dev_info {
    uint16_t model_id;
    uint16_t stream_id;
    uint16_t label_id;
} ts_label_dev_info_t;

typedef struct tag_ts_stream_label_switch_by_index_task_t {
    uint64_t index_ptr;
    uint64_t label_info_ptr;
    uint32_t max;
    uint16_t next_task_idx_bak;
    uint8_t reserved[18];
    uint32_t indexPtrVirAddr;
    uint32_t labelInfoPtrVirAddr;
} ts_stream_label_switch_by_index_task_t;

typedef struct tag_ts_stream_label_goto_task_t {
    uint16_t label_id;
    uint16_t model_id;
    uint16_t next_task_idx_bak;
    uint8_t reserved[42];
} ts_stream_label_goto_task_t;

typedef struct tag_ts_device_ringbuffer_control_task_t {
    uint64_t ringbuffer_offset;
    uint64_t ringbuffer_phy_addr;
    uint64_t pid;
    uint32_t total_len;
    uint8_t ringbuffer_del_flag; // 0:create 1:delete
    uint8_t reserved[19];
} ts_ringbuffer_control_t;

typedef struct tag_ts_timeout_set_task_t {
    uint32_t op_wait_timeout_en : 1;
    uint32_t op_execute_timeout_en : 1;
    uint32_t rsv : 30;
    uint32_t op_wait_timeout;
    uint32_t op_execute_timeout;
    int16_t timewheel_slot; // used only by ts
    uint8_t reserved[34];
} ts_timeout_set_task_t;

typedef struct tag_ts_get_dev_msg_task_t {
    uint64_t dev_addr;
    uint64_t offset;
    uint32_t len;
    uint16_t type;
    uint8_t reserved[26];
} ts_get_dev_msg_task_t;

typedef struct tag_ts_stream_overflow_switch_t {
    uint16_t stream_id;
    uint16_t is_switch_on : 1;
    uint16_t rsv : 15;
    uint32_t reserved[11];
} ts_stream_overflow_switch_t;

typedef struct tag_ts_stream_set_tag_t {
    uint16_t stream_id;
    uint16_t rsv;
    uint32_t geOpTag;
    uint32_t reserved[10];
} ts_stream_set_tag_t;

typedef struct tag_ts_set_stream_mode_t {
    uint64_t mode;
    uint8_t reserved[40];
} ts_set_stream_mode_t;

typedef struct tag_ts_flip_task_t {
    uint16_t flip_num;
    uint8_t reserved[46];
} ts_flip_task_t;

#ifndef STARS_CTRL_CPU
typedef struct tag_ts_model_update_task_t { // rtMdlTaskUpdate_t
    uint64_t desc_buff_offset;              // rtFftsPlusTaskInfo_t-->descBuf
    uint64_t tiling_key_offset;
    uint64_t num_blocks_offset;
    uint64_t tiling_tab_offset;
    uint16_t tiling_tab_len;
    uint16_t des_stream_id;
    uint16_t des_task_id;
    uint16_t exe_stream_id;
    uint8_t reserved[8];
} ts_model_update_task_t;
#else
typedef struct tag_ts_model_update_task_t { // rtMdlTaskUpdate_t
    uint64_t tiling_key_offset;
    uint64_t num_blocks_offset;
    uint64_t tiling_tab_offset;
    uint16_t tiling_tab_len;
    uint16_t des_stream_id;
    uint16_t des_task_id;
    uint16_t exe_stream_id;
} ts_model_update_task_t;
#endif

typedef struct tag_ts_common_cmd_task {
    uint16_t cmd_type;
    uint16_t stream_id; // for streamclear
    uint32_t notify_id; // for notifyreset
    uint16_t step;      // for streamclear
    uint8_t reserved[38];
} ts_common_cmd_task_t;

typedef struct tag_ts_get_stars_version {
    uint32_t runtime_version;
    uint8_t reserved[44];
} ts_get_stars_version_t;

/**
 * @ingroup tsch
 * @brief the struct define of task
 */
typedef struct tag_ts_task {
    uint16_t streamID;        /**< offset 0 */
    uint16_t taskID;          /**< offset 2 */
    uint16_t next_task_idx;   /**< offset 4 */
    uint16_t type;            /**< offset 6 */
    uint16_t next_stream_idx; /**< offset 8 */
    uint16_t task_state;      /**< 10*/
    uint8_t task_prof_en : 7; /**< offset 12* */
    uint8_t isctrl : 1;       /* ctrltask is 1; othertask is 0 */
    uint8_t task_info_flag;   /* bit 0: is need send cq; bit 1: is bind task;
                               * bit 2: endgraph dump, bit 3: unsink flag
                               * bit 4: overflow dump, bit 5: invalid flag
                                 bit 6: fast cq flag */
    uint16_t task_info;       /**< now used in cloud model rmda stream deal */
    /* 48 bytes */
    union {
        ts_kernel_task_t kernel_task;
        ts_event_record_task_t event_task;
        ts_stream_wait_event_task_t stream_wait_event_task;
        ts_fusion_task_t fusion_task;
        ts_memcpy_task_t memcpy_task;
        ts_maintenance_task_t maintenance_task;
        ts_create_stream_t creat_stream;
        ts_create_l2_addr_t create_l2_addr;
        ts_profile_enable_t profile_enable;
        ts_profile_disable_t profile_disable;
        /* **BEGIN    ADD HCCL multi_device feature 2018-01-24    ********* */
        ts_remote_event_wait_t remote_event_wait_task;
        /* **END      ADD HCCL multi_device feature 2018-01-24    ********* */
        tsPCTrace_task_t pc_trace_task;
        ts_data_dump_task_t data_dump_task;
        ts_notify_wait_task_t notify_wait_task;
        ts_notify_record_task_t notify_record_task;
        ts_ipc_int_notice_task_t ipc_int_notice_task;
        ts_model_maintaince_task_t model_maintaince_task;
        ts_model_execute_task_t model_execute_task;
        ts_rdma_send_task_t rdma_send_task;
        ts_stream_switch_task_t stream_switch_task;
        ts_stream_active_task_t stream_active_task;
        ts_profiler_notify_task_t profiler_notify_task;
        ts_keypoint_task_t keypoint_task;
        ts_label_set_task_t label_set_task;
        ts_label_switch_task_t label_switch_task;
        ts_label_goto_task_t label_goto_task;
        ts_event_reset_task_t event_reset_task;
        ts_end_graph_t end_graph_task;
        ts_model_exit_t model_exit_task;
        ts_aicpu_task_t aicpu_task;
        ts_datadump_load_info_task_t datadump_load_info_task;
        ts_debug_register_task_t debug_register_task;
        ts_debug_unregister_task_t debug_unregister_task;
        ts_stream_switchN_task_t stream_switchN_task;
        ts_host_func_cb_task_t host_func_cb_task;
        ts_onlineprof_start_t onlineprof_start_task;
        ts_onlineprof_stop_t onlineprof_stop_task;
        ts_rdma_db_send_task_t rdma_db_send_task;
        ts_stream_label_switch_by_index_task_t stream_label_switch_index_task;
        ts_stream_label_goto_task_t stream_label_goto_task;
        ts_mdcprof_t mdcprof;
        ts_ringbuffer_control_t ringbuffer_control_task;
        ts_debug_register_with_stream_task_t debug_register_with_stream_task;
        ts_debug_unregister_with_stream_task_t debug_unregister_with_stream_task;
        ts_timeout_set_task_t timeout_set_task;
        ts_get_dev_msg_task_t get_dev_msg_task;
        ts_reduce_async_v2_task_t reduce_async_v2_task;
        ts_set_stream_mode_t set_stream_mode_task;
        ts_flip_task_t flip_task;
        ts_model_update_task_t model_update_task;
        ts_aicpu_load_info_task_t aicpu_load_info;
        ts_common_cmd_task_t common_cmd_task;
        ts_get_stars_version_t get_version_task;
    } u;
} ts_task_t;

enum tag_ts_aicpu_mail_box_cmd_type {
    AICPU_MODEL_OPERATE = 1,         /* 1 aicpu model operate */
    AICPU_MODEL_OPERATE_RESPONSE,    /* 2 aicpu model operate response */
    AIC_TASK_REPORT,                 /* 3 aic task report */
    AICPU_ACTIVE_STREAM,             /* 4 aicpu active stream */
    AICPU_NOTIFY_RECORD,             /* 5 aicpu notify */
    AICPU_DATADUMP_REPORT,           /* 6 data dump report */
    AICPU_DATADUMP_LOADINFO,         /* 7 data dump load info */
    AICPU_DATADUMP_RESPONSE,         /* 8 data dump response */
    AICPU_ABNORMAL,                  /* 9 aicpu abnormal report */
    AICPU_TASK_ACTIVE_FOR_WAIT,      /* 10 aicpu dvpp task active */
    AICPU_NOTICE_TS_PID,             /* 11 aicpu pid */
    AICPU_RECORD,                    /* 12 aicpu record: 1-event_record; 2-notify_record */
    AICPU_TIMEOUT_CONFIG,            /* 13 aicpu timeout config */
    AICPU_TIMEOUT_CONFIG_RESPONSE,   /* 14 aicpu timeout response */
    CALLBACK_RECORD,                 /* 15 synchronized callback record from runtime, not for aicpu */
    AICPU_ERR_MSG_REPORT,            /* 16 aicpu err msg report */
    AICPU_FFTS_PLUS_DATADUMP_REPORT, /* 17 ffts plus data dump report */
    AICPU_INFO_LOAD,                 /* 18 aicpu info load for tiling key sink */
    AICPU_INFO_LOAD_RESPONSE,        /* 19 aicpu info load  response */
    AIC_ERROR_REPORT,                /* 20 aic task err report */
    INVALID_AICPU_CMD,               /* invalid flag */
};

typedef struct tag_ts_aicpu_model_operate {
    volatile uint64_t arg_ptr;
    volatile uint16_t sq_id;
    volatile uint16_t task_id;
    volatile uint16_t model_id;
    volatile uint8_t cmd_type;
    volatile uint8_t reserved;
} ts_aicpu_model_operate_t;

typedef struct tag_ts_aicpu_model_operate_response {
    volatile uint8_t cmd_type;
    volatile uint8_t sub_cmd_type;
    volatile uint16_t model_id;
    volatile uint16_t task_id;
    volatile uint16_t result_code;
    volatile uint16_t sq_id;
    volatile uint8_t reserved[2];
} ts_aicpu_model_operate_response_t;

typedef struct tag_ts_to_aicpu_task_report {
    volatile uint16_t model_id;
    volatile uint16_t stream_id;
    volatile uint16_t task_id;
    volatile uint16_t result_code;
} ts_to_aicpu_task_report_t;

typedef struct tag_ts_to_aicpu_aic_err_report {
    volatile uint64_t aiv_err_bitmap;
    volatile uint32_t aic_err_bitmap;
    volatile uint16_t result_code;
} ts_to_aicpu_aic_err_report_t;

typedef struct tag_ts_aicpu_notify {
    volatile uint32_t notify_id;
    volatile uint16_t ret_code; // using ts_error_t
} ts_aicpu_notify_t;

typedef struct tag_ts_aicpu_event {
    volatile uint32_t record_id;
    volatile uint8_t record_type;
    volatile uint8_t reserved;
    volatile uint16_t ret_code;        // using ts_error_t
    volatile uint16_t fault_task_id;   // using report error of operator
    volatile uint16_t fault_stream_id; // using report error of operator
} ts_aicpu_record_t;

typedef struct tag_ts_aicpu_active_stream {
    volatile uint16_t stream_id;
    volatile uint8_t reserved[6];
    volatile uint64_t aicpu_stamp;
} ts_aicpu_active_stream_t;

typedef struct tag_ts_to_aicpu_datadump {
    volatile uint16_t model_id;
    volatile uint16_t stream_id;     // first dump task info
    volatile uint16_t task_id;       // first dump task info
    volatile uint16_t stream_id1;    // second dump task info
    volatile uint16_t task_id1;      // second dump task info
    volatile uint16_t ack_stream_id; // record overflow dump aic task info
    volatile uint16_t ack_task_id;   // record overflow dump aic task info
    volatile uint8_t reserved[2];
} ts_to_aicpu_datadump_t;

typedef struct tag_ts_to_aicpu_ffts_plus_datadump {
    volatile uint16_t model_id;   // model id
    volatile uint16_t stream_id;  // first dump task info
    volatile uint16_t task_id;    // first dump task info
    volatile uint16_t stream_id1; // second dump task info
    volatile uint16_t task_id1;   // second dump task info
    volatile uint16_t context_id; // context id
    volatile uint16_t thread_id;  // current thread ID
    volatile uint8_t reserved[2];
#if (defined(DAVINCI_CLOUD_V2) || defined(DAVINCI_CLOUD_V2_FFTS))
    volatile uint32_t pid;
#endif
} ts_to_aicpu_ffts_plus_datadump_t;

typedef struct tag_ts_datadumploadinfo {
    volatile uint64_t dumpinfo_ptr;
    volatile uint32_t length;
    volatile uint16_t stream_id;
    volatile uint16_t task_id;
    volatile uint16_t kernel_type;
    volatile uint16_t reserved;
} ts_datadumploadinfo_t;

typedef struct tag_ts_to_aicpu_datadumploadinfo {
    volatile uint64_t dumpinfoPtr;
    volatile uint32_t length;
    volatile uint16_t stream_id;
    volatile uint16_t task_id;
} ts_to_aicpu_datadumploadinfo_t;

typedef struct tag_ts_to_aicpu_loadinfo {
    volatile uint64_t aicpuInfoPtr;
    volatile uint32_t length;
    volatile uint16_t stream_id;
    volatile uint16_t task_id;
} ts_to_aicpu_loadinfo_t;

typedef struct tag_ts_aicpu_dump_response {
    volatile uint16_t task_id;
    volatile uint16_t result_code;
    volatile uint16_t stream_id;
    volatile uint8_t cmd_type;
    volatile uint8_t reserved;
} ts_aicpu_dump_response_t;

typedef struct tag_ts_aicpu_response {
    volatile uint16_t task_id;
    volatile uint16_t result_code;
    volatile uint16_t stream_id;
    volatile uint8_t cmd_type;
    volatile uint8_t reserved;
} ts_aicpu_response_t;

typedef struct tag_ts_aicpu_task_active_for_wait {
    volatile uint16_t stream_id;
    volatile uint16_t task_id;
    volatile uint32_t result_code;
} ts_aicpu_task_active_for_wait_t;

typedef struct tag_ts_to_aicpu_timeout_config {
    uint32_t op_wait_timeout_en : 1;
    uint32_t op_execute_timeout_en : 1;
    uint32_t rsv : 30;
    uint32_t op_wait_timeout;
    uint32_t op_execute_timeout;
} ts_to_aicpu_timeout_config_t;

typedef struct tag_ts_aicpu_timeout_config_response {
    uint32_t result;
} ts_aicpu_timeout_config_response_t;

typedef struct tag_ts_callback_record {
    volatile uint16_t stream_id;
    volatile uint16_t record_id;
    volatile uint16_t task_id;
    volatile uint16_t reserved;
} ts_callback_record_t;

typedef struct tag_ts_aicpu_err_msg_report {
    volatile uint32_t result_code;
    volatile uint16_t stream_id;
    volatile uint16_t task_id;
    volatile uint16_t offset;
    volatile uint8_t reserved[2];
} ts_aicpu_err_msg_report_t;

// 51 and 71 share this structure. 51 supports only 24 bytes and 71 supports 40 bytes
// It is recommended that macro control be performed when adding fields.
typedef struct tag_ts_aicpu_sqe {
    volatile uint32_t pid;
    volatile uint8_t cmd_type;
    volatile uint8_t vf_id;
    volatile uint8_t tid;
    volatile uint8_t ts_id;
    union {
        ts_aicpu_model_operate_t aicpu_model_operate;
        ts_aicpu_model_operate_response_t aicpu_model_operate_resp;
        ts_to_aicpu_task_report_t ts_to_aicpu_task_report;
        ts_aicpu_active_stream_t aicpu_active_stream;
        ts_aicpu_notify_t aicpu_notify;
        ts_aicpu_record_t aicpu_record;
        ts_to_aicpu_datadump_t ts_to_aicpu_datadump;
        ts_to_aicpu_datadumploadinfo_t ts_to_aicpu_datadumploadinfo;
        ts_aicpu_dump_response_t aicpu_dump_resp;
        ts_aicpu_task_active_for_wait_t task_active_for_wait;
        ts_to_aicpu_timeout_config_t ts_to_aicpu_timeout_cfg;
        ts_aicpu_timeout_config_response_t aicpu_timeout_cfg_resp;
        ts_callback_record_t callback_record; /* synchronized callback record from runtime, not for aicpu */
        ts_aicpu_err_msg_report_t aicpu_err_msg_report;
        ts_to_aicpu_ffts_plus_datadump_t ts_to_aicpu_ffts_plus_datadump;
        ts_to_aicpu_loadinfo_t ts_to_aicpu_info;
        ts_aicpu_response_t aicpu_resp;
        ts_to_aicpu_aic_err_report_t ts_to_aicpu_aic_err_report;
    } u;
} ts_aicpu_sqe_t;

typedef struct tag_stream_snapshot {
    uint16_t stream_id;
    uint16_t task_id;
    uint16_t sq_id : 12;
    uint16_t sq_fsm : 4;
    uint16_t acsq_id : 8;
    uint16_t acsq_fsm : 6;
    uint16_t is_swap_in : 1;
    uint16_t rsv : 1;
} ts_stream_snapshot_t;

// ts_to_aicpu_task_report_t  resultcode
#define TASK_REPORT_RESULT_CODE_FAIL (1U)

typedef enum tag_ts_aicpu_record_type {
    AICPU_MSG_EVENT_RECORD = 1, /* 1 aicpu event record */
    AICPU_MSG_NOTIFY_RECORD     /* 2 aicpu notify record */
} ts_aicpu_record_type_t;

typedef enum tag_ts_sq_alloc_type {
    SQ_ALLOC_TYPE_RT_DEFAULT = 0,
    SQ_ALLOC_TYPE_TS_FFTS_DSA = 1,
} ts_sq_alloc_type_t;

typedef struct {
    uint32_t stream_id;
    uint32_t priority;
    uint32_t overflow_en : 1;
    uint32_t sat_mode : 1;
    uint32_t sq_type : 1;
    uint32_t rsv : 29;
    uint32_t thread_disable_flag;
    uint32_t share_sq_id;
} ts_stream_alloc_info_t;

// store register data to ddr, RT_STARS_COND_ISA_OP_CODE_STORE
typedef struct tag_stars_cond_op_store {
    uint32_t op_code : 7;
    uint32_t immd_low : 5;
    uint32_t func3 : 3;
    uint32_t rs1 : 3;
    uint32_t reserved1 : 2;
    uint32_t rs2 : 3;
    uint32_t reserved2 : 2;
    uint32_t immd_high : 7;
} ts_stars_cond_op_store_t;

// Op imm:RT_STARS_COND_ISA_OP_CODE_OP_IMM
// func3 :ADDI/SLTI[U]/ANDI/ORI/XORI
typedef struct tag_stars_cond_op_imm {
    uint32_t op_code : 7;
    uint32_t rd : 3;
    uint32_t reserved0 : 2; // reserved
    uint32_t func3 : 3;
    uint32_t rs1 : 3;
    uint32_t reserved1 : 2; // reserved
    uint32_t immd : 12;
} ts_stars_cond_op_imm_t;

// nop is using op-imm ADDI
typedef ts_stars_cond_op_imm_t ts_stars_cond_op_nop_t;

// load addr data to register rd, RT_STARS_COND_ISA_OP_CODE_LOAD_IMM
typedef struct tag_stars_cond_op_loadimm {
    uint32_t op_code : 7;
    uint32_t rd : 3;
    uint32_t reserved : 2; // reserved
    uint32_t func3 : 3;
    uint32_t immd_addr_high : 17;
    uint32_t immd_addr_low;
} ts_stars_cond_op_loadimm_t;

// LWI LHWI:RT_STARS_COND_ISA_OP_CODE_LWI
typedef struct tag_stars_cond_op_LHWI {
    uint32_t op_code : 7;
    uint32_t rd : 3;
    uint32_t reserved0 : 2; // reserved
    uint32_t func3 : 3;
    uint32_t reserved1 : 2; // reserved
    uint32_t immd : 15;
} ts_stars_cond_op_LHWI_t;

// LWI LLWI: RT_STARS_COND_ISA_OP_CODE_LWI
typedef struct tag_stars_cond_op_LLWI {
    uint32_t op_code : 7;
    uint32_t rd : 3;
    uint32_t reserved0 : 2; // reserved
    uint32_t func3 : 3;
    uint32_t immd_high : 17;
    uint32_t immd_low;
} ts_stars_cond_op_LLWI_t;

typedef struct tag_stars_cond_op_function_call {
    uint32_t opCode : 7;
    uint32_t reserved0 : 3;
    uint32_t reserved1 : 2;
    uint32_t func3 : 3;
    uint32_t rs1 : 3;
    uint32_t reserved2 : 2;
    uint32_t rs2 : 3;
    uint32_t reserved3 : 2;
    uint32_t reserved4 : 7;
} ts_stars_cond_op_function_call_t;

/* stars sqe struct begin */
#define STARS_SQE_UNIT_LEN 64
#define STARS_CQE_UNIT_LEN 16
#define TS_STARS_SQE_TYPE_INVALID 63

#ifndef STARS_CTRL_CPU
typedef enum ts_stars_sqe_type {
    TS_STARS_SQE_TYPE_FFTS = 0,          // FFTS
    TS_STARS_SQE_TYPE_AICPU = 1,         // AICPU
    TS_STARS_SQE_TYPE_PLACE_HOLDER = 3,  // PLACE_HOLDER
    TS_STARS_SQE_TYPE_EVENT_RECORD = 4,  // EVENT_RECORD
    TS_STARS_SQE_TYPE_EVENT_WAIT = 5,    // EVENT_WAIT
    TS_STARS_SQE_TYPE_NOTIFY_RECORD = 6, // NOTIFY_RECORD
    TS_STARS_SQE_TYPE_NOTIFY_WAIT = 7,   // NOTIFY_WAIT
    TS_STARS_SQE_TYPE_WRITE_VALUE = 8,   // for EVENT_RESET task
    TS_STARS_SQE_TYPE_SDMA = 11,         // SDMA
    TS_STARS_SQE_TYPE_VPC = 12,          // VPC
    TS_STARS_SQE_TYPE_JPEGE = 13,        // JPEGE
    TS_STARS_SQE_TYPE_JPEGD = 14,        // JPEGD
    TS_STARS_SQE_TYPE_DSA = 15,          // DSA
    TS_STARS_SQE_TYPE_ROCCE = 16,        // RoCCE
    TS_STARS_SQE_TYPE_PCIE_DMA = 17,     // PCIE_DMA
    TS_STARS_SQE_TYPE_RESERVE = 18,      // RESERVE
    TS_STARS_SQE_TYPE_CDQM = 19,         // CDQM
    TS_STARS_SQE_TYPE_COND = 20,         // condition
    TS_STARS_SQE_TYPE_END
} ts_stars_sqe_type_t;
#else
typedef enum ts_stars_sqe_type {
    TS_STARS_SQE_TYPE_AIC = 0,
    TS_STARS_SQE_TYPE_AIV = 1,
    TS_STARS_SQE_TYPE_FUSION = 2,
    TS_STARS_SQE_TYPE_PLACE_HOLDER = 3, // PLACE_HOLDER
    TS_STARS_SQE_TYPE_AICPU_H = 4,
    TS_STARS_SQE_TYPE_AICPU_D = 5, // NOTIFY_RECORD
    TS_STARS_SQE_TYPE_NOTIFY_RECORD = 6,
    TS_STARS_SQE_TYPE_NOTIFY_WAIT = 7,
    TS_STARS_SQE_TYPE_WRITE_VALUE = 8,
    TS_STARS_SQE_TYPE_UBDMA = 9,
    TS_STARS_SQE_TYPE_ASYNCDMA = 10,
    TS_STARS_SQE_TYPE_SDMA = 11,
    TS_STARS_SQE_TYPE_VPC = 12,
    TS_STARS_SQE_TYPE_JPEGE = 13,
    TS_STARS_SQE_TYPE_JPEGD = 14,
    TS_STARS_SQE_TYPE_CMO = 15,
    TS_STARS_SQE_TYPE_CCU = 16,
    TS_STARS_SQE_TYPE_COND = 20,
    TS_STARS_SQE_TYPE_END = 63,
} ts_stars_sqe_type_t;
#endif

enum rtTopicType_t {
    TOPIC_TYPE_DEVICE_AICPU_ONLY = 0,
    TOPIC_TYPE_DEVICE_AICPU_FIRST = 1,
    TOPIC_TYPE_HOST_AICPU_ONLY = 2,
    TOPIC_TYPE_HOST_AICPU_FIRST = 3,
    TOPIC_TYPE_HOST_CTRL_CPU = 4,
    TOPIC_TYPE_DATA_CPU = 5,
    TOPIC_TYPE_DEVICE_CTRL_CPU = 6,
    TOPIC_TYPE_TS_CPU = 7,
    TOPIC_TYPE_DVPP_CPU = 8,
    TOPIC_TYPE_DEVICE_HOST_CTRL_CPU = 9,
};

enum ts_ffts_type {
    TS_FFTS_TYPE_AIC_ONLY = 0U,      // aic only
    TS_FFTS_TYPE_AIV_ONLY = 1U,      // aiv only
    TS_FFTS_TYPE_AUTO_THREAD = 2U,   // ffts auto thread mode, same as ffts define
    TS_FFTS_TYPE_MANUAL_THREAD = 3U, // ffts manual thread mode, same as ffts define
    TS_FFTS_TYPE_FFTS_PLUS = 4U,     // ffts plus
    TS_FFTS_TYPE_AIC_AIV_MIX = 5U    // aic/aiv mixed(1910b)
};

enum ts_aic_type {
    TS_TYPE_AIC_ONLY = 0U, // aic only
    TS_TYPE_AIV_ONLY = 1U, // aiv only
};

enum ts_stars_write_value_sub_type {
    TS_STARS_WRITE_VALUE_SUB_TYPE_DEFAULT = 0,
    TS_STARS_WRITE_VALUE_SUB_TYPE_EVENT_RESET = 1,
    TS_STARS_WRITE_VALUE_SUB_TYPE_RDMA_DB_SEND = 2,
    TS_STARS_WRITE_VALUE_SUB_TYPE_NOTIFY_RECORD_IPC_NO_PCIE = 3,
    TS_STARS_WRITE_VALUE_SUB_TYPE_NOTIFY_RECORD_IPC_PCIE = 4,
    TS_STARS_WRITE_VALUE_SUB_TYPE_MAX,
};

#pragma pack(push)
#pragma pack(1)
#ifndef STARS_CTRL_CPU
typedef struct ts_stars_sqe_header {
    uint8_t type : 6;
    uint8_t l1_lock : 1;
    uint8_t l1_unlock : 1;

    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t reserved : 1;

    uint16_t num_blocks;
    uint16_t rt_stream_id;
    uint16_t task_id;
} ts_stars_sqe_header_t;

typedef struct ts_stars_sqe_word0 {
    uint8_t type : 6;
    uint8_t l1_lock : 1;
    uint8_t l1_unlock : 1;

    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t reserved : 1;

    uint16_t num_blocks;
} ts_stars_sqe_word0_t;

typedef struct ts_stars_ph_sqe_word0 {
    uint8_t type : 6;
    uint8_t l2_lock : 1;
    uint8_t l2_unlock : 1;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res0 : 1;
    uint16_t task_type;
} ts_stars_ph_sqe_word0_t;

/* stars sqe struct */
typedef struct ts_stars_sqe_normal {
    uint8_t type : 6;
    uint8_t l2_lock : 1;
    uint8_t l2_unlock : 1;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res0 : 1;
    uint16_t res1;
    uint16_t stream_id;
    uint16_t task_id;
    uint32_t res2;
    uint16_t res3;
    uint8_t kernel_credit;
    uint8_t res4;
    uint32_t res[12];
} ts_stars_sqe_t;

typedef struct ts_stars_ph_sqe {
    uint8_t type : 6;
    uint8_t l2_lock : 1;
    uint8_t l2_unlock : 1;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res0 : 1;
    uint16_t task_type;
    uint16_t stream_id;
    uint16_t task_id;
    uint32_t res1;
    uint16_t res2;
    uint8_t kernel_credit;
    uint8_t res3;
    union {
        ts_maintaince_task_t maintaince_info;
        ts_datadumploadinfo_t datadumploadinfo;
        ts_keypoint_task_t keypoint_task;
        ts_dynamic_profile_enable dynamic_profiling_info;
        ts_debug_register_task_t model_debug_register_task;
        ts_debug_register_with_stream_task_t stream_debug_register_task;
        ts_model_maintaince_task_t model_maintaince_task;
        ts_ringbuffer_control_t ringbuffer_control_task;
        ts_stream_overflow_switch_t stream_overflow_swith_task;
        ts_get_dev_msg_task_t get_dev_msg_task;
        ts_stream_set_tag_t stream_set_tag_task;
        ts_alloc_dsa_addr_task_t alloc_dsa_addr_task;
        ts_debug_status_t debug_status_info;
        ts_flip_task_t flip_task;
        ts_model_update_task_t model_task_updata_info;
        ts_to_aicpu_loadinfo_t aicpuloadinfo;
        ts_common_cmd_task_t common_cmd_task;
        ts_get_stars_version_t get_version_task;
        uint32_t resv[12];
    } u;
} ts_stars_ph_sqe_t;

typedef struct ts_stars_notify_sqe {
    ts_stars_sqe_header_t header;

    uint32_t notify_id : 13;
    uint32_t res2 : 19;

    uint16_t res3;
    uint8_t kernel_credit;
    uint8_t res4;
    uint32_t timeout;
    uint32_t res5[11];
} ts_stars_notify_sqe_t;

typedef struct ts_stars_event_sqe {
    ts_stars_sqe_header_t header;
    uint16_t event_id;
    uint16_t res2;

    uint16_t res3;
    uint8_t kernel_credit;
    uint8_t res4;

    uint32_t exe_result;
    uint32_t timeout;
    uint32_t res5[10];
} ts_stars_event_sqe_t;

typedef struct stars_aicpu_sqe {
    /* word0-1 */
    uint8_t type : 6;
    uint8_t l1_lock : 1;
    uint8_t l1_unlock : 1;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t reserved : 1;
    uint16_t num_blocks;
    uint16_t rt_stream_id;
    uint16_t task_id;

    /* word2 */
    uint16_t res0;
    uint16_t kernel_type : 7;
    uint16_t batch_mode : 1;
    uint16_t topic_type : 4;
    uint16_t qos : 3;
    uint16_t res7 : 1;

    /* word3 */
    uint16_t sqe_index;
    uint16_t kernel_credit : 8;
    uint16_t post_p_bak : 2;
    uint16_t type_bak : 6;

    /* word4-5 */
    uint32_t task_so_addr_low;
    uint32_t task_so_addr_high : 16;
    uint32_t res3 : 16;

    /* word6-7 */
    uint32_t param_addr_low;
    uint32_t param_addr_high : 16;
    uint32_t res4 : 16;

    /* word8-9 */
    uint32_t task_name_str_ptr_low;
    uint32_t task_name_str_ptr_high : 16;
    uint32_t res5 : 16;

    /* word10-11 */
    uint32_t p_l2ctrl_low; // use for vf & topic aicpu hostpid
    uint32_t p_l2ctrl_high : 16;
    uint32_t overflow_en : 1;
    uint32_t res6 : 15;

    /* word12-13 */
    uint32_t extra_field_low; // send task id info to aicpu
    uint32_t extra_field_high;

    /* word14 */
    uint32_t sub_topic_id : 12;
    uint32_t topic_id : 6;
    uint32_t group_id : 6;
    uint32_t usr_data_len : 8;

    /* word15 */
    uint32_t dest_pid;
} ts_stars_aicpu_sqe_t;

typedef struct ts_stars_vpc_sqe {
    uint8_t type : 6;
    uint8_t l2_lock : 1;
    uint8_t l2_unlock : 1;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res0 : 1;
    uint16_t res1;
    uint16_t stream_id;
    uint16_t task_id; // stars sqe header
    uint32_t res2;
    uint16_t res3;
    uint8_t kernel_credit;
    uint8_t ptr_mode : 1;
    uint8_t error_times : 2; // ts defined field, record task error times for vpc task exception
    uint8_t post_p_bak : 2;  // ts defined field, back post_p flag on exception
    uint8_t res4 : 3;
    uint32_t res[12];
} ts_stars_vpc_sqe_t;

typedef struct ts_stars_jpegd_sqe {
    uint8_t type : 6;
    uint8_t l2_lock : 1;
    uint8_t l2_unlock : 1;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res0 : 1;
    uint16_t res1;
    uint16_t stream_id;
    uint16_t task_id; // stars sqe header
    uint32_t res2;
    uint16_t res3;
    uint8_t kernel_credit;
    uint8_t ptr_mode : 1;
    uint8_t aicpu_task_pos : 7;
    uint32_t res[12];
} ts_stars_jpegd_sqe_t;

typedef struct stars_function_call_sqe {
    ts_stars_sqe_header_t header;

    uint8_t conds_sub_type; // CONDS_SUB_TYPE_STREAM_ACTIVE, 1910b tiny  only
    uint8_t reserved0[3];
    uint16_t reserved1;
    uint8_t kernel_credit;
    uint8_t reserved2 : 7;
    uint8_t csc : 1;

    ts_stars_cond_op_LHWI_t lhwi1;
    ts_stars_cond_op_LLWI_t llwi1;
    ts_stars_cond_op_LHWI_t lhwi2;
    ts_stars_cond_op_LLWI_t llwi2;
    ts_stars_cond_op_function_call_t func_call;
    ts_stars_cond_op_nop_t nop[5];
} ts_stars_function_call_sqe_t;

typedef struct stars_get_float_status_sqe {
    uint8_t type : 6;
    uint8_t sq_lock : 1;
    uint8_t sq_unlock : 1;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res0 : 1;
    uint16_t task_type;
    uint16_t stream_id;
    uint16_t task_id;

    uint32_t reserved0;
    uint16_t reserved1;
    uint8_t kernel_credit;
    uint8_t reserved2 : 6;
    uint8_t debug_flag : 1;
    uint8_t csc : 1;

    ts_stars_cond_op_loadimm_t ldi;
    ts_stars_cond_op_LLWI_t llwi;
    ts_stars_cond_op_store_t sd_overflow_cnt;
    ts_stars_cond_op_store_t sd_zero[7];
} ts_stars_get_float_status_sqe_t;

#else
typedef enum tag_david_notify_sub_type {
    NOTIFY_SUB_TYPE_SINGLE_NOTIFY_RECORD = 0U,
    NOTIFY_SUB_TYPE_SINGLE_NOTIFY_WAIT = 1U,
    NOTIFY_SUB_TYPE_COUNT_NOTIFY_RECORD = 2U,
    NOTIFY_SUB_TYPE_COUNT_NOTIFY_WAIT = 3U,
    NOTIFY_SUB_TYPE_EVENT_USE_SINGLE_NOTIFY_RECORD = 4U,
    NOTIFY_SUB_TYPE_EVENT_USE_SINGLE_NOTIFY_WAIT = 5U,
    NOTIFY_SUB_TYPE_EVENT_USE_COUNT_NOTIFY_RECORD = 6U,
    NOTIFY_SUB_TYPE_EVENT_USE_COUNT_NOTIFY_WAIT = 7U,
    NOTIFY_SUB_TYPE_MAX
} david_notify_sub_type_t;

typedef struct ts_stars_sqe_normal {
    uint8_t type : 6;
    uint8_t l1_lock : 1;
    uint8_t l1_unlock : 1;
    uint8_t ie : 1;
    uint8_t pre_p : 1;
    uint8_t post_p : 1;
    uint8_t wr_cqe : 1;
    uint8_t ptr_mode : 1;
    uint8_t rtt_mode : 1;
    uint8_t head_update : 1;
    uint8_t res0 : 1;
    uint16_t res1;

    uint16_t stream_id;
    uint16_t task_id;

    uint32_t res2 : 11;
    uint32_t sub_type : 5; // for fusion use
    uint32_t resv : 16;

    uint16_t res3;
    uint8_t kernel_credit;
    uint8_t res4 : 5;
    uint8_t sqe_length : 3;

    uint32_t res[12];
} ts_stars_sqe_t;

typedef struct ts_stars_sqe_header {
    uint8_t type : 6;
    uint8_t l1_lock : 1;
    uint8_t l1_unlock : 1;

    uint8_t ie : 1;
    uint8_t pre_p : 1;
    uint8_t post_p : 1;
    uint8_t wr_cqe : 1;
    uint8_t ptr_mode : 1;
    uint8_t rtt_mode : 1;
    uint8_t head_update : 1;
    uint8_t reserved : 1;

    uint16_t num_blocks;
    uint16_t rt_stream_id;
    uint16_t task_id;
} ts_stars_sqe_header_t;

typedef struct tag_ts_call_back_info {
    uint16_t cb_cq_id;
    uint16_t cb_group_id;
    uint16_t dev_id;
    uint16_t stream_id;

    /* word6-7 */
    uint32_t notify_id;
    uint16_t task_id;
    uint8_t is_block;
    uint8_t res1;

    /* word8-11 */
    uint32_t hostfunc_addr_low;
    uint32_t hostfunc_addr_high;
    uint32_t fndata_low;
    uint32_t fndata_high;

    /* word12-13 */
    uint32_t res2; // only vf & topic AICPU & callback msg use for hostpid.
    uint32_t res3;

    /* word14 */
    uint32_t sub_topic_id : 12;
    uint32_t topic_id : 6;
    uint32_t group_id : 6;
    uint32_t usr_data_len : 8;

    /* word15 */
    uint32_t dest_pid;
} ts_call_back_info_t;

typedef struct ts_stars_ph_sqe {
    uint8_t type : 6;
    uint8_t l1_lock : 1;
    uint8_t l1_unlock : 1;
    uint8_t ie : 1;
    uint8_t pre_p : 1;
    uint8_t post_p : 1;
    uint8_t wr_cqe : 1;
    uint8_t ptr_mode : 1;
    uint8_t ptt_mode : 1;
    uint8_t head_update : 1;
    uint8_t res0 : 1;
    uint16_t num_blocks;

    uint16_t stream_id;
    uint16_t task_id;

    uint32_t res1; // use for RUNTIME_BUILD_VERSION

    uint16_t task_type;
    uint8_t kernel_credit;
    uint8_t timeout_type; // use for timeout cqe in david
    union {
        ts_maintaince_task_t maintaince_info;
        ts_datadumploadinfo_t datadumploadinfo;
        ts_keypoint_task_t keypoint_task;
        ts_dynamic_profile_enable dynamic_profiling_info;
        ts_debug_register_task_t model_debug_register_task;
        ts_debug_register_with_stream_task_t stream_debug_register_task;
        ts_model_maintaince_task_t model_maintaince_task;
        ts_ringbuffer_control_t ringbuffer_control_task;
        ts_stream_overflow_switch_t stream_overflow_swith_task;
        ts_get_dev_msg_task_t get_dev_msg_task;
        ts_stream_set_tag_t stream_set_tag_task;
        ts_alloc_dsa_addr_task_t alloc_dsa_addr_task;
        ts_debug_status_t debug_status_info;
        ts_flip_task_t flip_task;
        ts_model_update_task_t model_task_update_info;
        ts_to_aicpu_loadinfo_t aicpuloadinfo;
        ts_call_back_info_t call_back_info;
        uint32_t resv[12];
    } u;
} ts_stars_ph_sqe_t;

typedef struct stars_function_call_sqe {
    ts_stars_sqe_header_t header;

    uint8_t conds_sub_type; // CONDS_SUB_TYPE_STREAM_ACTIVE, 1910b tiny  only
    uint16_t reserved0;
    uint8_t reserved1 : 7;
    uint8_t csc : 1;
    uint16_t reserved2;
    uint8_t kernel_credit;
    uint8_t reserved3 : 4;
    uint8_t debug_flag : 1;
    uint8_t sqe_length : 3; // use reserved filed

    ts_stars_cond_op_LHWI_t lhwi1;
    ts_stars_cond_op_LLWI_t llwi1;
    ts_stars_cond_op_LHWI_t lhwi2;
    ts_stars_cond_op_LLWI_t llwi2;
    ts_stars_cond_op_function_call_t func_call;
    ts_stars_cond_op_nop_t nop[5];
} ts_stars_function_call_sqe_t;

typedef struct ts_stars_aic_aiv_sqe {
    /* word0-1 */
    ts_stars_sqe_header_t header;

    /* word2 */
    uint16_t group_dim;
    uint16_t group_num_blocks;

    /* word3 */
    uint8_t res0; // res0 used for DATADUMP BIUPERF L2CACHE flag
    uint8_t res1;
    uint8_t kernel_credit;
    uint8_t die_friendly : 1;
    uint8_t mix : 1;
    uint8_t loose : 1;
    uint8_t res2 : 2;
    uint8_t sqe_length : 3;

    /* word4-5 */
    uint32_t stack_phy_base_low;
    uint32_t stack_phy_base_high;

    /* word6 */
    uint16_t aic_pmg : 2;
    uint16_t aic_ns : 1;
    uint16_t aic_part_id : 8;
    uint16_t pi_mix : 1;
    uint16_t aic_qos : 4;
    uint16_t aic_wrr_rd : 3;
    uint16_t aic_wrr_wr : 3;
    uint16_t aic_icache_prefetch_cnt : 5;
    uint16_t aiv_icache_prefetch_cnt : 5;

    /* word7 */
    uint16_t aiv_pmg : 2;
    uint16_t aiv_ns : 1;
    uint16_t aiv_part_id : 8;
    uint16_t res4 : 1;
    uint16_t aiv_qos : 4;
    uint16_t aiv_wrr_rd : 3;
    uint16_t aiv_wrr_wr : 3;
    uint16_t schem : 2;
    uint16_t ratio : 8;

    /* word8-9 */
    uint32_t aic_start_pc_low;
    uint32_t aiv_start_pc_low;

    /* word10 */
    uint16_t aic_start_pc_high;
    uint16_t aiv_start_pc_high;

    /* word11-15 */
    uint32_t aiv_simt_dcu_sm_size;
    uint32_t aic_task_param_ptr_l;
    uint32_t aic_task_param_ptr_h;
    uint32_t aiv_task_param_ptr_l;
    uint32_t aiv_task_param_ptr_h;
} ts_stars_aic_aiv_sqe_t;

typedef struct ts_memcpy_stride00 {
    /* word7 */
    uint16_t dst_stream_id;
    uint16_t dst_sub_stream_id;

    /* word8-9 */
    uint32_t src_addr_low;
    uint32_t src_addr_high;

    /* word10-11 */
    uint32_t dst_addr_low;
    uint32_t dst_addr_high;

    /* word12 */
    uint32_t length_move;

    /* word13-15 */
    uint32_t src_offset_low;
    uint32_t dst_offset_low;
    uint16_t src_offset_high;
    uint16_t dst_offset_high;
} ts_memcpy_stride00_t;

typedef struct ts_memcpy_stride01 {
    /* word7 */
    uint16_t dst_stream_id;
    uint16_t dst_sub_stream_id;

    /* word8-9 */
    uint32_t src_addr_low;
    uint32_t src_addr_high;

    /* word10-11 */
    uint32_t dst_addr_low;
    uint32_t dst_addr_high;

    /* word12 */
    uint32_t length_move;

    /* word13-15 */
    uint32_t src_stride_length;
    uint32_t dst_stride_length;
    uint32_t stride_num;
} ts_memcpy_stride01_t;

typedef struct ts_memcpy_stride10 {
    /* word7 */
    uint16_t num_outer;
    uint16_t num_inner;

    /* word8-9 */
    uint32_t src_addr_low;
    uint32_t src_addr_high;

    /* word10-11 */
    uint32_t stride_outer;
    uint32_t stride_inner;

    /* word12 */
    uint32_t length_inner;

    /* word13-15 */
    uint32_t reserved[3];
} ts_memcpy_stride10_t;

typedef struct ts_david_stars_memcpy_sqe {
    /* word0-1 */
    ts_stars_sqe_header_t header;

    /* word2 */
    uint32_t res1;

    /* word3 */
    uint16_t res2;
    uint8_t kernel_credit;
    uint8_t res3;

    /* word4 */
    uint32_t opcode : 8;
    uint32_t sssv : 1;
    uint32_t dssv : 1;
    uint32_t sns : 1;
    uint32_t dns : 1;
    uint32_t sro : 1;
    uint32_t dro : 1;
    uint32_t stride : 2;
    uint32_t ie2 : 1;
    uint32_t comp_en : 1;
    uint32_t va_valid : 1;
    uint32_t res4 : 13;

    /* word5 */
    uint16_t sqe_id;
    uint8_t mapam_part_id;
    uint8_t mpamns : 1;
    uint8_t pmg : 2;
    uint8_t qos : 4;
    uint8_t d2d_offset_flag : 1; // use reserved filed

    /* word6 */
    uint16_t src_stream_id;
    uint16_t src_sub_stream_id;

    /* word7-15 */
    union {
        ts_memcpy_stride00_t stride_mode0;
        ts_memcpy_stride01_t stride_mode1;
        ts_memcpy_stride10_t stride_mode2;
    } u;
} ts_david_stars_memcpy_sqe_t;

typedef struct ts_stars_notify_sqe {
    /* word0-1 */
    ts_stars_sqe_header_t header;

    /* word2 */
    uint32_t notify_id : 17;
    uint32_t res2 : 13;
    uint32_t cnt_flag : 1;
    uint32_t clr_flag : 1;

    /* word3 */
    uint16_t sub_type; // This field is reserved and used by software.
    uint8_t kernel_credit;
    uint8_t res4 : 5;
    uint8_t sqe_length : 3;

    /* word4 */
    uint32_t cnt_value;

    /* word5 */
    uint32_t wait_mode_bit : 2;   // bit 0:equal, bit 1:bigger
    uint32_t record_mode_bit : 3; // bit 0:add, bit 1:write, bit 2:clear
    uint32_t bitmap : 1;          // 1: wait bitmap mode, priority is higher than wait_mode_bit
    uint32_t res5 : 26;

    /* word6 */
    uint32_t timeout; // This field is reserved and used by software.

    /* word7 */
    uint32_t exe_result; // for Two-phase operator

    /* word8 */
    uint32_t res7[8];
} ts_stars_notify_sqe_t;

typedef struct ts_stars_ubdma_db_sqe {
    /* word0-1 */
    ts_stars_sqe_header_t header;

    /* word2 */
    uint16_t mode : 1;
    uint16_t doorbell_num : 2;
    uint16_t res0 : 13;
    uint16_t res1;

    /* word3 */
    uint16_t res2;
    uint8_t kernel_credit;
    uint8_t res3 : 5;
    uint8_t sqe_length : 3;

    /* word4 */
    uint32_t jetty_id1 : 16;
    uint32_t res4 : 9;
    uint32_t func_id1 : 7;

    /* word5 */
    uint16_t pi_value1;
    uint16_t res5 : 15;
    uint16_t die_id1 : 1;

    /* word6 */
    uint32_t jetty_id2 : 16;
    uint32_t res6 : 9;
    uint32_t func_id2 : 7;

    /* word7 */
    uint16_t pi_value2;
    uint16_t res7 : 15;
    uint16_t die_id2 : 1;

    /* word8-15 */
    uint32_t res8[8];
} ts_stars_ubdma_db_sqe_t;

typedef struct tag_ts_host_func_call_back_info {
    uint16_t cb_cq_id;
    uint16_t cb_group_id;
    uint16_t dev_id;
    uint16_t stream_id;

    /* word6-7 */
    uint16_t event_id;
    uint16_t is_block;
    uint16_t task_id;
    uint16_t res1;

    /* word8-11 */
    uint32_t hostfunc_addr_low;
    uint32_t hostfunc_addr_high;
    uint32_t fndata_low;
    uint32_t fndata_high;
} ts_host_func_call_back_info_t;

typedef struct ts_stars_ubdma_direct_wqe_sqe {
    /* word0-1 */
    ts_stars_sqe_header_t header;

    /* word2 */
    uint16_t mode : 1;
    uint16_t die_id : 1;
    uint16_t res1 : 13;
    uint16_t wqe_size : 1;
    uint16_t res2;

    /* word3 */
    uint16_t res3;
    uint8_t kernel_credit;
    uint8_t res4 : 5;
    uint8_t sqe_length : 3;

    /* word4 */
    uint32_t jetty_id : 16;
    uint32_t res5 : 9;
    uint32_t func_id : 7;

    /* word5-15 */
    uint32_t res6[11];
} ts_stars_ubdma_direct_wqe_sqe_t;

typedef struct ts_stars_jpegd_sqe {
    /* word0-1 */
    ts_stars_sqe_header_t header; // stars sqe header

    /* word2 */
    uint32_t cmdBufSize;

    /* word3 */
    uint16_t res1;
    uint8_t kernel_credit;
    uint8_t res2 : 1;
    uint8_t aicpu_task_pos : 7;

    /* word4-15 */
    uint32_t res[12];
} ts_stars_jpegd_sqe_t;

typedef struct ts_stars_vpc_sqe {
    /* word0-1 */
    ts_stars_sqe_header_t header; // stars sqe header

    /* word2 */
    uint32_t cmdBufSize;

    /* word3 */
    uint16_t res1;
    uint8_t kernel_credit;
    uint8_t res2 : 1;
    uint8_t error_times : 2; // ts defined field, record task error times for vpc task exception
    uint8_t post_p_bak : 2;  // ts defined field, back post_p flag on exception
    uint8_t res3 : 3;

    /* word4-15 */
    uint32_t res[12];
} ts_stars_vpc_sqe_t;

typedef struct stars_aicpu_sqe {
    /* word0-1 */
    ts_stars_sqe_header_t header; // stars sqe header

    /* word2 */
    uint16_t res0;
    uint16_t kernel_type : 7;
    uint16_t batch_mode : 1;
    uint16_t topic_type : 4;
    uint16_t qos : 3;
    uint16_t res1 : 1;

    /* word3 */
    uint16_t sqe_index;
    uint16_t kernel_credit : 8;
    uint16_t res2 : 4;
    uint16_t sqe_length : 3;

    /* word4-5 */
    uint32_t task_so_addr_low;
    uint32_t task_so_addr_high : 16;
    uint32_t post_p_bak : 2;
    uint32_t type_bak : 6;
    uint32_t res3 : 8;

    /* word6-7 */
    uint32_t param_addr_low;
    uint32_t param_addr_high : 16;
    uint32_t res4 : 16;

    /* word8-9 */
    uint32_t task_name_str_ptr_low;
    uint32_t task_name_str_ptr_high : 16;
    uint32_t res5 : 16;

    /* word10-11 */
    uint32_t p_l2ctrl_low;       // david no use
    uint32_t p_l2ctrl_high : 16; // use for aicpu Software decoding
    uint32_t overflow_en : 1;
    uint32_t res6 : 15;

    /* word12-13 */
    uint32_t extra_field_low; // send task id info to aicpu
    uint32_t extra_field_high;

    /* word14 */
    uint32_t sub_topic_id : 12;
    uint32_t topic_id : 6;
    uint32_t group_id : 6;
    uint32_t usr_data_len : 8;

    /* word15 */
    uint32_t dest_pid;
} ts_stars_aicpu_sqe_t;

typedef struct stars_get_float_status_sqe {
    ts_stars_sqe_header_t header;

    uint8_t conds_sub_type; // use reserved filed
    uint16_t reserved0;
    uint8_t reserved1 : 7;
    uint8_t csc : 1; // use reserved filed
    uint16_t reserved2;
    uint8_t kernel_credit;
    uint8_t reserved3 : 4;
    uint8_t debug_flag : 1;
    uint8_t sqe_length : 3; // use reserved filed

    ts_stars_cond_op_loadimm_t ldi;
    ts_stars_cond_op_LLWI_t llwi;
    ts_stars_cond_op_store_t sd_overflow_cnt;
    ts_stars_cond_op_store_t sd_zero[7];
} ts_stars_get_float_status_sqe_t;
#endif

typedef struct ts_stars_memcpy_async_sqe {
    uint8_t type : 6;
    uint8_t res0 : 2;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res1 : 1;

    uint16_t res2;
    /********4 bytes**********/

    uint16_t rt_stream_id;
    uint16_t task_id;
    /********8 bytes**********/

    uint32_t res3;
    /********12 bytes**********/

    uint16_t res4; // max_retry(u8) retry_cnt(u8)
    uint8_t kernel_credit;
    uint8_t ptr_mode : 1;
    uint8_t res5 : 7;
    /********16 bytes**********/

    uint32_t opcode : 8;
    uint32_t ie2 : 1;
    uint32_t sssv : 1;
    uint32_t dssv : 1;
    uint32_t sns : 1;
    uint32_t dns : 1;
    uint32_t qos : 4;
    uint32_t sro : 1;
    uint32_t dro : 1;
    uint32_t partid : 8;
    uint32_t mpam : 1;
    uint32_t d2d_offset_flag : 1;
    uint32_t res6 : 3;
    /********20 bytes**********/

    uint16_t src_streamid;
    uint16_t src_sub_streamid;
    uint16_t dst_streamid;
    uint16_t dst_sub_streamid;
    /********28 bytes**********/

    uint32_t length;
    uint32_t src_addr_low;
    uint32_t src_addr_high;
    uint32_t dst_addr_low;
    uint32_t dst_addr_high;

    uint32_t src_offset_low;
    uint32_t dst_offset_low;
    uint16_t src_offset_high;
    uint16_t dst_offset_high;
    uint32_t res_last[1];
} ts_stars_memcpy_async_sqe_t;

typedef struct ts_stars_memcpy_ptr_async_sqe {
    uint8_t type : 6;
    uint8_t res0 : 2;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res1 : 1;

    uint16_t res2;
    /********4 bytes**********/

    uint16_t rt_stream_id;
    uint16_t task_id;
    /********8 bytes**********/

    uint32_t res3;
    /********12 bytes**********/

    uint16_t res4; // max_retry(u8) retry_cnt(u8)
    uint8_t kernel_credit;
    uint8_t ptr_mode : 1;
    uint8_t res5 : 7;
    /********16 bytes**********/

    uint32_t sdma_sqe_base_addr_low;
    uint32_t sdma_sqe_base_addr_high : 17;
    uint32_t res6 : 14;
    uint32_t va : 1;
    uint32_t res_last[10];
} ts_stars_memcpy_ptr_async_sqe_t;

typedef struct ts_stars_pciedma_sqe {
    /* word0~1 */
    ts_stars_sqe_header_t header;

    /* word2 */
    uint32_t res0;

    /* word3 */
    uint16_t res1;
    uint16_t kernel_credit : 8;
    uint16_t res2 : 8;

    /* word4 */
    uint32_t sq_addr_low;

    /* word5 */
    uint32_t sq_addr_high;

    /* word6 */
    uint16_t sq_tail_ptr;
    uint16_t res3;

    /* word7 */
    uint32_t is_converted : 1;
    uint32_t is_dsa_update : 1;
    uint32_t is_sqe_update : 1;
    uint32_t offset : 8;
    uint32_t res4 : 21;
    /* word8~11 */
    uint64_t src;
    uint64_t dst;
    /* word12-15 */
    uint64_t length;
    uint32_t passid;
    uint32_t res5;
} ts_stars_pciedma_sqe_t;

typedef struct ffts_sqe {
    uint8_t type : 6;
    uint8_t l2_lock : 1;
    uint8_t l2_unlock : 1;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t reserved : 1;
    uint16_t reserved1;

    uint16_t stream_id;
    uint16_t task_id;

    uint16_t ffts_type : 3;
    uint16_t reserved2 : 9;
    uint16_t wrr_ratio : 4;
    uint16_t reserved3;

    uint16_t sqe_index;
    uint16_t kernel_credit : 8;
    uint16_t reserved4 : 8;

    uint32_t reserved5[4];
    uint32_t task_start_pc_l;
    uint16_t task_start_pc_h;
    uint16_t reserved6;
    uint32_t task_param_ptr_l;
    uint32_t task_param_ptr_h;
    uint32_t reserved7[4];
} ts_ffts_sqe_t;

typedef struct ts_stars_callback_sqe {
    /* word0-1 */
    uint8_t type : 6;
    uint8_t l1_lock : 1;
    uint8_t l1_unlock : 1;

    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t reserved : 1;

    uint16_t num_blocks; // num_blocks or res

    uint16_t rt_stream_id;
    uint16_t header_task_id;

    /* word2 */
    uint16_t res0;
    uint16_t kernel_type : 7;
    uint16_t batch_mode : 1;
    uint16_t topic_type : 4;
    uint16_t qos : 3;
    uint16_t res1 : 1;

    /* word3 */
    uint16_t sqe_index;
    uint16_t kernel_credit : 8;
    uint16_t res2 : 8;

    /* word4-5 */
    uint16_t cb_cq_id;
    uint16_t cb_group_id;
    uint16_t dev_id;
    uint16_t stream_id;

    /* word6-7 */
    uint16_t event_id;
    uint16_t is_block;
    uint16_t task_id;
    uint16_t res4;

    /* word8-11 */
    uint32_t hostfunc_addr_low;
    uint32_t hostfunc_addr_high;
    uint32_t fndata_low;
    uint32_t fndata_high;

    /* word12-13 */
    uint32_t res5; // only vf & topic AICPU & callback use for hostpid.
    uint32_t res6;

    /* word14 */
    uint32_t sub_topic_id : 12;
    uint32_t topic_id : 6;
    uint32_t group_id : 6;
    uint32_t usr_data_len : 8;

    /* word15 */
    uint32_t dest_pid;
} ts_stars_callback_sqe_t;

#define TOPIC_SCHED_USER_DATA_LEN (40U)

typedef struct ts_stars_topic_sched_sqe {
    /* word0-1 */
    uint8_t type : 6;
    uint8_t l1_lock : 1;
    uint8_t l1_unlock : 1;

    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t reserved : 1;

    uint16_t num_blocks;

    uint16_t rt_stream_id;
    uint16_t header_task_id;

    /* word2 */
    uint16_t res0;
    uint16_t kernel_type : 7;
    uint16_t batch_mode : 1;
    uint16_t topic_type : 4;
    uint16_t qos : 3;
    uint16_t dest_pid_flag : 1;

    /* word3 */
    uint16_t sqe_index;
    uint16_t kernel_credit : 8;
    uint16_t res1 : 8;

    /* word4-13 */
    /* user_data format:
      "user msg" + 2Byte devid(if devid_flag == 1) + 2Byte tid(if tid_flag == 1) + 4Byte pid(if dest_pid_flag == 1) */
    uint8_t user_data[TOPIC_SCHED_USER_DATA_LEN];

    /* word14 */
    uint32_t sub_topic_id : 12;
    uint32_t topic_id : 6;
    uint32_t group_id : 6;
    uint32_t usr_data_len : 6;
    uint32_t devid_flag : 1;
    uint32_t tid_flag : 1;

    /* word15 */
    uint32_t dest_pid;
} ts_stars_topic_sched_sqe_t;

typedef struct ts_stars_write_value_sqe {
    uint8_t type : 6;
    uint8_t res0 : 2;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res1 : 1;

    uint16_t res2;
    /********4 bytes**********/

    uint16_t rt_stream_id;
    uint16_t task_id;
    /********8 bytes**********/

    uint32_t res3;

    uint32_t res4 : 16;
    uint32_t kernel_credit : 8;
    uint8_t ptr_mode : 1;
    uint32_t res5 : 7;

    uint32_t write_addr_low;

    uint32_t write_addr_high : 17;
    uint32_t res6 : 3;
    uint32_t awsize : 3;
    uint32_t snoop : 1;
    uint32_t awcache : 4;
    uint32_t awprot : 3;
    uint32_t va : 1; // 1: virtual address; 0: phy addr

    uint32_t res7; // event id for event reset
    uint32_t sub_type;

    uint32_t write_value_part0;
    uint32_t write_value_part1;
    uint32_t write_value_part2;
    uint32_t write_value_part3;
    uint32_t write_value_part4;
    uint32_t write_value_part5;
    uint32_t write_value_part6;
    uint32_t write_value_part7;
} ts_stars_write_value_sqe_t;

typedef struct ts_stars_ffts_plus_sqe_header {
    uint8_t type : 6;
    uint8_t l1_lock : 1;
    uint8_t l1_unlock : 1;

    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    /* tell mcu if this subgraph is overflow-enabled and mcu will send this flag to aicpu when aicpu ctx is executed */
    uint8_t overflow_en : 1;

    uint16_t num_blocks;
    uint16_t rt_stream_id;
    uint16_t task_id;
} ts_stars_ffts_plus_sqe_header_t;

typedef struct ts_ffts_plus_sqe {
    // 0-7 bytes
    ts_stars_ffts_plus_sqe_header_t header;

    // 8-11 bytes
    uint16_t ffts_type : 3;
    uint16_t cmo : 1;
    uint16_t schedule_dfx_flag : 1;
    uint16_t res3 : 7;
    uint16_t wrr_ratio : 4;
    uint16_t dsa_sq_id : 11; // must be no-zero when valid
    uint16_t res4 : 5;
    // 12-15 bytes
    uint16_t sqe_index;
    uint8_t kernel_credit;
    uint8_t sub_type;
    // 16-23 bytes
    uint32_t stack_phy_base_l;
    uint32_t stack_phy_base_h;
    // 24-31 bytes
    uint16_t total_context_num;
    uint16_t ready_context_num;
    uint16_t preload_context_num;
    uint16_t timeout;
    // 32-35 bytes
    uint16_t res7;
    uint16_t prefetch_ost_num : 5;
    uint16_t res8 : 3;
    uint16_t cmaint_ost_num : 5;
    uint16_t res9 : 3;
    // 36-39 bytes
    uint16_t aic_prefetch_lower : 5;
    uint16_t res10 : 3;
    uint16_t aic_prefetch_upper : 5;
    uint16_t res11 : 3;
    uint16_t aiv_prefetch_lower : 5;
    uint16_t res12 : 3;
    uint16_t aiv_prefetch_upper : 5;
    uint16_t res13 : 3;
    // 40-47 bytes
    uint32_t context_address_base_l;
    uint32_t context_address_base_h : 17;
    uint32_t res14 : 15;

    // 48-51 bytes
    uint32_t pid;
    // 52-63 bytes
    uint32_t res15[3];
} ts_ffts_plus_sqe_t;

typedef struct ts_stars_cmo_sqe {
    ts_stars_sqe_header_t header;
    /********8 ~ 15 bytes**********/
    uint16_t ffts_type : 3;
    uint16_t cmo : 1;
    uint16_t res1 : 8;
    uint16_t wrr_ratio : 4;
    uint16_t res2;
    uint16_t res3;
    uint16_t kernel_credit : 8;
    uint16_t schem : 2;
    uint16_t res4 : 1;
    uint16_t icache_prefetch_cnt : 5;
    /********16 ~ 31 bytes**********/
    uint16_t cmo_type;
    uint16_t cmo_id;
    uint32_t res5;
    uint32_t res6;
    uint32_t res7;
    /********32 ~ 35 bytes**********/
    uint32_t opcode : 8;
    uint32_t ie2 : 1;
    uint32_t sssv : 1;
    uint32_t dssv : 1;
    uint32_t sns : 1;
    uint32_t dns : 1;
    uint32_t qos : 4;
    uint32_t sro : 1;
    uint32_t dro : 1;
    uint32_t part_id : 8;
    uint32_t mpam : 1;
    uint32_t pmg : 2;
    uint32_t format : 1;
    uint32_t res8 : 1;
    /********36 ~  63 bytes**********/
    uint16_t src_streamid;
    uint16_t src_sub_streamid;
    uint16_t num_outer;
    uint16_t num_inner;
    uint32_t length;
    uint32_t src_addr_low;
    uint32_t src_addr_high;
    uint32_t stride_outer;
    uint32_t stride_inner;
} ts_stars_cmo_sqe_t;

typedef struct ts_stars_cdqm_sqe {
    ts_stars_sqe_header_t header;

    uint32_t sqe_index : 16;
    uint32_t res0 : 16;
    uint32_t res1 : 16;
    uint32_t kernel_credit : 8;
    uint32_t ptr_mode : 1;
    uint32_t res3 : 7;

    uint32_t cdqe_addr_low;
    uint32_t cdqe_addr_high : 17;
    uint32_t res4 : 14;
    uint32_t va : 1;

    uint32_t cdqe_index : 16;
    uint32_t cdq_id : 16;

    uint32_t die_id : 16;
    uint32_t res5 : 16;

    uint8_t cdqe[15]; /* word8-11 cdqe */
    uint8_t res6 : 7;
    uint8_t ready : 1;
    uint32_t res7[4];
} ts_stars_cdqm_sqe_t;

typedef struct ts_stars_dsa_sqe {
    // 0-7 bytes
    ts_stars_sqe_header_t header;
    // 8-11 bytes
    uint32_t start : 1;
    uint32_t function_type : 3;
    uint32_t data_type : 3;
    uint32_t algo_type : 3;
    uint32_t param_vld_bitmap : 5;
    uint32_t param_addr_val_bitmap : 7;
    uint32_t res0 : 10;
    // 12-15 bytes
    uint16_t sqe_index;
    uint8_t kernel_credit;
    uint8_t res1;
    // 16-31 bytes
    uint32_t cfg_result_addr_low;
    uint32_t cfg_result_addr_high;
    uint32_t cfg_state_addr_low;
    uint32_t cfg_state_addr_high;
    // 32-47 bytes
    uint32_t cfg_param_addr_low;
    uint32_t cfg_param_addr_high;
    uint32_t cfg_seed_low;
    uint32_t cfg_seed_high;
    // 48-55 bytes
    uint32_t cfg_number_low;
    uint32_t cfg_number_high;

    // 56-63 bytes only used for ffts+ dsa
    uint16_t ctx_id;
    uint16_t slot_id : 4;
    uint16_t err_bit : 1;
    uint16_t res2 : 11;
    uint16_t thread_id;
    uint16_t res3;
} ts_stars_dsa_sqe_t;

typedef struct ts_stars_cqe {
    uint16_t phase : 1;
    uint16_t warn : 1; /* process warning */
    uint16_t evt : 1;  /* event record flag */
    uint16_t place_hold : 1;
    uint16_t sq_id : 11;
    uint16_t error_bit : 1;
    uint16_t sq_head;
    uint16_t stream_id;
    uint16_t task_id;
    uint16_t err_type : 8;
    uint16_t drop_flag : 1; /* software defined, means drop hw cqe and no dispatch to logic cq */
    uint16_t rsv : 1;
    uint16_t sqe_type : 6;
    uint16_t sq_index;
    uint32_t status;
} ts_stars_cqe_t;

#define MAX_CMO_INFO_NUM (6U)
typedef struct ts_stars_cmo_info {
    uint16_t cmo_type;
    uint16_t cmo_id;
} ts_stars_cmo_info_t;

typedef struct ts_stars_barrier_sqe {
    ts_stars_sqe_header_t header;
    /********8 ~ 15 bytes**********/
    uint16_t ffts_type : 3;
    uint16_t cmo : 1;
    uint16_t res1 : 8;
    uint16_t wrr_ratio : 4;
    uint16_t res2;
    uint16_t sqe_index;
    uint16_t kernel_credit : 8;
    uint16_t schem : 2;
    uint16_t res3 : 1;
    uint16_t icache_prefetch_cnt : 5;
    /********16 ~ 43 bytes**********/
    uint16_t cmo_type;
    uint16_t cmo_bitmap : 6;
    uint16_t res4 : 10;
    ts_stars_cmo_info_t cmo_info[MAX_CMO_INFO_NUM];
    /********44 ~ 63 bytes**********/
    uint32_t res5[5];
} ts_stars_barrier_sqe_t;

#if defined(CFG_SOC_PLATFORM_KPSTARS)
typedef struct ts_stars_rdma_sqe {
    /* header */
    uint8_t type : 6;
    uint8_t res0 : 2;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res1 : 1;
    uint16_t res2;
    uint16_t rt_stream_id;
    uint16_t task_id;
    uint32_t res3;
    uint16_t res4;
    uint8_t kernel_credit;
    uint8_t reserved0 : 7;
    uint8_t clr : 1;

    /* payload */
    uint32_t rdma_qp_addr_low;
    uint32_t rdma_qp_addr_high;
    uint32_t tag : 24;
    uint32_t cmd : 4;
    uint32_t reserved1 : 3;
    uint32_t flag : 1;
    uint32_t param;
    uint32_t wait_cqe_num : 16;
    uint32_t hac_func_id : 8;
    uint32_t qp_va : 1;
    uint32_t ssv : 1;
    uint32_t reserved2 : 6;
    uint32_t rdma_smmu_stream_id : 16;
    uint32_t rdma_smmu_substream_id : 16;
    uint32_t res_last[6];
} ts_stars_rdma_sqe_t;

typedef struct ts_stars_sdma_sqe {
    /* header */
    uint8_t type : 6;
    uint8_t res0 : 2;
    uint8_t ie : 2;
    uint8_t pre_p : 2;
    uint8_t post_p : 2;
    uint8_t wr_cqe : 1;
    uint8_t res1 : 1;
    uint16_t res2;
    uint16_t rt_stream_id;
    uint16_t task_id;
    uint32_t res3;
    uint16_t res4;
    uint8_t kernel_credit;
    uint8_t reserved0 : 7;
    uint8_t clr : 1;

    /* payload */
    /* dw0 */
    uint32_t opcode : 8;
    uint32_t sssv : 1;
    uint32_t dssv : 1;
    uint32_t sns : 1;
    uint32_t dns : 1;
    uint32_t sro : 1;
    uint32_t dro : 1;
    uint32_t stride : 2;
    uint32_t ie1 : 1;
    uint32_t comp_en : 1;
    uint32_t reserved1 : 14;
    /* dw1 */
    uint32_t sqe_id : 16;
    uint32_t mpam_partid : 8;
    uint32_t mpamns : 1;
    uint32_t pmg : 2;
    uint32_t qos : 4;
    uint32_t reserved2 : 1;
    /* dw2 */
    uint32_t src_streamid : 16;
    uint32_t src_substreamid : 16;
    /* dw3 */
    uint32_t dst_streamid : 16;
    uint32_t dst_substreamid : 16;
    /* dw4 dw5 */
    uint32_t src_addr_l;
    uint32_t src_addr_h;
    /* dw6 dw7 */
    uint32_t dst_addr_l;
    uint32_t dst_addr_h;
    /* dw8 */
    uint32_t length_move : 32;
    /* dw9 dw10 dw11 */
    uint32_t src_stride_len : 32;
    uint32_t dst_stride_len : 32;
    uint32_t stride_num : 32;
} ts_stars_sdma_sqe_t;
#endif
typedef struct ts_stars_aic_sqe_t {
    ts_stars_sqe_header_t header;
    uint16_t ffts_type : 3;
    uint16_t res1 : 9;
    uint16_t wrr_ratio : 4;
    uint16_t res2;
    uint16_t sqe_index;
    uint16_t kernel_credit : 8;
    uint16_t schem : 2;
    uint16_t res3 : 1;
    uint16_t icache_prefetch_cnt : 5;
    uint32_t stack_phy_base_low;
    uint32_t stack_phy_base_high;
    uint32_t res4;
    uint32_t pmg : 2;
    uint32_t ns : 1;
    uint32_t part_id : 8;
    uint32_t res5 : 1;
    uint32_t qos : 4;
    uint32_t res6 : 16;
    uint32_t pc_addr_low;
    uint32_t pc_addr_high : 16;
    uint32_t res7 : 16;
    uint32_t param_addr_low;
    uint32_t param_addr_high;
    // use res8[1] bit 4 for l2cache
    uint32_t res8[4];
} ts_stars_aic_sqe_t;

#pragma pack(pop)
/* stars sqe struct end */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* TS_TASK_STRUCT_H */
