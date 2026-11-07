# 概述

## 原子操作概述

### 数据搬运随路原子操作

数据搬运随路原子操作接口用于对后续目的地址为GM的数据搬运指令开启原子操作，涉及的接口请参见[表1](#datamove_atomic_operation_interface)。如下图1的左侧子图所示，未开启原子操作时，写入GM的数据搬运完成后，GM中原始数据将被新搬运数据完全覆盖。如图1右侧子图所示，当数据搬运随路原子操作接口被调用后，系统将为后续写入GM的数据搬运开启原子操作。此时，数据搬运完成后，GM中的最终数据由原始GM数据与新搬运数据共同决定。

**图1**  数据搬运随路原子累加效果

![](../../../figures/atomic_operation_pipelined_data_movement_effect.png "数据搬运随路原子累加效果")

### 标量原子操作

标量原子操作接口用于在Global Memory（GM）的指定地址上完成单个元素的原子读-改-写操作：读取该地址的旧值`old_value`，按接口语义计算新值并写回该地址，同时返回`old_value`。整个过程由硬件保证为原子操作，其他原子更新无法插入其中，因此多个AI Core或同一AI Core内的多条指令并发访问同一GM地址时不会相互覆盖，最终结果符合预期。不使用原子操作与使用原子操作的效果对比如[标量原子累加效果示意图](#scalar_atomic_operation_diagram)所示。

标量原子操作适用于多个核需要更新同一个GM地址的场景，例如多核并行累加同一个统计量、维护多个核共享的计数器、在指定地址上比较并更新状态等场景。

标量原子操作与数据搬运随路原子操作相比，两者的差异如下：

- 数据搬运随路原子操作的对象是搬运的Tensor，需要先调用数据搬运随路原子操作接口开启原子操作，开启后对后续通过MTE单元搬运到GM的整个Tensor生效，会影响后续搬运指令的行为。
- 标量原子操作的对象是传入的GM地址上的单个元素，无需开启配置，仅对本次调用传入的GM地址上的单个元素生效，不会影响后续搬运指令。

如下图2左侧子图所示，不使用`asc_atomic_add`接口时，多个AI Core同时对同一GM地址执行累加操作会相互覆盖，操作不具备原子性，最终结果不可预期。如右侧子图所示，使用`asc_atomic_add`接口后，各AI Core的累加操作串行化执行，确保每次累加操作的原子性，最终结果符合预期。

**图2**  标量原子累加效果<a id="scalar_atomic_operation_diagram"></a>

![](../../../figures/scalar_atomic_operation_diagram.png "标量原子累加效果")

原子操作的确定性计算与同步方案请参见[关键特性说明](key_features.md)。

## API列表

### 搬运原子操作

**表1** 数据搬运随路原子操作接口<a id="datamove_atomic_operation_interface"></a>

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_set_atomic_add](datamove_atomic/asc_set_atomic_add.md) | 对后续目的地址为GM的数据搬运开启原子累加。原子累加过程：将待拷贝的内容和GM已有内容进行求和，然后将求和结果写入GM。 | `c_api/atomic/datamove_atomic.h` |
| [asc_set_atomic_max](datamove_atomic/asc_set_atomic_max.md) | 设置后续搬运到GM的数据是否执行原子比较：将待拷贝的内容和GM已有内容进行比较，然后将最大值写入GM。 | `c_api/atomic/datamove_atomic.h` |
| [asc_set_atomic_min](datamove_atomic/asc_set_atomic_min.md) | 设置后续搬运到GM的数据是否执行原子比较：将待拷贝的内容和GM已有内容进行比较，然后将最小值写入GM。 | `c_api/atomic/datamove_atomic.h` |
| [asc_disable_dma_atomic](datamove_atomic/asc_disable_dma_atomic.md) | 关闭数据搬运随路原子操作功能。 | `c_api/atomic/datamove_atomic.h` |

### 标量原子操作

标量原子操作接口如表2所示，各接口支持的数据类型如表3所示。其中`old_value`为`address`指向的GM地址上的旧值，`new_value`为写回该地址的新值，表2中的原子计算接口均返回`old_value`。

**表2** 标量原子操作接口

| 接口名称 | 接口描述 | 计算公式 | 最细粒度公共头文件 |
| --- | --- | --- | --- |
| [asc_atomic_add](scalar_atomic/asc_atomic_add.md) | 原子加：将`address`指向的GM地址上的旧值与输入标量值`val`求和，并将结果写回该地址。 | $new\_value = old\_value + val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_sub](scalar_atomic/asc_atomic_sub.md) | 原子减：将`address`指向的GM地址上的旧值减去输入标量值`val`，并将结果写回该地址。 | $new\_value = old\_value - val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_max](scalar_atomic/asc_atomic_max.md) | 原子取大：比较`address`指向的GM地址上的旧值与输入标量值`val`，并将较大值写回该地址。 | $new\_value = Max(old\_value, val)$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_min](scalar_atomic/asc_atomic_min.md) | 原子取小：比较`address`指向的GM地址上的旧值与输入标量值`val`，并将较小值写回该地址。 | $new\_value = Min(old\_value, val)$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_inc](scalar_atomic/asc_atomic_inc.md) | 原子递增：`address`指向的GM地址上的旧值大于或等于`val`时将该地址置为0，否则将旧值加1后写回。 | $new\_value = (old\_value >= val) ? 0 : (old\_value + 1)$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_dec](scalar_atomic/asc_atomic_dec.md) | 原子递减：`address`指向的GM地址上的旧值等于0或大于`val`时将该地址置为`val`，否则将旧值减1后写回。 | $new\_value = (old\_value == 0 \vert\vert old\_value > val) ? val : (old\_value - 1)$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_and](scalar_atomic/asc_atomic_and.md) | 原子与：将`address`指向的GM地址上的旧值与输入标量值`val`按位与，并将结果写回该地址。 | $new\_value = old\_value \& val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_or](scalar_atomic/asc_atomic_or.md) | 原子或：将`address`指向的GM地址上的旧值与输入标量值`val`按位或，并将结果写回该地址。 | $new\_value = old\_value \vert val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_xor](scalar_atomic/asc_atomic_xor.md) | 原子异或：将`address`指向的GM地址上的旧值与输入标量值`val`按位异或，并将结果写回该地址。 | $new\_value = old\_value \oplus val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_exch](scalar_atomic/asc_atomic_exch.md) | 原子交换：将输入标量值`val`写入`address`指向的GM地址以替换旧值。 | $new\_value = val$ | `c_api/atomic/scalar_atomic.h` |
| [asc_atomic_cas](scalar_atomic/asc_atomic_cas.md) | 原子比较交换：`address`指向的GM地址上的旧值与`compare`相等时，将`val`写入该地址；不相等时保持该地址的值不变。 | $new\_value = (old\_value == compare) ? val : old\_value$ | `c_api/atomic/scalar_atomic.h` |
| [asc_set_store_atomic_config_v1](scalar_atomic/asc_set_store_atomic_config_v1.md) | 设置标量原子操作配置：操作类型（仅支持求和操作）和数据类型。 | - | `c_api/atomic/scalar_atomic.h` |
| [asc_get_store_atomic_config](scalar_atomic/asc_get_store_atomic_config.md) | 获取`asc_set_store_atomic_config_v1`设置的标量原子操作配置。 | - | `c_api/atomic/scalar_atomic.h` |

