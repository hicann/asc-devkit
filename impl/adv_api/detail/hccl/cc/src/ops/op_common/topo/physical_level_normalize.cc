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
#include <cstring>
#include <functional>
#include <numeric>

#include "log.h"

namespace mc2_ops_hccl {
namespace {

void SortUnique(std::vector<u32>& ranks)
{
    std::sort(ranks.begin(), ranks.end());
    ranks.erase(std::unique(ranks.begin(), ranks.end()), ranks.end());
}

bool ContainsRank(const std::vector<u32>& sortedRanks, u32 rank)
{
    return std::binary_search(sortedRanks.begin(), sortedRanks.end(), rank);
}

/**
 * topoType定序, 含义是互联紧密度递减(MESH直连, CLOS经交换)。不能用枚举值代替 —— 枚举里
 * COMM_TOPO_CLOS=0 < COMM_TOPO_1DMESH=1, 正好相反。返回false(预期外类型)时必须整体降级。
 */
bool TopoTypeOrder(CommTopo type, u32& order)
{
    switch (type) {
        case CommTopo::COMM_TOPO_1DMESH:
            order = 0;
            return true;
        case CommTopo::COMM_TOPO_CLOS:
            order = 1;
            return true;
        default:
            return false;
    }
}

// 无TopoInstance的Level没有形态可言, 排在同键位的有形态Level之后。
// 取值必须与TopoTypeOrder的输出空间不重叠, 否则两类Level会在第三键上打平
constexpr u32 TOPO_TYPE_ORDER_NO_TOPO_INST = 2;

u32 LevelTopoOrder(const PhysicalLevelInfo& level)
{
    u32 order = TOPO_TYPE_ORDER_NO_TOPO_INST;
    if (level.hasTopoInst) {
        // 排序前已逐个校验过topoType可定序, 此处必然成功
        (void)TopoTypeOrder(level.topoType, order);
    }
    return order;
}

// 排序三键 + 两个确定性兜底键, 各键语义见下方逐段注释
bool LevelLess(const PhysicalLevelInfo& lhs, const PhysicalLevelInfo& rhs)
{
    // 键1: 当前rank在该级的块大小。GLOBAL与LOCAL级的localRanks同量纲, 可直接比较
    if (lhs.localRanks.size() != rhs.localRanks.size()) {
        return lhs.localRanks.size() < rhs.localRanks.size();
    }
    // 键2: LOCAL(0)在GLOBAL(1)之前
    if (lhs.view != rhs.view) {
        return lhs.view < rhs.view;
    }
    // 键3: netLayer 0同时挂MESH和CLOS且rank集合相同时, 前两键全部打平, 定序完全依赖这一键
    const u32 lhsOrder = LevelTopoOrder(lhs);
    const u32 rhsOrder = LevelTopoOrder(rhs);
    if (lhsOrder != rhsOrder) {
        return lhsOrder < rhsOrder;
    }
    // 兜底键1, 正常输入上永不决定顺序: 此时两级必然互相重叠且互不包含, 会在链校验中被拒。
    // 保留它只为让"被拒"这件事本身也是确定的, 不随RankGraph的哈希返回序抖动
    if (lhs.localRanks != rhs.localRanks) {
        return lhs.localRanks < rhs.localRanks;
    }
    // 兜底键2: 原始身份。前面所有键都相同仍可能是两个不同Level(如两个netLayer的本地
    // NetInstance恰好同范围)。少了这一键它们在比较器下等价, std::sort的相对顺序未指定,
    // 各rank排出的下标语义会分叉; netLayer与topoInstId跨rank一致, 补上后比较器成为全序
    if (lhs.ref.netLayer != rhs.ref.netLayer) {
        return lhs.ref.netLayer < rhs.ref.netLayer;
    }
    return lhs.ref.topoInstId < rhs.ref.topoInstId;
}

bool IsStrictlyAscending(const std::vector<u32>& ranks)
{
    for (size_t idx = 1; idx < ranks.size(); idx++) {
        if (ranks[idx] <= ranks[idx - 1]) {
            return false;
        }
    }
    return true;
}

bool IsSuperSetOf(const std::vector<u32>& outer, const std::vector<u32>& inner)
{
    return std::includes(outer.begin(), outer.end(), inner.begin(), inner.end());
}

} // namespace

bool EndpointDescLess(const EndpointDesc& lhs, const EndpointDesc& rhs)
{
    if (lhs.protocol != rhs.protocol) {
        return lhs.protocol < rhs.protocol;
    }
    if (lhs.loc.locType != rhs.loc.locType) {
        return lhs.loc.locType < rhs.loc.locType;
    }
    if (lhs.commAddr.type != rhs.commAddr.type) {
        return lhs.commAddr.type < rhs.commAddr.type;
    }
    return memcmp(lhs.commAddr.raws, rhs.commAddr.raws, sizeof(lhs.commAddr.raws)) < 0;
}

bool CommAddrEqual(const CommAddr& lhs, const CommAddr& rhs)
{
    return lhs.type == rhs.type && memcmp(lhs.raws, rhs.raws, sizeof(lhs.raws)) == 0;
}

HcclResult NormalizePhysicalLevels(
    std::vector<PhysicalLevelInfo>& candidates, u32 userRank, u32 userRankSize, std::vector<PhysicalLevelInfo>& levels)
{
    levels.clear();
    if (userRankSize == 0 || userRank >= userRankSize) {
        HCCL_WARNING(
            "[PhysicalLevel][Normalize] invalid rank info, userRank[%u], userRankSize[%u]", userRank, userRankSize);
        return HCCL_E_NOT_SUPPORT;
    }

    // 1. 归一: rank列表排序去重、portNums降序、protocols去重升序, 剔除不含当前rank的候选,
    //    并确认topoType可定序(排序第三键的前提)。instSizeListByLayer不参与归一 —— 重排会毁掉布局语义。
    //    构建侧已做过同样的规范化, 这一步在正常路径上幂等, 保留是为了让本函数可离线UT
    std::vector<PhysicalLevelInfo> validCands;
    validCands.reserve(candidates.size());
    for (auto& cand : candidates) {
        SortUnique(cand.localRanks);
        // 不能去重: 两条8口链路就是{8,8}, 求和才是总端口数。去重的是iface, 不是端口数值
        std::sort(cand.portNums.begin(), cand.portNums.end(), std::greater<u32>());
        std::sort(cand.protocols.begin(), cand.protocols.end());
        cand.protocols.erase(std::unique(cand.protocols.begin(), cand.protocols.end()), cand.protocols.end());
        if (!ContainsRank(cand.localRanks, userRank)) {
            HCCL_DEBUG(
                "[PhysicalLevel][Normalize] drop candidate without myRank[%u], rankNum[%zu]", userRank,
                cand.localRanks.size());
            continue;
        }
        u32 unusedOrder = 0;
        if (cand.hasTopoInst && !TopoTypeOrder(cand.topoType, unusedOrder)) {
            HCCL_WARNING(
                "[PhysicalLevel][Normalize] level at layer[%u] inst[%u] has unorderable topoType[%d], rank[%u]",
                cand.ref.netLayer, cand.ref.topoInstId, static_cast<s32>(cand.topoType), userRank);
            return HCCL_E_NOT_SUPPORT;
        }
        validCands.push_back(std::move(cand));
    }
    if (validCands.empty()) {
        HCCL_WARNING("[PhysicalLevel][Normalize] no valid candidate for rank[%u]", userRank);
        return HCCL_E_NOT_SUPPORT;
    }

    // 2. 三键排序, 不做合并(Level与NetInstance/TopoInstance一一对应)。用sort而非stable_sort:
    //    LevelLess是全序, 结果与输入顺序无关 —— 输入顺序来自RankGraph的哈希遍历, 本就不可依赖
    std::sort(validCands.begin(), validCands.end(), LevelLess);
    levels = std::move(validCands);

    // 3. 链校验: 相邻范围必须满足包含关系, 允许相等。
    //    互相重叠但互不包含的范围(典型为2D Mesh的x/y环)在此被拒绝
    for (size_t idx = 1; idx < levels.size(); idx++) {
        if (!IsSuperSetOf(levels[idx].localRanks, levels[idx - 1].localRanks)) {
            HCCL_WARNING(
                "[PhysicalLevel][Normalize] level[%zu] with rankNum[%zu] does not contain level[%zu] with "
                "rankNum[%zu], ranges do not form a chain, rank[%u]",
                idx, levels[idx].localRanks.size(), idx - 1, levels[idx - 1].localRanks.size(), userRank);
            levels.clear();
            return HCCL_E_NOT_SUPPORT;
        }
    }

    HCCL_INFO("[PhysicalLevel][Normalize] rank[%u] got [%zu] levels", userRank, levels.size());
    return HCCL_SUCCESS;
}

namespace {

// 非空、升序严格递增(等价于无重复)、无越界、含当前rank
HcclResult ValidateRankList(const PhysicalLevelInfo& level, size_t idx, u32 userRank, u32 userRankSize)
{
    if (level.localRanks.empty() || !IsStrictlyAscending(level.localRanks)) {
        HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] rank list is empty or not ascending", idx);
        return HCCL_E_NOT_SUPPORT;
    }
    if (level.localRanks.back() >= userRankSize) {
        HCCL_WARNING(
            "[PhysicalLevel][Validate] level[%zu] max rank[%u] exceeds userRankSize[%u]", idx, level.localRanks.back(),
            userRankSize);
        return HCCL_E_NOT_SUPPORT;
    }
    if (!ContainsRank(level.localRanks, userRank)) {
        HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] does not contain myRank[%u]", idx, userRank);
        return HCCL_E_NOT_SUPPORT;
    }
    return HCCL_SUCCESS;
}

