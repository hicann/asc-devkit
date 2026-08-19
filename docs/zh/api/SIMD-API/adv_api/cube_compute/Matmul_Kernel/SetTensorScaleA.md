# SetTensorScaleA

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：不支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：不支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：不支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorScaleA_res.md#id1 -->

## 功能说明

MxMatmul场景，设置矩阵乘中左矩阵的量化系数矩阵scaleA。

## 函数原型

```
__aicore__ inline void SetTensorScaleA(const GlobalTensor<ScaleT>& gm, bool isTransposeScaleA = false);
```

```
__aicore__ inline void SetTensorScaleA(const LocalTensor<ScaleT>& leftMatrix, bool isTransposeScaleA = false);
```

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| gm | 输入 | 量化系数scaleA矩阵。类型为[GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md)。支持的数据类型为：fp8_e8m0_t。 |
| leftMatrix | 输入 | 量化系数scaleA矩阵。类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为TSCM/VECOUT。支持的数据类型为：fp8_e8m0_t。 |
| isTransposeScaleA | 输入 | scaleA矩阵是否需要转置。<br><br>参数支持的取值如下：<br>false：默认值，scaleA矩阵不转置。true：scaleA矩阵转置。<br><br>注意：<br>scaleA矩阵为NZ格式时，该参数只支持取值为false。<br>若scaleA矩阵的MatmulTypeWithScale参数中的SCALE_ISTRANS设置为true，除scaleA为NZ格式场景，该参数支持取值为true、false，即运行时scaleA矩阵可以转置和非转置交替使用。<br>若scaleA矩阵的MatmulTypeWithScale参数中的SCALE_ISTRANS设置为false，该参数只支持取值为false，若强行设置为true，精度会有异常。<br><br>对于有Bias输入的场景，为了确保Tiling侧与核函数（Kernel）侧L1 Buffer空间计算大小保持一致及结果精度正确，该参数取值必须与核函数（Kernel）侧定义A矩阵MatmulTypeWithScale的SCALE_ISTRANS参数以及Tiling侧SetScaleAType()接口的[isScaleTrans](../Matmul_Tiling/SetScaleAType.md#p2934103115919)参数保持一致，即有Bias输入的场景，上述三个参数必须同时设置为true或同时设置为false。 |

## 返回值说明

无

## 约束说明

-   传入的scaleA地址空间大小必须不小于[singleCoreM](../Matmul_Tiling/TCubeTiling_struct.md)\*[singleCoreK](../Matmul_Tiling/TCubeTiling_struct.md)/32。
-   当开启MixDualMaster（双主模式）场景时，即模板参数[enableMixDualMaster](MatmulConfig.md#matmulconfig-params)设置为true，不支持使用该接口。

## 调用示例

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
mm.SetTensorScaleA(gm_scaleA);    // 设置左矩阵的量化系数矩阵scaleA
mm.SetTensorScaleB(gm_scaleB);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
```
