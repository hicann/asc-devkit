/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#include "physical_level.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include <string>

#include "log.h"
#include "hccl_rank_graph_dl.h"

namespace mc2_ops_hccl {
namespace {

// 日志里一个vector最多展开的元素数, 超出部分省略
constexpr size_t LOG_VEC_MAX_ITEM = 16;

std::string VecToStr(const std::vector<u32>& vec)
{
    std::string str;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i >= LOG_VEC_MAX_ITEM) {
            str += ",...";
            break;
        }
        if (i > 0) {
            str += ",";
        }
        str += std::to_string(vec[i]);
    }
    return str;
}

// 把一个Level拼成一行日志。localRanks只打首尾与个数, 不整条展开: 顶层那一级等于整个通信域,
// 万卡场景整条打出来没人看得完, 而升序与含myRank由校验侧保证
std::string DescribeLevel(const PhysicalLevelInfo& level, size_t idx, size_t total)
{
    return "level[" + std::to_string(idx) + "/" + std::to_string(total) + "] rankNum[" +
           std::to_string(level.localRanks.size()) + "] ranks[" +
           (level.localRanks.empty() ?
                std::string("-") :
                std::to_string(level.localRanks.front()) + ".." + std::to_string(level.localRanks.back())) +
           "] view[" + std::to_string(static_cast<u32>(level.view)) + "] instSizeListByLayer[" +
           VecToStr(level.instSizeListByLayer) + "] ref[layer " + std::to_string(level.ref.netLayer) + " inst " +
           std::to_string(level.ref.topoInstId) + "] hasTopoInst[" + std::to_string(level.hasTopoInst ? 1 : 0) +
           "] topoType[" + std::to_string(static_cast<s32>(level.topoType)) + "] locType[" +
           std::to_string(static_cast<s32>(level.locType)) + "] protocolNum[" + std::to_string(level.protocols.size()) +
           "] portNums[" + VecToStr(level.portNums) + "]";
}

/**
 * 提取当前rank在指定TopoInstance上的Endpoint快照。
 * 返回void: endpoints是payload叶子, 不参与排序键与范围链结构, 全部失败路径都局部降级为空。
 */
void FetchEndpoints(HcclComm comm, u32 layer, u32 instId, std::vector<EndpointDesc>& out)
{
    out.clear();
    u32 num = 0;
    // 与下面的num == 0分开判: 取数失败是真异常, 需要留日志
    if (HcclRankGraphGetEndpointNum(comm, layer, instId, &num) != HCCL_SUCCESS) {
        HCCL_WARNING(
            "[PhysicalLevel][Build] get endpoint num failed at layer[%u] inst[%u], skip endpoints", layer, instId);
        return;
    }
    // 0是合法结果, 不是错误: 当前rank在该topoInst上没有接口/协议时就是0
    if (num == 0) {
        HCCL_DEBUG("[PhysicalLevel][Build] no endpoint at layer[%u] inst[%u]", layer, instId);
        return;
    }
    // 合理性阈值, 不截断: 截断只会把异常掩盖成"正常但数据少", 直接局部降级并告警
    if (num > ENDPOINT_NUM_SANITY_LIMIT) {
        HCCL_WARNING(
            "[PhysicalLevel][Build] implausible endpoint num[%u] at layer[%u] inst[%u], skip endpoints", num, layer,
            instId);
        return;
    }

    // num是实际写入条数的上界(GetEndpointNum求和时不去重), 必须以回写的descNum为准resize
    std::vector<EndpointDesc> buf(num);
    u32 actualNum = num;
    if (HcclRankGraphGetEndpointDesc(comm, layer, instId, &actualNum, buf.data()) != HCCL_SUCCESS) {
        HCCL_WARNING(
            "[PhysicalLevel][Build] get endpoint desc failed at layer[%u] inst[%u], skip endpoints", layer, instId);
        return;
    }
    if (actualNum > num) {
        HCCL_WARNING(
            "[PhysicalLevel][Build] endpoint descNum[%u] exceeds requested[%u] at layer[%u] inst[%u], skip "
            "endpoints",
            actualNum, num, layer, instId);
        return;
    }
    buf.resize(actualNum);
    // GetEndpointDesc的输出顺序是unordered_map哈希序, 必须归一化后再保存
    std::sort(buf.begin(), buf.end(), EndpointDescLess);
    out = std::move(buf);
}

