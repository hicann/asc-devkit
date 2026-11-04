# SIMD Built-In Keywords<a name="ZH-CN_TOPIC_0000002509863893"></a>

<!-- md-trans-meta sourceCommit=0b167b15c4f79241e5c530a8509210919591bb0a translatedAt=2026-10-23T15:14:12.408Z pushedAt=2026-11-03T07:05:13.468Z nl=2 -->

## Predefined Macros<a name="section2035104120360"></a>

As with other languages, some built-in macros are provided to facilitate program development. This section on predefined macros focuses on the macros frequently used in heterogeneous programming and explains their meanings.

-   <a id="npu-arch"></a>**\_\_NPU\_ARCH\_\_**

    \_\_NPU\_ARCH\_\_ is a preprocessing macro in the Device-side AI Core code, used to identify the architecture version of the AI processor. With this macro, developers can perform differentiated code adaptation and optimization for different AI processors. The correspondence between product models and NPU architecture versions is as follows:

    <!-- npu="950" id1 -->
    - Ascend 950PR&950DT products: 3510
    <!-- end id1 -->
    <!-- npu="A3" id2 -->
    - Atlas A3 products: 2201
    <!-- end id2 -->
    <!-- npu="910b" id3 -->
    - Atlas A2 products: 2201
    <!-- end id3 -->
    <!-- npu="310b" id4 -->
    - Atlas 200I/500 A2 inference products: 3002
    <!-- end id4 -->
    <!-- npu="310p" id5 -->
    - Atlas inference products: 2002
    <!-- end id5 -->
    <!-- npu="910" id6 -->
    - Atlas training products: 1001
    <!-- end id6 -->

    The following is an example of using \_\_NPU\_ARCH\_\_ to control the rounding mode of operator output values on different AI processors.

    ```
    __aicore__ static inline void CopyOut(uint64_t mulLen)
    {
    #if __NPU_ARCH__ == 2002
        Cast(dstLocal, srcLocal, RoundMode::CAST_NONE, mulLen); // CAST_NONE indicates that the rounding mode uses CAST_RINT when the conversion involves precision loss, and does not round when no precision loss is involved
    #elif __NPU_ARCH__ == 2201
        Cast(dstLocal, srcLocal, RoundMode::CAST_RINT, mulLen); // CAST_RINT indicates that the rounding mode is round half to even
    #endif
        event_t eventVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventVToMTE3);
        WaitFlag<HardEvent::V_MTE3>(eventVToMTE3);
        CommonCopyOut<float>(dstLocal, mulLen);  // Copy LocalTensor to GlobalTensor
    }
    ```

