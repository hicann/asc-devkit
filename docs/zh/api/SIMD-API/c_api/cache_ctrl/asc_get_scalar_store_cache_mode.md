# asc_get_scalar_store_cache_mode

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

读取当前标量写（store）路径的L2 Cache管理策略，并返回[asc_store_l2_cache_mode](../defs/enum/asc_store_l2_cache_mode.md)枚举值。

常用于临时切换策略前保存原配置，或在设置后做回读校验。

## 函数原型

```c
__aicore__ inline asc_store_l2_cache_mode asc_get_scalar_store_cache_mode()
```

## 参数说明

无

## 返回值说明

当前标量store路径的L2 Cache管理策略，类型为[asc_store_l2_cache_mode](../defs/enum/asc_store_l2_cache_mode.md)。

## 流水类型

PIPE_S

## 约束说明

- 本接口仅反映标量store侧策略，与标量load侧以及向量搬运接口参数中的`l2_cache_mode`无关。

## 调用示例

场景：与[asc_set_scalar_cache_mode](asc_set_scalar_cache_mode.md)场景B一致。多核算子通过[asc_atomic_add](../atomic/scalar_atomic/asc_atomic_add.md)向GM统计counter做标量原子累加；进入累加前查询并保存当前store策略，临时切换为`NOTALLOC_CLEAN`避免不必要的L2分配，累加结束后恢复原策略。

```cpp
__gm__ int32_t* counter_gm;  // GM上的多核共享计数器

// 查询并保存当前标量store策略
asc_store_l2_cache_mode old_mode = asc_get_scalar_store_cache_mode();
// 标量原子写GM控制字段，避免额外L2分配
asc_set_scalar_cache_mode(asc_store_l2_cache_mode::NOTALLOC_CLEAN);

asc_dcci_entire_all();  // 标量原子操作前保证DCache一致性
asc_atomic_add(counter_gm, 1);

// 恢复进入本段逻辑前的标量store策略
asc_set_scalar_cache_mode(old_mode);
```
