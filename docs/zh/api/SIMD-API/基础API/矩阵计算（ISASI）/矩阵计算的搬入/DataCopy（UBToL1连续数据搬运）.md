# DataCopy（UBToL1连续数据搬运）

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
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：不支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：不支持
<!-- end id9 -->

## 功能说明

> [!NOTE]说明
> 本接口为软件仿真实现，是在Matmul高阶API的基础上，利用Matmul高阶API中的workspace GM空间作为数据中转空间，数据先搬入GM，再搬入L1 Buffer。因此，在使用本接口时，需要先使用REGISTER_MATMUL注册高阶API。

头文件路径为：`"basic_api/kernel_operator_data_copy_intf.h"`。

该接口实现将矩阵从Unified Buffer（UB，TPosition为VECIN/VECCALC/VECOUT）搬运至L1 Buffer，搬运方式为连续搬运，数据搬运时格式和内容保持不变。

## 函数原型

```cpp
// 连续搬运
template <typename T>
__aicore__ inline void DataCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| ---------- | ---------- |
| T | 操作数的数据类型。源操作数和目的操作数的数据类型保持一致。 |

**表2** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dst | 输出 | 目的操作数，类型为LocalTensor，存储位置为L1 Buffer，起始地址要求32字节对齐。 |
| src | 输入 | 源操作数，类型为LocalTensor，存储位置为Unified Buffer（TPosition为VECIN/VECCALC/VECOUT），起始地址要求32字节对齐。 |
| count | 输入 | 参与搬运的元素个数。<br>**注：count * sizeof(T)需要32字节对齐，若未对齐，搬运量会向下取整到32字节对齐。** |

## 数据类型

源矩阵和目的矩阵支持的数据类型保持一致。

<!-- npu="950" id10 -->
针对Ascend 950PR/Ascend 950DT，支持数据类型为：bool、int8_t、uint8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、fp8_e8m0_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32、int64_t、uint64_t、double、complex64。
<!-- end id10 -->

<!-- npu="A3" id11 -->
针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、bfloat16_t、float、double。
<!-- end id11 -->

<!-- npu="910b" id12 -->
针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、bfloat16_t、float、double。
<!-- end id12 -->

<!-- npu="310p" id13 -->
针对Atlas 推理系列产品AI Core，支持数据类型为：int8_t、uint8_t、int16_t、uint16_t、int32_t、uint32_t、int64_t、uint64_t、half、float、double。
<!-- end id13 -->

## 返回值说明

无

## 约束说明

- 如果需要执行多个DataCopy指令，且DataCopy的目的地址存在重叠，需要通过调用[PipeBarrier(ISASI)](../../同步控制/核内同步/PipeBarrier(ISASI).md)来插入同步指令，保证多个DataCopy指令的串行化，防止出现异常数据。

- 如果搬运的目的地址L1 Buffer存在重叠，两条搬运指令之间需要调用PipeBarrier\<PIPE_MTE2\>()添加MTE2搬入流水的同步。

<!-- npu="A3,910b" id14 -->
- 针对如下产品型号：

    <!-- npu="A3" id15 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品；
    <!-- end id15 -->

    <!-- npu="910b" id16 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品；
    <!-- end id16 -->

    在跨卡通信算子开发场景，DataCopy类接口支持跨卡数据搬运，仅支持HCCS物理链路，不支持其他通路；开发者开发过程中，需要关注涉及卡间通信的物理通路，可通过npu-smi info -t topo命令查询HCCS物理链路。
<!-- end id14 -->

<!-- npu="950" id17 -->
- 针对Ascend 950PR/Ascend 950DT，在UB->L1 Buffer的数据搬运时，可以通过配置编译选项ENABLE_CV_COMM_VIA_SSBUF来选择两种搬运通路，当ENABLE_CV_COMM_VIA_SSBUF配置为true时，使用SSBuffer进行通信，数据通过UB->L1 Buffer之间的硬件通道进行搬运（推荐），参考样例[硬通道搬运](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1)；当ENABLE_CV_COMM_VIA_SSBUF为false时，数据搬运到L1 Buffer经过GM，该场景下需要借助Matmul高阶API进行注册操作。
<!-- end id17 -->

## 调用示例

示例代码片段如下，完整示例请参考：[DataCopy_UB2L1样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/00_data_movement/data_copy_ub2l1)。

```cpp
// dstLocal、srcLocal为half类型的LocalTensor，分别位于L1 Buffer和UB
// 使用传入count参数的搬运接口，完成连续搬运
AscendC::DataCopy(dstLocal, srcLocal, 512);
```
