# asc_get_program_counter

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

获取程序计数器的指针，程序计数器用于记录当前程序执行的位置。常用于调试时定位当前执行位置，也可配合[asc_icache_preload](../../cache_ctrl/asc_icache_preload.md)等接口，将当前指令地址作为预取的起始地址。

## 函数原型

```c
__aicore__ inline int64_t asc_get_program_counter()
```

## 参数说明

无

## 返回值说明

返回int64_t类型的程序计数器指针。

## 流水类型

PIPE_S

## 约束说明

本接口返回64位程序计数器值，其中有效地址位为0~47位，高16位为无效位。如下代码片段所示，使用时需通过掩码`0xFFFFFFFFFFFF`（低48位全1）将高位无效位清零，以得到当前指令在GM中的有效48位地址。

```cpp
int64_t prefetch_length = 6;
// 通过asc_get_program_counter获取当前程序计数器（PC）值。
// PC为64位返回值，有效地址位为0~47位，通过掩码0xFFFFFFFFFFFF（低48位全1）将高位无效位清零，
// 得到当前指令在GM中的有效48位地址，作为预加载的起始地址。
int64_t pc = asc_get_program_counter() & 0xFFFFFFFFFFFF;
asc_icache_preload(reinterpret_cast<void *>(pc), prefetch_length);
```

## 调用示例

获取PC后，可将其作为指令预加载起始地址，完整的预加载调用方式请参考[asc_icache_preload调用示例](../../cache_ctrl/asc_icache_preload.md#调用示例)。