// view必须是有效枚举值。底层类型是u32, 不白名单则非法值会静默落进else被当成GLOBAL
// ref.netLayer恒有效。无效值说明构建侧漏填, 消费侧回查时会拿到错误的层
HcclResult ValidateViewAndRef(const PhysicalLevelInfo& level, size_t idx)
{
    if (level.view != PhysicalLevelView::LOCAL && level.view != PhysicalLevelView::GLOBAL) {
        HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] has invalid view[%u]", idx, static_cast<u32>(level.view));
        return HCCL_E_NOT_SUPPORT;
    }
    if (level.ref.netLayer == INVALID_UINT) {
        HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] has no valid netLayer", idx);
        return HCCL_E_NOT_SUPPORT;
    }
    return HCCL_SUCCESS;
}

// 链路属性必须自洽, 否则消费侧会把无效值当成真实链路事实建模
HcclResult ValidateTopoInstAttrs(const PhysicalLevelInfo& level, size_t idx)
{
    if (level.ref.topoInstId == INVALID_UINT) {
        HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] hasTopoInst but topoInstId is invalid", idx);
        return HCCL_E_NOT_SUPPORT;
    }
    // portNums按iface去重, 条数不会超过endpoint数; 超过说明去重逻辑坏了, 总端口数会被算大
    if (level.portNums.size() > level.endpoints.size()) {
        HCCL_WARNING(
            "[PhysicalLevel][Validate] level[%zu] portNum count[%zu] exceeds endpoint count[%zu]", idx,
            level.portNums.size(), level.endpoints.size());
        return HCCL_E_NOT_SUPPORT;
    }
    // 0能完整穿过下面的降序检查(排在末尾), 于是不存在的链路会被当成真实出口计入
    for (u32 portNum : level.portNums) {
        if (portNum == 0 || portNum > PORT_NUM_SANITY_LIMIT) {
            HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] has implausible portNum[%u]", idx, portNum);
            return HCCL_E_NOT_SUPPORT;
        }
    }
    // 降序规范化: 采集顺序来自endpoints的哈希序, 不规范化则跨进程字节流不同
    if (!std::is_sorted(level.portNums.begin(), level.portNums.end(), std::greater<u32>())) {
        HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] portNums is not sorted descending", idx);
        return HCCL_E_NOT_SUPPORT;
    }
    // protocols去重升序, 理由同上
    if (!std::is_sorted(level.protocols.begin(), level.protocols.end()) ||
        std::adjacent_find(level.protocols.begin(), level.protocols.end()) != level.protocols.end()) {
        HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] protocols is not sorted and unique", idx);
        return HCCL_E_NOT_SUPPORT;
    }
    return HCCL_SUCCESS;
}

