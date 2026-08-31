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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/sync_control/intra_core_sync/Lock_res.md#id1 -->

## 功能说明<a name="section54681522111017"></a>

头文件路径为：`"basic_api/kernel_common.h"`。

根据MutexID获取Mutex，若Mutex已被锁定，将阻塞指定流水上后续指令的执行，直到所有流水中具有相同MutexID的Mutex都已经被[Unlock](Unlock.md)释放。

> [!NOTE]说明
> Lock接口需与[Unlock](Unlock.md)接口配合使用，对同一个MutexID，Lock和Unlock必须严格成对出现，否则属于未定义行为。具体约束请参见[约束说明](#约束说明)。

## 函数原型<a name="section1568410468104"></a>

```cpp
template <pipe_t pipe>
static __aicore__ inline void Lock(MutexID id)
```

## 参数说明<a name="section74061251191017"></a>

**表 1**  模板参数说明

| 参数名 | 描述 |
| --- | --- |
| pipe | 模板参数，表示流水类别。支持PIPE_S、PIPE_M、PIPE_V、PIPE_MTE1、PIPE_MTE2、PIPE_MTE3、PIPE_FIX，各流水类型的含义请参考[硬件流水类型](intra_core_sync_overview.md#硬件流水类型)。 |

**表 2**  参数说明

| 参数名 | 输入/输出 | 描述 |
| --- | --- | --- |
| id | 输入 | 进行流水同步管理的MutexID。在不同编程范式中，该ID的获取以及释放方式不同，详细情况参考[约束说明](#约束说明)。 |

## 返回值说明<a name="section640mcpsimp"></a>

无

## 约束说明<a name="section184751024101111"></a>

- 每个锁有固定的一个MutexID，在不同编程范式中，该ID的获取以及释放方式不同：
    - 采用[TPipe-TQue框架编程范式](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md)时，MutexID需要通过[AllocMutexID](AllocMutexID_ISASI.md)/[ReleaseMutexID](ReleaseMutexID_ISASI.md)进行申请释放。
    - 采用[静态Tensor编程范式](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md)时，MutexID由开发者自行管理，建议使用0-27，28-31为系统内部规划预留，不建议使用。
- Lock与Unlock必须严格成对使用，并使用相同的pipe和id。此外，对应的Unlock必须始终写在Lock之后，否则属于未定义行为。

    ```cpp
    // 反例：先Unlock再Lock，顺序颠倒。
    AscendC::Mutex::Unlock<PIPE_MTE2>(0);
    AscendC::Mutex::Lock<PIPE_MTE2>(0);
    ```

- 对于id相同的Lock与Unlock组合，无论pipe是否相同，都不得在代码中嵌套使用，否则属于未定义行为。

    ```cpp
    // 反例：id相同，Lock与Unlock嵌套。
    AscendC::Mutex::Lock<PIPE_MTE2>(0);
    AscendC::Mutex::Lock<PIPE_MTE3>(0);
    AscendC::Mutex::Unlock<PIPE_MTE3>(0);
    AscendC::Mutex::Unlock<PIPE_MTE2>(0);
    ```

- 当具有相同id与pipe的两对Lock与Unlock连续调用时，第一次调用的Lock将由参数pipe指定的流水阻塞后，第二次调用的Lock不能再次阻塞该流水。换言之，连续调用的、具有相同id与pipe的两对Lock与Unlock不能实现单流水（参数pipe指定）内不同指令之间的同步，单流水内多个指令之间的同步请使用[PipeBarrier](PipeBarrier_ISASI.md)接口。

    如[图1](#fig_mte2_overlap)所示，两次搬运的目的地址在Unified Buffer（UB）存在重叠时，开发者需要控制PIPE_MTE2上两条指令执行的先后顺序。在此场景中如下写法（具有相同id与pipe的两对Lock与Unlock连续调用）只能保证两条PIPE_MTE2上指令执行后，PIPE_V上指令才能执行，但并不能控制PIPE_MTE2上两条指令执行的先后顺序，正确写法是在`CopyInY`与`CopyInX`之间插入`PipeBarrier<PIPE_MTE2>()`。

    **图1**  PIPE_MTE2上两次搬运目的地址（UB）存在重叠 <a name="fig_mte2_overlap"></a>    
    ![](../../../../figures/mte2_overlap.png)

    ```cpp
    // x和y在UB上的地址存在重叠（重叠部分x覆盖y），预期先搬入y后搬入x，需要保证搬入顺序。
    void CopyInX(LocalTensor<float>& x, const GlobalTensor<float>& xGm, uint32_t len, uint8_t mutexId)
    {
        AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);
        AscendC::DataCopy(x, xGm, len);
        AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);
    }

    void CopyInY(LocalTensor<float>& y, const GlobalTensor<float>& yGm, uint32_t len, uint8_t mutexId)
    {
        AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);
        AscendC::DataCopy(y, yGm, len);
        AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);
    }

    void Process(LocalTensor<float>& z, LocalTensor<float>& x, LocalTensor<float>& y, uint32_t len, uint8_t mutexId)
    {
        // 先搬入y，再搬入x（重叠部分x覆盖y），顺序错误会导致精度异常。
        CopyInY(y, yGm, len, mutexId);

        /*
        * // 必须在CopyInY与CopyInX之间插入PipeBarrier，保证前一次搬入完成后才执行下一次搬入。
        * AscendC::PipeBarrier<PIPE_MTE2>();
        */

        CopyInX(x, xGm, len, mutexId);
        AscendC::Mutex::Lock<PIPE_V>(mutexId);
        AscendC::Add(z, x, y, len);
        AscendC::Mutex::Unlock<PIPE_V>(mutexId);
    }
    ```

## 调用示例<a name="section123275308128"></a>

```cpp
// 申请两个MutexID，供双缓冲流水交替复用。
uint8_t mutexId0 = AscendC::AllocMutexID();
uint8_t mutexId1 = AscendC::AllocMutexID();

// 交替使用两个MutexID，保证MTE2、V、MTE3三段流水按顺序串联。
for (int32_t i = 0; i < loopCount; i++) {
    uint8_t mutexId = (i % 2 == 0) ? mutexId0 : mutexId1;

    // 锁住MTE2流水，保证当前tile的搬入按该MutexID顺序执行。
    AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);
    AscendC::DataCopy(xLocal, src0Global[TILE_LENGTH * progress], TILE_LENGTH);
    AscendC::DataCopy(yLocal, src1Global[TILE_LENGTH * progress], TILE_LENGTH);
    // 搬入完成后解锁MTE2流水，允许后续阶段继续推进。
    AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);

    // 锁住V流水，等待对应tile的搬入完成后再开始计算。
    AscendC::Mutex::Lock<PIPE_V>(mutexId);
    AscendC::Add(zLocal, xLocal, yLocal, TILE_LENGTH);
    // 计算完成后解锁V流水，放行后续计算或搬出。
    AscendC::Mutex::Unlock<PIPE_V>(mutexId);

    // 锁住MTE3流水，确保计算结果完成后再写回GM。
    AscendC::Mutex::Lock<PIPE_MTE3>(mutexId);
    AscendC::DataCopy(dstGlobal[TILE_LENGTH * progress], zLocal, TILE_LENGTH);
    // 搬出完成后解锁MTE3流水，结束当前tile的处理。
    AscendC::Mutex::Unlock<PIPE_MTE3>(mutexId);
}

// 释放本次样例申请的两个MutexID。
AscendC::ReleaseMutexID(mutexId0);
AscendC::ReleaseMutexID(mutexId1);
```

完整样例请参考：[Mutex样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/05_sync_control/mutex)。
