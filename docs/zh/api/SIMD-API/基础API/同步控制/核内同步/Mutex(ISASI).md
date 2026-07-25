# Mutex(ISASI)<a name="ZH-CN_TOPIC_0000002565915364"></a>

Mutex用于核内异步流水指令之间的同步控制，其功能类似于传统CPU中的锁机制。通过先锁定再释放指定流水来完成流水间的同步依赖，锁定/解锁指定流水对应Lock/Unlock接口。

- [Lock](Lock.md)：根据MutexID获取Mutex，若Mutex已被锁定，将阻塞后续指定流水指令队列，直到当前流水的前序指令中对应MutexID的Mutex被[Unlock](Unlock.md)。
- [Unlock](Unlock.md)：直到当前流水的前序指令执行完成后，根据MutexID释放对应Mutex。

**图 1**  Lock/UnLock接口<a name="fig58242463299"></a>  
![](../../../../figures/lock_unlock.png "Lock-UnLock接口")

每个锁有固定的一个MutexID，在不同编程范式中，该ID的获取以及释放方式不同：

- 采用[TPipe-TQue框架编程范式](../../../../../guide/编程指南/编程模型/AI-Core-SIMD编程/基于TPipe-TQue框架编程/TPipe-TQue框架编程范式.md)时，MutexID需要通过[AllocMutexID](AllocMutexID-(ISASI).md)/[ReleaseMutexID](ReleaseMutexID-(ISASI).md)进行申请释放。
- 采用[静态Tensor编程范式](../../../../../guide/编程指南/编程模型/AI-Core-SIMD编程/基于Tensor的CPP编程/静态Tensor编程.md)时，MutexID由开发者自行管理，建议使用0-27，28-31为系统内部规划预留，不建议使用。

相对于[SetFlag/WaitFlag](SetFlag-WaitFlag(ISASI).md)同步机制，使用Lock/Unlock接口有以下优势：

- 内聚性更强，使用时与其它流水线解耦，可以简化反向同步，如图2所示，以PIPE\_MTE2与PIPE\_V之间同步为例。
- 可以使用更多的同步信号量。

**图 2**  SetFlag/WaitFlag同步与Lock/Unlock同步代码差异<a name="fig27101823165111"></a>  
![](../../../../figures/setflag_waitflag_vs_lock_unlock_diff.png "SetFlag-WaitFlag同步与Lock-Unlock同步代码差异")

Mutex类定义如下：

```cpp
class Mutex {
template <pipe_t pipe>
static __aicore__ inline void Lock(MutexID id);
template <pipe_t pipe>
static __aicore__ inline void Unlock(MutexID id);
};
```