/**
 * 采集本rank在该Level上各条物理链路的端口数, 降序写入out。按iface(commAddr)去重, 一条链路一项:
 * 一个iface有N种协议就有N个EndpointDesc, 逐endpoint查会把同一条链路的端口数重复计入。
 * 全有或全无: 任一条取不到就整个清空 —— 残缺数组会让消费侧算出"看着合理但偏小"的总端口数。
 */
void FetchPortNums(HcclComm comm, u32 myRank, const std::vector<EndpointDesc>& endpoints, std::vector<u32>& out)
{
    out.clear();
    // 空有两种来源: 本就没有接口, 或FetchEndpoints已降级(含HCOMM低版本弱符号未命中),
    // 因此这里不需要再做一次能力探测
    if (endpoints.empty()) {
        return;
    }

    std::vector<CommAddr> seenAddrs; // 已计入的iface。规模是个位数, 线性查找即可
    std::vector<u32> portNums;
    for (const auto& desc : endpoints) {
        bool seen = false;
        for (const auto& addr : seenAddrs) {
            if (CommAddrEqual(addr, desc.commAddr)) {
                seen = true;
                break;
            }
        }
        if (seen) {
            continue; // 同一个iface的另一种协议, 端口数已经计过
        }

        EndpointAttrBwCoeff portNum{};
        // ENDPOINT_ATTR_BW_COEFF名为"带宽系数", HCOMM侧实现即iface->GetPorts().size()
        if (HcclRankGraphGetEndpointInfo(
                comm, myRank, &desc, ENDPOINT_ATTR_BW_COEFF, sizeof(EndpointAttrBwCoeff), &portNum) != HCCL_SUCCESS) {
            HCCL_WARNING(
                "[PhysicalLevel][Build] get port num failed for rank[%u] protocol[%d], drop port nums of this "
                "level",
                myRank, static_cast<s32>(desc.protocol));
            return;
        }
        // 0与超限都判为不可信, 口径对齐op_common.cc的BuildChannelInfo
        if (portNum == 0 || portNum > PORT_NUM_SANITY_LIMIT) {
            HCCL_WARNING(
                "[PhysicalLevel][Build] implausible port num[%u] for rank[%u] protocol[%d], drop port nums of "
                "this level",
                portNum, myRank, static_cast<s32>(desc.protocol));
            return;
        }
        seenAddrs.push_back(desc.commAddr);
        portNums.push_back(static_cast<u32>(portNum));
    }
    // 降序。与endpoints的(protocol, locType, addr)序无关, 两者是同一批iface的两种独立排列
    std::sort(portNums.begin(), portNums.end(), std::greater<u32>());
    out = std::move(portNums);
}

/**
 * 从endpoints提炼该Level的位置与协议集合。locType各endpoint不一致时置RESERVED并告警:
 * 一个Level对应一种网络平面, 位置本应唯一, 给出任一个都会误导"是否需要host网卡"的判断。
 */
void FetchLocAndProtocols(
    const std::vector<EndpointDesc>& endpoints, EndpointLocType& locType, std::vector<CommProtocol>& protocols)
{
    locType = EndpointLocType::ENDPOINT_LOC_TYPE_RESERVED;
    protocols.clear();
    if (endpoints.empty()) {
        return;
    }

    locType = endpoints.front().loc.locType;
    for (const auto& desc : endpoints) {
        if (desc.loc.locType != locType) {
            HCCL_WARNING(
                "[PhysicalLevel][Build] mixed endpoint locType[%d] vs [%d] on one level, mark location unknown",
                static_cast<s32>(desc.loc.locType), static_cast<s32>(locType));
            locType = EndpointLocType::ENDPOINT_LOC_TYPE_RESERVED;
            break;
        }
    }

    protocols.reserve(endpoints.size());
    for (const auto& desc : endpoints) {
        protocols.push_back(desc.protocol);
    }
    // 去重升序: endpoints已按protocol为首键排过, 但同一协议可能出现在多个iface上
    std::sort(protocols.begin(), protocols.end());
    protocols.erase(std::unique(protocols.begin(), protocols.end()), protocols.end());
}