// 全部链路属性必须保持无效值。
HcclResult ValidateNoTopoInstAttrs(const PhysicalLevelInfo& level, size_t idx)
{
    const bool clean = level.ref.topoInstId == INVALID_UINT && level.topoType == CommTopo::COMM_TOPO_RESERVED &&
                       level.locType == EndpointLocType::ENDPOINT_LOC_TYPE_RESERVED && level.protocols.empty() &&
                       level.portNums.empty() && level.endpoints.empty();
    if (!clean) {
        HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] has no topo instance but carries link attributes", idx);
        return HCCL_E_NOT_SUPPORT;
    }
    return HCCL_SUCCESS;
}

// GLOBAL级的instSizeListByLayer是该netLayer对整个通信域的一次完整划分
HcclResult ValidatePartitionList(const PhysicalLevelInfo& level, size_t idx, u32 userRankSize)
{
    const auto begin = level.instSizeListByLayer.begin();
    const auto end = level.instSizeListByLayer.end();
    const u32 total = std::accumulate(begin, end, 0U);
    if (level.instSizeListByLayer.empty() || total != userRankSize) {
        HCCL_WARNING(
            "[PhysicalLevel][Validate] level[%zu] instSizeListByLayer sum[%u] mismatches userRankSize[%u]", idx, total,
            userRankSize);
        return HCCL_E_NOT_SUPPORT;
    }
    // 每个分区非空。0能完整穿过本块其余检查, 于是幽灵空分区会被当成真实Instance计入
    if (std::find(begin, end, 0U) != end) {
        HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] instSizeListByLayer contains a zero entry", idx);
        return HCCL_E_NOT_SUPPORT;
    }
    // 当前rank的块大小必须是这一层的某个真实分区。这是instSizeListByLayer上
    // 唯一一条既能本地验证、又与元素顺序无关的性质 —— HCOMM侧以unordered_map实现,
    // 返回序在非对称拓扑上会真实乱序, 任何依赖下标位置的校验都不成立
    if (std::find(begin, end, static_cast<u32>(level.localRanks.size())) == end) {
        HCCL_WARNING(
            "[PhysicalLevel][Validate] level[%zu] localRankNum[%zu] is not one of the inst sizes", idx,
            level.localRanks.size());
        return HCCL_E_NOT_SUPPORT;
    }
    return HCCL_SUCCESS;
}

