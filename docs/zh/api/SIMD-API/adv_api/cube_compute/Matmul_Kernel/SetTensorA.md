# SetTensorA

## 产品支持情况

<!-- npu="950" id3 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas 训练系列产品：不支持
<!-- end id8 -->
<!-- npu="x90" id1 -->
- Kirin X90：支持
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin 9030：支持
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorA_res.md#id1 -->

## 功能说明

设置矩阵乘的左矩阵A。

## 函数原型

```
__aicore__ inline void SetTensorA(const GlobalTensor<SrcAT>& gm, bool isTransposeA = false)
```

```
__aicore__ inline void SetTensorA(const LocalTensor<SrcAT>& leftMatrix, bool isTransposeA = false)
```

```
__aicore__ inline void SetTensorA(SrcAT aScalar)
```

<!-- npu="310p" id19 -->
Atlas 推理系列产品AI Core不支持SetTensorA\(SrcAT aScalar\)接口原型。
<!-- end id19 -->

<!-- npu="310b" id20 -->
Atlas 200I/500 A2 推理产品，不支持SetTensorA\(SrcAT aScalar\)接口原型。
<!-- end id20 -->

## 参数说明

**表1**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| gm | 输入 | A矩阵。类型为[GlobalTensor](../../../basic_api/data_structures/GlobalTensor/GlobalTensor.md)。SrcAT参数表示A矩阵的数据类型。不同型号支持的数据类型请参考[gm和leftMatrix支持的数据类型](#li12616155731720)。|
| leftMatrix | 输入 | A矩阵。类型为[LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为TSCM/VECOUT。SrcAT参数表示A矩阵的数据类型。不同型号支持的数据类型请参考[gm和leftMatrix支持的数据类型](#li12616155731720)<br><br>若设置TSCM首地址，默认矩阵可全载，已经位于TSCM，Iterate接口无需再进行GM->L1 Buffer（A1/B1）搬运。 |
| aScalar | 输入 | A矩阵中设置的值。支持传入标量数据，标量数据会被扩展为一个形状为[1, K]的tensor参与矩阵乘计算，tensor的数值均为该标量值。例如，开发者可以通过将aScalar设置为1来实现矩阵B在K方向的reduce sum操作。SrcAT参数表示A矩阵的数据类型。不同型号支持的数据类型请参考[aScalar支持的数据类型](#li12616155731723)。 |
| isTransposeA | 输入 | A矩阵是否需要转置。<br><br>注意：<br>若A矩阵MatmulType的ISTRANS参数设置为true，该参数可以为true也可以为false，即运行时可以转置和非转置交替使用；<br>若A矩阵MatmulType的ISTRANS参数设置为false，该参数只能设置为false，若强行设置为true，精度会有异常；<br>对于非half、非bfloat16_t输入类型的场景，为了确保Tiling侧与核函数（Kernel）侧L1 Buffer空间计算大小保持一致及结果精度正确，该参数取值必须与核函数（Kernel）侧定义A矩阵MatmulType的[ISTRANS](Matmul_usage.md)参数以及Tiling侧SetAType()接口的[isTrans](../Matmul_Tiling/SetAType.md)参数保持一致，即上述三个参数必须同时设置为true或同时设置为false。<!-- npu="310p" id21 --><br><br>Atlas 推理系列产品AI Core，A矩阵为int8_t数据类型时不支持转置，即不支持该参数设置为true。<!-- end id21 --><!-- npu="910b" id22 --><br><br>Atlas A2 训练系列产品/Atlas A2 推理系列产品，A矩阵为int4b_t数据类型时不支持转置，即不支持该参数设置为true。<!-- end id22 --><!-- npu="A3" id23 --><br><br>Atlas A3 训练系列产品/Atlas A3 推理系列产品，A矩阵为int4b_t数据类型时不支持转置，即不支持该参数设置为true。<!-- end id23 --> |

## 返回值说明

无

## 约束说明

-   传入的TensorA地址空间大小需要保证不小于singleM \* singleK。
<!-- npu="950,A3,910b,310p,310b,x90,9030" id24 -->
-   gm和leftMatrix支持的数据类型<a id="li12616155731720"></a>

    <!-- npu="950" id9 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：half、bfloat16_t、int8_t、float、fp8_e4m3fn_t、fp8_e5m2_t、hifloat8_t。
    <!-- end id9 -->

    <!-- npu="A3" id10 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：int4b_t、half、bfloat16_t、int8_t、float。
    <!-- end id10 -->

    <!-- npu="910b" id11 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：int4b_t、half、bfloat16_t、int8_t、float。
    <!-- end id11 -->

    <!-- npu="310p" id12 -->
    Atlas 推理系列产品AI Core，支持的数据类型为：half、int8_t、float。
    <!-- end id12 -->

    <!-- npu="310b" id13 -->
    Atlas 200I/500 A2 推理产品，支持的数据类型为：half、bfloat16_t、int8_t、float。
    <!-- end id13 -->

    <!-- npu="x90" id3 -->
    Kirin X90，支持的数据类型为：half、int8_t。
    <!-- end id3 -->

    <!-- npu="9030" id4 -->
    Kirin 9030，支持的数据类型为：half。
    <!-- end id4 -->

-   aScalar支持的数据类型<a id="li12616155731723"></a>

    <!-- npu="950" id14 -->
    Ascend 950PR/Ascend 950DT，支持的数据类型为：half、float。
    <!-- end id14 -->

    <!-- npu="A3" id15 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的数据类型为：half、float。
    <!-- end id15 -->

    <!-- npu="910b" id16 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的数据类型为：half、float。
    <!-- end id16 -->

    <!-- npu="310p" id17 -->
    Atlas 推理系列产品AI Core，不支持该参数。
    <!-- end id17 -->

    <!-- npu="310b" id18 -->
    Atlas 200I/500 A2 推理产品，不支持该参数。
    <!-- end id18 -->

    <!-- npu="x90" id5 -->
    Kirin X90，支持的数据类型为：half。
    <!-- end id5 -->

    <!-- npu="9030" id6 -->
    Kirin 9030，支持的数据类型为：half。
    <!-- end id6 -->

<!-- end id24 -->

<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetTensorA_res.md#id2 -->

## 调用示例

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);
// 示例一：左矩阵在Global Memory
mm.SetTensorA(gm_a, isTransposeA);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    mm.SetBias(gmBias);
}
mm.IterateAll(gm_c);
mm.End();
// 示例二：左矩阵在Local Memory
mm.SetTensorA(local_a, isTransposeA);
// 示例三：设置标量数据
mm.SetTensorA(scalar_a, isTransposeA);
```
