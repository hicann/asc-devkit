# asc_get_block_num（废弃）

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

头文件路径为：`"c_api/utils/sys_var.h"`。

**该接口已废弃，请使用[内置变量block_num](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#内置变量)替代。**

获取当前任务配置的核数，用于代码内部的多核逻辑控制等。建议在代码中直接使用内置变量`block_num`：

```cpp
uint32_t blockLength = C_API_TOTAL_LENGTH / block_num;  // 替代 asc_get_block_num()
```

### 迁移说明

内置变量`block_num`的值为当前任务配置的核数，与原接口返回值一致，替代时直接使用即可。使用前请参考[内置变量说明](../../../../guide/programming_guide/language_extension/simd_builtin_keywords.md#内置变量)确认适用场景。

## 函数原型

```cpp
__aicore__ inline int64_t asc_get_block_num()
```

## 参数说明

无

## 返回值说明

当前任务配置的核数。

## 流水类型

`PIPE_S`

## 约束说明

无

## 调用示例

```cpp
uint32_t blockLength = C_API_TOTAL_LENGTH / block_num;  // 获取当前任务配置的核数，替代 asc_get_block_num()
```