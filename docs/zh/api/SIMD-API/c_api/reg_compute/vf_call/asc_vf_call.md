# asc\_vf\_call

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

在SIMD C API编程场景下使用。用于在Kernel中启动Vector Function（VF）子任务，VF函数内可调用Reg矢量计算C API完成寄存器级搬运和计算。

## 函数原型

```cpp
template <auto funcPtr, typename... Args>
__aicore__ inline void asc_vf_call(Args &&...args)
```

## 参数说明

**表1** 模板参数说明

| 参数名 | 描述 |
| :-- | :------------ |
| funcPtr | 用于指定Vector Function入口函数。 |
| Args | 定义可变参数，用于传递实参到Vector Function入口函数。 |

**表2** 参数说明

| 参数名 | 输入/输出 | 描述 |
| :-- | :------------ | :-- |
| args | 输入 | 可变参数，用于传递实参到Vector Function入口函数。 |

## 返回值说明

无

## 约束说明

- `asc_vf_call`启动Vector Function子任务时，子任务函数不能是类的成员函数，推荐使用普通函数或类静态函数，且入口函数必须使用`__simd_vf__`修饰宏。
- `asc_vf_call`启动Vector Function子任务时，入参仅支持Ascend C的内置数据类型及其组成的指针、数组、结构体类型；传入Reg矢量计算C API的源/目的地址时，指针类型必须指向Unified Buffer（UB）内存。
- Vector Function的流水类型为`PIPE_V`。Vector Function内部如存在UB地址重叠或跨流水依赖，需要根据具体接口约束插入[asc_mem_bar](../reg_sync/asc_mem_bar.md)。

## 调用示例

使用Reg矢量计算C API在Vector Function中对UB数据执行加法计算。

```cpp
constexpr uint32_t ELEMENT_COUNT = 64;

__simd_vf__ inline void add_vf(__ubuf__ float* dst, __ubuf__ float* src0, __ubuf__ float* src1)
{
    vector_float dst_reg;
    vector_float src0_reg;
    vector_float src1_reg;
    uint32_t count = ELEMENT_COUNT;
    vector_bool mask = asc_update_mask_b32(count);

    asc_loadalign(src0_reg, src0);
    asc_loadalign(src1_reg, src1);
    asc_add(dst_reg, src0_reg, src1_reg, mask);
    asc_storealign(dst, dst_reg, mask);
}

__global__ __vector__ void asc_add_kernel(__gm__ float* dst, __gm__ float* src0, __gm__ float* src1)
{
    asc_init();
    __ubuf__ float dst_local[ELEMENT_COUNT];
    __ubuf__ float src0_local[ELEMENT_COUNT];
    __ubuf__ float src1_local[ELEMENT_COUNT];

    asc_copy_gm2ub_align(src0_local, src0, ELEMENT_COUNT * sizeof(float));
    asc_copy_gm2ub_align(src1_local, src1, ELEMENT_COUNT * sizeof(float));
    asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);
    asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);

    asc_vf_call<add_vf>(dst_local, src0_local, src1_local);

    asc_sync_notify(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_sync_wait(PIPE_V, PIPE_MTE3, EVENT_ID0);
    asc_copy_ub2gm_align(dst, dst_local, ELEMENT_COUNT * sizeof(float));
    asc_sync();
}
```