**表3** 标量原子操作接口支持的数据类型

| 接口名称 | `dtype`支持的数据类型 |
| --- | --- |
| [asc_atomic_add](scalar_atomic/asc_atomic_add.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_atomic_sub](scalar_atomic/asc_atomic_sub.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_atomic_max](scalar_atomic/asc_atomic_max.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_atomic_min](scalar_atomic/asc_atomic_min.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_atomic_inc](scalar_atomic/asc_atomic_inc.md) | `uint32_t`、`uint64_t` |
| [asc_atomic_dec](scalar_atomic/asc_atomic_dec.md) | `uint32_t`、`uint64_t` |
| [asc_atomic_and](scalar_atomic/asc_atomic_and.md) | `int32_t`、`uint32_t`、`int64_t`、`uint64_t` |
| [asc_atomic_or](scalar_atomic/asc_atomic_or.md) | `int32_t`、`uint32_t`、`int64_t`、`uint64_t` |
| [asc_atomic_xor](scalar_atomic/asc_atomic_xor.md) | `int32_t`、`uint32_t`、`int64_t`、`uint64_t` |
| [asc_atomic_exch](scalar_atomic/asc_atomic_exch.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_atomic_cas](scalar_atomic/asc_atomic_cas.md) | `int32_t`、`uint32_t`、`float`、`int64_t`、`uint64_t` |
| [asc_set_store_atomic_config_v1](scalar_atomic/asc_set_store_atomic_config_v1.md) | `float`、`half`、`int16_t`、`int32_t`、`int8_t`、`bfloat16_t` |
| [asc_get_store_atomic_config](scalar_atomic/asc_get_store_atomic_config.md) | - |
