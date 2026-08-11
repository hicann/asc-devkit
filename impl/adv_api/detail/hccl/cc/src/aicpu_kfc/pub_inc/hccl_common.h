/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef HCCL_COMMON_H
#define HCCL_COMMON_H

#include <map>
#include <set>
#include <vector>
#include <climits>
#include <queue>
#include <string>
#include <unordered_map>
#include <algorithm>

#include "dtype_common.h"
#include "hccl_ip_address.h"
#include "log.h"
#include "pkg_inc/hccl/hcom.h"

#ifndef T_DESC
#define T_DESC(_msg, _y) ((_y) ? true : false)
#endif

#ifndef HCCL_DEPRECATED
#define HCCL_DEPRECATED(class) [[deprecated("Use " #class " instead")]]
#endif

using char_t = char;

using HcclRtStream = void*;
using HcclRtNotify = void*;
using RdmaHandle = void*;
using SocketHandle = void*;
using FdHandle = void*;
using QpHandle = void*;
using HcclRtContext = void*;
using HcclDispatcher = void*;
using HcclRtEvent = void*;
using HcclTraHandle = intptr_t;
using MrHandle = void*;

#if T_DESC("公共常量及宏", true)
/* 未使用的参数声明 */
#define UNUSED_PARAM(x) (void)(x)
constexpr u32 INVALID_VALUE_RANKID = 0xFFFFFFFF;   // rank id非法值
constexpr u32 INVALID_VALUE_RANKSIZE = 0xFFFFFFFF; // rank size非法值
constexpr u32 INVALID_SUBCOMM_ID = 0xFFFFFFFF;     // 子通信域ID非法值
constexpr u32 INVALID_UINT = 0xFFFFFFFF;
constexpr u64 INVALID_U64 = 0xFFFFFFFFFFFFFFFF;
constexpr s32 INVALID_INT = 0xFFFFFFFF;
constexpr s64 INVALID_S64 = 0xFFFFFFFFFFFFFFFF;
constexpr s32 INVALID_VALUE_STAGE = -1;
constexpr u32 INVALID_QOSCFG = 0xFFFFFFFF;
// 系统常用参数
constexpr u64 SYS_MAX_COUNT = 0x7FFFFFFFF;  // 系统当前支持的最大count数
constexpr u32 HCCL_AISERVER_DEVICE_NUM = 8; // 单个Server 支持最大的设备数量
constexpr u32 MAX_MODULE_DEVICE_NUM = 32;   // 单server双模组时支持最大的设备数量
constexpr u32 HCCL_DEVICE_NIC_NUM = 3; // device网卡上最大的device ip数目，1个ipv4,ipv4自动转换的ipv6,用户配置的ipv6
constexpr u32 HCCL_HOST_NIC_NUM = 1000; // host 支持最大的网卡数量
constexpr int HCCL_DEVICE_MINNUM = 1;
constexpr s32 HCCL_DEVICE_NOT_SET = -1; // device id 的无效值
constexpr s32 HOST_DEVICE_ID = -1;

constexpr int HCCL_MODULE_NUM_TWO = 2; // 910B A+X集群中 serverNum为1，moduleNum等于此数值时，要校验RDMA
constexpr int HCCL_DEVICE_NUM_ONE = 1;
constexpr int HCCL_DEVICE_NUM_TWO = 2; // 平均device num小于等于此数值时，无法通过HCCS链路类型接口判定当前硬件环境
constexpr int HCCL_DEVICE_NUM_FOUR = 4; // 平均device num等于此数值时，需校验server内device选取合法性
constexpr int HCCL_DEVICE_NUM_EIGHT = 8;
constexpr u32 GROUP_NAME_MAX_LEN = 127; // 最大的group name 长度
constexpr u32 RANK_TABLE_MAX_LEN = PATH_MAX - 1;
constexpr u32 IDENTIFY_MAX_LEN = 127;                                    // 最大的identify  长度
constexpr s32 STRING_MAX_LENGTH = 40 * 1024 * 1024;                      // rankTable string length 40*1024*1024=40M.
constexpr u64 DEVICE_MEMORY_MAX_ALLOC_SIZE = 16ULL * 1024 * 1024 * 1024; // device memory size 16GB
constexpr int HCCL_BARRIER_DEFAULT_COUNT = 8;
constexpr s32 NOTIFY_DEFAULT_WAIT_TIME = 27 * 68;    // notifywait默认1836等待时长
constexpr u32 NOTIFY_INVALID_WAIT_TIME = 0xFFFFFFFF; // notifywait时长非法值
constexpr u32 HCCL_FFTS_CAPACITY = 65535;            // FFTS+子图最大容量
constexpr char HCCL_WORLD_GROUP[] = "hccl_world_group";

