/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file op_cfg_generator.h
 * \brief
 */

#ifndef STUB_OP_CFG_GENERATOR_H
#define STUB_OP_CFG_GENERATOR_H

#include <set>
#include "op_generator.h"
#include "op_generator_factory.h"
#include "register/op_def.h"
#include "register/op_def_factory.h"
#include "op_build_error_codes.h"

namespace ops {
const std::set<std::string> VALID_SOC_SET = {
    "ascend310b", "ascend310p", "ascend610", "ascend610lite", "ascend950", "ascend350",    "ascend910_93", "ascend910",
    "ascend910b", "bs9sx1a",    "bs9sx2a",   "mc61am21a",     "mc62",      "ascend910_96", "kirinx90",     "kirin9030",
};
class CfgGenerator : public Generator {
public:
    std::string GetDataTypeName(const ge::DataType& type) const;
    std::string GetParamTypeName(uint32_t paramType) const;
    void GetParamFormats(std::vector<ge::Format>& formats, std::string& fmtstr) const;
    void GetParamDataTypes(std::vector<ge::DataType>& types, std::string& tpstr) const;
    void GenParamInfo(std::ofstream& outfile, std::vector<OpParamDef>& param, bool isOutput) const;
    void GenAttrInfo(std::ofstream& outfile, std::vector<OpAttrDef>& attrs) const;
    void GenImplFile(std::ofstream& outfile, std::string& opType, OpAICoreConfig& aicoreConfig) const;
    void GenExtendInfo(std::ofstream& outfile, OpAICoreConfig& aicoreConfig, const bool enableFallBack) const;
    void GenMC2Info(std::ofstream& outfile, std::vector<ge::AscendString>& mc2Grps) const;
    void GenInitValue(std::ofstream& outfile, const std::string& type, const size_t ind, OpParamDef& def) const;
    void GenVectorInitValue(
        std::ofstream& outfile, OpParamDef& def, const std::vector<ge::DataType>& dataTypeVec,
        const std::string& typeName) const;
    void ParseSingleComputeUnitOfOp(
        OpDef& opsDef, std::string& opType, OpAICoreConfig& aicoreConfig, bool enableFallBack,
        std::ofstream& outfile) const;
    void GetOutFilePtr(
        std::string& genPath, std::string& socVer, std::ofstream& outfile, const std::string resolvedGenPath,
        std::map<std::string, std::string>& cfgFileStreams) const;
    void GenAllOpCfgWithoutComputeUint(
        const std::vector<std::string>& ops, std::string& genPath, const std::string resolvedGenPath,
        std::map<std::string, std::string>& cfgFileStreams) const;
    explicit CfgGenerator(std::vector<std::string>& ops);
    opbuild::Status GenerateCode(void) override;
    ~CfgGenerator() override = default;
};
} // namespace ops

#endif
