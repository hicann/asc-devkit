# AI CPU Programming<a name="ZH-CN_TOPIC_0000002509743871"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-20T10:58:58.937Z pushedAt=2026-08-26T03:07:22.877Z -->

The AI CPU is a processor based on the ARM64 architecture on the device side. It has the same memory access capability as the AI Core and can directly access memory resources on the device side. Like the CPU on the Host side, it can also perform similar data compute. As a supplement to the AI Core, it mainly handles non-cube and branch-intensive compute with complex logic. The AI CPU runs in a basic Linux environment and supports the libc library, C++ standard library, and STL template library during programming. The following figure shows its hardware architecture.

**Figure 1**  AI CPU hardware architecture<a name="fig1146075111341"></a>  
![](../../figures/ai_cpu_hardware_architecture.png "AI CPU hardware architecture")

The AI CPU programming described in this section supports only the following product models:

-   Ascend 950PR/Ascend 950DT
-   Atlas A3 training products/Atlas A3 inference products
-   Atlas A2 training products/Atlas A2 inference products

## AI CPU Kernel Function Definition<a name="section4987175618443"></a>

When programming on the AI CPU, similar to the AI Core, you also need to define a device-side function entry (that is, the kernel function). This function must be declared with the \_\_aicpu\_\_ identifier and used together with the \_\_global\_\_ identifier to indicate that it can only be called from the host side. The device-side implementation file of the AI CPU must use the .aicpu suffix (or the .cpp suffix, with the **-x aicpu** option added during compilation). This implementation file contains the kernel function described above as well as the definitions of ordinary AI CPU functions. Ordinary AI CPU functions do not need to add an execution space identifier.

The following is an example of an AI CPU "Hello World" program. The content of the **hello\_world.aicpu** file is as follows:

```
// Header file that must be included to call the printf API.
#include "aicpu_api.h"

__global__ __aicpu__ uint32_t hello_world(void *args)
{
    AscendC::printf("Hello World!!!\n");
    return 0;
}
```

>[!NOTE] Note
>The following specifications must be followed during programming:
>- A \_\_aicpu\_\_ \_\_global\_\_ function cannot have a void return type, and its input parameter can only be a pointer.
>- A \_\_aicpu\_\_ \_\_global\_\_ function cannot be a member function of a class, nor can it exist in an anonymous namespace.
>- Although the AI CPU kernel function has a return value, this return value is used only by the Runtime component to report the running status. Developers do not need to write return logic and cannot use this return value. Therefore, for users, the AI CPU kernel function is equivalent to the void type and cannot be used as an right value.

## AI CPU Kernel Function Call<a name="section178512255013"></a>

AI CPU kernel functions are called in the .asc file. Similar to AI Core operator calls, the <<<\>\>\> syntax is used.

```
hello_world<<<numBlocks, nullptr, stream>>>(&args, sizeof(KernelArgs));
```

-   numBlocks: The AI CPU device side does not support multi-core splitting, so calling multiple cores from the host side is meaningless. It is recommended to set this parameter to 1.
-   l2ctrl: Reserved parameter, currently fixed to nullptr. Developers do not need to pay attention to it.
-   stream: The type is aclrtStream. A stream is used to maintain the execution order of asynchronous operations, ensuring that they are executed on the device in the order in which they are called in the application code. For stream creation and other management APIs, see [*Runtime APIs*](https://hiascend.com/document/redirect/CannCommunityRuntimeApi).

> [!NOTE] Note
> When writing the call code, comply with the following rules:
> - The \_\_aicpu\_\_ \_\_global\_\_ function cannot be defined in the .asc file. It can only be declared, and the declaration must use extern.
> - When the host side calls the \_\_global\_\_ \_\_aicpu\_\_ function, the <<<\>\>\> heterogeneous call syntax must be used. In addition to the input parameter pointer, the size of the data read from the pointer must also be passed in.
> - When the host side uses the kernel call operator <<<...\>\>\> to call AI Core and AI CPU operators, the same stream cannot be used.

When loading and running operators, you need to use the Runtime API to complete runtime management and configuration. For details, see [Operator Running](../compilation_and_execution/asynchronous_execution.md). For AI CPU operator build, see [AI CPU Operator Build](../compilation_and_execution/operator_compilation/ai_cpu_operator_compilation_basics.md).

## AI CPU Template Kernel Function<a name="section135075471718"></a>

To use a template kernel function, you need to provide the instantiation declaration of the template kernel function in the .aicpu file, as shown below:

```
template<typename T, int BUFF_SIZE>
__global__ __aicpu__ uint32_t hello_world(void *args)
{
    AscendC::printf("Hello World!!!\n");
    AscendC::printf("buffer_size is %d\n", BUFF_SIZE);
    return 0;
}
template __global__ __aicpu__ uint32_t hello_world<KernelArgs, 4096>(void *args);
```

Then add the extern declaration of the template kernel function instantiation in the .asc file:

```
template<typename T, int BUFF_SIZE>
extern __global__ __aicpu__ uint32_t hello_world(void *args);

template extern __global__ __aicpu__ uint32_t hello_world<KernelArgs, 4096>(void *args);
```

## More Advanced Usage<a name="section109268265171"></a>

>[!NOTE] Note 
>For more information about how to use AI CPU APIs, see [AI CPU APIs](../../../api/AI-CPU-API/AI-CPU-API.md).