// 取该layer本地NetInstance的rank集合, 并与netLayerDetails做跨调用一致性校验
HcclResult FetchLocalNetRanks(
    HcclComm comm, const NetLayerDetails& details, u32 layer, u32 myRank, std::vector<u32>& ranks)
{
    u32* rawRanks = nullptr;
    u32 rankNum = 0;
    if (HcclRankGraphGetRanksByLayer(comm, layer, &rawRanks, &rankNum) != HCCL_SUCCESS || rawRanks == nullptr) {
        HCCL_WARNING("[PhysicalLevel][Build] get ranks by layer[%u] failed", layer);
        return HCCL_E_INTERNAL;
    }
    // HCOMM为该接口只持有一个成员vector, 下一次调用会clear()并重填它, 必须立即复制
    ranks.assign(rawRanks, rawRanks + rankNum);

    // 跨调用一致性校验: localNetInsSizeOfLayer来自ExtractNetLayerDetails中的另一次调用,
    // 与此处不同源, 不一致说明RankGraph在两次调用之间发生了变化
    if (ranks.size() != details.localNetInsSizeOfLayer[layer]) {
        HCCL_WARNING(
            "[PhysicalLevel][Build] netLayer[%u] rankNum[%zu] mismatches localNetInsSize[%u]", layer, ranks.size(),
            details.localNetInsSizeOfLayer[layer]);
        return HCCL_E_INTERNAL;
    }
    if (std::find(ranks.begin(), ranks.end(), myRank) == ranks.end()) {
        HCCL_WARNING("[PhysicalLevel][Build] netLayer[%u] local instance does not contain myRank[%u]", layer, myRank);
        return HCCL_E_INTERNAL;
    }
    return HCCL_SUCCESS;
}

/**
 * 校验分区: 非空、总和等于通信域规模、无空分区, 且本地实例的规模是其中某个分区的大小。
 */
HcclResult ValidateInstSizeLayout(
    const std::vector<u32>& instSizeList, u32 layer, size_t localRankNum, u32 userRankSize)
{
    if (instSizeList.empty()) {
        HCCL_WARNING("[PhysicalLevel][Build] netLayer[%u] inst size list is empty", layer);
        return HCCL_E_INTERNAL;
    }
    // 正常路径永不触发(ExtractNetLayerDetails已用同一等式先行校验过)。
    // 保留它只为在HCOMM改变分层语义时第一时间暴露
    const u32 totalRankNum = std::accumulate(instSizeList.begin(), instSizeList.end(), 0U);
    if (totalRankNum != userRankSize) {
        HCCL_WARNING(
            "[PhysicalLevel][Build] netLayer[%u] inst size sum[%u] mismatches userRankSize[%u]", layer, totalRankNum,
            userRankSize);
        return HCCL_E_INTERNAL;
    }
    // 0能完整穿过其余检查, 于是幽灵空分区会被当成真实Instance计入
    if (std::find(instSizeList.begin(), instSizeList.end(), 0U) != instSizeList.end()) {
        HCCL_WARNING("[PhysicalLevel][Build] netLayer[%u] inst size list contains a zero entry", layer);
        return HCCL_E_INTERNAL;
    }
    // 顺序无关的自检: 本地实例的规模必须是这一层真实存在的某个分区大小。两个量来源独立
    // (前者来自GetRanksByLayer, 后者来自GetInstSizeListByLayer), 对不上说明两次调用之间
    // RankGraph发生了变化
    if (std::find(instSizeList.begin(), instSizeList.end(), static_cast<u32>(localRankNum)) == instSizeList.end()) {
        HCCL_WARNING(
            "[PhysicalLevel][Build] netLayer[%u] local instance has [%zu] ranks, which is not one of the "
            "inst sizes",
            layer, localRankNum);
        return HCCL_E_INTERNAL;
    }
    return HCCL_SUCCESS;
}

