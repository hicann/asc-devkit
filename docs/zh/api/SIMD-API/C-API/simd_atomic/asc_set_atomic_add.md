# asc_set_atomic_add

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

设置对后续的从Unified Buffer/L0C Buffer/L1 Buffer到Global Memory的数据传输开启原子累加。累加的数据类型支持int8_t、int16_t、int32_t、bfloat16_t、half、float。
<!-- npu="950" id8 -->
特别地，针对Ascend 950PR/Ascend 950DT，不支持L1 Buffer到Global Memory的通路。
<!-- end id8 -->

## 函数原型

```c++
__aicore__ inline void asc_set_atomic_add_int8()
__aicore__ inline void asc_set_atomic_add_int16()
__aicore__ inline void asc_set_atomic_add_int()
__aicore__ inline void asc_set_atomic_add_bfloat()
__aicore__ inline void asc_set_atomic_add_float16()
__aicore__ inline void asc_set_atomic_add_float()
```

## 参数说明

无
## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 使用完成后，建议清空原子操作的状态（详见[asc_set_atomic_none](./asc_set_atomic_none.md)），以免影响后续相关指令功能。
- 该指令执行前不会对GM的数据做清零操作，开发者需在需要时手动添加清零操作。
<!-- npu="950" id9 -->
- Ascend 950PR/Ascend 950DT，不支持L1 Buffer到Global Memory的通路。
<!-- end id9 -->

## 调用示例

```c++
//total_length指参与搬运的数据总长度。dst是外部输入的int8_t类型的GM内存。
constexpr uint32_t total_length = 256;
__ubuf__ int8_t src0[total_length];
__ubuf__ int8_t src1[total_length];
asc_copy_ub2gm(dst, src0, total_length * sizeof(int8_t));
asc_sync_pipe(PIPE_MTE3);
asc_set_atomic_add_int8();
asc_copy_ub2gm(dst, src1, total_length * sizeof(int8_t));
asc_set_atomic_none();
```

结果示例：

```
输入数据src0：[1, 1, 1, ..., 1]  // int8_t类型
输入数据src1：[2, 2, 2, ..., 2]  // int8_t类型
输出数据dst：[3, 3, 3, ..., 3]   // int8_t类型
```
