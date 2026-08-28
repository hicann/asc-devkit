/**
 * Copyright (c) 2025 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TILING_KEY_ADD_CUSTOM_TEMPLATE_H
#define TILING_KEY_ADD_CUSTOM_TEMPLATE_H
#include "ascendc/host_api/tiling/template_argument.h"

/**
 * ASCENDC_TPL_ARGS_DECL(args0, ...): 算子的模板参数定义。
 *     args0 表示算子唯一标识，建议与 opType 保持一致。
 *     后续参数为若干个 DATATYPE、FORMAT、UINT、BOOL 的模板参数定义。
 *
 * ASCENDC_TPL_DATATYPE_DECL(args0, ...): DATATYPE 类型的模板参数定义。
 *     args0: 表示参数名。
 *     args1-argsn: 存在两种情况：
 *         1. 后续若干个参数为穷举的 DATATYPE 选项【用于 SEL】。
 *         2. 对应输入参数的索引值（使用 ASCENDC_TPL_INPUT(x) 指定）或对应输出参数的索引值
 *            （使用 ASCENDC_TPL_OUTPUT(x) 指定）。存在多个时，仅第一个生效；此参数会在 KERNEL
 *            编译过程中基于 DATATYPE 自动选择对应的模板组合进行编译，减少编译产物体积和编译时间。
 *
 * ASCENDC_TPL_FORMAT_DECL(args0, ...): FORMAT 类型的模板参数定义。
 *     args0 表示参数名，后续若干个参数为穷举的 FORMAT 枚举值。
 *
 * ASCENDC_TPL_UINT_DECL(args0, args1, args2, ...): UINT 类型的模板参数定义。
 *     args0: 参数名。
 *     args1: 比特位，表示该 UINT 定义范围内数据的个数能用多少个比特位表示。
 *     args2: 后续 ... 参数表示类型，支持 ASCENDC_TPL_UI_RANGE、ASCENDC_TPL_UI_LIST、
 *            ASCENDC_TPL_UI_MIX。
 *         ASCENDC_TPL_UI_RANGE: 范围模式。后续紧跟的第一个值表示范围个数，后续每两个数值为
 *             一组范围的起、终位置；注意定义的范围个数要和后续的组数保持一致。
 *         ASCENDC_TPL_UI_LIST: 穷举模式。后续将穷举出所有的值；注意穷举的位置决定后续
 *             tilingkey 编码值，数值大小不会影响编码值。
 *         ASCENDC_TPL_UI_MIX: 混合模式。前 n 个数值为范围模式的参数定义，后 m 个数值为
 *             穷举模式的参数定义。
 *
 *     实例说明1 ASCENDC_TPL_UI_RANGE, 2, 0, 2, 3, 5:
 *         表示 2 组范围参数，范围为 {0, 2}, {3, 5}；合法值为 {0, 1, 2, 3, 4, 5}。
 *     实例说明2 ASCENDC_TPL_UI_LIST, 10, 12, 13, 9, 8:
 *         表示 1 组穷举参数，合法值为 {10, 12, 13, 9, 8}。
 *     实例说明3 ASCENDC_TPL_UI_MIX, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8:
 *         表示范围 {0, 2}, {3, 5} 和穷举值 [10, 12, 13, 9, 8] 的混合；
 *         合法值为 {0, 1, 2, 3, 4, 5, 10, 12, 13, 9, 8}。
 *
 *     【使用注意事项】:
 *         1. tilingkey 编码是以 UINT 定义的位置进行编码。更新 ASCENDC_TPL_UINT_DECL 参数定义时，
 *            一定要注意不影响原有的参数位置。
 *         2. ASCENDC_TPL_UINT_DECL 中的比特位一经定义后续不可更改，否则会影响其他参数的编码位置
 *            偏移，引起 tilingkey 兼容问题。
 *
 * ASCENDC_TPL_BOOL_DECL(args0, ...): BOOL 类型的模板参数定义。
 *     args0 表示参数名，后续可自定义 true、false 中的一个或两个值。
 */
ASCENDC_TPL_ARGS_DECL(
    AddCustomTemplate,                                          // 唯一标识，建议与 OpType 保持一致
    ASCENDC_TPL_DATATYPE_DECL(D_T_X, C_DT_FLOAT16, C_DT_FLOAT), // 内部定义编码位宽为 8 比特
    ASCENDC_TPL_DATATYPE_DECL(D_T_Y, C_DT_FLOAT16, C_DT_FLOAT),
    ASCENDC_TPL_DATATYPE_DECL(D_T_Z, C_DT_FLOAT16, C_DT_FLOAT),
    ASCENDC_TPL_UINT_DECL(
        // 用户定义编码位宽为 ASCENDC_TPL_8_BW 比特，需大于参数个数
        TILE_NUM, ASCENDC_TPL_8_BW, ASCENDC_TPL_UI_MIX, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8),
    ASCENDC_TPL_BOOL_DECL(IS_SPLIT, 0, 1), // 内部编码位宽为 1 比特
);

/**
 * ASCENDC_TPL_SEL(...): 算子的模板参数整体组合，可设置多个模板参数组合。
 *     ASCENDC_TPL_ARGS_DSEL(...): 算子的模板参数组合，可设置多个参数组合。
 *         ASCENDC_TPL_DATATYPE_SEL(args0, ...): DATATYPE 类型的模板参数组合。
 *             args0 表示参数名，args1-argsn 为对应 ASCENDC_TPL_DATATYPE_DECL 定义的参数选项范围子集。
 *         ASCENDC_TPL_FORMAT_SEL(args0, ...): FORMAT 类型的模板参数组合。
 *             args0 表示参数名，后续参数为对应 ASCENDC_TPL_FORMAT_DECL 定义的参数范围子集。
 *         ASCENDC_TPL_UINT_SEL(args0, args1, args2, ...): UINT 类型的模板参数定义。
 *             args0 表示参数名；args1 是参数表示类型，支持 ASCENDC_TPL_UI_RANGE、
 *             ASCENDC_TPL_UI_LIST、ASCENDC_TPL_UI_MIX；后续数值参考 ASCENDC_TPL_UINT_DECL 的规则。
 *         ASCENDC_TPL_BOOL_SEL(args0, ...): BOOL 类型的模板参数定义。
 *             args0 表示参数名，后续参数为对应 ASCENDC_TPL_BOOL_DECL 定义的参数范围子集。
 */
ASCENDC_TPL_SEL(
    ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT16), ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT16),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT16), ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1), ),
    ASCENDC_TPL_ARGS_SEL(
        ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT), ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT),
        ASCENDC_TPL_DATATYPE_SEL(D_T_Z, C_DT_FLOAT), ASCENDC_TPL_UINT_SEL(TILE_NUM, ASCENDC_TPL_UI_LIST, 1, 8),
        ASCENDC_TPL_BOOL_SEL(IS_SPLIT, 0, 1), ), );
#endif // TILING_KEY_ADD_CUSTOM_TEMPLATE_H