// 取该netLayer本地NetInstance的rank集合与全层分区。这是"合一"里ranktable那一半:
// 只有NetInstance看得到兄弟实例, 因此只有它能给出全局分区
HcclResult FetchNetInstance(
    HcclComm comm, const TopoInfoWithNetLayerDetails* topoInfo, u32 layer, std::vector<u32>& ranks,
    std::vector<u32>& instSizeListByLayer)
{
    const NetLayerDetails& details = topoInfo->netLayerDetails;
    const u32 myRank = topoInfo->userRank;
    if (layer >= details.localNetInsSizeOfLayer.size() || layer >= details.instSizeListOfLayer.size()) {
        HCCL_WARNING("[PhysicalLevel][Build] netLayer[%u] out of range of netLayerDetails arrays", layer);
        return HCCL_E_INTERNAL;
    }
    HcclResult ret = FetchLocalNetRanks(comm, details, layer, myRank, ranks);
    if (ret != HCCL_SUCCESS) {
        return ret;
    }
    // 原样透传HCOMM的返回序, 不重排。该序不保证任何含义(HCOMM侧以unordered_map实现),
    // 消费侧只做与顺序无关的判断(IsInstListSymmetric/CalcGcd), 排序反而会掩盖乱序这件事
    instSizeListByLayer = details.instSizeListOfLayer[layer];
    return ValidateInstSizeLayout(instSizeListByLayer, layer, ranks.size(), topoInfo->userRankSize);
}

// 取该layer上全部TopoInstance的id。空map是合法结果, 返回空列表且不算失败
HcclResult FetchTopoInstIds(HcclComm comm, u32 layer, std::vector<u32>& instIds)
{
    instIds.clear();
    u32* rawInstIds = nullptr;
    u32 instNum = 0;
    HcclResult ret = HcclRankGraphGetTopoInstsByLayer(comm, layer, &rawInstIds, &instNum);
    if (ret != HCCL_SUCCESS) {
        HCCL_WARNING("[PhysicalLevel][Build] get topo insts of layer[%u] failed, ret[%d]", layer, ret);
        return HCCL_E_INTERNAL;
    }
    if (instNum == 0) {
        // 空map即返回0且不报错。意味着该层没有endpoints与topoType可供建链,
        // 调用方据此把hasTopoInst置false
        HCCL_DEBUG("[PhysicalLevel][Build] layer[%u] has no topo instance", layer);
        return HCCL_SUCCESS;
    }
    if (rawInstIds == nullptr) {
        HCCL_WARNING("[PhysicalLevel][Build] topo insts of layer[%u] is null while num[%u]", layer, instNum);
        return HCCL_E_INTERNAL;
    }
    // 立即复制: 该接口的下一次调用会clear()并重填同一个成员vector
    instIds.assign(rawInstIds, rawInstIds + instNum);
    return HCCL_SUCCESS;
}

// 采集单个TopoInstance并追加到out。当前rank不在其中时跳过且不写out, 不算失败
HcclResult AppendTopoInstLevel(HcclComm comm, u32 myRank, u32 layer, u32 instId, std::vector<PhysicalLevelInfo>& out)
{
    u32* rawRanks = nullptr;
    u32 rankNum = 0;
    if (HcclRankGraphGetRanksByTopoInst(comm, layer, instId, &rawRanks, &rankNum) != HCCL_SUCCESS ||
        rawRanks == nullptr) {
        HCCL_WARNING("[PhysicalLevel][Build] get ranks by topo inst[%u] of layer[%u] failed", instId, layer);
        return HCCL_E_INTERNAL;
    }
    std::vector<u32> ranks(rawRanks, rawRanks + rankNum);

    // GetTopoInstsByLayer返回的应当只含当前rank所在的topoInstance, 这里再过滤一次兜底
    if (std::find(ranks.begin(), ranks.end(), myRank) == ranks.end()) {
        HCCL_DEBUG(
            "[PhysicalLevel][Build] skip sibling topo inst[%u] of layer[%u], myRank[%u] not in it", instId, layer,
            myRank);
        return HCCL_SUCCESS;
    }

    // 必须用按topoInst的GetTopoType。按netLayer的GetTopoTypeByLayer查的是NetType,
    // A5上Mesh层是TOPO_FILE_DESC描述的, 会返回COMM_TOPO_CUSTOM, TopoTypeOrder定不了序
    CommTopo topoType = CommTopo::COMM_TOPO_RESERVED;
    if (HcclRankGraphGetTopoType(comm, layer, instId, &topoType) != HCCL_SUCCESS) {
        HCCL_WARNING("[PhysicalLevel][Build] get topo type of inst[%u] layer[%u] failed", instId, layer);
        return HCCL_E_INTERNAL;
    }

    PhysicalLevelInfo level;
    level.localRanks = std::move(ranks);
    level.ref.netLayer = layer;
    level.ref.topoInstId = instId;
    level.hasTopoInst = true;
    level.topoType = topoType;
    // 以下三项无返回值: 内部失败一律局部降级, 理由见各自声明处
    FetchEndpoints(comm, layer, instId, level.endpoints);
    FetchLocAndProtocols(level.endpoints, level.locType, level.protocols);
    FetchPortNums(comm, myRank, level.endpoints, level.portNums);
    out.push_back(std::move(level));
    return HCCL_SUCCESS;
}