/**
 * view与instSizeListByLayer是否为空严格等价; 错开之后消费侧会把只知道本块的级
 * 当成全局分区来切算法。
 */
HcclResult ValidatePartition(const PhysicalLevelInfo& level, size_t idx, u32 userRankSize)
{
    if (level.view == PhysicalLevelView::LOCAL) {
        if (!level.instSizeListByLayer.empty()) {
            HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] is LOCAL but carries partition sizes", idx);
            return HCCL_E_NOT_SUPPORT;
        }
        return HCCL_SUCCESS;
    }
    return ValidatePartitionList(level, idx, userRankSize);
}

HcclResult ValidateLevel(const PhysicalLevelInfo& level, size_t idx, u32 userRank, u32 userRankSize)
{
    HcclResult ret = ValidateRankList(level, idx, userRank, userRankSize);
    if (ret == HCCL_SUCCESS) {
        ret = ValidateViewAndRef(level, idx);
    }
    if (ret == HCCL_SUCCESS) {
        ret = level.hasTopoInst ? ValidateTopoInstAttrs(level, idx) : ValidateNoTopoInstAttrs(level, idx);
    }
    if (ret == HCCL_SUCCESS) {
        ret = ValidatePartition(level, idx, userRankSize);
    }
    return ret;
}

// 不变量7: 大小非递减 + 包含链。允许相等 —— netLayer 0上同范围的MESH与CLOS两级、
// 两个netLayer的本地NetInstance恰好同范围, 都是合法的相等相邻对
HcclResult ValidateChain(const std::vector<PhysicalLevelInfo>& levels)
{
    for (size_t idx = 1; idx < levels.size(); idx++) {
        if (levels[idx].localRanks.size() < levels[idx - 1].localRanks.size()) {
            HCCL_WARNING(
                "[PhysicalLevel][Validate] level[%zu] rankNum[%zu] is less than level[%zu] rankNum[%zu]", idx,
                levels[idx].localRanks.size(), idx - 1, levels[idx - 1].localRanks.size());
            return HCCL_E_NOT_SUPPORT;
        }
        if (!IsSuperSetOf(levels[idx].localRanks, levels[idx - 1].localRanks)) {
            HCCL_WARNING("[PhysicalLevel][Validate] level[%zu] does not contain level[%zu]", idx, idx - 1);
            return HCCL_E_NOT_SUPPORT;
        }
    }
    return HCCL_SUCCESS;
}

