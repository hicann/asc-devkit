# DeInterleave<a name="ZH-CN_TOPIC_0000002131264492"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_duplicate_intf.h"`。

给定源操作数src0和src1，将src0和src1中的元素解交织存入结果操作数dst0和dst1中。解交织排列方式如下图所示，其中每个方格代表一个元素。

![](../../../../figures/deinterleave_memory.png)

## 函数原型<a name="section620mcpsimp"></a>

-   两个输入

    ```
    template <typename T>
    __aicore__ inline void DeInterleave(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0, const LocalTensor<T>& src1, const int32_t count)
    ```

-   一个输入

    ```
    template <typename T>
    __aicore__ inline void DeInterleave(const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src, const int32_t srcCount)
    ```

## 参数说明<a name="section176711403104"></a>

**表1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| T | 操作数数据类型。 |

**表2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| dst0/dst1 | 输出 | 目的操作数。类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。LocalTensor的起始地址需要32字节对齐。 |
| src/src0/src1 | 输入 | 源操作数。类型为[LocalTensor](../../数据结构/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。LocalTensor的起始地址需要32字节对齐。源操作数的数据类型需要与目的操作数保持一致。 |
| count | 输入 | 输入/输出数据元素个数，dst0/dst1/src0/src1长度大小为count。count必须为偶数。 |
| srcCount | 输入 | 输入数据元素个数，两个输出的大小都为输入的一半。srcCount必须为偶数。 |

## 数据类型

支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、int64_t、uint64_t。源操作数的数据类型需要与目的操作数保持一致。

## 返回值说明<a name="section14483414194"></a>

无

## 约束说明<a name="section633mcpsimp"></a>

不支持源操作数与目的操作数地址重叠。 

## 调用示例<a name="section176061616102911"></a>

-   两个输入

    ```
    AscendC::DeInterleave(dst0Local, dst1Local, src0Local, src1Local, 512);
    ```

    结果示例如下：

    ```
    输入数据src0Local：[1 2 3 ... 512]
    输入数据src1Local：[513 514 515 ... 1024]
    输出数据dst0Local：[1 3 5 ... 1023]
    输出数据dst1Local：[2 4 6 ... 1024]
    ```

-   一个输入

    ```
    AscendC::DeInterleave(dst0Local, dst1Local, srcLocal, 512);
    ```

    结果示例如下：

    ```
    输入数据srcLocal：[1 2 3 ... 512]
    输出数据dst0Local：[1 3 5 ... 511]
    输出数据dst1Local：[2 4 6 ... 512]
    ```