// Notify时间增量数值
constexpr u64 AICPU_H2D_TIMEOUT_INC = 27;    // AICPU host-device同步的notify超时时间增量数值
constexpr u64 AICPU_KERNEL_TIMEOUT_INC = 25; // AICPU kernel超时时间增量数值
constexpr u64 AICPU_SQE_TIMEOUT_INC = 22;    // AICPU 执行超时时间增量数值
constexpr u64 AICPU_RTSQ_TIMEOUT_INC = 20;   // AICPU 下发等待超时时间增量数值

constexpr s64 HCCL_ALIGN_SIZE = 4096;           // hccl  对齐方式， 按4KB来对齐
constexpr s64 HCCL_WORKSPACE_MEM_32_KB = 32768; // hccl内存大小，暂定32KB

constexpr u32 HCCP_SQ_TEMPLATE_CAPACITY = 12;                               // SQ模板深度为 12 个 wqe
constexpr u64 CCL_COMM_INBUFFER_UNALIGNED_RESERVE_SIZE = (1 * 1024 * 1024); // 1 * 1024 * 1024, 即1M

constexpr u32 MAX_FRAME_LEN = 2 * 1024; // 最大信息长度2*1024

constexpr s32 NOTIFY_MAX_WAIT_TIME = 255 * 68; // 非910B和910_93场景notify wait最大等待时长，由硬件决定
constexpr s32 NOTIFY_MAX_WAIT_TIME_910_93 = 2147483647; // 910B和910_93场景notify wait最大等待时长，由软件实现

// HCCL QOS 与 SDMA QOS 映射级别
constexpr uint32_t SDMA_QOS_LOW = 2;
constexpr uint32_t SDMA_QOS_MIDDLE = 4;
constexpr uint32_t SDMA_QOS_HIGH = 6;

constexpr uint32_t SDMA_QOS_DEFAULT = 6;

constexpr uint32_t HCCL_QOS_MIN = 0;
constexpr uint32_t HCCL_QOS_LEVEL_1_LIMIT = 2;
constexpr uint32_t HCCL_QOS_LEVEL_2_LIMIT = 4;
constexpr uint32_t HCCL_QOS_LEVEL_3_LIMIT = 7;

/* error message相关 */
/* EI0004 */
const std::string RANKTABLE_PARSE_ERROR_REASON =
    "The rankTable file path does not exist, the permission is insufficient, or the JSON format is incorrect.";
/* EI0006 */
const std::string GET_SOCKET_TIMEOUT_REASON_CLOSE_DETECT =
    "Connection fault detection has been disabled. "
    "1. The remote end does not initiate a connection request. some NPUs in the cluster are abnormal. "
    "2. The remote end does not initiate a connection request because the collective communication operator is "
    "started too late or is not started by some NPUs in the cluster. "
    "3. The communication link is disconnected. (For example, the IP addresses are not on "
    "the same network segment or the TLS configurations are inconsistent.)";
const std::string GET_SOCKET_TIMEOUT_REASON_WITH_EVENT =
    "1. If the link setup timeout is reported on both ends, check the network connectivity between the two ends."
    "2. If dstRank reports other exceptions, locate the cause based on the exception information of dstRank."
    "3. If dstRank does not report any error, the possible cause is that the service process is suspended or exits in "
    "advance";
const std::string GET_SOCKET_TIMEOUT_REASON_WITHOUT_EVENT =
    "1. The communication operator types or parameters called by some ranks in a communicator are inconsistent. "
    "For example, some ranks call allreduce, while other ranks call broadcast."
    "2. The send recv operator depends on the ring."
    "You can check the peer dependency relationship of link establishment and the operator information in the plog "
    "logs "
    "to locate and analyze the fault. (You need to enable log recording using HCCL_ENTRY_LOG_ENABLE.) "
    "For details about the troubleshooting method, search for the keyword \"EI0006\" on "
    "https://www.hiascend.com/en/document/.";

