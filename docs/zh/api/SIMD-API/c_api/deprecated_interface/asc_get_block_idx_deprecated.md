# asc_get_block_idx（废弃）

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
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

**该接口已废弃，请使用[内置变量block_idx](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#内置变量)结合[asc_get_sub_block_num](../utils/sys_var/asc_get_sub_block_num.md)和[asc_get_sub_block_id](../utils/sys_var/asc_get_sub_block_id.md)接口替代。**

获取当前运行核（逻辑核）的索引。替代时需按逻辑位置计算：

```cpp
int64_t blockIdx = block_idx * asc_get_sub_block_num() + asc_get_sub_block_id();  // 替代 asc_get_block_idx()
__gm__ float* xGm = x + blockIdx * blockLength;
```

### 迁移说明

- 在非Mix场景下，可直接使用内置变量`block_idx`，其值等于`asc_get_block_idx()`的返回值。
- 在Mix场景（使用`__mix__`函数执行空间限定符）下，内置变量`block_idx`仅标识当前组合在整个grid中的位置，逻辑核索引需结合子块信息计算：`logic_idx = block_idx * sub_block_num + sub_block_id`，即`block_idx * asc_get_sub_block_num() + asc_get_sub_block_id()`；该表达式的值与原接口返回值一致。
- 使用前请参考[内置变量说明](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#内置变量)确认适用场景。

## 函数原型

```cpp
__aicore__ inline int64_t asc_get_block_idx()
```

## 参数说明

无

## 返回值说明

当前运行核的索引。

## 流水类型

`PIPE_S`

## 约束说明

无

## 调用示例

```cpp
int64_t blockIdx = block_idx * asc_get_sub_block_num() + asc_get_sub_block_id();  // 获取当前运行核的索引，替代 asc_get_block_idx()
```
