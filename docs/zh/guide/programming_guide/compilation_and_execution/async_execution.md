# 异步执行
当开发者完成一个Kernel核函数的编写后，需要通过特定的下发/启动语法将其提交到指定的Stream（计算流）上执行。例如：
```c++
     kernel<<<numBlocks, ubufDynSize, stream>>>(参数列表);
     // 或使用对应的API：aclrtLaunchKernel*(...)
```
这些核Kernel核函数都是通过CANN Runtime完成Kernel的加载与运行。CANN Runtime是CANN软件栈中负责驱动硬件执行与管理AI计算任务的核心组件，它通过提供Device、Memory、Context、Stream、Kernel管理等API，使得上层应用和框架能够高效利用AI处理器的硬件计算资源。

完成Kernel加载和运行的主要流程分以下几步：
```txt
  1. 初始化：aclInit。
  2. 运行时资源申请：通过aclrtSetDevice和aclrtCreateStream分别申请Device、Stream运行管理资源。
  3. 如果需要Device内存参与计算，使用aclrtMalloc分配Device内存，通过aclrtMemcpy或aclrtMemcpyAsync将Host侧内存拷贝到Device侧。
  4. 通过kernel<<<numBlocks, ubufDynSize, stream>>>(参数列表)调用核函数，完成核函数的异步执行。
  5. 执行核函数后，需要同步等待核函数的执行完成。
  6. 如果Host需要Device内存的计算结果，使用aclrtMemcpy或aclrtMemcpyAsync将Device侧内存拷贝到Host侧。
  7. 资源释放：通过aclrtDestroyStream和aclrtResetDevice分别释放Stream、Device运行管理资源。
  8. 去初始化：aclFinalize。
```
![](../../figures/npu_verify.png)

在这个过程中我们提到了Stream创建与销毁，内存申请与拷贝，异步调用与同步等待。在本节的其余部分，我们将解释这些CANN Runtime的元素和对应的API。

##  Stream管理
Stream（计算流）本质上是一个有序的任务队列，为开发者提供了一种表示“先序执行”的抽象机制，程序可向队列中提交各类操作，例如内存拷贝、核函数下发等，所有任务将按入队顺序依次执行。单个计算流内，队列头部任务优先执行，执行完成后自动出队；后续队列任务依次前移，等待调度执行。同一个计算流中的任务具备串行执行特性，严格遵循入队先后顺序运行。

应用程序可同时创建并使用多个计算流。多流场景下，运行时会根据AI处理器的硬件资源占用状态，从存有待处理任务的计算流中择优调度任务执行。开发者可为计算流配置优先级，该优先级将作为运行时调度的参考依据，用以调整任务调度策略，但无法强制保证固定的执行顺序。

绑定在指定计算流内的API接口调用、核函数下发操作，相对于主机CPU线程均为异步执行。
应用程序可通过等待目标计算流清空所有任务完成流级同步，也可在整个设备维度完成全局同步。

CANN Runtime内置默认计算流，所有未显式指定计算流的运算操作、核函数下发任务，都会默认排入该流中执行。绝大多数未手动指定计算流的业务代码，都在隐式使用默认计算流。

- **创建与销毁Stream**： `aclrtCreateStream(&stream)`与`aclrtDestroyStream(stream)`。
- **指定Stream执行核函数**： `kernel<<<numBlocks, 0, stream>>>(...)`。
- **指定Stream异步拷贝**： `aclrtMemcpyAsync(..., stream)`。
- **指定Stream同步**：`aclrtSynchronizeStream(stream)`。

##  Memory（内存）管理
在异构计算架构中，系统由主机（Host）和设备（Device）组成。Host和Device各自拥有独立的内存，Host内存是指AI处理器所在服务器的主机内存（即CPU内存），而Device内存则是指AI处理器自带的设备内存。

内存管理中要做好的两件事是：
1. **可以访问内存**：Runtime提供了一套内存管理API，使开发者能够高效便捷地编写应用程序中的内存管理代码。由于Host和Device的内存相互独立，Runtime提供了专门的接口来分别申请和释放Host内存及Device内存。例如，申请和释放Host内存的接口为aclrtMallocHost和aclrtFreeHost，而申请和释放Device内存的接口为aclrtMalloc和aclrtFree。
2.  **高效访问内存**：算子在Device上执行过程中，访问Device上的Device内存数据性能更高。为此，Runtime提供了Host与Device之间互相拷贝内存的接口，支持同步和异步方式，例如aclrtMemcpy和aclrtMemcpyAsync等，以便开发者更好地规划数据的存储与访问。

## 异步调用
为了能够更好的发挥CPU和AI处理器的硬件利用率，CANN Runtime遵循“下发即返回”的异步执行策略，使Host在提交H2D/D2H拷贝或Kernel计算任务后立即继续执行后续逻辑，从而让主机侧调度、设备侧计算、主机与设备之间的数据传输尽可能重叠执行，隐藏传输和调度开销，提高端到端吞吐率。

典型<<<\>\>\>异步调用的场景是：将主机与设备计算操作重叠执行，从而提升主机侧与设备侧计算资源利用率。
![Kernel异步调用](../../figures/asynchronous_execution.png)


同样地，将aclrtMemcpy替换成aclrtMemcpyAsync接口，可以实现主机与设备的内存传输与计算操作重叠执行，从而降低甚至消除内存传输带来的性能开销。
![内存传输异步并发执行](../../figures/asynchronous_execution_async.png)



## 同步等待
核函数的调用是异步的，在核函数调用结束之后，控制权立刻返回给主机端，需要在host等待核函数的执行完成，可以通过以下接口强制Host程序等待核函数执行完毕：
|同步接口|接口描述|
|-|-|
|`aclrtSynchronizeDevice()` | 阻塞Host侧当前线程，直到与当前线程绑定的Context所对应的Device中的所有操作都已完成。|
|`aclrtSynchronizeStream()` |阻塞Host侧当前线程直到指定Stream中的所有操作都已完成。 |

## 更多的Runtime高级特性
更多的Runtime编程方式及接口，可以参考[《应用开发（C&C++）》](https://hiascend.com/document/redirect/CannCommunityadev)。