/* 对关键报错日志提供多级检索关键字 */
/* 一级检索关键字 */
const std::string LOG_KEYWORDS_TASK_EXEC = "TaskExecStage";   // 算子执行阶段异常
const std::string LOG_KEYWORDS_INIT_GROUP = "InitGroupStage"; // 通信域初始化阶段异常
const std::string LOG_KEYWORDS_INIT_CHANNEL = "InitChannelStage";
const std::string LOG_KEYWORDS_LINK_INFO = "LinkInfo";

/* 二级检索关键字 */
const std::string LOG_KEYWORDS_TIMEOUT = "Timeout";                      // 算子执行阶段超时
const std::string LOG_KEYWORDS_RUN_FAILED = "RunFailed";                 // 算子执行阶段失败，如SDMA ERROR
const std::string LOG_KEYWORDS_HEARTBEAT_EVETN = "HeartbeatAbnormal";    // 算子执行阶段心跳异常事件
const std::string LOG_KEYWORDS_ENV_CONFIG = "EnvConfig";                 // 环境变量配置异常
const std::string LOG_KEYWORDS_RANKTABLE_CONFIG = "RanktableConfig";     // ranktable读取失败
const std::string LOG_KEYWORDS_RANKTABLE_CHECK = "RanktableCheck";       // ranktable校验失败
const std::string LOG_KEYWORDS_RANKTABLE_DETECT = "RanktableDetect";     // ranktable协商失败
const std::string LOG_KEYWORDS_PARAMETER_CONFLICT = "ParameterConflict"; // 参数不一致
const std::string LOG_KEYWORDS_VERSION_CONFLICT = "VersionConflict";     // HCCL版本不一致
const std::string LOG_KEYWORDS_INVALID_ARGUMENT = "InvalidArgument";     // 外部入参非法
const std::string LOG_KEYWORDS_RESOURCE = "Resource";
const std::string LOG_KEYWORDS_NOT_SUPPORTED = "Not Supported";

/* 三级检索关键字 */
const std::string LOG_KEYWORDS_HOST = "HOST";
const std::string LOG_KEYWORDS_HOST_TS = "HOST_TS";
const std::string LOG_KEYWORDS_AIV = "AIV";
const std::string LOG_KEYWORDS_AICPU = "AICPU";
const std::string LOG_KEYWORDS_CQE_ERROR = "ROCE CQE ERROR";

/* 通信域及本卡信息关键字 */
const std::string LOG_KEYWORDS_COMMUNICATOR = "Communicator Key Info";
const std::string LOG_KEYWORDS_LOCALRANK = "LocalRank Key Info";

constexpr u32 HETEROG_CCL_PORT = 16666; // 通信默认端口
// host网卡相关参数
constexpr u32 PORT_MIN = 0;                   // port最小值
constexpr u32 PORT_MAX = 65535;               // port最大值
constexpr u32 HOST_CONTROL_BASE_PORT = 60000; // 控制面起始port
constexpr u32 HOST_PARA_BASE_PORT = 60008;    // 数据面起始port
constexpr u32 HOST_PORT_MAX = 65520;
#endif

// 内存相关
constexpr u64 LARGE_PAGE_MEMORY_MIN_SIZE = 2 * 1024 * 1024; // 申请内存用于MR注册时需要申请大页内存（最小2*1024*1024）

/* 公共模块函数返回值定义,跟业务层同步  */
const std::map<HcclDataType, std::string> HCOM_DATA_TYPE_STR_MAP{
    {HcclDataType::HCCL_DATA_TYPE_INT8, "int8"},       {HcclDataType::HCCL_DATA_TYPE_INT16, "int16"},
    {HcclDataType::HCCL_DATA_TYPE_INT32, "int32"},     {HcclDataType::HCCL_DATA_TYPE_INT64, "int64"},
    {HcclDataType::HCCL_DATA_TYPE_UINT64, "uint64"},   {HcclDataType::HCCL_DATA_TYPE_FP16, "float16"},
    {HcclDataType::HCCL_DATA_TYPE_FP32, "float32"},    {HcclDataType::HCCL_DATA_TYPE_UINT8, "uint8"},
    {HcclDataType::HCCL_DATA_TYPE_UINT16, "uint16"},   {HcclDataType::HCCL_DATA_TYPE_UINT32, "uint32"},
    {HcclDataType::HCCL_DATA_TYPE_FP64, "float64"},    {HcclDataType::HCCL_DATA_TYPE_BFP16, "bfloat16"},
    {HcclDataType::HCCL_DATA_TYPE_INT128, "int128"},   {HcclDataType::HCCL_DATA_TYPE_FP8E4M3, "fp8e4m3"},
    {HcclDataType::HCCL_DATA_TYPE_FP8E5M2, "fp8e5m2"}, {HcclDataType::HCCL_DATA_TYPE_RESERVED, "reserved"}};

