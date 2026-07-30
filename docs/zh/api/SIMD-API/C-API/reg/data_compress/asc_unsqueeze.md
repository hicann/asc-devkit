# asc_unsqueeze

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

根据mask进行解压缩，将生成的数据输出到dst。解压缩方式：dst中的第0个元素置为0，dst中的第i个元素等于mask中从第0个到第(i-1)个元素中1的数量。mask最高位被忽略不参与统计。算法逻辑表示如下：
  ```cpp
  dst[0] = 0;
  // dstLen为dst中包含的元素个数
  for (int i = 1; i < dstLen; i++) {
    dst[i] = mask[i - 1] ? (dst[i - 1] + 1) : dst[i - 1];
  }
  ```

## 函数原型

  ```cpp
  __simd_callee__ inline void asc_unsqueeze(vector_int8_t& dst, vector_bool mask)
  __simd_callee__ inline void asc_unsqueeze(vector_uint8_t& dst, vector_bool mask)
  __simd_callee__ inline void asc_unsqueeze(vector_int16_t& dst, vector_bool mask)
  __simd_callee__ inline void asc_unsqueeze(vector_uint16_t& dst, vector_bool mask)
  __simd_callee__ inline void asc_unsqueeze(vector_int32_t& dst, vector_bool mask)
  __simd_callee__ inline void asc_unsqueeze(vector_uint32_t& dst, vector_bool mask)
  ```

## 参数说明

| 参数名       | 输入/输出 | 描述                |
| --------- | ----- | ----------------- |
| dst       | 输出    | 目的操作数（矢量数据寄存器）。 |
| mask      | 输入    | 源操作数掩码（掩码寄存器），用于提供dstReg的解压缩信息。 |

矢量数据寄存器和掩码寄存器的详细说明请参见[data_type_definition.md](../reg_data_types/data_type_definition.md)。

## 返回值说明

无

## 流水类型

PIPE_V

## 约束说明

无

## 调用示例

```cpp
vector_int8_t dst;
vector_bool mask = asc_create_mask_b8(PAT_ALL);
asc_unsqueeze(dst, mask);
```
