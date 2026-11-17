# SetFixpipeNz2dnFlag

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR&950DT系列产品：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas训练系列产品：不支持
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_store_aux_config/SetFixpipeNz2dnFlag_res.md#id1 -->

## 功能说明

头文件路径为：`"basic_api/kernel_operator_fixpipe_intf.h"`。

DataCopy数据搬运（[L0C Buffer->UB](../cube_compute_store/DataCopy_L0CToUB.md)）过程中进行随路格式转换（Nz格式转换为DN格式）时，通过调用该接口设置格式转换的相关配置。

## 函数原型

```cpp
__aicore__ inline void SetFixpipeNz2dnFlag(uint16_t dnNum, uint16_t srcNzMatrixStride, uint32_t dstDnMatrixStride, uint16_t srcNzC0Stride);
```

## 参数说明

**表1** 参数说明

| 参数名称 | 输入/输出 | 含义 |
| ---------- | ---------- | ---------- |
| dnNum | 输入 | 传输DN矩阵的数目，取值范围为dnNum∈[0, 65535]。注：dnNum=0表示不执行搬运，该接口将被视为NOP（空操作）。 |
| srcNzMatrixStride | 输入 | 不同源Nz矩阵的偏移（头与头），取值范围srcNzMatrixStride∈[0, 65535]，单位C0_SIZE。当dnNum配置为1时，srcNzMatrixStride配置为0即可，不生效。 |
| dstDnMatrixStride | 输入 | 目的相邻ND矩阵起始地址之间的偏移，取值范围dstDnMatrixStride∈[0, 2^32 -1]，单位为element。当dnNum配置为1时，dstDnMatrixStride配置为0即可，不生效。 |
| srcNzC0Stride | 输入 | 源矩阵Nz分形中相邻行的地址偏移（头与头），取值范围srcNzC0Stride∈[0, 65535]，单位C0_SIZE。当启用Nz2DN时，srcNzC0Stride不能为0。 |

## 返回值说明

无

## 约束说明

- 调用DataCopy接口将数据从L0C Buffer搬运到UB，并且需要进行随路Nz格式转换为DN格式之前，必须先调用SetFixpipeNz2dnFlag设置格式转换配置。

## 调用示例

```cpp
uint16_t dnNum = 1;
uint16_t srcNzMatrixStride = 0;
uint32_t dstDnMatrixStride = 0;
uint16_t srcNzC0Stride = 1;
AscendC::DataCopyCO12DstParams params;
params.mSize = 16;
params.nSize = 32;
params.srcStride = 16;
params.dstStride = 16;
params.nz2dnEn = true;
AscendC::SetFixpipeNz2dnFlag(dnNum, srcNzMatrixStride, dstDnMatrixStride, srcNzC0Stride);
AscendC::DataCopy(dst, src, params);
```
