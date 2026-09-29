# 基于Mutex实现多流水同步

## 总述

当矢量算子的数据搬入、矢量计算和数据搬出等不同流水之间存在数据依赖时，通过`Mutex`建立流水同步关系，使不同流水中存在数据依赖的操作串行执行，可以避免不同流水并发访问同一资源时产生数据错误。

本文以Basic API中的[Mutex::Lock](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md)和[Mutex::Unlock](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Mutex_ISASI.md)为例，说明使用[Mutex实现双缓冲多流水同步](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/mutex/README.md)的原理。相关同步原理同样适用于C API中的[asc_lock](../../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_lock.md)和[asc_unlock](../../../../../api/SIMD-API/c_api/sync/intra_core_sync/asc_unlock.md)。C API双缓冲样例请参考[add_double_buffer样例](../../../../../../../examples/02_simd_c_api/02_features/01_reg_vector_compute/00_add_double_buffer/README.md)。

本文首先介绍双缓冲算子中的正向数据依赖和反向数据依赖，然后说明如何规划`MutexID`并建立流水同步关系，接着通过`Lock`/`Unlock`与`SetFlag`/`WaitFlag`的对比说明`Mutex`的使用方式，最后介绍`Mutex`接口的使用约束。

## 双缓冲算子的多流水数据依赖

一个典型的矢量算子包含数据搬入、矢量计算和数据搬出三个阶段，分别使用`PIPE_MTE2`、`PIPE_V`和`PIPE_MTE3`流水。数据量超过Unified Buffer（UB）容量时，算子通常将数据切分为多个切片，并循环处理这些切片。

采用[双缓冲](../../../../operator_practice/simd_operator_impl/vector_programming/double_buffer_scenario.md)后，两组Buffer资源交替使用。当前切片使用一组资源进行计算时，其他流水可以使用另一组资源处理相邻切片。

同一切片中的流水存在正向数据依赖，需要通过流水同步保证处理顺序：

- `PIPE_MTE2 -> PIPE_V`：数据搬入完成后，`PIPE_V`才能读取该切片的数据并进行计算。
- `PIPE_V -> PIPE_MTE3`：计算完成后，`PIPE_MTE3`才能搬出计算结果。

Buffer复用时还会产生反向数据依赖。第`i + 2`轮重新使用第`i`轮的同一组Buffer资源时，需要通过流水同步满足以下约束：

- `PIPE_V -> PIPE_MTE2`：第`i`轮的计算完成后，第`i + 2`轮才能覆盖输入Buffer。
- `PIPE_MTE3 -> PIPE_V`：第`i`轮的结果搬出完成后，第`i + 2`轮才能覆盖输出Buffer。

针对正向数据依赖建立的流水同步关系，可以保证同一切片内的处理顺序；针对反向数据依赖建立的流水同步关系，可以保证Buffer复用时不会覆盖仍被前序流水访问的数据。关于如何通过流水同步解决以上两种数据依赖关系的原理，更详细的描述请参考以下文档：

