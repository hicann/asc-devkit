# asc_set_gm2l1_loop1_stride

## 产品支持情况

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

## 功能说明

头文件路径为：`"c_api/cube_datamove/cube_datamove.h"`。

将数据从Global Memory（GM）搬运到L1 Buffer时，设置[asc_copy_gm2l1_align](asc_copy_gm2l1_align.md)循环填充模式内层循环中相邻迭代数据块的源和目的L1间隔。

外层循环步长和两层循环次数需分别通过[asc_set_gm2l1_loop2_stride](asc_set_gm2l1_loop2_stride.md)和[asc_set_gm2l1_loop_size](asc_set_gm2l1_loop_size.md)配置。

本接口仅在AIC上生效。

## 函数原型

```cpp
__aicore__ inline void asc_set_gm2l1_loop1_stride(uint64_t loop1_src_stride, uint64_t loop1_dst_stride)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| loop1_src_stride | 输入 | 内层循环中相邻迭代源操作数的数据块间隔，单位为字节，取值范围为[0, $2^{40}-1$]。 |
| loop1_dst_stride | 输入 | 内层循环中相邻迭代目的L1数据块间隔，单位为字节，取值范围为[0, $2^{21}-1$]，且必须32字节对齐。 |

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口在非AIC上调用直接返回。
- 配置仅由[asc_copy_gm2l1_align](asc_copy_gm2l1_align.md)循环填充模式取用；循环填充模式不支持左右填充。
- `loop1_dst_stride`必须32字节对齐，即使`loop1_size`为`1`也生效。

## 调用示例

完整的2×2 GM到L1循环搬运、L1到UB回读和Host侧逐字节校验示例请参见[asc_set_gm2l1_loop_size调用示例](asc_set_gm2l1_loop_size.md#调用示例)。

以下代码配置内层循环中相邻数据块的源间隔为64B、目的L1间隔为32B：

```cpp
// loop1执行2次，loop2执行2次。
asc_set_gm2l1_loop_size(2, 2);
// 内层循环：相邻迭代在GM相隔64B，在L1相隔32B。
asc_set_gm2l1_loop1_stride(64, 32);
asc_set_gm2l1_loop2_stride(128, 64);
asc_copy_gm2l1_align(dst, src, 1, 32, 0, 0, false, 4, 0, 32);
// 搬运结束后复位循环次数。
asc_set_gm2l1_loop_size(1, 1);
```