// 身份(netLayer, topoInstId)全域唯一。LevelLess的兜底键正是靠这两项才构成全序,
// 重复则两个Level在比较器下等价, std::sort的相对顺序未指定, 各rank的下标语义会分叉。
// levels规模是个位数(上限PHYSICAL_LEVEL_NUM_LIMIT), 两两比较不需要额外容器
HcclResult ValidateUniqueSource(const std::vector<PhysicalLevelInfo>& levels)
{
    for (size_t i = 0; i < levels.size(); i++) {
        for (size_t j = i + 1; j < levels.size(); j++) {
            if (levels[i].ref.netLayer == levels[j].ref.netLayer &&
                levels[i].ref.topoInstId == levels[j].ref.topoInstId) {
                HCCL_WARNING(
                    "[PhysicalLevel][Validate] level[%zu] and level[%zu] share the same source: layer[%u] inst[%u]", i,
                    j, levels[i].ref.netLayer, levels[i].ref.topoInstId);
                return HCCL_E_NOT_SUPPORT;
            }
        }
    }
    return HCCL_SUCCESS;
}

} // namespace

HcclResult ValidatePhysicalLevels(const std::vector<PhysicalLevelInfo>& levels, u32 userRank, u32 userRankSize)
{
    // 不变量1
    if (userRankSize == 0 || userRank >= userRankSize) {
        HCCL_WARNING(
            "[PhysicalLevel][Validate] invalid rank info, userRank[%u], userRankSize[%u]", userRank, userRankSize);
        return HCCL_E_NOT_SUPPORT;
    }
    if (levels.empty()) {
        HCCL_WARNING("[PhysicalLevel][Validate] levels is empty, rank[%u]", userRank);
        return HCCL_E_NOT_SUPPORT;
    }

    for (size_t idx = 0; idx < levels.size(); idx++) {
        HcclResult ret = ValidateLevel(levels[idx], idx, userRank, userRankSize);
        if (ret != HCCL_SUCCESS) {
            return ret;
        }
    }

    HcclResult ret = ValidateChain(levels);
    if (ret == HCCL_SUCCESS) {
        ret = ValidateUniqueSource(levels);
    }
    return ret;
}

} // namespace mc2_ops_hccl