// 取该netLayer上、含当前rank的每个TopoInstance的rank集合, 并把链路属性填进level。
// 这是"合一"里topo那一半: 只有TopoInstance带得出形态/位置/协议/端口数
HcclResult FetchTopoInstances(HcclComm comm, u32 myRank, u32 layer, std::vector<PhysicalLevelInfo>& out)
{
    out.clear();
    std::vector<u32> instIds;
    HcclResult ret = FetchTopoInstIds(comm, layer, instIds);
    if (ret != HCCL_SUCCESS) {
        return ret;
    }
    for (u32 instId : instIds) {
        ret = AppendTopoInstLevel(comm, myRank, layer, instId, out);
        if (ret != HCCL_SUCCESS) {
            return ret;
        }
    }
    return HCCL_SUCCESS;
}

/**
 * 按netLayer把ranktable层级与topo层级合成候选Level。合并规则:
 *   同范围的TopoInstance -> 与NetInstance合并成一级(view=GLOBAL); 更小的 -> 独立成级(view=LOCAL);
 *   该层没有TopoInstance -> NetInstance独立成级(hasTopoInst=false)。
 */
HcclResult BuildLayerCandidates(
    HcclComm comm, const TopoInfoWithNetLayerDetails* topoInfo, u32 layer, std::vector<PhysicalLevelInfo>& candidates)
{
    const u32 myRank = topoInfo->userRank;
    std::vector<u32> netRanks;
    std::vector<u32> instSizeListByLayer;
    CHK_RET(FetchNetInstance(comm, topoInfo, layer, netRanks, instSizeListByLayer));

    std::vector<PhysicalLevelInfo> topoLevels;
    CHK_RET(FetchTopoInstances(comm, myRank, layer, topoLevels));

    // 两侧都来自HCOMM且均为升序无重复, 直接比vector即可
    std::vector<u32> sortedNetRanks = netRanks;
    std::sort(sortedNetRanks.begin(), sortedNetRanks.end());

    bool merged = false;
    for (auto& level : topoLevels) {
        std::vector<u32> sortedTopoRanks = level.localRanks;
        std::sort(sortedTopoRanks.begin(), sortedTopoRanks.end());
        if (sortedTopoRanks == sortedNetRanks) {
            // 同范围: 把NetInstance的全局分区并进来。分区是该层的全局事实, 同层多个
            // 同范围TopoInstance(如netLayer 0的Mesh与CLOS)各自都持有它
            level.view = PhysicalLevelView::GLOBAL;
            level.instSizeListByLayer = instSizeListByLayer;
            merged = true;
        } else {
            // 比NetInstance更细: 看不到兄弟NetInstance, 没有全局分区可言
            level.view = PhysicalLevelView::LOCAL;
            level.instSizeListByLayer.clear();
        }
        HCCL_DEBUG(
            "[PhysicalLevel][Build] layer[%u] inst[%u] rankNum[%zu] view[%u] topoType[%d] locType[%d] "
            "protocolNum[%zu] portNumCnt[%zu]",
            layer, level.ref.topoInstId, level.localRanks.size(), static_cast<u32>(level.view),
            static_cast<s32>(level.topoType), static_cast<s32>(level.locType), level.protocols.size(),
            level.portNums.size());
        candidates.push_back(std::move(level));
    }

    if (!merged) {
        // 没有同范围的TopoInstance: 分区信息仍有效但拿不到链路属性, 用hasTopoInst=false显式注明
        PhysicalLevelInfo level;
        level.localRanks = std::move(netRanks);
        level.view = PhysicalLevelView::GLOBAL;
        level.instSizeListByLayer = std::move(instSizeListByLayer);
        level.ref.netLayer = layer;
        level.ref.topoInstId = INVALID_UINT;
        level.hasTopoInst = false;
        HCCL_DEBUG(
            "[PhysicalLevel][Build] layer[%u] has no same-range topo instance, level carries partition only, "
            "rankNum[%zu]",
            layer, level.localRanks.size());
        candidates.push_back(std::move(level));
    }
    return HCCL_SUCCESS;
}