inline std::string GetDataTypeEnumStr(HcclDataType dataType)
{
    auto iter = HCOM_DATA_TYPE_STR_MAP.find(dataType);
    if (iter == HCOM_DATA_TYPE_STR_MAP.end()) {
        return "HcclDataType(" + std::to_string(dataType) + ")";
    } else {
        return iter->second;
    }
}

inline std::string GetDataTypeEnumStr(u32 dataType)
{
    auto hcclDataType = static_cast<HcclDataType>(dataType);
    return GetDataTypeEnumStr(hcclDataType);
}

const std::map<HcclReduceOp, std::string> HCOM_REDUCE_OP_STR_MAP{
    {HcclReduceOp::HCCL_REDUCE_SUM, "sum"},
    {HcclReduceOp::HCCL_REDUCE_PROD, "prod"},
    {HcclReduceOp::HCCL_REDUCE_MAX, "max"},
    {HcclReduceOp::HCCL_REDUCE_MIN, "min"},
    {HcclReduceOp::HCCL_REDUCE_RESERVED, "reserved"}};

inline std::string GetReduceOpEnumStr(HcclReduceOp reduceOp)
{
    auto iter = HCOM_REDUCE_OP_STR_MAP.find(reduceOp);
    if (iter == HCOM_REDUCE_OP_STR_MAP.end()) {
        return "HcclReduceOp(" + std::to_string(reduceOp) + ")";
    } else {
        return iter->second;
    }
}

constexpr u32 HCCL_LEVEL_ALGO_WIDTH = 8;

// 参数平面位置
enum class NICDeployment { NIC_DEPLOYMENT_HOST = 0, NIC_DEPLOYMENT_DEVICE, NIC_DEPLOYMENT_RESERVED };

// server内link类型
enum class LinkTypeInServer { HCCS_TYPE = 0, PXI_TYPE = 1, SIO_TYPE = 2, HCCS_SW_TYPE = 3, RESERVED_LINK_TYPE };
// notifywait超时类型
enum class SyncMode { DEFAULT_TIMEWAITSYNCMODE = 0, CONFIGURABLE_TIMEWAITSYNCMODE = 1, UNLIMITED_TIMEWAITSYNCMODE };

// notifywait超时时间配置类型
enum class HcclExecTimeoutSet {
    HCCL_EXEC_TIMEOUT_NOT_SET = 0,
    HCCL_EXEC_TIMEOUT_SET_BY_OPTIONS,
    HCCL_EXEC_TIMEOUT_SET_BY_ENV
};

using IpSocket = struct IpSocket {
    SocketHandle nicSocketHandle;
    RdmaHandle nicRdmaHandle;
    std::set<u32> listenedPort;

    IpSocket() : nicSocketHandle(nullptr), nicRdmaHandle(nullptr), listenedPort() {}
};
using RaResourceInfo = struct TagRaResourceInfo {
    std::map<hccl::HcclIpAddress, IpSocket> vnicSocketMap;
    std::map<hccl::HcclIpAddress, IpSocket> nicSocketMap;
    std::map<hccl::HcclIpAddress, IpSocket> hostNetSocketMap;

    TagRaResourceInfo() {}

    TagRaResourceInfo(const TagRaResourceInfo& that)
        : vnicSocketMap(that.vnicSocketMap), nicSocketMap(that.nicSocketMap), hostNetSocketMap(that.hostNetSocketMap)
    {}

    TagRaResourceInfo& operator=(const TagRaResourceInfo& that)
    {
        if (&that != this) {
            vnicSocketMap = that.vnicSocketMap;
            nicSocketMap = that.nicSocketMap;
            hostNetSocketMap = that.hostNetSocketMap;
        }
        return *this;
    }

    ~TagRaResourceInfo()
    {
        vnicSocketMap.clear();
        nicSocketMap.clear();
        hostNetSocketMap.clear();
    }
};

