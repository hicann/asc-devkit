# UnPad

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
- Atlas 推理系列产品Vector Core：不支持
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas 训练系列产品：不支持
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/tensor_transform/UnPad_res.md#id1 -->

## 功能说明

对height \* width的二维Tensor在width方向上进行unpad，如果Tensor的width非32B对齐，则不支持调用本接口unpad。本接口具体功能场景如下：Tensor的width已32B对齐，以half为例，如16\*16，进行UnPad，变成16\*15。

## 函数原型

由于该接口的内部实现中涉及复杂的计算，需要额外的临时空间来存储计算过程中的中间变量。临时空间大小BufferSize的获取方法：通过[UnPad Tiling](UnPad-Tiling.md)中提供的**GetUnPadMaxMinTmpSize**接口获取所需最大和最小临时空间大小，最小空间可以保证功能正确，最大空间用于提升性能。

临时空间支持**接口框架申请**和开发者**通过sharedTmpBuffer入参传入**两种方式，因此UnPad接口的函数原型有两种：

-   通过sharedTmpBuffer入参传入临时空间

    ```
    template <typename T>
    __aicore__ inline void UnPad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, UnPadParams& unPadParams, LocalTensor<uint8_t>& sharedTmpBuffer, UnPadTiling& tiling)
    ```

    该方式下开发者需自行申请并管理临时内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。

-   接口框架申请临时空间

    ```
    template <typename T>
    __aicore__ inline void UnPad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, UnPadParams& unPadParams, UnPadTiling& tiling)
    ```

    该方式下开发者无需申请，但是需要预留临时空间的大小。

## 参数说明

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数的数据类型。支持的数据类型为：int16_t、uint16_t、half、int32_t、uint32_t、float。 |

**表2**  接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| --- | --- | --- |
| dstTensor | 输出 | 目的操作数，shape为二维，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| srcTensor | 输入 | 源操作数，shape为二维，LocalTensor数据结构的定义请参考[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| UnPadParams | 输入 | UnPad详细参数，UnPadParams数据类型，UnPadParams结构体的定义如下方代码所示，具体结构体参数说明如下：<br>leftPad，左边unpad的数据量。leftPad要求小于32B。单位：列。当前暂不生效。<br>rightPad，右边unpad的数据量。rightPad要求小于32B，大于0。单位：列。当前只支持在右边进行unpad。 |
| sharedTmpBuffer | 输入 | 共享缓冲区，用于存放API内部计算产生的临时数据。该方式开发者可以自行管理sharedTmpBuffer内存空间，并在接口调用完成后，复用该部分内存，内存不会反复申请释放，灵活性较高，内存利用率也较高。共享缓冲区大小的获取方式请参考[UnPad Tiling](UnPad-Tiling.md)。<br><br>类型为[LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。 |
| tiling | 输入 | 计算所需tiling信息，Tiling信息的获取请参考[UnPad Tiling](UnPad-Tiling.md)。 |

```
struct UnPadParams {
    uint16_t leftPad = 0;
    uint16_t rightPad = 0;
};
```

## 返回值说明

无

## 约束说明

-   操作数地址对齐要求请参见[通用地址对齐约束](../../general_description_and_constraints.md#section796754519912)。

## 调用示例

本样例：Tensor的width已32B对齐，以half为例，如16\*16，进行UnPad，变成16\*15。输入数据类型均为half。

```
// dstLocal：输出Tensor
// srcLocal：输入Tensor
// unPadParams：控制去除填充的参数
AscendC::UnPadParams unPadParams{0, 1}; // 左边去掉0列，右边去掉1列，当前暂不支持左边unpad
AscendC::UnPad(dstLocal, srcLocal, unPadParams, tilingData.unpadTilingData);
```
