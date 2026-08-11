/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef MULTI_QP_INFO_MANAGER_H
#define MULTI_QP_INFO_MANAGER_H
#include "hccl_common.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <utility>
#include "hccl_ip_address.h"

namespace hccl {
constexpr u32 MULTI_QP_CONFIG_SUB_STRING_NUM = 2; // 配置信息格式为"sip,dip=sport0,sport1,...", 因此会被=分为两个子串
constexpr u32 MULTI_QP_CONFIG_IP_NUM = 2; // 有两个ip，分别为源ip和目的ip
constexpr u32 MULTI_QP_CONFIG_IP_PAIR_SHIFT_NUM = 32;
constexpr u32 MULTI_QP_CONFIG_FILE_LINE_MAX = 128 * 1024; // 配置文件最多只能配置128k行有效内容
constexpr u32 MULTI_QP_CONFIG_SRC_PORT_NUM_MAX = 32;      // 一对ip对最多配置32个源端口号
constexpr u32 MULTI_QP_CONFIG_SRC_PORT_ID_MAX = 65535;
enum class MUL_QP_FROM : std::uint8_t {
    MUL_QP_FROM_DEV_CFG,
    MUL_QP_FROM_DEV_NSLB,
    MUL_QP_FROM_ENV_PORT_CONFIG_PATH,
    MUL_QP_FROM_ENV_PER_CONNECTION,
    MUL_QP_FROM_UNKNOWN
};

using SourceIpAddress = HcclIpAddress;
using DstIpAddress = HcclIpAddress;
using KeyPair = std::pair<SourceIpAddress, DstIpAddress>; // srcIp,DstIp
struct HcclIpAddressPairHash {
    std::size_t operator()(const std::pair<SourceIpAddress, DstIpAddress>& p) const
    {
        const std::size_t h1 = std::hash<const char*>{}(p.first.GetReadableAddress());
        const std::size_t h2 = std::hash<const char*>{}(p.second.GetReadableAddress());
        return h1 ^ (h2 << 1);
    }
};
using Port = std::uint16_t;
using MulQpSourcePorts = std::vector<Port>;
using DevCfgQpInfo = MulQpSourcePorts;
using DevNslbPort = MulQpSourcePorts;
using EnvConfigPathQpInfo = std::unordered_map<std::string, MulQpSourcePorts>;
using EnvPerConnectionQpInfo = std::uint16_t;
using PortNum = std::uint32_t;

struct InitParams {
    explicit InitParams(const NICDeployment nicDeployment, const std::int32_t phyId, const DevType devType)
        : nicDeployment_(nicDeployment), phyId_(phyId), devType_(devType)
    {}
    NICDeployment GetNicDeployment() const { return nicDeployment_; }

    DevType GetDevType() const { return devType_; }

    std::int32_t GetPhyId() const { return phyId_; }

private:
    NICDeployment nicDeployment_{NICDeployment::NIC_DEPLOYMENT_RESERVED};
    std::int32_t phyId_{-1};
    DevType devType_{DevType::DEV_TYPE_COUNT};
};

struct MulQpInfoCacheBase {
    virtual ~MulQpInfoCacheBase() = default;
    virtual HcclResult Init();
    virtual bool IsAvailable() const { return false; }
    void SetMulQpInfoFrom(MUL_QP_FROM mulQpInfoFrom);
    MUL_QP_FROM MulQpInfoFrom() const;
    // 获取ip->IP port个数
    virtual HcclResult GetPortsNumByIpPair(PortNum& portNum, const KeyPair& ipPair = KeyPair()) const;
    // 获取ip->ip port个数
    virtual HcclResult GetSpecialSourcePortsByIpPair(
        MulQpSourcePorts& sourcePorts, const KeyPair& ipPair = KeyPair()) const;

protected:
    HcclResult initStatus_{HcclResult::HCCL_E_RESERVED};
    MUL_QP_FROM mulQpInfoFrom_{MUL_QP_FROM::MUL_QP_FROM_UNKNOWN};
};

struct DevCfgMulQpInfoCache : MulQpInfoCacheBase {
    explicit DevCfgMulQpInfoCache(const NICDeployment nicDeployment, const std::int32_t phyId);
    ~DevCfgMulQpInfoCache() override = default;
    HcclResult Init() override;
    bool IsAvailable() const override;

