# Lock<a name="ZH-CN_TOPIC_0000002372961532"></a>

## 产品支持情况<a name="section73648168211"></a>

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

## 功能说明<a name="section54681522111017"></a>

头文件路径为：`"basic_api/kernel_common.h"`。

根据MutexID获取Mutex，若Mutex已被锁定，将阻塞后续指定流水指令队列，直到当前流水的前序指令中对应MutexID的Mutex被[Unlock](Unlock.md)。

> [!NOTE]说明
> Lock接口需与[Unlock](Unlock.md)接口配合使用，对同一个MutexID，Lock和Unlock必须严格成对出现，否则硬件行为将不可预测。具体约束请参见[约束说明](#约束说明)。

## 函数原型<a name="section1568410468104"></a>

```cpp
template <pipe_t pipe>
static __aicore__ inline void Lock(MutexID id)
```

## 参数说明<a name="section74061251191017"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| pipe | 模板参数，表示流水类别。支持的流水类型为PIPE_S/PIPE_M/PIPE_V/PIPE_MTE1/PIPE_MTE2/PIPE_MTE3/PIPE_FIX |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| id | 输入 | 进行流水同步管理的MutexID。在不同编程范式中，该ID的获取以及释放方式不同，详细情况参考[约束说明](#约束说明)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section184751024101111"></a>

- 每个锁有固定的一个MutexID，在不同编程范式中，该ID的获取以及释放方式不同：
    - 采用[TPipe-TQue框架编程范式](../../../../../guide/编程指南/编程模型/AI-Core-SIMD编程/基于TPipe-TQue框架编程/TPipe-TQue框架编程范式.md)时，MutexID需要通过[AllocMutexID](AllocMutexID-(ISASI).md)/[ReleaseMutexID](ReleaseMutexID-(ISASI).md)进行申请释放。
    - 采用[静态Tensor编程范式](../../../../../guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/静态Tensor编程.md)时，MutexID由开发者自行管理，建议使用0-27，28-31为系统内部规划预留，不建议使用。
- 对于同一个MutexID，必须先执行Lock，然后才能执行Unlock，且指定的pipe需要相同。如果Lock和Unlock没有按照这种"成对出现"的顺序排列，硬件行为将不可预测。以下是常见的错误用法与正确用法示例：

    ```cpp
    // 错误写法 1：先Unlock再Lock，顺序颠倒。
    AscendC::Mutex::Unlock<PIPE_V>(0);
    AscendC::Mutex::Lock<PIPE_V>(0);
    
    // 错误写法 2：连续两次Lock后再连续两次Unlock，未遵循成对使用原则。
    AscendC::Mutex::Lock<PIPE_V>(0);
    AscendC::Mutex::Lock<PIPE_V>(0);
    AscendC::Mutex::Unlock<PIPE_V>(0);
    AscendC::Mutex::Unlock<PIPE_V>(0);


    
    
    // 正确写法：Lock和Unlock严格成对出现。
    AscendC::Mutex::Lock<PIPE_V>(0);
    AscendC::Mutex::Unlock<PIPE_V>(0);
    AscendC::Mutex::Lock<PIPE_V>(0);
    AscendC::Mutex::Unlock<PIPE_V>(0);
    ```

- 使用相同MutexID、相同流水的两组Lock/UnLock指令时，第二次Lock不会再阻塞流水，这种情况建议使用[PipeBarrier](PipeBarrier(ISASI).md)接口。以下是错误用法与正确用法的示例：

    ```cpp
    // 错误写法：第二次Lock已不再阻塞，起不到同步效果。
    AscendC::Mutex::Lock<PIPE_V>(0);
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    AscendC::Mutex::Unlock<PIPE_V>(0);
    // 不能达到阻塞PIPE_V的效果。
    AscendC::Mutex::Lock<PIPE_V>(0); 
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    AscendC::Mutex::Unlock<PIPE_V>(0);
    
    // 正确写法：使用PipeBarrier进行单流水内同步。
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    AscendC::PipeBarrier<PIPE_V>();
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    ```

## 调用示例<a name="section123275308128"></a>

```cpp
// 申请两个 MutexID，供双缓冲流水交替复用。
uint8_t mutexId0 = AscendC::AllocMutexID();
uint8_t mutexId1 = AscendC::AllocMutexID();

// 交替使用两个 MutexID，保证 MTE2、V、MTE3 三段流水按顺序串联。
for (int32_t i = 0; i < loopCount; i++) {
    uint8_t mutexId = (i % 2 == 0) ? mutexId0 : mutexId1;

    // 锁住 MTE2 流水，保证当前 tile 的搬入按该 MutexID 顺序执行。
    AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);
    AscendC::DataCopy(xLocal, src0Global[TILE_LENGTH * progress], TILE_LENGTH);
    AscendC::DataCopy(yLocal, src1Global[TILE_LENGTH * progress], TILE_LENGTH);
    // 搬入完成后解锁 MTE2 流水，允许后续阶段继续推进。
    AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);

    // 锁住 V 流水，等待对应 tile 的搬入完成后再开始计算。
    AscendC::Mutex::Lock<PIPE_V>(mutexId);
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    // 计算完成后解锁 V 流水，放行后续计算或搬出。
    AscendC::Mutex::Unlock<PIPE_V>(mutexId);

    // 锁住 MTE3 流水，确保计算结果完成后再写回 GM。
    AscendC::Mutex::Lock<PIPE_MTE3>(mutexId);
    AscendC::DataCopy(dstGlobal[TILE_LENGTH * progress], zLocal, TILE_LENGTH);
    // 搬出完成后解锁 MTE3 流水，结束当前 tile 的处理。
    AscendC::Mutex::Unlock<PIPE_MTE3>(mutexId);
}

// 释放本次样例申请的两个 MutexID。
AscendC::ReleaseMutexID(mutexId0);
AscendC::ReleaseMutexID(mutexId1);
```

完整样例请参考：[Mutex样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/mutex)。
