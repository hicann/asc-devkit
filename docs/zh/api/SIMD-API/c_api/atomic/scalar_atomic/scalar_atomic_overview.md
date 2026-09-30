# 标量原子操作概述

标量原子操作接口用于在Global Memory（GM）的指定地址上完成单个元素的原子读-改-写操作：读取该地址的旧值`old_value`，按接口语义计算新值并写回该地址，同时返回`old_value`。整个过程由硬件保证为原子操作，其他原子更新无法插入其中，因此多个AI Core或同一AI Core内的多条指令并发访问同一GM地址时不会相互覆盖，最终结果符合预期。不使用原子操作与使用原子操作的效果对比如[标量原子累加效果示意图](../atomic_operations_overview.md#scalar_atomic_operation_diagram)所示。

标量原子操作适用于多个核需要更新同一个GM地址的场景，例如多核并行累加同一个统计量、维护多个核共享的计数器、在指定地址上比较并更新状态等场景。

## 接口列表

标量原子操作接口如表1所示，各接口支持的数据类型如表2所示。其中`old_value`为`address`指向的GM地址上的旧值，`new_value`为写回该地址的新值，表1中的原子计算接口均返回`old_value`。

**表1**  标量原子操作接口

| 对应接口 | 接口功能描述 | 计算公式 |
| --- | --- | --- |
| [asc_atomic_add](asc_atomic_add.md) | 原子加：将`address`指向的GM地址上的旧值与输入标量值`val`求和，并将结果写回该地址。 | $new\_value = old\_value + val$ |
| [asc_atomic_sub](asc_atomic_sub.md) | 原子减：将`address`指向的GM地址上的旧值减去输入标量值`val`，并将结果写回该地址。 | $new\_value = old\_value - val$ |
| [asc_atomic_max](asc_atomic_max.md) | 原子取大：比较`address`指向的GM地址上的旧值与输入标量值`val`，并将较大值写回该地址。 | $new\_value = Max(old\_value, val)$ |
| [asc_atomic_min](asc_atomic_min.md) | 原子取小：比较`address`指向的GM地址上的旧值与输入标量值`val`，并将较小值写回该地址。 | $new\_value = Min(old\_value, val)$ |
| [asc_atomic_inc](asc_atomic_inc.md) | 原子递增：`address`指向的GM地址上的旧值大于或等于`val`时将该地址置为0，否则将旧值加1后写回。 | $new\_value = (old\_value >= val) ? 0 : (old\_value + 1)$ |
| [asc_atomic_dec](asc_atomic_dec.md) | 原子递减：`address`指向的GM地址上的旧值等于0或大于`val`时将该地址置为`val`，否则将旧值减1后写回。 | $new\_value = (old\_value == 0 \vert\vert old\_value > val) ? val : (old\_value - 1)$ |
| [asc_atomic_and](asc_atomic_and.md) | 原子与：将`address`指向的GM地址上的旧值与输入标量值`val`按位与，并将结果写回该地址。 | $new\_value = old\_value \& val$ |
| [asc_atomic_or](asc_atomic_or.md) | 原子或：将`address`指向的GM地址上的旧值与输入标量值`val`按位或，并将结果写回该地址。 | $new\_value = old\_value \vert val$ |
| [asc_atomic_xor](asc_atomic_xor.md) | 原子异或：将`address`指向的GM地址上的旧值与输入标量值`val`按位异或，并将结果写回该地址。 | $new\_value = old\_value \oplus val$ |
| [asc_atomic_exch](asc_atomic_exch.md) | 原子交换：将输入标量值`val`写入`address`指向的GM地址以替换旧值。 | $new\_value = val$ |
| [asc_atomic_cas](asc_atomic_cas.md) | 原子比较交换：`address`指向的GM地址上的旧值与`compare`相等时，将`val`写入该地址；不相等时保持该地址的值不变。 | $new\_value = (old\_value == compare) ? val : old\_value$ |
| [asc_set_store_atomic_config_v1](asc_set_store_atomic_config_v1.md) | 设置标量原子操作配置：操作类型（仅支持求和操作）和数据类型。 | - |
| [asc_get_store_atomic_config](asc_get_store_atomic_config.md) | 获取asc_set_store_atomic_config_v1设置的标量原子操作配置。 | - |

表1中两类接口的产品支持情况不同：

- [asc_atomic_add](asc_atomic_add.md)～[asc_atomic_cas](asc_atomic_cas.md)：仅Ascend 950PR&950DT系列产品支持。
- [asc_set_store_atomic_config_v1](asc_set_store_atomic_config_v1.md)：仅Atlas A2系列产品、Atlas A3系列产品支持，Ascend 950PR&950DT系列产品不支持，在Ascend 950PR&950DT系列产品可使用[asc_atomic_add](asc_atomic_add.md)实现原子累加。
- [asc_get_store_atomic_config](asc_get_store_atomic_config.md)：Atlas A2系列产品、Atlas A3系列产品、Ascend 950PR&950DT系列产品均支持，但在Ascend 950PR&950DT系列产品上已废弃，该系列产品可直接使用[asc_atomic_add](asc_atomic_add.md)实现原子累加。

**表2**  标量原子操作接口支持的数据类型

| 对应接口 | `dtype`支持的数据类型 |
| --- | --- |
| [asc_atomic_add](asc_atomic_add.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_atomic_sub](asc_atomic_sub.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_atomic_max](asc_atomic_max.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_atomic_min](asc_atomic_min.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_atomic_inc](asc_atomic_inc.md) | `uint32_t`、`uint64_t` |
| [asc_atomic_dec](asc_atomic_dec.md) | `uint32_t`、`uint64_t` |
| [asc_atomic_and](asc_atomic_and.md) | `int32_t`、`uint32_t`、`int64_t`、`uint64_t` |
| [asc_atomic_or](asc_atomic_or.md) | `int32_t`、`uint32_t`、`int64_t`、`uint64_t` |
| [asc_atomic_xor](asc_atomic_xor.md) | `int32_t`、`uint32_t`、`int64_t`、`uint64_t` |
| [asc_atomic_exch](asc_atomic_exch.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_atomic_cas](asc_atomic_cas.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_set_store_atomic_config_v1](asc_set_store_atomic_config_v1.md) | `float`、`half`、`int16_t`、`int32_t`、`int8_t`、`bfloat16_t` |
| [asc_get_store_atomic_config](asc_get_store_atomic_config.md) | - |