namespace hccl {

/* 抽象链路信息 */
enum class TransportType {
    TRANS_TYPE_IBV_EXP = 0,
    TRANS_TYPE_P2P = 1,
    TRANS_TYPE_HOST_SHM = 2,
    TRANS_TYPE_HOST_TCP = 3,
    TRANS_TYPE_ROCE = 4,
    TRANS_TYPE_HETEROG_P2P = 5,
    TRANS_TYPE_HETEROG_ROCE = 6,
    TRANS_TYPE_DEVICE_P2P = 7,
    TRANS_TYPE_DEVICE_IBVERBS = 8,
    TRANS_TYPE_DEVICE_DIRECT = 9,
    TRANS_TYPE_RESERVED = 255,
};

class Referenced {
public:
    // 初始化这个类，引用计数设为1，并且将p指向传入的地址
    Referenced() : refCount(0) {}

    // 引用计数加1
    int Ref() { return ++refCount; }

    // 引用计数减1
    int Unref() { return --refCount; }

    // 返回引用计数
    int Count() const { return refCount; }

    int Clear()
    {
        refCount = 0;
        return refCount;
    }
    bool IsZero() const { return refCount == 0; }
    ~Referenced() {}

private:
    int refCount; // 引用计数，表示有多少个变量引用这块内存
};

using RemoteRankInfo = struct TagRemoteRankInfo {
    s32 remoteDeviceId;
    u32 remoteRank;
    s32 remotePid;
    s32 remoteSdid;
    TagRemoteRankInfo()
        : remoteDeviceId(INVALID_INT), remoteRank(INVALID_UINT), remotePid(INVALID_INT), remoteSdid(INVALID_INT)
    {}
    TagRemoteRankInfo(s32 remoteDeviceId, u32 remoteRank)
        : remoteDeviceId(remoteDeviceId), remoteRank(remoteRank), remotePid(INVALID_INT), remoteSdid(INVALID_INT)
    {}
    TagRemoteRankInfo(s32 remoteDeviceId, u32 remoteRank, s32 remotePid)
        : remoteDeviceId(remoteDeviceId), remoteRank(remoteRank), remotePid(remotePid), remoteSdid(INVALID_INT)
    {}
    TagRemoteRankInfo(s32 remoteDeviceId, u32 remoteRank, s32 remotePid, s32 remoteSdid)
        : remoteDeviceId(remoteDeviceId), remoteRank(remoteRank), remotePid(remotePid), remoteSdid(remoteSdid)
    {}
};

enum class NotifyLoadType { HOST_NOTIFY = 0, DEVICE_NOTIFY };
} // namespace hccl

struct HcclSignalInfo {
    u64 resId; // 在代表event时为eventid，notify时为notifyid
    u64 addr;
    u32 devId;
    u32 tsId;
    u32 rankId;
    u32 flag;
};

struct HcclAicpuDispatcherInfo {
    u32 devId;
    u32 ssid;
    DevType devType = DevType::DEV_TYPE_COUNT;
    u64 overflowAddr;
    s64 chipId = INVALID_S64;
};

struct HcclComStreamInfo {
    int32_t actualStreamId; // 实际streamid
    int32_t sqId;
    uint32_t sqDepth;
    void* sqBaseAddr;
    u32 logicCqId; // 记录逻辑cqId
};

// offload P2P建链状态
constexpr u32 HETEROG_P2P_SUCCESS = 0;
constexpr u32 HETEROG_P2P_WAIT = 1;
constexpr u32 HETEROG_P2P_FAILED = 2;

constexpr s32 MAX_SCATTER_BUF_NUM = 64;

enum class QPMode { INVALID = -1, NORMAL = 0, OFFLOAD = 1 };

// 算子计数信息
struct OpCounterInfo {
    u64 headCountMem = 0;
    u64 tailCountMem = 0;
    u64 addOneMem = 0;
    u32 memSize = 0;
    bool isEnableCounter = false;
};
#endif // HCCL_COMMON_H
