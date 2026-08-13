/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef TS_TASK_SCHEDULER_ERROR_H
#define TS_TASK_SCHEDULER_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @ingroup tsch
 * @brief the return value of task scheduler module
 */
typedef enum tag_ts_error {
    TS_SUCCESS = 0x0,                                    /**< success */
    TS_ERROR_UNKNOWN = 0x1,                              /**< unknown error */
    TS_ERROR_NULL_PTR = 0x2,                             /**< NULL ptr */
    TS_ERROR_ILLEGAL_AI_CORE_ID = 0x3,                   /**< illegal ai core id */
    TS_ERROR_ILLEGAL_PARAM = 0x4,                        /**< illegal param */
    TS_ERROR_TASK_CMD_QUEUE_FULL = 0x5,                  /**< task cmd queue is full */
    TS_ERROR_TASK_CMD_QUEUE_EMPTY = 0x6,                 /**< task cmd queue is empty */
    TS_ERROR_TASK_REPORT_QUEUE_FULL = 0x7,               /**< task report queue is full */
    TS_ERROR_TASK_REPORT_QUEUE_EMPTY = 0x8,              /**< task report queue is empty */
    TS_ERROR_TASK_NODE_BUFF_ALL_OCCUPYED = 0x9,          /**< all the node of public task buff are occupyed */
    TS_ERROR_TASK_NODE_BUFF_ALL_FREED = 0xA,             /**< all the node of public task buff are free */
    TS_ERROR_L2_MEM_INSUFFICIENT_SPACE = 0xB,            /**< there is not enough space on l2 memory */
    TS_ERROR_L2_MALLOC_FAILED = 0xC,                     /**< malloc l2 failed */
    TS_ERROR_DMA_CHANNEL_ALL_OCCUPYED = 0xD,             /**< all the dma channel are occupyed */
    TS_ERROR_MEMCPY_OP_FAILED = 0xE,                     /**< memcpy failed */
    TS_ERROR_BS_SLOT_ALL_OCCUPYED = 0xF,                 /**< there is not free bs slot for new task */
    TS_ERROR_TBS_SLOT_REPEAT_FREE = 0x10,                /**< one free bs slot cannot be freed again */
    TS_ERROR_PRIORITY_TASK_LIST_FULL = 0x11,             /**< the priority list is full */
    TS_ERROR_PRIORITY_TASK_LIST_EMPTY = 0x12,            /**< the priority list is empty */
    TS_ERROR_NO_STREAM_LIST_NEED_TO_BE_PROCESSED = 0x13, /**< there is no stream list need service */
    TS_ERROR_REPEAT_MARK_STREAM_NEED_SERVICE = 0x14,     /**< repeat mark the stream list need service */
    TS_ERROR_SYS_DMA_CHANNEL_ALL_OCCUPAPYED = 0x15,      /**< system dma channel all occupapyed */
    TS_ERROR_NO_HBML2TASKNODE_FOUND = 0x16,              /**< no hbm l2 task node found */
    TS_ERROR_SQNODE_NODE_SLOT_ALL_OCCUPAPYED = 0x17,     /**< all the node of the current sq are occupyed */
    TS_ERROR_CQNODE_NODE_SLOT_ALL_OCCUPAPYED = 0x18,     /**< all the node of the current sq are occupyed */
    TS_ERROR_SQNODE_NOT_ENOUGH = 0x19,                   /**< the sq node is not enough for data transfer */
    TS_ERROR_SQNODE_SLOT_REPEAT_FREE = 0x1A,             /**< sq node slot repeat free */
    TS_ERROR_CQNODE_SLOT_REPEAT_FREE = 0x1B,             /**< cq node slot repeat free */
    TS_ERROR_CQ_REPORT_FAILED = 0x1C,                    /**< cq report failed */
    TS_SYS_DMA_RESET_SUCCESS = 0x1D,                     /**< sys dma reset success */
    TS_SYS_DMA_RESET_FAILED = 0x1E,                      /**< sys dma reset failed */
    TS_SYS_DMA_TRNSFER_FAILED = 0x1F,                    /**< sys dma transfer failed */
    TS_SYS_DMA_MEMADDRALIGN_FAILED = 0x20,               /**< sys dma mem address align failed */
    TS_SYS_DMA_ERROR_QUEUE_FULL = 0x21,                  /**< sys dma queue full */
    TS_SYS_DMA_ERROR_QUEUE_EMPTY = 0x22,                 /**< sys dma queue empty */
    TS_ERROR_TIMER_EVENT_FULL = 0x23,                    /**< timer event full */
    TS_TASK_L2_DESC_ENTRY_NOT_ENOUGH = 0x24,             /**< l2 description enter not enough */
    TS_ERROR_AICORE_TIMEOUT = 0x25,                      /**< aicore timeout */
    TS_ERROR_AICORE_EXCEPTION = 0x26,                    /**< aicore exception */
    TS_ERROR_AICORE_TRAP_EXCEPTION = 0x27,               /**< aicore trap exception */
    TS_ERROR_AICPU_TIMEOUT = 0x28,                       /**< aicpu timeout */
    TS_ERROR_SDMA_L2_TO_DDR_MALLOC_FAIL = 0x29,          /**< sdma move l2 to ddr malloc failed */
    TS_ERROR_AICPU_EXCEPTION = 0x2A,                     /**< aicpu exception */
    TS_ERROR_AICPU_DATADUMP_RSP_ERR = 0x2B,              /**< aicpu datadump response error */
    TS_ERROR_AICPU_MODEL_RSP_ERR = 0x2C,                 /**< aicpu model operate response error */
    TS_ERROR_REPEAT_ACTIVE_MODEL_STREAM = 0x2D,          /**< repeat active model stream */
    TS_ERROR_REPEAT_NOTIFY_WAIT = 0x2E,                  /**< repeat notify wait */
    TS_ERROR_AICORE_OVERFLOW = 0x2F,                     /**< aicore over flow error */
    TS_ERROR_VECTOR_CORE_TIMEOUT = 0x30,                 /**< vector core timeout */
    TS_ERROR_VECTOR_CORE_EXCEPTION = 0x31,               /**< vector core exception */
    TS_ERROR_VECTOR_CORE_TRAP_EXCEPTION = 0x32,          /**< vector core trap exception */
    TS_ERROR_AIVEC_OVERFLOW = 0x33,                      /**< aivec over flow error */
    TS_ERROR_AICPU_OVERFLOW = 0x34,                      /**< aicpu over flow error */
    TS_ERROR_SDMA_OVERFLOW = 0x35,                       /**< sdma over flow error */
    TS_ERROR_NORMAL_SQCQ_EXISTED = 0x36,                 /**< sq or cq id is existed*/
    TS_ERROR_SQ_CQ_INVALID = 0x37,                       /**< sq or cq invalid */
    TS_ERROR_SQ_RANGE_INVALID = 0x38,                    /**< sq range invalid */
    TS_ERROR_IRQ_INVALID = 0x39,                         /**< irq id invalid */
    TS_ERROR_LOGIC_CQID_INVALID = 0x40,                  /**< logic cqid invalid */
    TS_ERROR_APPFLAG_INVALID = 0x41,                     /**< app flag invalid */
    TS_ERROR_PHY_CQID_INVALID = 0x42,                    /**< physical cq id invalid */
    TS_ERROR_VFID_NOT_ZERO = 0x43,                       /**< vfid must be 0(physical vm) */
    TS_ERROR_VFID_NOT_RANGE = 0x44,                      /**< vfid not range(virtual vm) */
    TS_ERROR_STREAMID_INVALID = 0x45,                    /**< stream id invalid */
    TS_ERROR_VFGID_INVALID = 0x46,                       /**< vfgroup id invalid */
    TS_ERROR_AICORENUM_INVALID = 0x47,                   /**< aicore num invalid(virtual vm) */
    TS_ERROR_VM_IS_WORKING = 0x48,                       /**< vm is working(virtual vm) */
    TS_ERROR_VM_IS_IDLE = 0x49,                          /**< vm is idle(virtual vm) */
    TS_ERROR_VM_RESOURCE_CONFLICT = 0x50,                /**< vm resource conflict(virtual vm) */
    TS_ERROR_OPER_TYPE_INVALID = 0x51,                   /**< operation type invalid(must be 1 or 0) */
    TS_ERROR_RESOURCE_TYPE_NOT_SUPPORT = 0x52,           /**< resource_type not support */
    TS_ERROR_VIR_ID_INVALID = 0x53,                      /**< vir id invalid */
    TS_ERROR_PHY_ID_INVALID = 0x54,                      /**< phy id invalid */
    TS_ERROR_SQ_CTRL_FLAG_INVALID = 0x55,                /**< sq ctrl flag invalid */
    TS_ERROR_APP_COUNT_INVALID = 0x56,                   /**< app count invalid */
    TS_ERROR_AICPU_INFO_LOAD_RSP_ERR = 0x57,             /**< aicpu info load response error */
    TS_ERROR_AICORE_MTE_ERROR = 0x58,                    /**< aicore mte error */
    TS_ERROR_AICPU_ACTIVE_STREAM_FAIL = 0x59,            /**< aicpu active stream error */
    TS_ERROR_SUSPECT_MTE_ERROR = 0x5A,                   /**< suspect mte error */

    TS_ERROR_DEBUG_INVALID_SQCQ = 0x70,             /**< debug invalid sqcq */
    TS_ERROR_DEBUG_WRONG_COMMAND_TYPE = 0x71,       /**< debug wrong command type */
    TS_ERROR_DEBUG_CMD_PROCESS = 0x72,              /**< dbg cmd process */
    TS_ERROR_DEBUG_INVALID_DEVICE_STATUS = 0x73,    /**< debug invalid device status */
    TS_ERROR_DEBUG_NOT_IN_DEBUG_STATUS = 0x74,      /**< debug not in debug status */
    TS_ERROR_DEBUG_INVALID_TASK_STATUS = 0x75,      /**< debug invalid task status */
    TS_ERROR_DEBUG_TASK_EMPTY = 0x76,               /**< debug task empty */
    TS_ERROR_DEBUG_TASK_FULL = 0x77,                /**< debug task full */
    TS_ERROR_DEBUG_TASK_NOT_EXIST = 0x78,           /**< debug task not exist */
    TS_ERROR_DEBUG_AI_CORE_FULL = 0x79,             /**< debug ai core full */
    TS_ERROR_DEBUG_AI_CORE_NOT_EXIST = 0x7A,        /**< debug ai core not exist */
    TS_ERROR_DEBUG_AI_CORE_EXCEPTION = 0x7B,        /**< debug ai core exception */
    TS_ERROR_DEBUG_AI_CORE_TIMEOUT = 0x7C,          /**< debug ai core timeout */
    TS_ERROR_DEBUG_BREAKPOINT_FULL = 0x7D,          /**< debug breakpoint full */
    TS_ERROR_DEBUG_READ_ERROR = 0x7E,               /**< debug read error */
    TS_ERROR_DEBUG_WRITE_FAIL = 0x7F,               /**< debug write fail */
    TS_QUEUE_FULL = 0x80,                           /**< ts queue full */
    TS_QUEUE_EMPTY = 0x81,                          /**< ts queue empty */
    TS_QUEUE_ALLOC_MEM_FAIL = 0x82,                 /**< ts queue alloc mem fail */
    TS_QUEUE_DATA_SIZE_UNMATCH = 0x83,              /**< ts queue data size unmatch */
    TS_PCIE_DMA_INVLD_CPY_TYPE = 0x84,              /**< pcie dma invalid copy type */
    TS_INVLD_CPY_DIR = 0x85,                        /**< invalid copy dir */
    TS_PCIE_DMA_INVLD_CQ_DES = 0x86,                /**< pcie dma invalid cq des */
    TS_PCIE_DMA_CPY_ERR = 0x87,                     /**< pcie dma copy error */
    TS_PCIE_DMA_LNK_CHN_BUSY = 0x88,                /**< pcie dma link channel busy */
    TS_ERROR_PROFILE_BUFF_FULL = 0x89,              /**< profile buff full */
    TS_ERROR_PROFILE_MODE_CONFLICT = 0x8A,          /**< profile mode conflict */
    TS_ERROR_PROFILE_OTHER_PID_ON = 0x8B,           /**< profile other pid on */
    TS_ERROR_SCHD_AIC_TASK_PRELOAD_FAILED = 0x8C,   /**< sched aic task preload failed */
    TS_ERROR_TSCPU_CLOSE_FAILED = 0x8D,             /**< tscpu close failed */
    TS_ERROR_EXPECT_FAIL = 0x8E,                    /**< expect failed */
    TS_ERROR_REPEAT_MODEL_STREAM = 0x8F,            /**< repeat model stream */
    TS_ERROR_STREAM_MODEL_UNBIND = 0x90,            /**< the model stream unbind failed */
    TS_MODEL_STREAM_EXE_FAILED = 0x91,              /**< the model stream execute failed */
    TS_ERROR_IPC_SEND_FAILED = 0x92,                /**< ipc send failed */
    TS_ERROR_IPC_PROC_REG_FAILED = 0x93,            /**< ipc process register failed */
    TS_STREAM_SQ_FULL = 0x94,                       /**< stream sq full */
    TS_ERROR_END_OF_SEQUENCE = 0x95,                /**< end of sequence */
    TS_ERROR_SWITCH_STREAM_LABEL = 0x96,            /**< switch stream label failed */
    TS_ERROR_TASK_TYPE_NOT_SUPPORT = 0x97,          /**< task type not support */
    TS_ERROR_TRANS_SQE_FAIL = 0x98,                 /**< trans sqe fail */
    TS_MODEL_ABORT_NORMAL = 0x99,                   /**< model exit */
    TS_ERROR_RINGBUFFER_NOT_FIND = 0x100,           /**< not find the ringbuffer */
    TS_ERROR_RINGBUFFER_MANAGE_IS_NOT_FREE = 0x101, /**< not find the idle ringbuffer manage */
    TS_ERROR_RINGBUFFER_IS_FULL = 0x102,            /**< the current ringbuffer is full */
    TS_ERROR_TASK_EXCEPTION = 0x103,                /**< task exception */
    TS_ERROR_TASK_TRAP = 0x104,                     /**< task trap exception */
    TS_ERROR_TASK_TIMEOUT = 0x105,                  /**< task timeout */
    TS_ERROR_TASK_SQE_ERROR = 0x106,                /**< task sqe error */
    TS_ERROR_VF_SLOT_ALLOC_FAIL = 0x107,            /**< vf slot alloc fail */
    TS_PCIE_DMA_LIMIT_CONTROL = 0x108,              /**< vf mode pcie copy limit control */
    TS_ERROR_DEBUG_REGISTER_CONFLICT = 0x109,       /**< debug register conflict */
    TS_ERROR_AIC_TRAP_RD_OVERFLOW = 0x10A,          /**< aicore trap read overflow */
    TS_ERROR_AIC_TRAP_WR_OVERFLOW = 0x10B,          /**< aicore trap write overflow */
    TS_ERROR_AIV_TRAP_RD_OVERFLOW = 0x10C,          /**< aivec trap read overflow */
    TS_ERROR_AIV_TRAP_WR_OVERFLOW = 0x10D,          /**< aivec trap write overflow */
    TS_ERROR_MODEL_ABORTED = 0x10E,                 /**< model abort */
    TS_ERROR_FFTSPLUS_TASK_EXCEPTION = 0x10F,       /* ftsplus exception */
    TS_ERROR_FFTSPLUS_TASK_TRAP = 0x110,            /* fftsplus trap exception */
    TS_ERROR_FFTSPLUS_TASK_TIMEOUT = 0x111,         /* fftsplus timeout */
    TS_ERROR_TASK_BUS_ERROR = 0x112,                /* bus error on david */
    TS_ERROR_TASK_RES_CONFLICT_ERROR = 0x113,       /* res conflict error on david */
    TS_ERROR_TASK_SW_STATUS_ERROR = 0x114,          /* sq sw status error on david */
    TS_ERROR_APP_QUEUE_FULL = 0x115,                /* app queue full*/
    TS_ERROR_ABORT_UNFINISHED = 0x116,              /* ts is aborting */
    TS_ERROR_STARS_QOS_ERROR = 0x117,
    // the following error codes are defined for error msg
    TS_ERR_MSG_EXCEED_MAX_SIZE = 0x200,
    TS_ERROR_MALLOC_MEM_FAILED = 0X201,               /**< malloc memory fail, system error */
    TS_ERROR_DELETE_NODE_FAILED = 0X202,              /* delete node from list error */
    TS_ERROR_ONLINE_PROF_ADDR_CANNOT_BE_ZERO = 0X203, /* online profiling addr cannot be zero */
    TS_ERROR_GET_HWTS_TASK_LOG_ERROR = 0X204,         /* not get hwts task log */
    TS_ERROR_KERNEL_DATA_FULL = 0X205,                /* ts kernel data full */
    TS_ERROR_SHARED_BUFF_INVALID = 0x206,             /* shared buffer invalid */
    TS_ERROR_READ_REGISTER_FAILED = 0x207,            /* read register failed */
    TS_ERROR_TS_STATUS_POWER_OFF = 0x208,             /* ts status power off */
    TS_ERROR_PRIVATE_BUFFER_FULL = 0x209,             /* ts private buffer full */
    TS_ERROR_TRANS_ADDR_FAILED = 0x20A,               /* ts trans addr failed */
    TS_ERROR_SDMA_ERROR = 0x20B,
    TS_ERROR_SDMA_TIMEOUT = 0x20C,                   /* hwts sdma timeout */
    TS_ERROR_AICORE_RESET_FAILED = 0x20D,            /* aicore reset failed */
    TS_ERROR_AIV_RESET_FAILED = 0x20E,               /* aiv reset failed */
    TS_ERROR_HWTS_SW_STATUS_ERROR = 0x20F,           /* hwts sw status failed */
    TS_ERROR_HWTS_EXCEPTION = 0x210,                 /* hwts exception */
    TS_ERROR_MEMSET_FAILED = 0x211,                  /* memset failed */
    TS_ERROR_AICPU_BS_DONE_PRE_CHECK_FAILED = 0x212, /* aicpu bs done pre check failed */
    TS_ERROR_GOING_DOWN = 0x213,
    TS_ERROR_HWTS_POOL_CONFLICT = 0x214,        /* hwts pool conflict */
    TS_ERROR_GET_SQ_ID_FAILED = 0x215,          /* get stream sq id failed */
    TS_ERROR_MODEL_END_GRAPH_EXCEPTION = 0x216, /* end graph exception */
    TS_ERROR_SDMA_COPY_ERROR = 0x217,
    TS_ERROR_SDMA_REDUCE_ERROR = 0x218,
    TS_ERROR_ALLOC_STREAM_ID_FAILED = 0x219,     /* alloc stream id failed */
    TS_ERROR_SDMA_DDRC_ERROR = 0x220,            /**< sdma ddrc error */
    TS_ERROR_SDMA_POISON_ERROR = 0x221,          /**< sdma position error */
    TS_ERROR_SDMA_LINK_ERROR = 0x222,            /**< sdma link error */
    TS_ERROR_HCCL_OTHER_ERROR = 0x223,           /**< hccl other error */
    TS_ERROR_AICPU_HCCL_OP_RETRY_FAILED = 0x224, /* aicpu hccl op retry failed */
    TS_ERROR_LOCAL_MEM_ERROR = 0x225,            /*local mem error*/
    TS_ERROR_REMOTE_MEM_ERROR = 0x226,           /*remote mem error*/
    TS_ERROR_GET_PAGES_WRONG = 0x300,            /* get pages wrong */
    // the following error codes are ts inner codes, no need return to runtime
    TS_EXEC_AGAIN = 0x900,                      /**< task exec again */
    TS_PROFILE_CMD_COMPLETED = 0x901,           /**< profiling config completed */
    TS_PROFILE_CMD_FAILED = 0x902,              /**< profiling config failed */
    TS_EXEC_NEW_SCHE = 0x903,                   /**< task need use new slot sche */
    TS_APP_EXIT_UNFINISHED = 0x904,             /**< app exit is still processing */
    TS_ERROR_STREAM_NOT_IN_MODEL = 0x905,       /* target stream is not in model */
    TS_ERROR_MODEL_EXEC_REC_ACT_STREAM = 0x906, /* model is running, but active stream receive */
    TS_APP_EXIT_NO_SEND_CQ = 0x907,             /**< app exit no send cq report */
    TS_STREAM_RECYCLED = 0x908,                 /**< stream is recycled */
    TS_ERROR_TS_CLOSED = 0x909,                 /**< sentinel mode, ts is closed */
    TS_ERROR_FEATURE_NOT_SUPPORT = 0x90A,       /**< feature not support on this chip */
    // the following error codes are ts passes to log daemon, no need to runtime
    TS_LOG_DEAMON_RESET_ACC_SWITCH_INVALID = 0x90B,  /* log daemon reset acc switch param invalid */
    TS_LOG_DEAMON_CORE_SWITCH_INVALID = 0x90C,       /* core switch invalid */
    TS_LOG_DEAMON_CORE_ID_INVALID = 0x90D,           /* core id invalid */
    TS_LOG_DEAMON_NO_VALID_CORE_ID = 0x90E,          /* no  valid core id */
    TS_LOG_DEAMON_CORE_ID_PG_DOWN = 0x90F,           /* core mask on pgmap */
    TS_LOG_DEAMON_NOT_SUPPORT_CORE_MASK = 0x910,     /* 1910b not support core mask */
    TS_LOG_DEAMON_SINGLE_COMMIT_INVALID = 0x911,     /* single commit invalid */
    TS_LOG_DEAMON_NOT_SUPPORT_AIV_CORE_MASK = 0x912, /* 1980 not support aiv core mask */
    TS_LOG_DEAMON_NOT_IN_POOL = 0x913,               /* 51 core maybe not in pool */
    TS_LOG_DEAMON_CORE_POOLING_STATUS_FAIL = 0x914,  /* 51 disable/enable core need pooling status */
    TS_ERROR_RESERVED,                               /**< unknown error */
} ts_error_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TS_TASK_SCHEDULER_ERROR_H */
