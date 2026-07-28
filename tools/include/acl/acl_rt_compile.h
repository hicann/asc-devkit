/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#ifndef ASCENDC_ACL_RT_COMPILE_H
#define ASCENDC_ACL_RT_COMPILE_H
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int aclError;
typedef void* aclrtcProg;

/**
 * @brief Creates an instance of aclrtcProg with the given input parameters.
 * @param[out] prog Runtime Compilation program.
 * @param[in] src Program source.
 * @param[in] name Program name. "default_program" is used when name is "".
 * @param[in] numHeaders Currently must be 0. Header support not implemented.
 * @param[in] headers Currently must be NULL.
 * @param[in] includeNames Currently must be NULL.
 * @return aclError: ACL_SUCCESS or ACL_ERROR_RTC_XXX
 */
aclError aclrtcCreateProg(
    aclrtcProg* prog, const char* src, const char* name, int numHeaders, const char** headers,
    const char** includeNames);

/**
 * @brief Compiles the given program.
 * @param[in] prog Runtime Compilation program.
 * @param[in] numOptions Number of compiler options.
 * @param[in] options Array of option strings.
 * @return aclError: ACL_SUCCESS or ACL_ERROR_RTC_XXX
 */
aclError aclrtcCompileProg(aclrtcProg prog, int numOptions, const char** options);

/**
 * @brief Notes the given name expression denoting the address of a global function or device variable.
 * @param[in] prog Runtime Compilation program.
 * @param[in] nameExpression Constant expression denoting the address of a global function or
 *                           device variable. The identical name expression string must be provided
 *                           on a subsequent call to aclrtcGetLoweredName to extract the lowered name.
 * @return aclError: ACL_SUCCESS or ACL_ERROR_RTC_XXX
 */
aclError aclrtcAddNameExpr(aclrtcProg prog, const char* const nameExpression);

/**
 * @brief Extracts the lowered (mangled) name for a global function or device variable.
 * @param[in] prog Runtime Compilation program.
 * @param[in] nameExpression Constant expression denoting the address of a global function or
 *                           device variable. Must be identical to the name expression previously
 *                           provided to aclrtcAddNameExpr.
 * @param[out] loweredName Lowered (mangled) name corresponding to the provided name expression.
 *                         The memory containing the name is released when the program is destroyed
 *                         by aclrtcDestroyProg.
 * @return aclError: ACL_SUCCESS or ACL_ERROR_RTC_XXX
 */
aclError aclrtcGetLoweredName(aclrtcProg prog, const char* nameExpression, const char** loweredName);

/**
 * @brief Destroys the given program.
 * @param[in,out] prog Runtime Compilation program.
 * @return aclError: ACL_SUCCESS or ACL_ERROR_RTC_XXX
 */
aclError aclrtcDestroyProg(aclrtcProg* prog);

/**
 * @brief Retrieves the compiled device ELF binary.
 * @param[in] prog Runtime Compilation program.
 * @param[out] binData Compiled result.
 * @return aclError: ACL_SUCCESS or ACL_ERROR_RTC_XXX
 */
aclError aclrtcGetBinData(aclrtcProg prog, char* binData);

/**
 * @brief Retrieves the size of the compiled device ELF binary.
 * @param[in] prog Runtime Compilation program.
 * @param[out] deviceELFSizeRet Size of the ELF binary.
 * @return aclError: ACL_SUCCESS or ACL_ERROR_RTC_XXX
 */
aclError aclrtcGetBinDataSize(aclrtcProg prog, size_t* binDataSizeRet);

/**
 * @brief Retrieves the size of the compilation log.
 * @param[in] prog Runtime Compilation program.
 * @param[out] logSizeRet Size of the log string.
 * @return aclError: ACL_SUCCESS or ACL_ERROR_RTC_XXX
 */
aclError aclrtcGetCompileLogSize(aclrtcProg prog, size_t* logSizeRet);

/**
 * @brief Retrieves the compilation log.
 * @param[in] prog Runtime Compilation program.
 * @param[out] log Compilation log.
 * @return aclError: ACL_SUCCESS or ACL_ERROR_RTC_XXX
 */
aclError aclrtcGetCompileLog(aclrtcProg prog, char* log);

#ifdef __cplusplus
}
#endif

#endif // ASCENDC_ACL_RT_COMPILE_H
