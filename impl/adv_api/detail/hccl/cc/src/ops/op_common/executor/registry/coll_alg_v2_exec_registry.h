/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef COLL_ALG_V2_EXEC_REGISTRY_H
#define COLL_ALG_V2_EXEC_REGISTRY_H

#include <unordered_map>
#include <functional>
#include <memory>
#include <mutex>
#include "executor_v2_base.h"

namespace mc2_ops_hccl {

using CollExecCreatorV2 = std::function<InsCollAlgBase*()>;

template <typename P>
static InsCollAlgBase* DefaultExecCreatorV2()
{
    static_assert(
        std::is_base_of<InsCollAlgBase, P>::value, "Executor type must derived from Hccl::DefaultExecCreatorV2");
    return new (std::nothrow) P();
}

class CollAlgExecRegistryV2 {
public:
    static CollAlgExecRegistryV2& Instance();
    HcclResult Register(const HcclCMDType type, const std::string& tag, const CollExecCreatorV2& collExecCreator);
    bool IsRegistered(const HcclCMDType type, const std::string& tag) const;
    std::unique_ptr<InsCollAlgBase> GetAlgExec(const HcclCMDType type, const std::string& tag);

private:
    std::map<HcclCMDType, std::map<std::string, const CollExecCreatorV2>> execCreators_;
    mutable std::mutex mu_;
};

#define REGISTER_EXECUTOR_IMPL_HELPER(ctr, type, name, insCollAlgBase) \
    static HcclResult g_func_##name##_##ctr =                          \
        CollAlgExecRegistryV2::Instance().Register(type, std::string(#name), DefaultExecCreatorV2<insCollAlgBase>)

#define REGISTER_EXECUTOR_IMPL_HELPER_1(ctr, type, name, insCollAlgBase) \
    REGISTER_EXECUTOR_IMPL_HELPER(ctr, type, name, insCollAlgBase)

#define REGISTER_EXECUTOR_IMPL(type, name, insCollAlgBase) \
    REGISTER_EXECUTOR_IMPL_HELPER_1(__COUNTER__, type, name, insCollAlgBase)

#define REGISTER_EXECUTOR_IMPL_HELPER_NO_TOPOMATCH(ctr, type, name, insCollAlgBase, InsAlgTemplate) \
    static HcclResult g_func_##name##_##ctr = CollAlgExecRegistryV2::Instance().Register(           \
        type, std::string(#name), DefaultExecCreatorV2<insCollAlgBase<InsAlgTemplate>>)

#define REGISTER_EXECUTOR_IMPL_HELPER_NO_TOPOMATCH_1(ctr, type, name, insCollAlgBase, InsAlgTemplate) \
    REGISTER_EXECUTOR_IMPL_HELPER_NO_TOPOMATCH(ctr, type, name, insCollAlgBase, InsAlgTemplate)

#define REGISTER_EXECUTOR_IMPL_NO_TOPOMATCH(type, name, insCollAlgBase, InsAlgTemplate) \
    REGISTER_EXECUTOR_IMPL_HELPER_NO_TOPOMATCH_1(__COUNTER__, type, name, insCollAlgBase, InsAlgTemplate)

#define REGISTER_EXECUTOR_HELPER(ctr, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate) \
    static HcclResult g_func_##name##_##ctr = CollAlgExecRegistryV2::Instance().Register(       \
        type, std::string(#name), DefaultExecCreatorV2<insCollAlgBase<AlgTopoMatch, InsAlgTemplate>>)

#define REGISTER_EXECUTOR_HELPER_1(ctr, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate) \
    REGISTER_EXECUTOR_HELPER(ctr, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate)

#define REGISTER_EXEC_V2(type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate) \
    REGISTER_EXECUTOR_HELPER_1(__COUNTER__, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate)

#define REGISTER_EXECUTOR_BY_TOPO_HELPER(ctr, type, name, insCollAlgBase, AlgTopoMatch)   \
    static HcclResult g_func_##name##_##ctr = CollAlgExecRegistryV2::Instance().Register( \
        type, std::string(#name), DefaultExecCreatorV2<insCollAlgBase<AlgTopoMatch>>)

#define REGISTER_EXECUTOR_BY_TOPO_HELPER_1(ctr, type, name, insCollAlgBase, AlgTopoMatch) \
    REGISTER_EXECUTOR_BY_TOPO_HELPER(ctr, type, name, insCollAlgBase, AlgTopoMatch)

#define REGISTER_EXECUTOR_BY_TOPO(type, name, insCollAlgBase, AlgTopoMatch) \
    REGISTER_EXECUTOR_BY_TOPO_HELPER_1(__COUNTER__, type, name, insCollAlgBase, AlgTopoMatch)

#define REGISTER_EXECUTOR_BY_TWO_TEMPS_HELPER(                                            \
    ctr, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1)      \
    static HcclResult g_func_##name##_##ctr = CollAlgExecRegistryV2::Instance().Register( \
        type, std::string(#name),                                                         \
        DefaultExecCreatorV2<insCollAlgBase<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1>>)

#define REGISTER_EXECUTOR_BY_TWO_TEMPS_HELPER_1(                                     \
    ctr, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1) \
    REGISTER_EXECUTOR_BY_TWO_TEMPS_HELPER(                                           \
        ctr, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1)

#define REGISTER_EXECUTOR_BY_TWO_TEMPS(type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1) \
    REGISTER_EXECUTOR_BY_TWO_TEMPS_HELPER_1(                                                                       \
        __COUNTER__, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1)

#define REGISTER_EXECUTOR_BY_FOUR_TEMPS_HELPER(                                                                        \
    ctr, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3) \
    static HcclResult g_func_##name##_##ctr = CollAlgExecRegistryV2::Instance().Register(                              \
        type, std::string(#name),                                                                                      \
        DefaultExecCreatorV2<                                                                                          \
            insCollAlgBase<AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3>>)

#define REGISTER_EXECUTOR_BY_FOUR_TEMPS_HELPER_1(                                                                      \
    ctr, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3) \
    REGISTER_EXECUTOR_BY_FOUR_TEMPS_HELPER(                                                                            \
        ctr, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2,              \
        InsAlgTemplate3)

#define REGISTER_EXECUTOR_BY_FOUR_TEMPS(                                                                          \
    type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, InsAlgTemplate3) \
    REGISTER_EXECUTOR_BY_FOUR_TEMPS_HELPER_1(                                                                     \
        __COUNTER__, type, name, insCollAlgBase, AlgTopoMatch, InsAlgTemplate0, InsAlgTemplate1, InsAlgTemplate2, \
        InsAlgTemplate3)
} // namespace mc2_ops_hccl
#endif
