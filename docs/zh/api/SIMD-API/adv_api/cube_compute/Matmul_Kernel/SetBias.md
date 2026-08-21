# SetBias

## 产品支持情况

<!-- npu="950" id2 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id2 -->
<!-- npu="A3" id3 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id3 -->
<!-- npu="910b" id4 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id4 -->
<!-- npu="310b" id5 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetBias_res.md#id1 -->

## 功能说明

设置矩阵乘的Bias。

## 函数原型

```
__aicore__ inline void SetBias(const GlobalTensor<BiasT>& biasGlobal)
```

```
__aicore__ inline void SetBias(const LocalTensor<BiasT>& inputBias)
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| biasGlobal | 输入 | Bias矩阵。类型为[GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md)。不同型号支持的数据类型请参考[支持的数据类型](#li12616155731720)。<br><br>A矩阵、B矩阵、Bias支持的数据类型组合可参考[Matmul输入输出数据类型的组合说明](Matmul_usage.md#table1996113269499)；在MxMatmul场景，A矩阵、B矩阵、Bias支持的数据类型组合可参考[MatmulTypeWithScale参数说明](../../../../../guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/mx_matmul_scenario.md#zh-cn_topic_0000002270097206_table14759942142014)。 |
| inputBias | 输入 | Bias矩阵。类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为TSCM/VECOUT。不同型号支持的数据类型请参考[支持的数据类型](#li12616155731720)。<br><br>A矩阵、B矩阵、Bias支持的数据类型组合可参考[Matmul输入输出数据类型的组合说明](Matmul_usage.md#table1996113269499)；在MxMatmul场景，A矩阵、B矩阵、Bias支持的数据类型组合可参考[MatmulTypeWithScale参数说明](../../../../../guide/operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/mx_matmul_scenario.md#zh-cn_topic_0000002270097206_table14759942142014)。 |

## 返回值说明

无

## 约束说明

-   在Matmul Tiling计算中，必须配置TCubeTiling结构中的[isBias](../Matmul_Tiling/TCubeTiling_struct.md#p2051215216314)参数为1，即启用Bias后，才能调用本接口设置Bias矩阵。
-   传入的Bias地址空间大小需要保证不小于singleN。
<!-- npu="950" id13 -->
-   对于Ascend 950PR/Ascend 950DT，Bias矩阵的内存逻辑位置为TSCM且数据类型为float或int32\_t时，Bias矩阵的LocalTensor空间必须64字节对齐。
<!-- end id13 -->
<!-- npu="950,A3,910b,310p,310b,x90" id14 -->
-   支持的数据类型<a id="li12616155731720"></a>

    <!-- npu="950" id8 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：half、bfloat16_t、float、int32_t，其中仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。
    <!-- end id8 -->

    <!-- npu="A3" id9 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、float、int32_t，其中仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。
    <!-- end id9 -->

    <!-- npu="910b" id10 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、float、int32_t，其中仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。
    <!-- end id10 -->

    <!-- npu="310p" id11 -->
    Atlas 推理系列产品AI Core，支持的数据类型为：half、float、int32_t，对于包含biasGlobal参数的原型，仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。
    <!-- end id11 -->

    <!-- npu="310b" id12 -->
    Atlas 200I/500 A2 推理产品，支持的数据类型为：half、float、int32_t，其中仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。
    <!-- end id12 -->

    <!-- npu="x90" id15 -->
    Kirin X90，支持的数据类型为：half、int32_t，其中仅在A、B的数据类型为int8_t时，Bias的数据类型可以设置为int32_t。
    <!-- end id15 -->

<!-- end id14 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetBias_res.md#id2 -->

## 调用示例

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);  // 设置Bias
}
mm.IterateAll(gm_c);
mm.End();
```
