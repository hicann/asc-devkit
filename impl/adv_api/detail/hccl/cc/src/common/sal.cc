/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#include <cmath>
#include <climits>
#include <cstdlib>
#include <fcntl.h>
#include <mutex>
#include <syscall.h>
#include <securec.h>
#include <unistd.h>

#include "hccl_common.h"
#include "log.h"
#include "sal.h"

using namespace std;

HcclResult SalStrToDouble(const std::string str, double& val)
{
    try {
        val = std::stod(str);
    } catch (std::invalid_argument&) {
        HCCL_ERROR("[Transform][StrToDouble]stod invalid argument, str[%s] val[%f]", str.c_str(), val);
        return HCCL_E_PARA;
    } catch (std::out_of_range&) {
        HCCL_ERROR("[Transform][StrToDouble]stod out of range, str[%s] val[%f]", str.c_str(), val);
        return HCCL_E_PARA;
    } catch (...) {
        HCCL_ERROR("[Transform][StrToDouble]stod catch error, str[%s] val[%f]", str.c_str(), val);
        return HCCL_E_PARA;
    }
    return HCCL_SUCCESS;
}

// 字串符转换成无符号整型
HcclResult SalStrToULong(const std::string str, int base, u32& val)
{
    try {
        u64 tmp = std::stoull(str, nullptr, base);
        if (tmp > INVALID_UINT) {
            HCCL_ERROR("[Transform][StrToULong]stoul out of range, str[%s] base[%d] val[%llu]", str.c_str(), base, tmp);
            return HCCL_E_PARA;
        } else {
            val = static_cast<u32>(tmp);
        }
    } catch (std::invalid_argument&) {
        HCCL_ERROR("[Transform][StrToULong]stoull invalid argument, str[%s] base[%d] val[%u]", str.c_str(), base, val);
        return HCCL_E_PARA;
    } catch (std::out_of_range&) {
        HCCL_ERROR("[Transform][StrToULong]stoull out of range, str[%s] base[%d] val[%u]", str.c_str(), base, val);
        return HCCL_E_PARA;
    } catch (...) {
        HCCL_ERROR("[Transform][StrToULong]stoull catch error, str[%s] base[%d] val[%u]", str.c_str(), base, val);
        return HCCL_E_PARA;
    }
    return HCCL_SUCCESS;
}

HcclResult IsAllDigit(const char* strNum)
{
    // 参数有效性检查
    CHK_PTR_NULL(strNum);
    u32 index = 0;

    u32 nLength = strnlen(strNum, INT_MAX);
    if (strNum[0] == '-') {
        index = 1;
    }
    for (; index < nLength; index++) {
        if (!isdigit(strNum[index])) {
            HCCL_ERROR(
                "[Check][Isdigit]errNo[0x%016llx] In judge all digit, check isdigit failed."
                "ensure that the number is an integer. strNum[%u] is [%d](Dec)",
                HCCL_ERROR_CODE(HCCL_E_PARA), index, strNum[index]);
            return HCCL_E_PARA;
        }
    }
    return HCCL_SUCCESS;
}
