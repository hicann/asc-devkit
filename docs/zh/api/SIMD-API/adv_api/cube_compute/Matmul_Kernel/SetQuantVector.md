# SetQuantVector

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/cube_compute/Matmul_Kernel/SetQuantVector_res.md#id1 -->

## 功能说明

本接口提供对输出矩阵采用向量进行量化或反量化的功能，即对于输入shape为\[1, N\]的参数向量，N值为Matmul矩阵计算时M/N/K中的N值，对输出矩阵的每一列都采用该向量中对应列的系数进行量化或反量化。

Matmul反量化场景：在Matmul计算时，左、右矩阵的输入为int8\_t或int4b\_t类型，输出为half类型；或者左、右矩阵的输入为int8\_t类型，输出为int8\_t类型。该场景下，输出C矩阵的数据从L0C Buffer（CO1）搬出到Global Memory时，会执行反量化操作，将最终结果反量化为对应的half或int8\_t类型。

Matmul量化场景：在Matmul计算时，左、右矩阵的输入为half或bfloat16\_t类型，输出为int8\_t类型。该场景下，输出C矩阵的数据从L0C Buffer（CO1）搬出到Global Memory时，会执行量化操作，将最终结果量化为int8\_t类型。

## 函数原型

-   量化参数的存储位置为GM

    ```
    __aicore__ inline void SetQuantVector(const GlobalTensor<uint64_t>& quantTensor)
    ```

-   量化参数的存储位置为L1 Buffer

    ```
    __aicore__ inline void SetQuantVector(const LocalTensor<uint64_t>& quantTensor)
    ```

    <!-- npu="310b" id8 -->
    -   Atlas 200I/500 A2 推理产品暂不支持量化参数的存储位置为L1 Buffer。
    <!-- end id8 -->
    <!-- npu="310p" id9 -->
    -   Atlas 推理系列产品AI Core暂不支持量化参数的存储位置为L1 Buffer。
    <!-- end id9 -->
    <!-- npu="x90" id2 -->
    -   Kirin X90暂不支持量化参数的存储位置为L1 Buffer。
    <!-- end id2 -->

## 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| quantTensor | 输入 | 量化或反量化运算时的参数向量。参数向量的存储位置为GM或L1 Buffer。 |

将float数据类型的量化计算参数scale、offset转换为uint64类型的入参的计算公式如下：

1.  quantTensor为64位格式，初始为0。
2.  scale按bit位取高19位截断，存储于quantTensor的bit位32位处，并将46位修改为1。

    quantTensor = quantTensor ∣ \(scale & 0xFFFFE000\) ∣ \(1 ≪ 46\)

3.  根据offset取值进行后续计算：
    -   若offset不存在，不再进行后续计算。
    -   若offset存在：
        1.  将offset值处理为int，范围为\[-256, 255\]。

            offset = Max\(Min\(INT\(Round\(offset\)\), 255\), −256\)

        2.  再将offset按bit位保留9位并存储于quantTensor的37到45位。

            quantTensor = \(quantTensor & 0x4000FFFFFFFF\) ∣ \(\(offset & 0x1FF\) ≪ 37\)

## 返回值说明

无

## 约束说明

需与[SetDequantType](../Matmul_Tiling/SetDequantType.md)保持一致。

本接口必须在[Iterate](Iterate.md)或者[IterateAll](IterateAll.md)前调用。

## 调用示例

```
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling);

// quantTensor：量化或反量化运算时的Tensor，支持的存储位置为GM和L1
mm.SetQuantVector(quantTensor);

mm.SetTensorA(gm_a);
mm.SetTensorB(gm_b);
if (tiling.isBias) {
    matmulObj.SetBias(biasGlobal);
}
mm.IterateAll(gm_c);
mm.End();
```