    // 获取ip->IP port个数
    HcclResult GetPortsNumByIpPair(PortNum& portNum, const KeyPair& ipPair = KeyPair()) const override;
    // 获取ip->ip port个数
    HcclResult GetSpecialSourcePortsByIpPair(
        MulQpSourcePorts& sourcePorts, const KeyPair& ipPair = KeyPair()) const override;

private:
    NICDeployment nicDeployment_{NICDeployment::NIC_DEPLOYMENT_RESERVED};
    std::int32_t phyId_{-1};
    DevCfgQpInfo cacheInfo_;
};

struct DevNslbMulQpInfoCache : MulQpInfoCacheBase {
    explicit DevNslbMulQpInfoCache(const NICDeployment nicDeployment, const std::int32_t phyId);
    ~DevNslbMulQpInfoCache() override = default;
    HcclResult Init() override;
    bool IsAvailable() const override;

    // 获取ip->IP port个数
    HcclResult GetPortsNumByIpPair(PortNum& portNum, const KeyPair& ipPair = KeyPair()) const override;
    // 获取ip->ip port个数
    HcclResult GetSpecialSourcePortsByIpPair(
        MulQpSourcePorts& sourcePorts, const KeyPair& ipPair = KeyPair()) const override;

private:
    NICDeployment nicDeployment_{NICDeployment::NIC_DEPLOYMENT_RESERVED};
    std::int32_t phyId_{-1};
    bool isEnableNslb_{false};
};

struct EnvConfigPathCache : MulQpInfoCacheBase {
    EnvConfigPathCache();
    ~EnvConfigPathCache() override = default;
    HcclResult Init() override;
    bool IsAvailable() const override;
    HcclResult LoadMultiQpSrcPortFromFile();
    static HcclResult GetIpPairFromString(
        std::string& s, std::string& ipPair, const std::uint32_t lineCnt, const std::string& lineAvator);

    // 获取ip->IP port个数
    HcclResult GetPortsNumByIpPair(PortNum& portNum, const KeyPair& ipPair = KeyPair()) const override;
    // 获取ip->ip port个数
    HcclResult GetSpecialSourcePortsByIpPair(
        MulQpSourcePorts& sourcePorts, const KeyPair& ipPair = KeyPair()) const override;

private:
    EnvConfigPathQpInfo cacheInfo_;
};

struct EnvPerConnectionQpInfoCache : MulQpInfoCacheBase {
    EnvPerConnectionQpInfoCache();
    ~EnvPerConnectionQpInfoCache() override = default;
    HcclResult Init() override;
    bool IsAvailable() const override;
    // 获取ip->IP port个数
    HcclResult GetPortsNumByIpPair(PortNum& portNum, const KeyPair& ipPair = KeyPair()) const override;
    // 获取ip->ip port个数
    HcclResult GetSpecialSourcePortsByIpPair(
        MulQpSourcePorts& sourcePorts, const KeyPair& ipPair = KeyPair()) const override;

private:
    EnvPerConnectionQpInfo cacheInfo_{};
    bool isSetEnvPerConnectionQp_{};
};

class MulQpInfo {
public:
    explicit MulQpInfo() = default;
    virtual ~MulQpInfo();
    MulQpInfo(const MulQpInfo&) = delete;
    MulQpInfo& operator=(const MulQpInfo&) = delete;
    MulQpInfo(const MulQpInfo&&) = delete;
    MulQpInfo& operator=(const MulQpInfo&&) = delete;

    HcclResult Init(const InitParams& params);
    // 已经调用Init函数
    bool IsInitialized();
    // 表示多QP信息按序解析正确且存在配置 // 否则为多QP未配置 配置错误init就会失败
    HcclResult IsEnableMulQp(bool& isEnableMulQp);
    HcclResult GetMulQpFromType(MUL_QP_FROM& type);

    // 获取ip->IP port个数
    HcclResult GetPortsNumByIpPair(PortNum& portNum, const KeyPair& ipPair = KeyPair());
    // 获取ip->ip port个数
    HcclResult GetSpecialSourcePortsByIpPair(MulQpSourcePorts& sourcePorts, const KeyPair& ipPair = KeyPair());

private:
    mutable std::mutex initLock_;
    HcclResult initStatus_{HcclResult::HCCL_E_RESERVED}; // 解析数据过程未发现配置错误，或者 未/非必须 配置
    std::unique_ptr<MulQpInfoCacheBase> config_{nullptr};
};
} // namespace hccl

namespace std {
template <>
struct hash<const hccl::MUL_QP_FROM> {
    std::size_t operator()(const hccl::MUL_QP_FROM& type) const noexcept { return static_cast<std::size_t>(type); }
};

template <>
struct hash<hccl::MUL_QP_FROM> {
    std::size_t operator()(const hccl::MUL_QP_FROM& type) const noexcept { return static_cast<std::size_t>(type); }
};
} // namespace std
#endif // MULTI_QP_INFO_MANAGER_H
