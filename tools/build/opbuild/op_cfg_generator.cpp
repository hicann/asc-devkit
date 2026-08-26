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
 * \file op_cfg_generator.cpp
 * \brief
 */

#include <fstream>
#include <sstream>
#include <cctype>
#include <unordered_map>
#include <limits.h>
#include "ascendc_tool_log.h"
#include "op_cfg_generator.h"
#include "op_build_params.h"
#include "op_dtype_name_utils.h"

namespace ops {
namespace {
std::string NormalizeSupportedSoc(const std::string& socVer)
{
    std::string lowerSocVer = socVer;
    for (char& c : lowerSocVer) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    if (VALID_SOC_SET.find(lowerSocVer) != VALID_SOC_SET.end()) {
        return lowerSocVer;
    }
    return socVer;
}
} // namespace

std::string CfgGenerator::GetDataTypeName(const ge::DataType& type) const { return FindCfgDataTypeName(type); }

std::string CfgGenerator::GetParamTypeName(uint32_t paramType) const
{
    std::vector<std::string> ptstrs = {"ignore", "optional", "required", "dynamic", "optional"};
    if (paramType >= ptstrs.size()) {
        return std::string("unknow");
    }
    return ptstrs[paramType];
}

void CfgGenerator::GetParamFormats(std::vector<ge::Format>& formats, std::string& fmtstr) const
{
    fmtstr = "";
    for (auto fmt : formats) {
        fmtstr += std::string(ge::GetFormatName(fmt)) + ",";
    }
    fmtstr.resize(fmtstr.size() - 1);
}

void CfgGenerator::GetParamDataTypes(std::vector<ge::DataType>& types, std::string& tpstr) const
{
    tpstr = "";
    for (auto type : types) {
        tpstr += GetDataTypeName(type) + ",";
    }
    tpstr.resize(tpstr.size() - 1);
}

bool inline CfgDtypeFormatCheck(
    const std::string& paramterName, const std::string& firstArgName, const std::string& secondArgName,
    const size_t firstArgSize, const size_t secondArgSize)
{
    // Check if size of arg1 and arg2 are equal.
    if (firstArgSize > 0 && secondArgSize > 0 && firstArgSize != secondArgSize) {
        ASCENDLOGE(
            "InitValue parameter :%s %s and %s size do not match, %s size is %zu, %s size is %zu", paramterName.c_str(),
            firstArgName.c_str(), secondArgName.c_str(), firstArgName.c_str(), firstArgSize, secondArgName.c_str(),
            secondArgSize);
        Generator::SetErrorMessage("CfgDtypeFormatCheck: InitValue size do not match dtype, View plog for details");
        return false;
    }
    return true;
}

void GenSingleInitValueTypeAndValue(std::ofstream& outfile, const ScalarVar& scalar)
{
    switch (scalar.scalar_type) {
        case ScalarType::UINT64:
            outfile << "\"type\": \"uint64\", \"value\": " << std::to_string(scalar.scalar_num.value_u64);
            break;
        case ScalarType::INT64:
            outfile << "\"type\": \"int64\", \"value\": " << std::to_string(scalar.scalar_num.value_i64);
            break;
        case ScalarType::UINT32:
            outfile << "\"type\": \"uint32\", \"value\": " << std::to_string(scalar.scalar_num.value_u64);
            break;
        case ScalarType::INT32:
            outfile << "\"type\": \"int32\", \"value\": " << std::to_string(scalar.scalar_num.value_i64);
            break;
        case ScalarType::UINT16:
            outfile << "\"type\": \"uint16\", \"value\": " << std::to_string(scalar.scalar_num.value_u64);
            break;
        case ScalarType::INT16:
            outfile << "\"type\": \"int16\", \"value\": " << std::to_string(scalar.scalar_num.value_i64);
            break;
        case ScalarType::UINT8:
            outfile << "\"type\": \"uint8\", \"value\": " << std::to_string(scalar.scalar_num.value_u64);
            break;
        case ScalarType::INT8:
            outfile << "\"type\": \"int8\", \"value\": " << std::to_string(scalar.scalar_num.value_i64);
            break;
        case ScalarType::FLOAT32:
            outfile << "\"type\": \"float32\", \"value\": " << std::to_string(scalar.scalar_num.value_f32);
            break;
        case ScalarType::FLOAT16:
            outfile << "\"type\": \"float16\", \"value\": " << std::to_string(scalar.scalar_num.value_f32);
            break;
        default:
            ASCENDLOGE("InitValue(ScalarVar) use unsupport type, please check!");
            Generator::SetErrorMessage(
                "GenSingleInitValueTypeAndValue: InitValue(ScalarVar) use unsupport type, View plog for details");
            break;
    }
    return;
}

void CfgGenerator::GenVectorInitValue(
    std::ofstream& outfile, OpParamDef& def, const std::vector<ge::DataType>& dataTypeVec,
    const std::string& typeName) const
{
    auto& scalarVec = def.GetInitValueList();
    // check initValue and dtype size are equal.
    if (!CfgDtypeFormatCheck(
            def.GetParamName().GetString(), "initValue", typeName, scalarVec.size(), dataTypeVec.size())) {
        return;
    }
    // push initValue into map and check initvalues of the same type are the same.
    std::unordered_map<ge::DataType, ScalarVar> typeToScalars;
    for (size_t scalarIndex = 0; scalarIndex < scalarVec.size(); ++scalarIndex) {
        if (typeToScalars.find(dataTypeVec[scalarIndex]) != typeToScalars.end()) {
            auto& judgeScalar = typeToScalars[dataTypeVec[scalarIndex]];
            if (judgeScalar == scalarVec[scalarIndex]) {
                continue;
            }
            // if initValue of the same type are not the same, print error log and exit.
            ASCENDLOGE(
                "InitValue(std::vector<ScalarVar>) should ensure that same type has the same initValue "
                "Scalar, paramName: %s, dtype %s has more than two different initValue",
                def.GetParamName().GetString(), GetDataTypeName(dataTypeVec[scalarIndex]).c_str());
            Generator::SetErrorMessage(
                "GenVectorInitValue: initValue of the same type are not the same, View plog for details");
            return;
        } else {
            typeToScalars.emplace(dataTypeVec[scalarIndex], scalarVec[scalarIndex]);
            ASCENDLOGI(
                "InitValue push back, dtype: %s initValue type: %u, initValue num: %lu\n",
                GetDataTypeName(dataTypeVec[scalarIndex]).c_str(),
                static_cast<uint32_t>(scalarVec[scalarIndex].scalar_type), scalarVec[scalarIndex].scalar_num.value_u64);
        }
    }
    bool commmaFlag = true;
    for (auto& typeToScalar : typeToScalars) {
        if (commmaFlag) {
            commmaFlag = false;
        } else {
            outfile << ", ";
        }
        outfile << "\"" << GetDataTypeName(typeToScalar.first) << "\": { ";
        GenSingleInitValueTypeAndValue(outfile, typeToScalar.second);
        outfile << " }";
    }
}

void CfgGenerator::GenInitValue(
    std::ofstream& outfile, const std::string& type, const size_t ind, OpParamDef& def) const
{
    bool hasGenInitValue = false;
    // initValue by InitValue(uint64_t).
    if (def.GetInitValueType() != InitValueType::INIT_VALUE_DEFAULT) {
        switch (def.GetInitValueType()) {
            case InitValueType::INIT_VALUE_UINT64_T:
                hasGenInitValue = true;
                if (def.GetInitValue().value_u64 != 0) {
                    ASCENDLOGW(
                        "Parameter: %s initValue is %lu, when the parameter type is not uint64_t, undefined "
                        "behavior may occur.",
                        def.GetParamName().GetString(), def.GetInitValue().value_u64);
                    Generator::SetErrorMessage(
                        "GenInitValue: The parameter type of initvalue is not uint64_t, View plog for details");
                }
                ASCENDLOGI(
                    "The initial value of the paramete: %s is set to %lu by InitValue(uint64_t).",
                    def.GetParamName().GetString(), def.GetInitValue().value_u64);
                outfile << type << ind << ".initValue=" << std::to_string(def.GetInitValue().value_u64) << std::endl;
                break;
            default:
                ASCENDLOGE("InitValue(uint64_t) only support uint64_t now, please check!");
                Generator::SetErrorMessage(
                    "GenInitValue: InitValue(uint64_t) only support uint64_t now, View plog for details");
                break;
        }
    }
    // initValue by InitValue(ScalarVar) or InitValue(std::vector<ScalarVar>)
    if (def.GetInitValueList().size() > 0) {
        if (hasGenInitValue) {
            ASCENDLOGW(
                "Parameter: %s set initvalue in different ways at the same time results in undefined behavior.",
                def.GetParamName().GetString());
            Generator::SetErrorMessage("GenInitValue: Set initvalue in different ways at the same time results in "
                                       "undefined behavior, View plog for details");
            return;
        }
        // initValue by InitValue(ScalarVar)
        if (def.GetInitValueList().size() == 1) {
            ASCENDLOGI(
                "The initValue of the paramete: %s is set by InitValue(ScalarVar).", def.GetParamName().GetString());
            auto scalar = def.GetInitValueList()[0];
            outfile << type << ind << ".initValue={ \"is_list\" : false, ";
            GenSingleInitValueTypeAndValue(outfile, scalar);
            outfile << "}" << std::endl;
        } else { // initValue by InitValue(std::vector<ScalarVar>)
            ASCENDLOGI(
                "The initValue of the parameter: %s is set by InitValue(std::vector<ScalarVar>).",
                def.GetParamName().GetString());
            outfile << type << ind << ".initValue={ \"is_list\" : true, ";
            // type not set by list
            if (def.IsDtype()) {
                GenVectorInitValue(outfile, def, def.GetOriginDataTypes(), "type");
            } else { // type set by list
                GenVectorInitValue(outfile, def, def.GetDataTypesList(), "typeList");
            }
            outfile << "}" << std::endl;
        }
    }
    return;
}

void CfgGenerator::GenParamInfo(std::ofstream& outfile, std::vector<OpParamDef>& param, bool isOutput) const
{
    std::string type = (isOutput ? "output" : "input");
    std::string tpstr;
    std::string fmtstr;
    size_t ind = 0;
    for (auto def : param) {
        outfile << type << ind << ".name=" << def.GetParamName().GetString() << std::endl;
        if (def.GetDataTypes().size() > 0) {
            GetParamDataTypes(def.GetDataTypes(), tpstr);
            outfile << type << ind << ".dtype=" << tpstr << std::endl;
        }
        if (def.IsSetDtypeForBin()) {
            GetParamDataTypes(def.GetDataTypesForBin(), tpstr);
            outfile << type << ind << ".for_bin_dtype=" << tpstr << std::endl;
        }
        if (def.GetFormats().size() > 0) {
            GetParamFormats(def.GetFormats(), fmtstr);
            outfile << type << ind << ".format=" << fmtstr << std::endl;
        }
        if (def.IsSetFormatForBin()) {
            GetParamFormats(def.GetFormatsForBin(), tpstr);
            outfile << type << ind << ".for_bin_format=" << tpstr << std::endl;
        }
        if (def.GetUnknownShapeFormats().size() > 0) {
            GetParamFormats(def.GetUnknownShapeFormats(), fmtstr);
            outfile << type << ind << ".unknownshape_format=" << fmtstr << std::endl;
        }
        outfile << type << ind << ".shape=all" << std::endl;
        outfile << type << ind << ".paramType=" << GetParamTypeName(def.GetParamType()) << std::endl;
        if (def.GetParamType() == VIRTUAL) {
            outfile << type << ind << ".virtual=true" << std::endl;
        }
        if (def.GetValueDepend().GetLength() > 0) {
            outfile << type << ind << ".valueDepend=" << def.GetValueDepend().GetString() << std::endl;
        }
        GenInitValue(outfile, type, ind, def);
        if (def.IsOutputShapeDependOnCompute() == true) {
            outfile << type << ind << ".outputShapeDependOnCompute=true" << std::endl;
        }
        ind++;
    }
}

void CfgGenerator::GenAttrInfo(std::ofstream& outfile, std::vector<OpAttrDef>& attrs) const
{
    std::string attrList = "";

    if (attrs.size() <= 0) {
        return;
    }
    for (auto attr : attrs) {
        attrList += std::string(attr.GetName().GetString()) + ",";
    }
    attrList.resize(attrList.size() - 1);
    outfile << "attr.list=" << attrList << std::endl;
    for (auto attr : attrs) {
        outfile << "attr_" << attr.GetName().GetString() << ".type=" << attr.GetCfgDataType().GetString() << std::endl;
        outfile << "attr_" << attr.GetName().GetString() << ".value=all" << std::endl;
        outfile << "attr_" << attr.GetName().GetString()
                << ".paramType=" << (attr.IsRequired() ? "required" : "optional") << std::endl;
        if (!attr.IsRequired()) {
            outfile << "attr_" << attr.GetName().GetString()
                    << ".defaultValue=" << attr.GetAttrDefaultVal("[]").GetString() << std::endl;
        }
    }
}

void CfgGenerator::GenImplFile(std::ofstream& outfile, std::string& opType, OpAICoreConfig& aicoreConfig) const
{
    std::string snakeName = ConvertToSnakeCase(opType);
    std::map<ge::AscendString, ge::AscendString>& cfgInfo = aicoreConfig.GetCfgInfo();
    auto it = cfgInfo.find("opFile.value");
    if (it == cfgInfo.cend()) {
        outfile << "opFile.value=" << snakeName << std::endl;
    }
    it = cfgInfo.find("opInterface.value");
    if (it == cfgInfo.cend()) {
        outfile << "opInterface.value=" << snakeName << std::endl;
    }
}

void CfgGenerator::GenExtendInfo(std::ofstream& outfile, OpAICoreConfig& aicoreConfig, const bool enableFallBack) const
{
    bool hasSetAclnnSupport = false;
    bool hasSetPrebuildPattern = false;
    bool hasSetCoreType = false;
    bool hasSetJitCompile = false;
    std::string sourceFlag = "";
    const char* sourcePackage = std::getenv("ENABLE_SOURCE_PACKAGE");
    if (sourcePackage != nullptr && strlen(sourcePackage) != 0) {
        sourceFlag = std::string(sourcePackage);
    }
    std::map<ge::AscendString, ge::AscendString>& cfgInfo = aicoreConfig.GetCfgInfo();
    for (auto& key : aicoreConfig.GetCfgKeys()) {
        outfile << key.GetString() << "=" << cfgInfo[key].GetString() << std::endl;
        if (std::string(key.GetString()) == "aclnnSupport.value") {
            hasSetAclnnSupport = true;
        }
        if (std::string(key.GetString()) == "prebuildPattern.value") {
            hasSetPrebuildPattern = true;
        }
        if (std::string(key.GetString()) == "coreType.value") {
            hasSetCoreType = true;
        }
        if (std::string(key.GetString()) == "jitCompile.flag") {
            hasSetCoreType = true;
        }
    }
    if (enableFallBack && !hasSetAclnnSupport) {
        outfile << "aclnnSupport.value=support_aclnn" << std::endl;
    }
    if (!hasSetPrebuildPattern) {
        // The AscendC operator do not have prebuild schedule, which used to tbe codegen and soon).
        outfile << "prebuildPattern.value=Opaque" << std::endl;
    }
    if (!hasSetCoreType) {
        // The AscendC operator can only be an Aicore operator.
        outfile << "coreType.value=AiCore" << std::endl;
    }
    if (sourceFlag == "False" && !hasSetJitCompile) {
        // Disable source code release and configure the ini to
        // enable both dynamic and static reuse binary by default.
        outfile << "jitCompile.flag=static_false,dynamic_false" << std::endl;
    }
}

void CfgGenerator::GenMC2Info(std::ofstream& outfile, std::vector<ge::AscendString>& mc2Grps) const
{
    uint32_t index = 0;
    std::string mc2Cfg = "";
    for (; index < mc2Grps.size(); index++) {
        auto name = std::string("mc2_context_") + static_cast<char>(0x30 + index);
        mc2Cfg += name + ",";
    }
    mc2Cfg.resize(mc2Cfg.size() - 1);
    outfile << "mc2.ctx=" << mc2Cfg << std::endl;
}

void Split(const std::string& str, const char delimiter, std::vector<std::string>& result)
{
    std::stringstream ss(str);
    std::string tmp;
    while (std::getline(ss, tmp, delimiter)) {
        result.push_back(tmp);
    }
}

void CfgGenerator::ParseSingleComputeUnitOfOp(
    OpDef& opsDef, std::string& opType, OpAICoreConfig& aicoreConfig, bool enableFallBack, std::ofstream& outfile) const
{
    outfile << "[" << opsDef.GetOpType().GetString() << "]" << std::endl;
    std::vector<OpParamDef> inputs = opsDef.GetMergeInputs(aicoreConfig);
    std::vector<OpParamDef> outputs = opsDef.GetMergeOutputs(aicoreConfig);
    GenParamInfo(outfile, inputs, false);
    GenParamInfo(outfile, outputs, true);
    GenAttrInfo(outfile, opsDef.GetAttrs());
    GenExtendInfo(outfile, aicoreConfig, enableFallBack);
    GenImplFile(outfile, opType, aicoreConfig);
    std::vector<ge::AscendString> mc2Grps = opsDef.MC2().GetHcclGroups();
    if (mc2Grps.size() != 0) {
        GenMC2Info(outfile, mc2Grps);
    }
}

void CfgGenerator::GetOutFilePtr(
    std::string& genPath, std::string& socVer, std::ofstream& outfile, const std::string resolvedGenPath,
    std::map<std::string, std::string>& cfgFileStreams) const
{
    std::string cfgFileName = genPath + "/aic-" + socVer + "-ops-info.ini";
    std::string realCfgFile = std::string(resolvedGenPath) + "/aic-" + socVer + "-ops-info.ini";
    auto iter = cfgFileStreams.find(cfgFileName);
    if (iter == cfgFileStreams.end()) {
        cfgFileStreams.emplace(cfgFileName, cfgFileName);
        outfile.open(realCfgFile, std::ofstream::out | std::ofstream::trunc);
    } else {
        outfile.open(realCfgFile, std::ofstream::out | std::ofstream::app);
    }
}

void CfgGenerator::GenAllOpCfgWithoutComputeUint(
    const std::vector<std::string>& ops, std::string& genPath, const std::string resolvedGenPath,
    std::map<std::string, std::string>& cfgFileStreams) const
{
    for (auto op : ops) {
        OpDef opsDef = OpDefFactory::OpDefCreate(op.c_str());
        bool enableFallBack = opsDef.IsEnableFallBack();
        for (auto& aicoreItem : opsDef.AICore().GetAICoreConfigs()) {
            std::string socVer = NormalizeSupportedSoc(aicoreItem.first.GetString());
            OpAICoreConfig aicoreConfig = aicoreItem.second;
            std::ofstream outfile;
            GetOutFilePtr(genPath, socVer, outfile, resolvedGenPath, cfgFileStreams);
            ParseSingleComputeUnitOfOp(opsDef, op, aicoreConfig, enableFallBack, outfile);
            outfile.close();
        }
    }
}

opbuild::Status CfgGenerator::GenerateCode(void)
{
    std::string genPath;
    ASCENDLOGI("Cfg GenerateCode called!");
    Generator::GetGenPath(genPath);
    char resolvedGenPath[PATH_MAX] = {0};
    if (realpath(genPath.c_str(), resolvedGenPath) == nullptr) {
        ASCENDLOGE("Generate Path %s is invalid!", genPath.c_str());
        std::cerr << "Path: " << genPath << " is not valid!" << std::endl;
        return opbuild::OPBUILD_FAILED;
    }
    std::vector<std::string> ops = this->GetAllOp();
    std::map<std::string, std::string> cfgFileStreams;

    auto computeUnitCfg = opbuild::Params::GetInstance().Optional("compute_unit");
    ASCENDLOGI("Compute unit cfg is %s ", computeUnitCfg.c_str());
    if (computeUnitCfg.size() == 0) {
        GenAllOpCfgWithoutComputeUint(ops, genPath, std::string(resolvedGenPath), cfgFileStreams);
        ASCENDLOGI("Cfg GenerateCode end!");
        return opbuild::OPBUILD_SUCCESS;
    }

    std::vector<std::string> computeUnits;
    Split(computeUnitCfg, ';', computeUnits);

    for (auto op : ops) {
        OpDef opsDef = OpDefFactory::OpDefCreate(op.c_str());
        bool enableFallBack = opsDef.IsEnableFallBack();
        auto allAICoreConfig = opsDef.AICore().GetAICoreConfigs();
        for (uint32_t i = 0; i < computeUnits.size(); ++i) {
            auto socVer = computeUnits[i];
            if (VALID_SOC_SET.find(socVer) == VALID_SOC_SET.end()) {
                ASCENDLOGW("Invlid soc version %s\n of ASCEND_COMPUTE_UNIT", socVer.c_str());
                continue;
            }
            auto cfgIter = allAICoreConfig.end();
            for (auto iter = allAICoreConfig.begin(); iter != allAICoreConfig.end(); ++iter) {
                if (NormalizeSupportedSoc(iter->first.GetString()) == socVer) {
                    cfgIter = iter;
                    break;
                }
            }
            OpAICoreConfig aicoreConfig;
            if (cfgIter == allAICoreConfig.end()) {
                aicoreConfig = OpAICoreConfig(socVer.c_str());
            } else {
                aicoreConfig = cfgIter->second;
            }
            std::ofstream outfile;
            GetOutFilePtr(genPath, socVer, outfile, std::string(resolvedGenPath), cfgFileStreams);
            ParseSingleComputeUnitOfOp(opsDef, op, aicoreConfig, enableFallBack, outfile);
            outfile.close();
        }
    }
    ASCENDLOGI("Cfg Generator of compute unit config %s end", computeUnitCfg.c_str());
    return opbuild::OPBUILD_SUCCESS;
}

CfgGenerator::CfgGenerator(std::vector<std::string>& ops) : Generator(ops) { ASCENDLOGI("Stub Generator construct!"); }

static opbuild::Status CfgGeneratorBuilder(std::vector<std::string>& ops)
{
    CfgGenerator g(ops);
    return g.GenerateCode();
}

static void AddCfgGenerator(void) __attribute__((constructor));
void AddCfgGenerator(void) { GeneratorFactory::AddBuilder("cfg", CfgGeneratorBuilder); }
} // namespace ops
