# 原子操作概述<a name="ZH-CN_TOPIC_0000002555621180"></a>

数据搬运随路原子操作接口用于对后续目的地址为GM的数据搬运开启原子操作，涉及的接口请参见[表1](#table1)。如下图1的左侧子图所示，未开启原子操作时，写入GM的数据搬运完成后，GM中原始数据将被新搬运数据完全覆盖。如图1右侧子图所示，当数据搬运随路原子操作接口被调用后，系统将为后续写入GM的数据搬运开启原子操作。此时，数据搬运完成后，GM中的最终数据由原始GM数据与新搬运数据共同决定。

**图1**  数据搬运随路原子累加效果  
![](../../../figures/atomic_operation_pipelined_data_movement_effect.png "数据搬运随路原子累加效果")

**表1**  数据搬运随路原子操作接口<a name="table1"></a>

| 对应接口 | 接口功能描述 |
| --- | --- |
| [SetAtomicAdd](SetAtomicAdd.md) | 对后续目的地址为GM的数据搬运开启原子累加。原子累加过程：将待拷贝的内容和GM已有内容进行求和，然后将求和结果写入GM。 |
| [SetAtomicType](SetAtomicType.md) | 通过设置模板参数来设定原子操作不同的数据类型。 |
| [DisableDmaAtomic](DisableDmaAtomic.md) | 关闭数据搬运随路原子操作功能。 |
| [SetAtomicMax(ISASI)](SetAtomicMax_ISASI.md) | 设置后续搬运到GM的数据是否执行原子比较：将待拷贝的内容和GM已有内容进行比较，然后将最大值写入GM。 |
| [SetAtomicMin(ISASI)](SetAtomicMin_ISASI.md) | 设置后续搬运到GM的数据是否执行原子比较：将待拷贝的内容和GM已有内容进行比较，然后将最小值写入GM。 |
| [SetStoreAtomicConfig(ISASI)](SetStoreAtomicConfig_ISASI.md) | 设置数据搬运的原子操作配置。 |
| [GetStoreAtomicConfig(ISASI)](GetStoreAtomicConfig_ISASI.md) | 获取数据搬运的原子操作配置。 |

<!-- npu="950" id1 -->
针对Ascend 950PR/Ascend 950DT新增Scalar原子操作接口，能够在指定GM地址上进行单点原子计算操作，涉及的接口请参见[表2](#table2)。对比数据搬运随路原子操作接口，Scalar原子操作接口不会影响后续向GM搬运数据的指令。

如下图2左侧子图所示，不使用AtomicAdd接口时，多个AI Core同时对同一GM地址执行累加操作会相互覆盖，操作不具备原子性，最终结果不可预期。如右侧子图所示，使用AtomicAdd接口后，各AI Core的累加操作串行化执行，确保每次累加操作的原子性，最终结果符合预期。

**图2**  标量原子累加效果  
![](../../../figures/scalar_atomic_operation_diagram.png "标量原子累加效果")

**表2**  Scalar原子操作接口<a name="table2"></a>

| 对应接口 | 接口功能描述 |
| --- | --- |
| [AtomicAdd](AtomicAdd.md) | 用于在指定GM地址上进行原子加操作，将address指向的GM地址上的旧值（old_value）与输入标量值（value）求和，将和结果（new_value）写回GM地址，返回该地址修改前的值（old_value）。 |
| [AtomicMin](AtomicMin.md) | 用于在指定GM地址上进行原子取最小值操作，将address指向的GM地址上的旧值（old_value）与输入标量值（value）做比较，将较小值（new_value）写回GM地址，返回该地址修改前的值（old_value）。 |
| [AtomicMax](AtomicMax.md) | 用于在指定GM地址上进行原子取大操作，将address指向的GM地址上的旧值（old_value）与输入的标量值（value）进行比较，将较大值（new_value）写回GM地址，返回该地址修改前的值（old_value）。 |
| [AtomicCas](AtomicCas.md) | 在指定GM地址上进行原子比较操作，读取address指向的GM地址上的旧值（old_value）与输入标量值value1进行比较：如果相等，则将输入标量值value2写入GM地址；如果不相等，则GM地址上的值保持不变。返回该地址修改前的值（old_value）。 |
| [AtomicExch](AtomicExch.md) | 用于在GM内存中执行原子交换操作，读取address指向的GM地址上的旧值（old_value），并将输入的标量值（value）替换旧值存储回同一地址，返回该地址修改前的值（old_value）。 |
<!-- end id1 -->