-   <a name="li19530175294118"></a>**ASCEND\_IS\_AIV, ASCEND\_IS\_AIC**

    **ASCEND\_IS\_AIV** and **ASCEND\_IS\_AIC** are conditional judgment statements implemented through C++ macros, used to implement conditional compilation of code in functions modified by **\_\_aicore\_\_**. When developing a fusion operator based on the decoupled mode (AIC core and AIV core separated), the operator logic involves the processing logic of both the AIV core and the AIC core, and inter-core synchronization is required. In this case, **ASCEND\_IS\_AIV**/**ASCEND\_IS\_AIC** must be used to isolate the AIV and AIC core code.

    >[!NOTE] Note
    >When the advanced API Matmul is used, it has already implemented the isolation of AIV and AIC core code through the **REGIST\_MATMUL\_OBJ** macro, so users do not need to use this macro for processing.

    Take the MatmulNzCustom operator as an example. In decoupled mode, this operator needs to implement different logic on the AIV core and the AIC core respectively. Specifically, the AIV core is responsible for moving cube data into the Unified Buffer (UB), completing data rearrangement (converting the cube data into NZ format), and writing it to Global Memory. The AIC core directly reads the rearranged NZ-format data from Global Memory and performs the cube multiplication (Matmul) computation. Because the code logic of the AIV core and the AIC core differs, code isolation is required through the ASCEND\_IS\_AIV and ASCEND\_IS\_AIC macros to ensure that code applicable to the AIV core and the AIC core is generated separately at compile time.

    The example pseudocode is as follows:

    ```
    template <typename AType, typename BType, typename CType, typename BiasType>
    __aicore__ inline void MatmulKernel<AType, BType, CType, BiasType>::Process(AscendC::TPipe *pipe)
    {
        // Use the Vector compute unit of the AIV core to implement ND2NZ format conversion. In the following code, MatrixBtoNZ is the function that converts cube B from ND to NZ format.
        if ASCEND_IS_AIV {
            pipe->InitBuffer(ubBuf, TOTAL_UB_SIZE);
            MatrixBtoNZ<typename B_TYPE::T>(tempGM,
                bGMNZ,
                tiling,
                isTransB,
                ubBuf,
                tiling.baseK,
                tiling.baseN);  // ND2NZ function implemented on the Vector side
            SyncAll();
            // Synchronize the AIC core and the AIV core
            AscendC::CrossCoreSetFlag<0x2, PIPE_MTE3>(0x4);
            return;
        }
        if ASCEND_IS_AIC {
            AscendC::CrossCoreWaitFlag<0x2>(0x4);   // Wait for the AIV core to complete the ND2NZ format conversion
        }
        ... ...
        // Set left cube A, right cube B, and Bias.
        matmulObj.SetTail(tailM, tailN);
        matmulObj.SetTensorA(aGlobal, false);
        matmulObj.SetTensorB(bGlobal, false);
        if (tiling.isBias) {
            matmulObj.SetBias(biasGlobal);
        }
        // Complete the cube multiplication operation
        matmulObj.IterateAll(cGlobal);
        // End the cube multiplication operation
        matmulObj.End();
    }
    ```

-   **ASCENDC\_CUBE\_ONLY**

    **ASCENDC\_CUBE\_ONLY** is a conditional statement implemented through a C++ macro, used for conditional compilation of code in functions modified by **\_\_aicore\_\_**.

    When developing non-fused operators based on the decoupled mode, in operator scenarios that involve only cube computation, you can set **ASCENDC\_CUBE\_ONLY** to enable the cube-only mode for Matmul computation, reducing the performance overhead of message communication and improving operator performance.

    >[!CAUTION] Caution
    >The **ASCENDC\_CUBE\_ONLY** macro must be set before **\#include "lib/matmul\_intf.h"**.

    Taking the matmul_custom operator as an example, the advanced API Matmul uses the MIX mode by default, that is, the user initiates a message from the AIV side, and after the message is relayed through the message communication framework, the Matmul computation is executed on the AIC side. This message processing mechanism introduces additional Scalar performance overhead. Compared with the MIX mode, the cube-only mode can directly bypass the message communication framework to complete the Matmul computation, thereby improving operator performance.

    The example pseudocode is as follows:

    ```
    #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"

    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_NORM> matmulObj;
    ```

## Function Execution Space Qualifiers<a name="section1074418132518"></a>

The function execution space qualifier indicates whether a function is executed on the host side or the device side, and whether it can be called from the host side or the device side.

-   **\_\_global\_\_**

    \_\_global\_\_ The execution space qualifier is used to declare a kernel function and has the following attributes:

    -   Executed on the device side and can only be called by host-side functions.
    -   A function modified by \_\_global\_\_ must return the void type and cannot be a member function of a class.
    -   It must be used together with \_\_aicore\_\_ / \_\_aicpu\_\_ / \_\_cube\_\_ / \_\_vector\_\_ / \_\_mix\_\_\(cube, vec\).
    -   When calling a function modified by \_\_global\_\_, the <<<\>\>\> heterogeneous call syntax must be used.
    -   Calling a function modified by \_\_global\_\_ is an asynchronous operation, that is, the call returns before the function finishes executing on the device side. If synchronization is required, call a Runtime synchronization API to explicitly synchronize, such as the **aclrtSynchronizeStream** API.

-   **\_\_aicore\_\_**

    The \_\_aicore\_\_ execution space qualifier declares a function, and it has the following attributes:

    -   Executed on the device side.
    -   On the device side, can only be called by functions modified with \_\_global\_\_ or other functions modified with \_\_aicore\_\_.
    -   When a host-side function calls a function modified with \_\_global\_\_ \_\_aicore\_\_, the <<<\>\>\> heterogeneous call syntax must be used.

    ```
    // Can only be called by device functions with the same execution space type
    __aicore__ void bar() {}

    // Define the kernel function executed on the AI Core
    __global__ __aicore__ void foo() {
      bar(); // Correct
    }
    ```

-   **\_\_host\_\_**

    The \_\_host\_\_ execution space qualifier declares a function, and it has the following attributes:

    -   Executed only on the host side.
    -   Can only be called by host-side functions.
    -   \_\_global\_\_ and \_\_host\_\_ cannot be used together.
    -   The \_\_host\_\_ qualifier is optional. A function not modified with any function execution space qualifier is a host function by default. 

    ```
    __aicore__ int f() {}

    // Define a Host-side function
    int foo() {}

    // Define a Host-side function
    __host__ int bar() {
      f();     // Error
      foo();   // Correct
    }

    // Error
    __global__ __host__ void kfunc() {}
    ```

-   **\_\_aicpu\_\_**

    The AI CPU function execution space qualifier \_\_aicpu\_\_ is used to indicate whether a function is an AI CPU kernel function, and it has the following attributes:

    -   Executed on the device side and can only be called by host-side functions, so it must be declared together with \_\_global\_\_
    -   A function modified by \_\_global\_\_ \_\_aicpu\_\_ cannot have a void return type, and its input parameter can only be a pointer.
    -   A function modified by \_\_global\_\_ \_\_aicpu\_\_ can only be declared with extern in the .asc file and cannot be defined in that file.
    -   When the Host side calls a function modified by \_\_global\_\_ \_\_aicpu\_\_, the <<<\>\>\> heterogeneous call syntax must be used. When calling, in addition to passing the parameter pointer, you must also specify the size of the data to be read from that pointer.
    -   Calling a function modified by \_\_global\_\_ is an asynchronous operation, that is, the call returns before the function finishes executing on the Device side. If synchronization is required, you can call a Runtime synchronization API to synchronize explicitly, such as the **aclrtSynchronizeStream** API.

    ```
    // Define the AI CPU kernel function in the AI CPU Device file.
    __aicpu__ void foo() {} // Error: only the __aicpu__ identifier is used, and the __global__ identifier is missing.
    __global__ void foo() {} // Error: only the __global__ identifier is used, and the __aicpu__ identifier is missing.
    __global__ __aicpu__  void foo() {} // Error: the return type is void.
    __global__ __aicpu__  int foo(void *a) {} // Correct.
    __global__ __aicpu__  int foo(int a) {} // Error: the input parameter is not a pointer type.
    __global__ __aicpu__  int foo(void *a, void *b) {} // Error: the number of input parameters is not 1.
    ```

    ```
    // Declare the AI CPU kernel function in the .asc file.
    extern __global__ __aicpu__ uint32_t hello_world(void *args); // Correct.
    ```

-   **\_\_cube\_\_**

    \_\_cube\_\_ is used to identify a kernel function that executes only on the Cube core. It has the following properties:

    -   For the hardware architecture in coupled mode, this modifier does not take effect.
    -   Executed on the device side and can only be called by host-side functions, so it must be declared together with \_\_global\_\_
    -   When the Host side calls a function modified by \_\_global\_\_ \_\_cube\_\_, the <<<\>\>\> heterogeneous call syntax must be used.

    ```
    __global__ __cube__ void mmad_custom(__gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* bias, __gm__ uint8_t* c){...}
    ```

-   **\_\_vector\_\_**

    \_\_vector\_\_ is used to identify a kernel function that executes only on the Vector core. It has the following properties:
    
    -   For the hardware architecture in coupled mode, this modifier does not take effect.
    -   Executed on the device side and can only be called by host-side functions, so it must be declared together with \_\_global\_\_
    -   When the Host side calls a function modified by \_\_global\_\_ \_\_vector\_\_, the <<<\>\>\> heterogeneous call syntax must be used.

    ```
    __global__ __vector__ void kernel(__gm__ float* x, __gm__ float* y){...}
    ```

-   **\_\_mix\_\_\(cube, vec\)**

    \_\_mix\_\_\(cube, vec\) is used to identify a kernel function that executes on both the Cube core and the Vector core. It has the following properties:
    
    -   \(cube, vec\) respectively indicate the ratio of Cube cores and Vector cores started by the kernel function, supporting \(1, 0\), \(0, 1\), \(1, 1\), and \(1, 2\).
    -   On the non-coupled architecture where this modifier takes effect, `__mix__(1, 0)` is equivalent to `__cube__`, and `__mix__(0, 1)` is equivalent to `__vector__`. They can be replaced by `__cube__` and `__vector__` respectively.
    -   For the hardware architecture in coupled mode, this modifier does not take effect.
    -   Executed on the device side and can only be called by host-side functions, so it must be declared together with \_\_global\_\_
    -   When the Host side calls a function modified by \_\_global\_\_ \_\_mix\_\_\(cube, vec\), the <<<\>\>\> heterogeneous call syntax must be used.

    ```
    __global__ __mix__(1,2) void kernel(__gm__ float* x, __gm__ float* y){...}
    ```

## Function Marker Macro<a name="section192521344610"></a>

-   \_\_simd\_vf\_\_

    Function marking macro, used to mark the SIMD VF entrypoint function. The function has no return value. Use [asc\_vf\_call](../../../api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md) to call the SIMD VF entrypoint function and start the VF subtask.

    ```
    __simd_vf__ inline void KernelAdd(__ubuf__ float* x, __ubuf__ float* y, __ubuf__ float* z)
    ```

    A SIMD VF marked with \_\_simd\_vf\_\_ has the following input parameter constraints:

    -   Pass-by-Pointer is supported. The pointer variable must be modified with the \_\_ubuf\_\_ address space qualifier.
    -   Pass-by-Reference is not supported.
    -   Passing a function pointer is not supported.

        An example of using \_\_simd\_vf\_\_ is as follows:

        ```
        __simd_vf__ inline void simd_adds(__ubuf__ float *output, __ubuf__ float *input,
            uint32_t count, uint16_t oneRepeatSize, uint16_t repeatTimes)
        {
            AscendC::Reg::RegTensor<float> srcReg;
            AscendC::Reg::RegTensor<float> dstReg;
            // asc_update_mask() will be supported later.
            // init MaskReg with the count of all numbers.
            AscendC::Reg::MaskReg maskReg = AscendC::Reg::UpdateMask<float>(count);
            for (uint16_t i = 0; i < repeatTimes; i++) {
                // asc_load, asc_adds and asc_store will be supported later.
                // load data from UB to RegTensor.
                AscendC::Reg::LoadAlign(srcReg, input + i * oneRepeatSize);
                AscendC::Reg::Adds(dstReg, srcReg, 1.0f, maskReg);
                // store data from RegTensor to UB.
                AscendC::Reg::StoreAlign(output + i * oneRepeatSize, dstReg, maskReg);
            }
        }
        ```

-   \_\_simd\_callee\_\_

    Function marker macro. The function can have a return value and can be called by a SIMD VF entrypoint function or other non-entrypoint functions.

    ```
    __simd_callee__ inline float add(float x, float y)
    ```

-   \_\_schedmode\_\_\(mode\)

    \_\_schedmode\_\_\(mode\) is used to identify the execution scheduling mode of the kernel function. As shown in the following figure:

    -   mode = 0 : normal mode, which dispatches the kernel function to as many idle physical cores as possible. If the number of idle physical cores cannot meet the requirement of the current kernel function, the undispatched part waits until cores become idle before execution. In this case, OP1 and OP2 may overlap during execution.

    -   mode = 1 : batch mode, which checks before dispatching the core function (Kernel). If the number of idle physical cores cannot meet the requirement of the current core function (Kernel), it waits until the number of idle physical cores satisfies all physical cores required by the core function (Kernel), and then dispatches them simultaneously. The execution of OP1 and OP2 is split, and no overlap occurs.

    ![](../../figures/batchmode.png)

    In multi-stream concurrency scenarios where multiple operators execute in parallel, if the total number of cores to be executed exceeds the maximum number of physical cores and multiple operators use inter-core synchronization APIs such as SyncAll, it is recommended to set mode to 1 to prevent multiple operators from waiting for idle core scheduling and causing a deadlock. The default value of mode is 0.

    ```
     __schedmode__(1) __global__ __mix__(1, 2) void OP1() // OP1 uses the SyncAll API and may have concurrent multi-stream execution, so batch mode (mode 1) needs to be set.
    {
        AscendC::SyncAll();
        ....
    }
     __schedmode__(1) __global__ __mix__(1, 2) void OP2() // OP2 uses the SyncAll API and may have concurrent multi-stream execution, so batch mode (mode 1) needs to be set.
    {
        AscendC::SyncAll();
        ....
    }
    __schedmode__(0) __global__ __vector__ void OP3() {...} // OP3 does not use the SyncAll API, so it can be set to normal mode (mode 0) and the operator is executed according to normal rules.
    or
    __global__ __vector__ void OP3() {...} // If __schedmode__ is not set, normal mode is used by default.
    ```

-   \_\_inline\_\_

    The \_\_inline\_\_ qualifier declares a function, and it has the following attributes:

    -   It identifies a device-side function for forced inlining, which can reduce the overhead of instruction push and pop caused by frequent function calls, but may increase the size of the operator binary.
    -   The main difference from the C++ function modifier inline is that the device-side \_\_inline\_\_ forces inlining, whereas the C++ `inline` performs selective inlining based on compiler optimization.
    -   The AI Core has a limit on the function nesting depth, and a nesting depth of no more than 4 levels is generally recommended. Using forced inlining can reduce the call hierarchy.

## Function Parameter Annotations<a name="function-parameter-annotations"></a>

-   \_\_kfc\_workspace\_\_

    KFC (Kernel Function Call) indicates an inter-core communication call. In a mix operator that uses the Matmul advanced API, when the Vector core initiates communication with the Cube core, the KFC system workspace is required. `__kfc_workspace__` is used to qualify the workspace pointer parameter at the entry of a kernel function, so that the compiler can identify and initialize the KFC system workspace address. The usage example is as follows:

    ```cpp
    extern "C" __global__ __mix__(1, 2) void batch_matmul_custom(
        __gm__ uint8_t* a, __gm__ uint8_t* b, __gm__ uint8_t* c, __kfc_workspace__ __gm__ uint8_t* workspace,
        __gm__ uint8_t* tilingGm);
    ```

    -   The workspace parameter is qualified with `__kfc_workspace__`, indicating the system workspace allocated and passed in by the Host side. When the Kernel side registers a Matmul object, this workspace is passed to `REGIST_MATMUL_OBJ` through `GetSysWorkSpacePtr()`.
    -   `__kfc_workspace__` applies only to scenarios that use the Matmul advanced API, and to operators declared with `__mix__(1, 2)` or `__mix__(1, 1)`, that is, the ratio of Cube cores to Vector cores is 1:2 or 1:1.

## Address Space Qualifier<a name="section1624210295308"></a>

The AI Core has multiple levels of independent on-chip storage. Each address space is independently addressed and has its own memory access instructions. Depending on the architecture, some storage spaces have a unified address space (Generic Address Space), while others do not. Device-side programming uses syntax extensions to allow address spaces as valid type qualifiers, providing access capabilities for different address spaces and address space validity checks.

**Table 1** Address space mapping

| Address Space Qualifier | AI Core Physical Storage Space |
|----------------|---------------------|
| \_\_gm\_\_ | Device-side memory GM |
| \_\_ubuf\_\_ | Vector UB |
| \_\_ca\_\_ | Cube L0A Buffer |
| \_\_cb\_\_ | Cube L0B Buffer |
| \_\_cc\_\_ | Cube L0C Buffer |
| \_\_cbuf\_\_ | Cube L1 Buffer |
| \_\_fbuf\_\_ | Fixpipe Buffer |
| \_\_ssbuf\_\_ | SSBuffer |

Address space qualifiers can be used in variable declarations to specify the region where an object is allocated. If the type of an object is qualified by an address space name, the object is allocated in the specified address space. Similarly, for pointers, the pointed-to type can be qualified by an address space to indicate the address space where the pointed-to object resides.

```
// declares a pointer p in the __gm__ address space that
// points to an object(has int type) in the __gm__ address space
__gm__ int *p;

__global__ __aicore__ void foo(...)
{
  // declares an array of 4 floats in the private address space.
  float x[4];
}
```

Address space qualifiers cannot be used for non-pointer return types, non-pointer function parameters, or function types. Multiple address space qualifiers are not allowed on the same type.

```
// OK.
__aicore__ int f() {...}

// Error. Address space qualifier cannot be used with a non-pointer return type.
__ubuf__ int f() { ... }

// OK. Address space qualifier can be used with a pointer return type.
__ubuf__ int *f() { ... }

// Error. Multiple address spaces specified for a type.
__ubuf__ __gm__ int i;

// OK. The first address space qualifies the object pointed to and the second
// qualifies the pointer.
__ubuf__ int * __gm__ ptr;
```

>[!NOTE] Note
>**Important**: The sizes of pointers in different address spaces may differ. For example, you cannot assume that sizeof(\_\_gm\_\_ int \*) is always equal to sizeof(\_\_ubuf\_\_ int \*). For instance, the compiler may store \_\_ubuf\_\_ pointers as 32-bit on some systems.

-   **Private address space**

    The private address space is the default address space for most variables, especially local variables.

    ```
    // m is in a specific kernel parameter address space,
    // it's physical location is implementation determined.
    __global__ __vector__ void foo(int m) {
      // OK. i is an int variable allocated in private address space
      int i;
    }

    __aicore__ void bar(int k) { //OK. k is in private address space
      // OK. i is an int variable allocated in private address space
      int i;
    }
    ```

-   **\_\_gm\_\_ address space**

    The \_\_gm\_\_ address space qualifier is used to indicate objects allocated in the device-side global memory. Global memory objects can be declared as pointers to scalars or user-defined structures.

    ```
    __gm__ int *var; // var point to an array of int elements

    typedef struct {
        float a[3];
        int b[2];
    } foo_t;

    __gm__ foo_t *info; // info point to an array of foo_t elements
    ```

-   **\_\_ubuf\_\_ address space**

    The \_\_ubuf\_\_ address space is used to describe variables stored in the UB storage space on the AI Core.

    ```
    __global__ __aicore__ void foo() {
      // ptr is in private address space, point to __ubuf__
      __ubuf__ int *ptr;
    }
    ```

-   **\_\_ca\_\_, \_\_cb\_\_, \_\_cc\_\_, \_\_cbuf\_\_ address spaces**

    The preceding address spaces are mainly used for access by specific DMA instructions and do not support direct scalar access.

    ```
    class ObjTy{
      ObjTy(){...}
      void print(){...}

    private:
      int a;
      int b;
    };

    __global__ __aicore__
    void foo(__ca__ int * ptr) { // Error. Cannot have __ca__
                                 // qualifier in kernel arguments
      // OK
      __ca__ int *ptr;
    }
    ```

## Built-in Constants<a name="section784531219338"></a>

**Table 2** Built-in constants

| Constant Name | Value | Function |
|--------|------|------|
| constexpr int32_t g_coreType | AscendC::AIC<br>AscendC::AIV | The constant value is automatically set by the framework. On an AIC core, it is configured as AscendC::AIC; on an AIV core, it is configured as AscendC::AIV. By checking this constant value, you can distinguish and isolate AIV and AIC core code. Its function is equivalent to directly using ASCEND_IS_AIV and ASCEND_IS_AIC. |
| constexpr uint64_t ASC_UB_SIZE | The value is determined by the current AI processor. If the AI processor does not have this space, the default value is 0. | Indicates the UB capacity under the current AI processor architecture, and can be used to obtain the UB resource size at compile time. |
| constexpr uint64_t ASC_L1_SIZE | The value is determined by the current AI processor. If the AI processor does not have this space, the default value is 0. | Indicates the L1 Buffer capacity under the current AI processor architecture, and can be used to obtain the L1 Buffer size at compile time. |
| constexpr uint64_t ASC_L0A_SIZE | The value is determined by the current AI processor. If the AI processor does not have this space, the default value is 0. | Indicates the L0A Buffer capacity under the current AI processor architecture, and can be used to obtain the L0A Buffer size at compile time. |
| constexpr uint64_t ASC_L0B_SIZE | The value is determined by the current AI processor. If the AI processor does not have this space, the default value is 0. | Indicates the L0B Buffer capacity under the current AI processor architecture, and can be used to obtain the L0B Buffer size at compile time. |
| constexpr uint64_t ASC_L0C_SIZE | The value is determined by the current AI processor. If the AI processor does not have this space, the default value is 0. | Indicates the L0C Buffer capacity under the current AI processor architecture, and can be used to obtain the L0C resource size at compile time. |
| constexpr uint64_t ASC_BT_SIZE | The value is determined by the current AI processor. If the AI processor does not have this space, the default value is 0. | Indicates the BiasTable Buffer capacity under the current AI processor architecture, and can be used to obtain the BiasTable resource size at compile time. |

## Built-in Variables<a name="section199434523343"></a>

Built-in variables are automatically set by the framework and can be directly referenced in Device-side code for multi-core logic control and data slicing. In a Mix execution scenario (in this section, this refers to a kernel function that uses the `__mix__(1, 1)` or `__mix__(1, 2)` function execution space qualifier), they must also be used in combination with some non-built-in variables. The description and acquisition method of each variable are as follows:

**Table 3** Built-in variables

| Variable | Description | Acquisition Method |
|------|------|---------|
| block_num | Number of cores configured for the current task. | Built-in variable. |
| block_idx | Index of the current combination. For the Pure Cube scenario, a combination contains one Cube Core. For the Pure Vector scenario, a combination contains one Vector Core. For the Mix (1,1) scenario, a combination contains one Cube Core and one Vector Core. For the Mix (1,2) scenario, a combination contains one Cube Core and two Vector Cores. | Built-in variable. |
| sub_block_num | Number of cores of the current type in the combination. | Non-built-in variable, obtained through [GetSubBlockNum](../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSubBlockNum_ISASI.md) or [asc_get_sub_block_num](../../../api/SIMD-API/c_api/utils/sys_var/asc_get_sub_block_num.md). |
| sub_block_idx | Position of the current core within the combination. | Non-built-in variable, obtained through [GetSubBlockIdx](../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetSubBlockIdx_ISASI.md) or [asc_get_sub_block_id](../../../api/SIMD-API/c_api/utils/sys_var/asc_get_sub_block_id.md). |



In multi-core splitting scenarios, you need to obtain the real logical position (logic_idx) of each core. The values of the above variables differ across execution scenarios, and the corresponding logic_idx calculation methods also differ. In the Mix scenario, a combination consists of one Cube Core (AIC) and several Vector Cores (AIV). All AIVs within a combination share the same block_idx. Therefore, when a combination contains multiple AIV cores, they cannot be located by block_idx alone; sub_block_idx must also be used. The calculation formula is as follows:

$$
\text{logic\_idx} = \text{block\_idx} \times \text{sub\_block\_num} + \text{sub\_block\_idx}
$$


The following table uses $\text{block\_num} = 4$ as an example to describe the values of each variable and the logic_idx calculation method in different scenarios (the basic API [GetBlockIdx](../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md) can be used to directly obtain logic_idx):

**Table 4** Values of each variable and the logic_idx calculation method in different scenarios

| Scenario | Execution Core | block_idx | sub_block_num | sub_block_idx | logic_idx 
|------|--------|-----------|---------------|---------------|-----------|
| Pure Cube | AIC | 0, 1, 2, 3 | 1 | 0 | Equal to block_idx | 
| Pure Vector | AIV | 0, 1, 2, 3 | 1 | 0 | Equal to block_idx | 
| Mix (1,1) | AIC, AIV | 0, 1, 2, 3 | 1 | 0 | Equal to block_idx
| Mix (1,2) | AIC | 0, 1, 2, 3 | 1 | 0 | Equal to block_idx |
| Mix (1,2) | AIV | 0, 0, 1, 1, 2, 2, 3, 3 (same value within the combination) | 2 | 0 or 1 | 0, 1, 2, 3, 4, 5, 6, 7 |

**Usage recommendations**

-   When using the **basic API**, use GetBlockIdx to obtain the logical position of the core instead of directly using the built-in variable.

    <!-- npu="310p" id7 -->
    In Atlas inference products, when KERNEL_TYPE_MIX_VECTOR_CORE is enabled, the operator runs on both AI Core and Vector Core. In this case, block_idx starts counting from 0 on both types of cores, so users cannot directly use block_idx to split data and control multi-core logic. However, GetBlockIdx adds an offset (the block_num of AI Core) to block_idx on Vector Core, ensuring that the returned value correctly reflects the actual logic in the multi-core environment.
    <!-- end id7 -->

-   When using the **C API**, for cube-only/vector-only/Mix (1,1) scenarios, use the built-in variable block_idx to obtain the logical position of the current core; for Mix (1,2) scenarios, use the built-in variables block_idx, asc_get_sub_block_num, and asc_get_sub_block_id to calculate logic_idx based on the formula.

## Built-in Data Types

**Table 5** Built-in data types

| Type | Data Type | Description | Size (Bit) | Value Range |
| --- | --- | --- | --- | --- |
| Integer | int4b_t | Signed 4-bit integer. | 8 | [-8, 7] |
| Boolean | bool | All zeros represent false; otherwise, true. | 8 | true, false |
| Integer | int4x2_t | Two signed 4-bit integers packed into one 8-bit storage unit. | 8 | The value range of each element is [-8, 7]. |
| Integer | int8_t | signed char | 8 | [-128, 127] |
| Integer | uint8_t | unsigned char | 8 | [0, 255] |
| Floating-point | fp4x2_e2m1_t | Two floating-point numbers, each consisting of a 1-bit sign, a 2-bit exponent, and a 1-bit mantissa, packed into one 8-bit storage unit. | 8 | [-6, 6] |
| Floating-point | fp4x2_e1m2_t | Two floating-point numbers, each consisting of a 1-bit sign, a 1-bit exponent, and a 2-bit mantissa, packed into one 8-bit storage unit. | 8 | [-7×2<sup>-2</sup>, 7×2<sup>-2</sup>] |
| Floating-point | hifloat8_t | Sign bit width 1, exponent bit width and mantissa bit width determined by point field encoding. | 8 | The point field encoding determines the data precision and value range. |
| Floating-point | fp8_e8m0_t | Sign bit width 0, exponent bit width 8, mantissa bit width 0. | 8 | [2<sup>-127</sup>, 2<sup>127</sup>] |
| Floating-point | fp8_e5m2_t | Sign bit width 1, exponent bit width 5, mantissa bit width 2. | 8 | [2<sup>13</sup> - 2<sup>16</sup>, 2<sup>16</sup> - 2<sup>13</sup>] |
| Floating-point | fp8_e4m3fn_t | Sign bit width 1, exponent bit width 4, mantissa bit width 3. | 8 | [2<sup>6</sup> - 2<sup>9</sup>, 2<sup>9</sup> - 2<sup>6</sup>] |
| Integer | int16_t | signed short | 16 | [-32768, 32767] |
| Integer | uint16_t | unsigned short | 16 | [0, 65535] |
| Floating-point | half | Sign bit width 1, exponent bit width 5, mantissa bit width 10. | 16 | [2<sup>5</sup> - 2<sup>16</sup>, 2<sup>16</sup> - 2<sup>5</sup>] |
| Floating-point | bfloat16_t | Sign bit width 1, exponent bit width 8, mantissa bit width 7. | 16 | [2<sup>120</sup> - 2<sup>128</sup>, 2<sup>128</sup> - 2<sup>120</sup>] |
| Integer | int32_t | signed int | 32 | [-2147483648, 2147483647] |
| Integer | uint32_t | unsigned int | 32 | [0, 4294967295] |
| Floating-point | float | Sign bit width 1, exponent bit width 8, mantissa bit width 23. | 32 | [2<sup>104</sup> - 2<sup>128</sup>, 2<sup>128</sup> - 2<sup>104</sup>] |
| Complex | complex32 | A complex number whose real and imaginary parts are both of the half type. | 32 | Real part: [2<sup>5</sup> - 2<sup>16</sup>, 2<sup>16</sup> - 2<sup>5</sup>], imaginary part: [2<sup>5</sup> - 2<sup>16</sup>, 2<sup>16</sup> - 2<sup>5</sup>] |
| Integer | int64_t | signed long | 64 | [-9223372036854775808, 9223372036854775807] |
| Integer | uint64_t | unsigned long | 64 | [0, 18446744073709551615] |
| Floating-point | double | Sign bit width 1, exponent bit width 11, mantissa bit width 52. | 64 | [2<sup>971</sup> - 2<sup>1024</sup>, 2<sup>1024</sup> - 2<sup>971</sup>] |
| Complex | complex64 | A complex number whose real and imaginary parts are both of the float type. | 64 | Real part: [2<sup>104</sup> - 2<sup>128</sup>, 2<sup>128</sup> - 2<sup>104</sup>], imaginary part: [2<sup>104</sup> - 2<sup>128</sup>, 2<sup>128</sup> - 2<sup>104</sup>] |

For more details, see [Built-in Data Types](../../../api/SIMD-API/basic_api/data_structures/builtin_data_types.md).

## Kernel Configuration<a name="kernel-configuration"></a><a name="section97005415463"></a>

When calling a function modified by the __global__ qualifier, the execution configuration must be specified. The execution configuration is specified by inserting an expression in the following form between the function name and the parenthesized argument list:

```
<<<numBlocks, dynUBufSize, stream>>>
```

Where:
-   numBlocks: specifies on how many cores the kernel will be executed. Each core that executes the kernel is assigned a logical ID, namely block\_idx, which can be obtained in the kernel implementation using the built-in variable [block_idx](../language_extension/simd_builtin_keywords.md#built-in-variables);

    >[!NOTE] Note 
    >numBlocks is a logical core concept, and its value range is \[1,65535\]. To fully utilize hardware resources, it is generally set to the number of physical cores or a multiple thereof.
    >- For coupled mode and decoupled mode, the meaning and setting rules of numBlocks at runtime differ as follows:
    >    - Coupled mode: Because the Vector and Cube units are integrated, numBlocks is used to set how many AI Core instances to start for execution, without distinguishing between Vector and Cube. The number of AI Core cores can be obtained through [GetCoreNumAiv](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAiv.md) or [GetCoreNumAic](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAic.md).
    >    - Decoupled mode
    >        - For operators that involve only Vector computation, numBlocks is used to set how many Vector (AIV) instances to start for execution. For example, a certain model of AI processor has 40 Vector cores, and the recommended setting is 40.
    >        - For operators that involve only Cube computation, numBlocks is used to set how many Cube (AIC) instances to start for execution. For example, a certain model of AI processor has 20 Cube cores, and the recommended setting is 20.
    >        - For operators that involve fused Vector/Cube computation, AIV and AIC are started in combination. numBlocks is used to set how many combinations to start for execution. For example, a certain model of AI processor has 40 Vector cores and 20 Cube cores, and one combination consists of 2 Vector cores and 1 Cube core. The recommended setting is 20, in which case 20 combinations are started, that is, 40 Vector cores and 20 Cube cores. **Note: In this scenario, the number of logical cores set for numBlocks must not exceed the number of physical cores (a combination of 2 Vector cores and 1 Cube core counts as 1 physical core).**
    >        - The number of AIC/AIV cores can be obtained through the [GetCoreNumAic](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAic.md) and [GetCoreNumAiv](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAiv.md) APIs, respectively.
    >- If the developer uses the Device resource limiting feature, the numBlocks set for the operator must not exceed the number of cores returned by the APIs provided by [PlatformAscendC](../../../api/Utils-API/platform_info/PlatformAscendC/PlatformAscendC.md) for obtaining the number of cores (GetCoreNum/GetCoreNumAic/GetCoreNumAiv, etc.). For example, if aclrtSetStreamResLimit is used to set the number of Vector cores at the Stream level to 8, then the GetCoreNumAiv API returns 8, and the numBlocks set for a Vector operator must not exceed 8. Otherwise, resources of other Streams will be preempted, causing resource limiting to become ineffective.

-   dynUBufSize: Dynamic UB Size, which configures the size of the space for UB dynamic memory allocation (UB only, excluding L1 and others), in bytes. The default value is 0;
-   stream: of the aclrtStream type. stream is used to maintain the execution order of some asynchronous operations, ensuring that they are executed on the device in the order in which they are called in the application code. The default value is nullptr. For stream creation and other management APIs, see [Runtime API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/README.md).

The following example shows how to declare and call a kernel.

```
// Declare
__global__ __vector__ void add_custom(float* x, float* y, float* z);
// Call
add_custom<<<numBlocks, dynUBufSize, stream>>>(x, y, z);
```
