# GetDataBlockSizeInBytes

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
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetDataBlockSizeInBytes_res.md#id1 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_sys_var_intf.h"`。

获取当前芯片版本一个DataBlock的大小，单位为Byte。

开发者根据DataBlock的大小来计算API指令中待传入的[repeatTime、dstBlkStride、srcblkStride、dstRepStride、srcRepStride等参数值](../../memory_vector_compute/SIMD_compute/high_dim_split.md)。

## 函数原型

```cpp
__aicore__ inline constexpr int16_t GetDataBlockSizeInBytes()
```

## 参数说明

无

## 返回值说明

当前芯片版本一个DataBlock的大小，单位为Byte。

## 约束说明

无

## 调用示例

如下样例通过`GetDataBlockSizeInBytes`获取DataBlock的大小并计算`repeatTime`的值：

```cpp
int16_t dataBlockSize = AscendC::GetDataBlockSizeInBytes();
// 每个repeat有8个DataBlock,可计算8 * dataBlockSize / sizeof(half)个数，mask配置为迭代内所有元素均参与计算
uint64_t mask = 8 * dataBlockSize / sizeof(half);
// 共计算512个数，除以每个repeat参与计算的元素个数，得到repeatTime
uint8_t repeatTime = 512 / mask;
// dstBlkStride, src0BlkStride, src1BlkStride = 1,单次迭代内数据连续读取和写入
// dstRepStride, src0RepStride, src1RepStride = 8,相邻迭代间数据连续读取和写入
AscendC::Add(dstLocal, src0Local, src1Local, mask, repeatTime, { 1, 1, 1, 8, 8, 8 });
```
