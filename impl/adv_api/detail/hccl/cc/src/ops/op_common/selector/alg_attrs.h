/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef ALG_ATTRS_H
#define ALG_ATTRS_H

#include <string>

namespace mc2_ops_hccl {

enum class AlgoType : uint8_t;

// bitmask: bit position = static_cast<uint8_t>(Level0Shape)
// Level0Shape::CLOS=0 → bit0, MESH_1D=1 → bit1, MESH_1D_CLOS=2 → bit2
constexpr uint8_t LEVEL0_TOPO_CLOS = 0x01;         // 1 << CLOS(0)
constexpr uint8_t LEVEL0_TOPO_MESH_1D = 0x02;      // 1 << MESH_1D(1)
constexpr uint8_t LEVEL0_TOPO_MESH_1D_CLOS = 0x04; // 1 << MESH_1D_CLOS(2)
constexpr uint8_t LEVEL0_TOPO_ANY = 0xFF;

// bitmask: bit position = static_cast<uint8_t>(Level0MeshType)
// NOT_MESH=0, SINGLE_DIE=1, TWO_DIE_REGULAR=2, TWO_DIE_NOT_REGULAR=3
constexpr uint8_t MESH_TYPE_NOT_MESH = 0x01;            // 1 << NOT_MESH(0)
constexpr uint8_t MESH_TYPE_SINGLE_DIE = 0x02;          // 1 << SINGLE_DIE(1)
constexpr uint8_t MESH_TYPE_TWO_DIE_REGULAR = 0x04;     // 1 << TWO_DIE_REGULAR(2)
constexpr uint8_t MESH_TYPE_TWO_DIE_NOT_REGULAR = 0x08; // 1 << TWO_DIE_NOT_REGULAR(3)
constexpr uint8_t MESH_TYPE_ANY = 0xFF;

struct TopoAttrs {
    uint8_t minTopoLevelNum = 1;
    uint8_t maxTopoLevelNum = 3;
    uint8_t supportLevel0Topos = LEVEL0_TOPO_MESH_1D;
    uint8_t supportLevel0MeshTypes = MESH_TYPE_NOT_MESH | MESH_TYPE_SINGLE_DIE;
    bool isSupportLevel1Nhr = false;
    bool isSupport2DieFullMesh = false;
    bool isSupportLevel0PcieMix = false;
    bool requireAllMeshConnected = false;
    // 空表示支持全部设备类型，非空时仅支持集合中的设备
    std::set<DevType> supportDevTypes = {};
    bool isHostDpuOnly = false;

    // 定制拓扑过滤条件，由 costmodel 在通信域初始化时调用一次。
    // 返回 true：保留该算法参与后续选择；返回 false：过滤掉该算法。
    std::function<bool(const TopoInfoWithNetLayerDetails*)> topoCustomCheck = nullptr;
    // 优先级拓扑条件，在过滤完成后从可运行算法中筛选。
    // 返回 true：该算法被优先选中（仅保留返回 true 的算法）；返回 false：正常参与 cost 竞争。
    std::function<bool(const TopoInfoWithNetLayerDetails*)> topoPriorityCheck = nullptr;
};

struct OpAttrs {
    bool isSupportProd = true;
    std::set<HcclDataType> unsupportedDataTypes = {};
    // 白名单：非空时仅允许列出的数据类型，优先于 unsupportedDataTypes。
    // 空 = 不限制（走 unsupportedDataTypes 黑名单）。
    std::set<HcclDataType> supportedDataTypes = {};
    bool isSupportInplace = true;
    bool isSupportFloatOrderPreserved = false;

    // 定制算子过滤条件，由 costtable 每次算子调用时过滤。
    // 返回 true：保留该算法参与 cost 竞争；返回 false：过滤掉该算法。
    std::function<bool(const OpParam&, const TopoInfoWithNetLayerDetails*)> opCustomCheck = nullptr;
    // 优先级算子条件，在过滤完成后从可运行算法中筛选。
    // 返回 true：该算法被优先选中（仅保留返回 true 的算法）；返回 false：正常参与 cost 竞争。
    std::function<bool(const OpParam&, const TopoInfoWithNetLayerDetails*)> opPriorityCheck = nullptr;
};

struct AlgAttrs {
    std::string name;
    HcclCMDType opType = HcclCMDType::HCCL_CMD_INVALID;
    OpExecuteConfig engine = OpExecuteConfig::DEFAULT;
    std::vector<AlgoType> algoTypes;
    TopoAttrs topo;
    OpAttrs op;
};

} // namespace mc2_ops_hccl

#endif // ALG_ATTRS_H
