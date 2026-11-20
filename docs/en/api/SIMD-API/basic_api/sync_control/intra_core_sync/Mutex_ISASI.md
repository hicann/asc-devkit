# Mutex(ISASI)<a name="ZH-CN_TOPIC_0000002565915364"></a>

<!-- md-trans-meta sourceCommit=7b11125165209a9c10e60b3777be99912c7e5f35 translatedAt=2026-11-05T05:21:28.226Z pushedAt=2026-11-18T01:25:31.170Z nl=2 -->

Mutex is used for synchronization control between intra-core asynchronous pipeline instructions. Its function is similar to the lock mechanism in a traditional CPU. Synchronization dependencies between pipelines are achieved by first locking and then releasing a specified pipeline. Locking/unlocking a specified pipeline corresponds to the **Lock**/**Unlock** APIs.

- [Lock](Lock.md): Obtains the Mutex based on the MutexID. If the Mutex is already locked, it blocks the execution of subsequent instructions on the specified pipeline until all Mutexes with the same MutexID in all pipelines have been released by [Unlock](Unlock.md).
- [Unlock](Unlock.md): Releases the corresponding Mutex based on the MutexID after the preceding instructions on the current pipeline have completed execution.

**Figure 1**  Lock/UnLock APIs<a name="fig58242463299"></a>  
![](../../../../figures/lock_unlock.png "Lock-UnLock APIs")

Each lock has a fixed MutexID. In different programming paradigms, the acquisition and release of this ID differ:

- When using the [TPipe-TQue framework programming paradigm](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/tpipe_tque_programming/tpipe_tque_paradigm.md), the MutexID must be applied for and released through [AllocMutexID](AllocMutexID_ISASI.md)/[ReleaseMutexID](ReleaseMutexID_ISASI.md). The framework manages the MutexID allocation records in a unified manner to avoid conflicts with the MutexIDs used by resource management APIs such as TQue.
- When using the [static Tensor programming paradigm](../../../../../guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md), the MutexID can be applied for and released through [AllocMutexID](AllocMutexID_ISASI.md)/[ReleaseMutexID](ReleaseMutexID_ISASI.md), or it can be managed by the developer. Self-management reduces API call overhead, but the developer must ensure that the MutexID does not conflict with the MutexIDs used by other synchronization or resource management APIs. When managing the MutexID by yourself, it is recommended to use 0-27. 28-31 are reserved for internal system planning and are not recommended for use.

Compared with the [SetFlag/WaitFlag](SetFlag_WaitFlag_ISASI.md) synchronization mechanism, the Lock/Unlock APIs offer the following advantages:

- Stronger cohesion. When used, they are decoupled from other pipelines, which simplifies reverse synchronization. As shown in Figure 2, take the synchronization between PIPE\_MTE2 and PIPE\_V as an example.
- More synchronization semaphores can be used.

**Figure 2**  Code difference between SetFlag/WaitFlag synchronization and Lock/Unlock synchronization<a name="fig27101823165111"></a>  
![](../../../../figures/setflag_waitflag_vs_lock_unlock_diff.png "Code difference between SetFlag-WaitFlag synchronization and Lock-Unlock synchronization")

The **Mutex** class is defined as follows:

```cpp
class Mutex {
template <pipe_t pipe>
static __aicore__ inline void Lock(MutexID id);
template <pipe_t pipe>
static __aicore__ inline void Unlock(MutexID id);
};
```
