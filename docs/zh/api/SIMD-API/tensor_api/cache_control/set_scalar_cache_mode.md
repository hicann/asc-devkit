# set_scalar_cache_mode

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

## 功能说明

头文件路径为：`tensor_api/tensor.h`。

配置标量单元访问Global Memory时的L2 Cache管理策略：

- 传入`load_l2_cache_mode`时，设置标量读（load）策略。
- 传入`store_l2_cache_mode`时，设置标量写（store）策略。

设置后，后续标量路径上的Global Memory读写将按该策略访问L2 Cache，直至再次调用本接口修改。

## 函数原型

```cpp
__aicore__ inline void set_scalar_cache_mode(load_l2_cache_mode mode)
__aicore__ inline void set_scalar_cache_mode(store_l2_cache_mode mode)
```

## 参数说明

**表1** 参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| mode | 输入 | L2 Cache管理策略。`load_l2_cache_mode`作用于标量load路径，`store_l2_cache_mode`作用于标量store路径。 |

### 枚举值说明

`load_l2_cache_mode`的枚举值说明如下：

**表2** `load_l2_cache_mode`枚举值说明

| 枚举值 | L2 Cache管理策略 |
| --- | --- |
| normal_first_victim | 启用L2 Cache，并将分配的Cache Line标记为高替换优先级，即在发生替换时优先被淘汰。 |
| normal_last_victim | 启用L2 Cache，并将分配的Cache Line标记为低替换优先级，即在发生替换时优先保留。 |
| normal_persistent | 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定数据始终保留在L2 Cache中，可采用驻留模式。被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。 |
| notalloc_keep | 不启用L2 Cache，每次都直接从GM中读取，并且保持已有Cache Line的状态不变。 |
| notalloc_clean | 不启用L2 Cache，若L2 Cache中已有同地址缓存会被保留并标记为Clean，标记为Clean的Cache Line被移出L2 Cache时会被直接丢弃。 |
| notalloc_drop | 不启用L2 Cache，若L2 Cache中已有同地址缓存，会被直接丢弃。 |

`store_l2_cache_mode`的枚举值说明如下：

**表3** `store_l2_cache_mode`枚举值说明

| 枚举值 | L2 Cache管理策略 |
| --- | --- |
| normal_first_victim | 启用L2 Cache，并将分配的Cache Line标记为高替换优先级，即在发生替换时优先被淘汰。 |
| normal_last_victim | 启用L2 Cache，并将分配的Cache Line标记为低替换优先级，即在发生替换时优先保留。 |
| normal_persistent | 启用L2 Cache。已存入L2 Cache中的数据可能被替换，若需确保特定数据始终保留在L2 Cache中，可采用驻留模式。被标记为驻留模式的Cache Line只能被其他同样被标记为驻留模式的Cache Line替换。 |
| notalloc_clean | 不启用L2 Cache，若L2 Cache中已有同地址缓存会被保留并标记为Clean，标记为Clean的Cache Line被移出L2 Cache时会被直接丢弃。 |

Clean和Dirty用于表示Cache Line是否需要回写GM：Clean表示该Cache Line未被标记为需要回写，被替换时可直接丢弃；Dirty表示该Cache Line包含尚未回写GM的修改数据，被替换时需要先将修改数据回写GM。

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

- 本接口的两个函数原型分别作用于标量load与标量store路径。标量load路径影响[load_dev](../scalar_compute/load_dev.md)、[datacache_preload](datacache_preload.md)和[icache_preload](icache_preload.md)；标量store路径影响[store_dev](../scalar_compute/store_dev.md)和标量原子操作。
- 本接口仅作用于标量访存路径的L2 Cache默认策略，不会改变向量搬运类接口参数中的缓存策略。

## 调用示例

以下示例临时将标量load路径设置为`normal_first_victim`，读取数据后恢复原策略。

```cpp
#include <cstdint>

#include "tensor_api/tensor.h"

__global__ __vector__ void set_cache_mode_kernel(__gm__ int32_t* data)
{
    using namespace asc::te;
    const load_l2_cache_mode old_mode = get_scalar_load_cache_mode();
    set_scalar_cache_mode(load_l2_cache_mode::normal_first_victim);
    const int32_t value = load_dev<int32_t>(data);
    set_scalar_cache_mode(old_mode);
    store_dev<int32_t>(data, value);
}
```