- [静态Tensor编程方式的同步管理](../../../programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#同步管理)
- [内存一致性中的跨流水内存一致性](../../memory_model/memory_consistency.md#跨流水内存一致性)

## MutexID规划

开发者需要根据流水间的数据依赖关系规划用于同步的`MutexID`：处理同一组Buffer资源且存在数据依赖的流水使用相同的`MutexID`，不同的资源组使用不同的`MutexID`。例如，Add算子的`xLocal`、`yLocal`和`zLocal`组成一组资源。

双缓冲使用两组Buffer资源和两个`MutexID`：

- 循环第`i`轮选择`bufferIndex = i % 2`，使同一个`MutexID`只管理其对应资源组的流水同步。
- 第`i + 2`轮再次选择相同的资源组和`MutexID`，此时`PIPE_MTE2`上的`Lock`会等待此前所有使用该`MutexID`的流水均已`Unlock`，其中包括第`i`轮`PIPE_MTE3`的`Unlock`，从而满足反向数据依赖，保证资源能够安全复用。

| 轮次 | Buffer资源组 | `MutexID` | 关键约束 |
| --- | --- | --- | --- |
| `i` | `buffer0` | `mutexId0` | `PIPE_MTE2`、`PIPE_V`、`PIPE_MTE3`按顺序处理当前切片。 |
| `i + 1` | `buffer1` | `mutexId1` | 使用另一组资源，可以与第`i`轮的异步流水重叠。 |
| `i + 2` | `buffer0` | `mutexId0` | `PIPE_MTE2`搬入前等待第`i`轮的资源释放，避免覆盖`buffer0`。 |

`MutexID`的获取和释放方式与编程范式有关，完整约束请参考[Lock约束说明](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Lock.md#约束说明)：

- TPipe-TQue框架编程范式需要通过[AllocMutexID](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/AllocMutexID_ISASI.md)申请，并在使用结束后通过[ReleaseMutexID](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/ReleaseMutexID_ISASI.md)释放。
- 静态Tensor编程范式下，`MutexID`可以通过`AllocMutexID`/`ReleaseMutexID`申请释放，也可以由开发者自行管理；自行管理时需要保证`MutexID`不与其他同步或资源管理接口使用的`MutexID`冲突，建议使用0-27，28-31为系统内部规划预留，不建议使用。

## Mutex与其他核内同步接口

### Lock/Unlock与SetFlag/WaitFlag的功能

`Lock`和`Unlock`必须配合使用，接口的功能如下：

- `Lock`根据`MutexID`获取`Mutex`；如果`Mutex`已被锁定，将阻塞指定流水上的后续指令，直到此前所有流水中使用相同`MutexID`的`Mutex`都已通过`Unlock`释放。
- `Unlock`在指定流水的前序指令执行完成后，根据`MutexID`释放对应`Mutex`。

`SetFlag`和`WaitFlag`也必须配合使用，接口的功能如下：

- `SetFlag`在源流水的前序指令完成后设置对应标志位，但不会阻塞源流水的后续指令。
- `WaitFlag`执行时，如果标志位为0，则阻塞目的流水的后续指令，如果标志位为1，则清除标志位并继续执行。

图1和图2以`PIPE_MTE2`与`PIPE_V`之间的同步为例，对比两组接口实现同步的原理：

- 图1中，`SetFlag`/`WaitFlag`通过设置和等待事件标志位，使`PIPE_V`在`PIPE_MTE2`完成前序指令后继续执行。
- 图2中，`Lock`/`Unlock`通过释放和获取同一个`Mutex`，使`PIPE_V`在`PIPE_MTE2`完成前序指令并释放`Mutex`后获取`Mutex`、继续执行。

对比图中的等待、状态变化和解除阻塞过程，可以理解两组接口如何通过不同机制解决不同流水间的数据依赖。

**图1**  `SetFlag`/`WaitFlag`同步时序示意图    
![](../../../../figures/set_flag_wait_flag_sequence_diagram.png)

**图2**  `Lock`/`Unlock`同步时序示意图    
![](../../../../figures/mutex_lock_unlock_sequence_diagram.png)

### 两组接口针对正向数据依赖和反向数据依赖的同步方式差异

`Mutex`将`MutexID`与一组Buffer资源绑定，记录该资源此前各阶段的完成状态。每个阶段只需要锁定当前流水：

```cpp
for (uint32_t i = 0; i < loopCount; ++i) {
    uint32_t bufferIndex = i % 2;
    uint8_t mutexId = (bufferIndex == 0) ? mutexId0 : mutexId1;

    // 等待当前资源组此前的访问完成，然后搬入当前切片。
    AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);
    // PIPE_MTE2搬入当前切片的数据。
    AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);

    // 等待PIPE_MTE2完成，然后计算当前切片。
    AscendC::Mutex::Lock<PIPE_V>(mutexId);
    // PIPE_V计算当前切片。
    AscendC::Mutex::Unlock<PIPE_V>(mutexId);

    // 等待PIPE_V完成，然后搬出计算结果。
    AscendC::Mutex::Lock<PIPE_MTE3>(mutexId);
    // PIPE_MTE3搬出当前切片的结果。
    AscendC::Mutex::Unlock<PIPE_MTE3>(mutexId);
}
```

在第`i + 2`轮复用同一组资源时，`PIPE_MTE2`和`PIPE_V`再次`Lock`同一个`MutexID`，会统一等待此前所有使用该`MutexID`的流水完成。因此，代码不需要在当前调用中指定前序流水，也不需要分别针对`PIPE_V -> PIPE_MTE2`和`PIPE_MTE3 -> PIPE_V`等反向数据依赖建立同步关系。

如果使用`SetFlag`/`WaitFlag`，则需要针对每一组从源流水到目的流水的数据依赖分别建立同步关系。下面代码只保留同步相关的核心逻辑，数据搬入、计算和搬出操作用注释表示：

```cpp
for (uint32_t i = 0; i < loopCount; ++i) {
    uint32_t bufferIndex = i % 2;

    if (i >= BUFFER_NUM) {
        // 复用输入资源前，等待同一资源组的上一轮（第i-2轮）PIPE_V完成：PIPE_V -> PIPE_MTE2。
        AscendC::WaitFlag<AscendC::HardEvent::V_MTE2>(eventVToMTE2[bufferIndex]);
    }

    // PIPE_MTE2搬入当前切片的数据。
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(eventMTE2ToV[bufferIndex]);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(eventMTE2ToV[bufferIndex]);

    if (i >= BUFFER_NUM) {
        // 复用输出资源前，等待同一资源组的上一轮（第i-2轮）PIPE_MTE3完成：PIPE_MTE3 -> PIPE_V。
        AscendC::WaitFlag<AscendC::HardEvent::MTE3_V>(eventMTE3ToV[bufferIndex]);
    }

    // PIPE_V计算当前切片。
    if (i + BUFFER_NUM < loopCount) {
        // 只有后续仍会复用输入资源时，才设置PIPE_V -> PIPE_MTE2事件。
        AscendC::SetFlag<AscendC::HardEvent::V_MTE2>(eventVToMTE2[bufferIndex]);
    }

    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(eventVToMTE3[bufferIndex]);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(eventVToMTE3[bufferIndex]);

    // PIPE_MTE3搬出当前切片的结果。
    if (i + BUFFER_NUM < loopCount) {
        // 只有后续仍会复用输出资源时，才设置PIPE_MTE3 -> PIPE_V事件。
        AscendC::SetFlag<AscendC::HardEvent::MTE3_V>(eventMTE3ToV[bufferIndex]);
    }
}
```

`SetFlag`/`WaitFlag`需要针对正向数据依赖和反向数据依赖分别选择`HardEvent`并管理事件ID，以建立对应的流水同步关系，还需要根据循环的开始和结束位置判断反向事件是否存在。相比之下，`Mutex`不需要传入源流水和目的流水组合，只需在当前流水上使用相同`MutexID`，即可同步管理同一组资源的多流水生命周期。

此外，`SetFlag`/`WaitFlag`通过`HardEvent`表示源流水到目的流水的同步关系。不同硬件架构支持的`HardEvent`组合可能不同，使用`SetFlag`/`WaitFlag`时需要根据目标架构确认对应的`HardEvent`组合是否存在。

## 使用约束

使用`Mutex`时需要遵守以下约束，完整内容请以[Lock约束说明](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Lock.md#约束说明)和[Unlock约束说明](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/Unlock.md#约束说明)为准：

- 对同一个`MutexID`，`Lock`和`Unlock`必须严格成对出现，并使用相同的流水类型和`MutexID`；对应的`Unlock`必须位于`Lock`之后。
- 对于相同`MutexID`的`Lock`/`Unlock`组合，无论流水类型是否相同，都不得嵌套使用，否则属于未定义行为。
- 连续使用相同`MutexID`和相同流水类型的`Lock`/`Unlock`，不能实现同一流水内不同指令之间的同步。同一流水内需要保证不同指令的执行顺序时，应使用[PipeBarrier](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/PipeBarrier_ISASI.md)建立单个流水内的同步关系。
- TPipe-TQue框架编程范式下，通过`AllocMutexID`申请的`MutexID`使用结束后应及时通过`ReleaseMutexID`释放，避免`MutexID`耗尽。
- 同一组Buffer资源的所有相关流水应使用相同`MutexID`，不同资源组应使用不同`MutexID`。
