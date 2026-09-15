# Introduction<a name="ZH-CN_TOPIC_0000002565915364"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T13:54:37.005Z -->

Mutex is used for synchronization control between asynchronous pipeline instructions within a core. Its function is similar to the lock mechanism in a traditional CPU. Synchronization dependencies between pipelines are achieved by first locking and then releasing a specified pipeline. Locking/unlocking a specified pipeline corresponds to the **Lock**/**Unlock** APIs.

- [Lock](Lock.md): Obtains a Mutex based on the MutexID. If the Mutex is already locked, it blocks the subsequent instruction queue of the specified pipeline until the Mutex with the corresponding MutexID among the preceding instructions of the current pipeline is released by [Unlock](Unlock.md).
- [Unlock](Unlock.md): Releases the corresponding Mutex based on the MutexID only after the preceding instructions of the current pipeline have completed execution.

**Figure 1**  Lock/UnLock API<a name="fig58242463299"></a>  
![](../../../../figures/lock_unlock.png "Lock-UnLock API")

Each lock has a fixed MutexID. The way this ID is obtained and released differs across programming paradigms:

- When the [TPipe-TQue framework programming paradigm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_framework_programming_paradigm.md) is used, the MutexID must be applied for and released through [AllocMutexID](AllocMutexID_ISASI.md)/[ReleaseMutexID](ReleaseMutexID_ISASI.md).
- When the [static Tensor programming paradigm](https://gitcode.com/cann/asc-devkit/blob/9.1.0/docs/en/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md) is used, the MutexID is managed by the developer. It is recommended to use 0-27, while 28-31 are reserved for internal system planning and are not recommended for use.

Compared with the [SetFlag/WaitFlag](SetFlag_WaitFlag_ISASI.md) synchronization mechanism, the **Lock**/**Unlock** APIs offer the following advantages:

- Stronger cohesion. When used, they are decoupled from other pipelines, which simplifies reverse synchronization. As shown in Figure 2, take the synchronization between **PIPE_MTE2** and **PIPE_V** as an example.
- More synchronization semaphores can be used.

**Figure 2**  Code differences between SetFlag/WaitFlag synchronization and Lock/Unlock synchronization<a name="fig27101823165111"></a>  
![](../../../../figures/setflag_waitflag_vs_lock_unlock_diff.png "Code differences between SetFlag-WaitFlag synchronization and Lock-Unlock synchronization")

The Mutex class is defined as follows:

```cpp
class Mutex {
template <pipe_t pipe>
static __aicore__ inline void Lock(MutexID id);
template <pipe_t pipe>
static __aicore__ inline void Unlock(MutexID id);
};
```
