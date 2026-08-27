# C API

C API开放芯片完备编程能力，支持[以数组形式分配内存](general_description_and_constraints.md#以数组方式申请内存)，一般基于指针编程。提供与业界一致的C语言编程体验。

## 定位与特性

C API是Ascend C三层梯度化编程接口中的**语言扩展层SIMD API**，定位为最底层的C语言接口，基于指针编程，提供完备的C语言编程能力。C API可直接映射NPU硬件指令，开发者自主管理内存搬运与同步（与TPipe/TQue自动管理内存、同步不同），开放全部底层硬件能力。适配C语言开发习惯，适合对性能和可控性要求较高的算子开发场景，是追求极致性能、充分释放NPU硬件潜能的核心路径。

详细内容请参考[C语言编程概述](../../../guide/programming_guide/programming_model/ai_core_simd_programming/c_pointer_programming/c_programming_overview.md)。

## 需要包含的头文件

> [!NOTE]说明 
> 包含asc_simd.h文件来调用C API相应接口。如无特殊说明，包含该头文件即可满足接口调用需求。
> 若API文档中有特殊说明，则应遵循API的具体说明。

```cpp
#include "c_api/asc_simd.h"
```

## API类型介绍

- 前n个数据计算：该类型API在计算时采用“紧密排布”的数据读取方式，即从起始位置开始，按顺序连续获取所需数据。例如，若需处理N个数据，则从源操作数的第0个位置开始，依次取至第N-1个位置。
- 高维切分计算：该类型API按照设定的规则“跳过部分数据”。适合处理需要间隔采样的场景，灵活度高，但需要额外配置相关参数。
- 同步计算：该类型API内部自动插入同步操作，易用性更强。

## 关键字

**表1** 关键字列表

|名称|描述|
|-----------------------|-----------------------|
|\_\_gm\_\_|存储空间定义修饰符，表示被修饰的变量位于Global Memory地址空间。|
|\_\_ubuf\_\_|存储空间定义修饰符，表示被修饰的变量位于UB地址空间。|
|\_\_aicore\_\_|执行空间限定符。表示该函数只能在Ai Core上执行。|
|\_\_cbuf\_\_|存储空间定义修饰符，表示被修饰的变量位于L1 Buffer地址空间。|
|\_\_simd\_callee\_\_|执行空间限定符。表示该函数只能在Reg上执行。|

## 流水类型

NPU内部有不同的计算单元，在计算时往往需要把计算数据搬运到计算单元上。不同计算单元上的计算过程、数据搬运过程可划分为不同的流水线。在AI Core上执行的操作会被分配到不同的流水线(PIPE)上执行，包括以下几种：
- PIPE_S：标量流水线，负责指令分发和标量计算。
- PIPE_V：矢量计算流水线。
- PIPE_M：矩阵计算流水线。
- PIPE_MTE1：搬运操作。包括从L1 Buffer到L0A Buffer或L0B Buffer，从L1 Buffer到Unified Buffer（UB）的搬运操作和L0A Buffer或L0B Buffer的初始化操作。
- PIPE_MTE2：搬运操作。包括从GM到L1 Buffer、从GM到L0A Buffer或L0B Buffer、从GM到UB的搬运操作和L1 Buffer的初始化操作。
- PIPE_MTE3：搬运操作。包括从UB到GM、从UB到L1 Buffer、从L1 Buffer到GM的操作和从UB到UB的搬运操作。
- PIPE_FIX：Fixpipe流水线。
- PIPE_ALL：所有流水线。

可通过[同步控制](sync/sync.md)类API控制同一流水线内的运行顺序和不同流水线间的执行顺序。

## API列表

|      目录      |
|-----------------------|
| [基础定义](c_api.md#基础定义) |
| [工具接口](c_api.md#工具接口) |
| [矩阵计算](c_api.md#矩阵计算) |
| [矩阵计算搬运](c_api.md#矩阵计算搬运) |
| [Memory矢量计算](c_api.md#memory矢量计算) |
| [矢量数据搬运](c_api.md#矢量数据搬运) |
| [Reg矢量计算](c_api.md#reg矢量计算) |
| [标量计算](c_api.md#标量计算) |
| [原子操作](c_api.md#原子操作) |
| [缓存控制](c_api.md#缓存控制) |
| [同步控制](c_api.md#同步控制) |
| [其他操作](c_api.md#其他操作) |
| [特殊寄存器访问](c_api.md#特殊寄存器访问) |
| [废弃接口](deprecated_interface/deprecated_interface.md) |

## 参考样例

Atlas A3 训练系列产品/Atlas A3 推理系列产品的C API样例请参考[C API Add样例](../../../../../examples/02_simd_c_api/00_introduction/01_add)。
Ascend 950PR/Ascend 950DT的C API样例请参考[C API Add样例](../../../../../examples/02_simd_c_api/00_introduction/04_reg_base_add_compute)。