HcclResult BuildPhysicalLevelCandidates(
    HcclComm comm, const TopoInfoWithNetLayerDetails* topoInfo, std::vector<PhysicalLevelInfo>& candidates)
{
    candidates.clear();
    if (topoInfo->netLayerDetails.netLayers.empty()) {
        HCCL_WARNING("[PhysicalLevel][Build] netLayers is empty, rank[%u]", topoInfo->userRank);
        return HCCL_E_INTERNAL;
    }
    // 只遍历netLayers里的layer: 规避HCOMM对非法layer的抛异常分支, 并保证ref.netLayer都来自
    // GetLayers的实际结果。收集先后无所谓, LevelLess是全序, 不依赖输入顺序
    for (u32 layer : topoInfo->netLayerDetails.netLayers) {
        CHK_RET(BuildLayerCandidates(comm, topoInfo, layer, candidates));
    }
    return HCCL_SUCCESS;
}

} // namespace

HcclResult BuildPhysicalLevels(HcclComm comm, TopoInfoWithNetLayerDetails* topoInfo)
{
    CHK_PTR_NULL(topoInfo);
    topoInfo->physicalLevels.clear();
    if (comm == nullptr) {
        HCCL_WARNING("[PhysicalLevel][Build] comm is null, physicalLevels stays empty");
        return HCCL_SUCCESS;
    }

    // 在临时对象中构建, 全部校验通过后再赋值; 降级或失败时physicalLevels保持为空
    std::vector<PhysicalLevelInfo> candidates;
    std::vector<PhysicalLevelInfo> levels;

    HcclResult ret = BuildPhysicalLevelCandidates(comm, topoInfo, candidates);
    if (ret == HCCL_SUCCESS) {
        ret = NormalizePhysicalLevels(candidates, topoInfo->userRank, topoInfo->userRankSize, levels);
    }
    if (ret == HCCL_SUCCESS) {
        ret = ValidatePhysicalLevels(levels, topoInfo->userRank, topoInfo->userRankSize);
    }
    if (ret != HCCL_SUCCESS) {
        // 任何失败一律降级, 不改变CalcTopoShape的返回值
        HCCL_WARNING(
            "[PhysicalLevel][Build] normalize degraded, ret[%d], rank[%u]. physicalLevels stays empty, legacy path "
            "unaffected.",
            ret, topoInfo->userRank);
        return HCCL_SUCCESS;
    }

    topoInfo->physicalLevels = std::move(levels);
    const size_t levelNum = topoInfo->physicalLevels.size();
    HCCL_RUN_INFO("[PhysicalLevel][Build] rank[%u] built [%zu] physical levels", topoInfo->userRank, levelNum);
    // 最终产物逐级各打一行, 打的是真正落进topoInfo的内容(BuildLayerCandidates那条DEBUG打的是候选)。
    // RUN_INFO让默认日志级别下就搜得到, INFO让这几行与同批INFO落在同一条时间线上
    for (size_t idx = 0; idx < levelNum; ++idx) {
        const std::string desc = DescribeLevel(topoInfo->physicalLevels[idx], idx, levelNum);
        HCCL_RUN_INFO("[PhysicalLevel][Build] rank[%u] %s", topoInfo->userRank, desc.c_str());
        HCCL_INFO("[PhysicalLevel][Build] rank[%u] %s", topoInfo->userRank, desc.c_str());
    }
    return HCCL_SUCCESS;
}

} // namespace mc2_ops_hccl
