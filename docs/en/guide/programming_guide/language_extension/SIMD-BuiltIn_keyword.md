# SIMD Built-In Keywords<a name="ZH-CN_TOPIC_0000002509863893"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T12:25:52.367Z pushedAt=2026-09-06T03:53:34.411Z -->

## Predefined Macros<a name="section2035104120360"></a>

As with other languages, some built-in macros are provided to facilitate program development. This section on predefined macros focuses on the macros frequently used in heterogeneous programming and explains their meanings.

-   **\_\_NPU\_ARCH\_\_**

    \_\_NPU\_ARCH\_\_ is a preprocessing macro in the device-side AI Core code, used to identify the architecture version of the AI processor. This macro consists of four digits, where the first three digits identify the IP core (Intellectual Property Core) type of the AI Core, and the fourth digit identifies the configuration version of the same IP core of the AI Core. With this macro, developers can adapt and optimize code differently for different AI processors. The mapping between product models and NPU architecture versions is shown in the following table:

    **Table 1** Mapping between product models and NPU architecture versions

    <a name="table65291052154114"></a>
    <table><thead align="left"><tr id="row1852915527411"><th class="cellrowborder" valign="top" width="49.59%" id="mcps1.2.3.1.1"><p id="p12529952134112"><a name="p12529952134112"></a><a name="p12529952134112"></a><span id="ph5758231184918"><a name="ph5758231184918"></a><a name="ph5758231184918"></a>AI Processor</span> Model</p>
    </th>
    <th class="cellrowborder" valign="top" width="50.41%" id="mcps1.2.3.1.2"><p id="p252955211411"><a name="p252955211411"></a><a name="p252955211411"></a>__NPU_ARCH__</p>
    </th>
    </tr>
    </thead>
    <tbody><tr id="row1986372151716"><td class="cellrowborder" valign="top" width="49.59%" headers="mcps1.2.3.1.1 "><p id="p2864142161711"><a name="p2864142161711"></a><a name="p2864142161711"></a><span id="ph2272194216543"><a name="ph2272194216543"></a><a name="ph2272194216543"></a>Ascend 950PR/Ascend 950DT</span></p>
    </td>
    <td class="cellrowborder" valign="top" width="50.41%" headers="mcps1.2.3.1.2 "><p id="p5880145213281"><a name="p5880145213281"></a><a name="p5880145213281"></a>3510</p>
    </td>
    </tr>
    <tr id="row75292521412"><td class="cellrowborder" valign="top" width="49.59%" headers="mcps1.2.3.1.1 "><p id="p052935264116"><a name="p052935264116"></a><a name="p052935264116"></a><span id="ph85291852104120"><a name="ph85291852104120"></a><a name="ph85291852104120"></a><term id="zh-cn_topic_0000001312391781_term1253731311225"><a name="zh-cn_topic_0000001312391781_term1253731311225"></a><a name="zh-cn_topic_0000001312391781_term1253731311225"></a>Atlas A3 training products</term>/<term id="zh-cn_topic_0000001312391781_term131434243115"><a name="zh-cn_topic_0000001312391781_term131434243115"></a><a name="zh-cn_topic_0000001312391781_term131434243115"></a>Atlas A3 inference products</term></span></p>
    </td>
    <td class="cellrowborder" valign="top" width="50.41%" headers="mcps1.2.3.1.2 "><p id="p11529165254118"><a name="p11529165254118"></a><a name="p11529165254118"></a>2201</p>
    </td>
    </tr>
    <tr id="row1652910526411"><td class="cellrowborder" valign="top" width="49.59%" headers="mcps1.2.3.1.1 "><p id="p45291152194112"><a name="p45291152194112"></a><a name="p45291152194112"></a><span id="ph25291652164115"><a name="ph25291652164115"></a><a name="ph25291652164115"></a><term id="zh-cn_topic_0000001312391781_term11962195213215"><a name="zh-cn_topic_0000001312391781_term11962195213215"></a><a name="zh-cn_topic_0000001312391781_term11962195213215"></a>Atlas A2 training products</term>/<term id="zh-cn_topic_0000001312391781_term184716139811"><a name="zh-cn_topic_0000001312391781_term184716139811"></a><a name="zh-cn_topic_0000001312391781_term184716139811"></a>Atlas A2 inference products</term></span></p>
    </td>
    <td class="cellrowborder" valign="top" width="50.41%" headers="mcps1.2.3.1.2 "><p id="p65291452164116"><a name="p65291452164116"></a><a name="p65291452164116"></a>2201</p>
    </td>
    </tr>
    <tr id="row155291352204114"><td class="cellrowborder" valign="top" width="49.59%" headers="mcps1.2.3.1.1 "><p id="p115291852144111"><a name="p115291852144111"></a><a name="p115291852144111"></a><span id="ph11529115214412"><a name="ph11529115214412"></a><a name="ph11529115214412"></a><term id="zh-cn_topic_0000001312391781_term354143892110"><a name="zh-cn_topic_0000001312391781_term354143892110"></a><a name="zh-cn_topic_0000001312391781_term354143892110"></a>Atlas 200I/500 A2 inference products</term></span></p>
    </td>
    <td class="cellrowborder" valign="top" width="50.41%" headers="mcps1.2.3.1.2 "><p id="p17530185224119"><a name="p17530185224119"></a><a name="p17530185224119"></a>3002</p>
    </td>
    </tr>
    <tr id="row1353025204112"><td class="cellrowborder" valign="top" width="49.59%" headers="mcps1.2.3.1.1 "><p id="p153075220411"><a name="p153075220411"></a><a name="p153075220411"></a><span id="ph1653025204119"><a name="ph1653025204119"></a><a name="ph1653025204119"></a><term id="zh-cn_topic_0000001312391781_term4363218112215"><a name="zh-cn_topic_0000001312391781_term4363218112215"></a><a name="zh-cn_topic_0000001312391781_term4363218112215"></a>Atlas inference products</term></span></p>
    </td>
    <td class="cellrowborder" valign="top" width="50.41%" headers="mcps1.2.3.1.2 "><p id="p85307521410"><a name="p85307521410"></a><a name="p85307521410"></a>2002</p>
    </td>
    </tr>
    <tr id="row653005218416"><td class="cellrowborder" valign="top" width="49.59%" headers="mcps1.2.3.1.1 "><p id="p053055274116"><a name="p053055274116"></a><a name="p053055274116"></a><span id="ph7530652174110"><a name="ph7530652174110"></a><a name="ph7530652174110"></a><term id="zh-cn_topic_0000001312391781_term71949488213"><a name="zh-cn_topic_0000001312391781_term71949488213"></a><a name="zh-cn_topic_0000001312391781_term71949488213"></a>Atlas training products</term></span></p>
    </td>
    <td class="cellrowborder" valign="top" width="50.41%" headers="mcps1.2.3.1.2 "><p id="p155301052144110"><a name="p155301052144110"></a><a name="p155301052144110"></a>1001</p>
    </td>
    </tr>
    </tbody>
    </table>

    The following is an example of using \_\_NPU\_ARCH\_\_ to control the rounding mode of operator output values on different AI processors.

    ```
    __aicore__ static inline void CopyOut(uint64_t mulLen)
    {
    #if __NPU_ARCH__ == 2002
        Cast(dstLocal, srcLocal, RoundMode::CAST_NONE, mulLen); // CAST_NONE indicates that the rounding mode uses the CAST_RINT mode when the conversion involves precision loss, and does not perform rounding when no precision loss is involved
    #elif __NPU_ARCH__ == 2201
        Cast(dstLocal, srcLocal, RoundMode::CAST_RINT, mulLen); // CAST_RINT indicates that the rounding mode is round half to even
    #endif
        event_t eventVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventVToMTE3);
        WaitFlag<HardEvent::V_MTE3>(eventVToMTE3);
        CommonCopyOut<float>(dstLocal, mulLen);  // Copy LocalTensor to GlobalTensor
    }
    ```

-   **ASCEND\_IS\_AIV, ASCEND\_IS\_AIC**<a name="li19530175294118"></a>

    **ASCEND\_IS\_AIV** and **ASCEND\_IS\_AIC** are conditional judgment statements implemented through C++ macros, used to implement conditional compilation of code in functions modified by **\_\_aicore\_\_**. When developing a fusion operator based on the separated mode (AIC core and AIV core separated), the operator logic involves the processing logic of both the AIV core and the AIC core, and inter-core synchronization is required. In this case, **ASCEND\_IS\_AIV**/**ASCEND\_IS\_AIC** must be used to isolate the AIV and AIC core code.

    >[!NOTE] Note
    >When the high-order API Matmul is used, it has already implemented the isolation of AIV and AIC core code through the **REGIST\_MATMUL\_OBJ** macro, so users do not need to use this macro for processing.

    Taking the MatmulNzCustom operator as an example, in the separated mode this operator needs to implement different logic on the AIV core and the AIC core respectively. Specifically, the AIV core is responsible for moving cube data into the Unified Buffer, completing the data rearrangement (converting the cube data to NZ format), and writing it to Global Memory. The AIC core directly reads the rearranged NZ format data from Global Memory and performs cube multiplication (Matmul) computation. Since the code logic of the AIV core and the AIC core is different, **ASCEND\_IS\_AIV** and **ASCEND\_IS\_AIC** macros must be used to isolate the code, ensuring that code applicable to the AIV core and the AIC core is generated separately at compile time.

    The example pseudocode is as follows:

    ```
    template <typename AType, typename BType, typename CType, typename BiasType>
    __aicore__ inline void MatmulKernel<AType, BType, CType, BiasType>::Process(AscendC::TPipe *pipe)
    {
        // Use the Vector compute unit of the AIV core to implement ND2NZ format conversion. In the following code, MatrixBtoNZ is the function that converts matrix B to ND2NZ format.
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
            AscendC::CrossCoreWaitFlag(0x4);   // Wait for the AIV core to complete the ND2NZ format conversion
        }
        ... ...
        // Set the left cube A, right cube B, and Bias.
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

    When developing non-fused operators based on the separate mode, in operator scenarios that involve only cube computation, you can set **ASCENDC\_CUBE\_ONLY** to enable the cube-only mode for Matmul computation, reducing the performance overhead of message communication and improving operator performance.

    >[!CAUTION] Caution
    >The **ASCENDC\_CUBE\_ONLY** macro must be set before **\#include "lib/matmul\_intf.h"**.

    Taking the matmul_custom operator as an example, the high-level API Matmul uses the MIX mode by default, that is, the user initiates a message from the AIV side, and after the message is relayed through the message communication framework, the Matmul computation is executed on the AIC side. This message processing mechanism introduces additional Scalar performance overhead. Compared with the MIX mode, the cube-only mode can directly bypass the message communication framework to complete the Matmul computation, thereby improving operator performance.

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

## Function Execution Space Qualifier<a name="section1074418132518"></a>

The function execution space qualifier indicates whether a function is executed on the host side or the device side, and whether it can be called from the host side or the device side.

-   **\_\_global\_\_**

    The \_\_global\_\_ execution space qualifier declares a kernel function. A kernel function has the following properties: it executes on the device; it can only be called by host-side functions; \_\_global\_\_ only indicates that this is the entrypoint of a device-side function and does not indicate the specific device type, which is marked by \_\_aicore\_\_. It has the following usage constraints:

    -   A \_\_global\_\_ function must return the void type and cannot be a member function of a class.
    -   A host-side call to a \_\_global\_\_ function must use the <<<\>\>\> heterogeneous call syntax.
    -   The call to \_\_global\_\_ is asynchronous, which means that the function return does not indicate that the kernel function has finished executing on the device side. If synchronization is required, use a Runtime synchronization API such as **aclrtSynchronizeStream** to synchronize explicitly.

-   **\_\_aicore\_\_**

    The \_\_aicore\_\_ execution space qualifier declares a function, and it has the following attributes:

    -   Executed on the device side
    -   Can only be called by a `__global__` function or another `__aicore__` function

    ```
    // Only callable from device functions with same kind
    // of execution space
    __aicore__ void bar() {}

    // Define a kernel function execute on AI Core device
    __global__ __aicore__ void foo() {
      bar(); // OK.
    }
    ```

-   **\_\_host\_\_**

    The \_\_host\_\_ execution space qualifier declares a function, and it has the following attributes:

    -   Executed only on the host side
    -   Can only be called by host-side functions
    -   Cannot be used together with \_\_global\_\_

    The \_\_host\_\_ qualifier is optional. A function defined without a function execution space qualifier is a host function by default.

    ```
    __aicore__ int f() {}

    // defines a host side function
    int foo() {}

    // defines a host side function
    __host__ int bar() {
      f();     // Error.
      foo();   // OK.
    }

    // Error.
    __global__ __host__ void kfunc() {}
    ```

-   **\_\_aicpu\_\_**

    The AI CPU function execution space qualifier \_\_aicpu\_\_ is used to indicate whether a function is an AI CPU kernel function, and it has the following attributes:

    -   Executed on the device side and can only be called by host-side functions, so it must be declared together with \_\_global\_\_
    -   A \_\_global\_\_ \_\_aicpu\_\_ function cannot have a void return type, and its input parameter can only be a pointer.
    -   A \_\_global\_\_ \_\_aicpu\_\_ function cannot be defined in an .asc file; it can only be declared, and must use extern.
    -   When the host side calls a \_\_global\_\_ \_\_aicpu\_\_ function, the <<<\>\>\> heterogeneous call syntax must be used. In addition to the input parameter pointer, the size of the data read from the pointer must also be passed in.
    -   The call to \_\_global\_\_ is asynchronous, which means that the function returning does not indicate that the kernel function has completed execution on the device side. If synchronization is required, use a Runtime synchronization API for explicit synchronization, such as the **aclrtSynchronizeStream** API.

    ```
    // Define a AI CPU kernel function in AI CPU device file
    __aicpu__ void foo() {} // Error, single __aicpu__ identifier without _global__
    __global__ void foo() {} // Error, single __global__ identifier without __aicpu__
    __global__ __aicpu__  void foo() {} // Error, return type is void
    __global__ __aicpu__  int foo(void *a) {} // OK
    __global__ __aicpu__  int foo(int a) {} // Error, input param is not pointer
    __global__ __aicpu__  int foo(void *a, void *b) {} // Error, input param num is not one
    ```

    ```
    // Declare a AI CPU kernel function in .asc file
    extern __global__ __aicpu__ uint32_t hello_world(void *args);// OK
    }
    ```

-   **\_\_inline\_\_**

    The \_\_inline\_\_ qualifier declares a function, and it has the following attributes:

    -   It identifies a device-side function for forced inlining, which can reduce the overhead of instruction push and pop caused by frequent function calls, but may increase the size of the operator binary.
    -   The main difference from the C++ function modifier inline is that the device-side \_\_inline\_\_ forces inlining, whereas the C++ `inline` performs selective inlining based on compiler optimization.
    -   The AI Core has a limit on the function nesting depth, and a nesting depth of no more than 4 levels is generally recommended. Using forced inlining can reduce the call hierarchy.

-   **\_\_cube\_\_**

    Identifies that the core function executes only on the cube core. For coupled mode hardware architecture, this modifier is not effective.

    ```
    extern "C" __global__ __cube__ void mmad_custom(GM_ADDR a, GM_ADDR b, GM_ADDR c)
    {
        KernelMmad op;
        op.Init(a, b, c);
        op.Process();
    }
    ```

-   **\_\_vector\_\_**

    Identifies that the core function executes only on the vector core. For coupled mode hardware architecture, this modifier is not effective.

    ```
     __vector__ __global__ __aicore__ void add_custom(){}
    ```

-   **\_\_mix\_\_\(cube, vec\)**

    Identifies that the core function executes on both the cube core and the vector core. (cube, vec) indicates the ratio of cube cores to vector cores started by the core function. The supported ratios are (1, 0), (0, 1), (1, 1), and (1, 2). For coupled mode hardware architecture, this modifier is not effective.

-   **\_\_schedmode\_\_\(mode\)**

    Identifies the execution scheduling mode of the core function, as shown in the following figure:

    -   mode = 0 : normal mode, which dispatches the core function to as many idle physical cores as possible. If the number of idle physical cores cannot meet the requirement of the current core function, the undispatched part waits until cores become idle before execution. In this case, OP1 and OP2 may overlap during execution.

    -   mode = 1 : batch mode, which checks before dispatching the core function. If the number of idle physical cores cannot meet the requirement of the current core function, it waits until the number of idle physical cores satisfies all physical cores required by the core function, and then dispatches them simultaneously. The execution of OP1 and OP2 is split, and no overlap occurs.

    ![](../../figures/batchmode_material.png)

In multi-stream concurrency scenarios where multiple operators execute in parallel, if the total number of cores to be executed exceeds the maximum number of physical cores and multiple operators use inter-core synchronization APIs such as SyncAll, it is recommended to set mode to 1 to prevent multiple operators from waiting for idle core scheduling and causing a deadlock. The default value of mode is 0.

    ```
     __schedmode__(1) __global__ __mix__(1, 2) void OP1() // OP1 uses the SyncAll API and may involve multi-stream concurrency, so batch mode (mode 1) needs to be set.
    {
        AscendC::SyncAll();
        ....
    }
     __schedmode__(1) __global__ __mix__(1, 2) void OP2() // OP2 uses the SyncAll API and may involve multi-stream concurrency, so batch mode (mode 1) needs to be set.
    {
        AscendC::SyncAll();
        ....
    }
    __schedmode__(0) __global__ __vector__ void OP3() {...} // OP3 does not use the SyncAll API, so it can be set to normal mode (mode 0) to execute the operator according to normal rules.
    or
    __global__ __vector__ void OP3() {...} // If __schedmode__ is not set, normal mode is used by default.
    ```

## Function Marker Macro<a name="section192521344610"></a>

-   \_\_simd\_vf\_\_

    Function marker macro used to mark a SIMD VF entrypoint function. The function has no return value. Use [asc\_vf\_call](../../../api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md) to call the SIMD VF entrypoint function and start the VF subtask.

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
                AscendC::Reg::StoreAlign(output + i * one_repeat_size, dstReg, maskReg);
            }
        }
        ```

-   \_\_simd\_callee\_\_

    Function marker macro. The function can have a return value and can be called by a SIMD VF entrypoint function or other non-entrypoint functions.

    ```
    __simd_callee__ inline float add(float x, float y)
    ```

## Address Space Qualifier<a name="section1624210295308"></a>

The AI Core has multiple levels of independent on-chip storage. Each address space is independently addressed and has its own memory access instructions. Depending on the architecture, some storage spaces have a unified address space (Generic Address Space), while others do not. Device-side programming uses syntax extensions to allow address spaces as valid type qualifiers, providing access capabilities for different address spaces and address space validity checks.

**Table 2** Address space mapping

<a name="table425403455214"></a>
<table><thead align="left"><tr id="row027120348524"><th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.1"><p id="p1227193465211"><a name="p1227193465211"></a><a name="p1227193465211"></a>Address Space Qualifier</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.2.3.1.2"><p id="p327111349525"><a name="p327111349525"></a><a name="p327111349525"></a>AI Core Physical Storage Space</p></th>
</tr>
</thead>
<tbody><tr id="row162721534115215"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p92721034185210"><a name="p92721034185210"></a><a name="p92721034185210"></a>__gm__</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p10272143417521"><a name="p10272143417521"></a><a name="p10272143417521"></a>Device-side memory GM</p></td>
</tr>
<tr id="row72726341527"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p4272634135214"><a name="p4272634135214"></a><a name="p4272634135214"></a>__ubuf__</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1427214347521"><a name="p1427214347521"></a><a name="p1427214347521"></a>Vector Unified Buffer</p></td>
</tr>
<tr id="row62721334195214"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p182722346528"><a name="p182722346528"></a><a name="p182722346528"></a>__ca__</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p102721034145219"><a name="p102721034145219"></a><a name="p102721034145219"></a>Cube L0A Buffer</p></td>
</tr>
<tr id="row182721834175211"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p14272434145216"><a name="p14272434145216"></a><a name="p14272434145216"></a>__cb__</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p327293455219"><a name="p327293455219"></a><a name="p327293455219"></a>Cube L0B Buffer</p></td>
</tr>
<tr id="row1027253413523"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p12272434195211"><a name="p12272434195211"></a><a name="p12272434195211"></a>__cc__</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p1927223411527"><a name="p1927223411527"></a><a name="p1927223411527"></a>Cube L0C Buffer</p></td>
</tr>
<tr id="row2272034135217"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p92721347529"><a name="p92721347529"></a><a name="p92721347529"></a>__cbuf__</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p727233495213"><a name="p727233495213"></a><a name="p727233495213"></a>Cube L1 Buffer</p></td>
</tr>
<tr id="row14137103317189"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p4798642111813"><a name="p4798642111813"></a><a name="p4798642111813"></a>__fbuf__</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p117981342201811"><a name="p117981342201811"></a><a name="p117981342201811"></a>Fixpipe Buffer</p></td>
</tr>
<tr id="row127473841817"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.1 "><p id="p14798942101818"><a name="p14798942101818"></a><a name="p14798942101818"></a>__ssbuf__</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.2.3.1.2 "><p id="p07981642171819"><a name="p07981642171819"></a><a name="p07981642171819"></a>SSBuffer</p></td>
</tr>
</tbody>
</table>

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

-   **private address space**

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

<a name="table878553753515"></a>
<table><thead align="left"><tr id="row578553718354"><th class="cellrowborder" valign="top" width="16.14838516148385%" id="mcps1.1.4.1.1"><p id="p1378573713352"><a name="p1378573713352"></a><a name="p1378573713352"></a>Constant Name</p></th>
<th class="cellrowborder" valign="top" width="25.847415258474154%" id="mcps1.1.4.1.2"><p id="p1378553763512"><a name="p1378553763512"></a><a name="p1378553763512"></a>Value</p></th>
<th class="cellrowborder" valign="top" width="58.00419958004199%" id="mcps1.1.4.1.3"><p id="p127858377356"><a name="p127858377356"></a><a name="p127858377356"></a>Function</p></th>
</tr>
</thead>
<tbody><tr id="row15785103718358"><td class="cellrowborder" valign="top" width="16.14838516148385%" headers="mcps1.1.4.1.1 "><p id="p1678553710357"><a name="p1678553710357"></a><a name="p1678553710357"></a>constexpr int32_t g_coreType</p></td>
<td class="cellrowborder" valign="top" width="25.847415258474154%" headers="mcps1.1.4.1.2 "><a name="ul107856379351"></a><a name="ul107856379351"></a><ul id="ul107856379351"><li>AscendC::AIC</li><li>AscendC::AIV</li></ul></td>
<td class="cellrowborder" valign="top" width="58.00419958004199%" headers="mcps1.1.4.1.3 "><p id="p778516378353"><a name="p778516378353"></a><a name="p778516378353"></a>The constant value is automatically set by the framework. On an AIC core, it is configured as AscendC::AIC; on an AIV core, it is configured as AscendC::AIV.</p>
<p id="p278616379351"><a name="p278616379351"></a><a name="p278616379351"></a>By checking this constant value, you can distinguish and isolate the AIV and AIC core code. Its function is equivalent to directly using ASCEND_IS_AIV and ASCEND_IS_AIC.</p></td>
</tr>
<tr><td class="cellrowborder" valign="top" width="16.14838516148385%" headers="mcps1.1.4.1.1 "><p>constexpr uint64_t ASC_UB_SIZE</p></td>
<td class="cellrowborder" valign="top" width="25.847415258474154%" headers="mcps1.1.4.1.2 "><p>The value is determined by the current AI processor. If the processor does not have this space, the default configuration is 0.</p></td>
<td class="cellrowborder" valign="top" width="58.00419958004199%" headers="mcps1.1.4.1.3 "><p>Indicates the capacity of the Unified Buffer (UB) under the current AI processor architecture, and can be used to obtain the UB resource size at compile time.</p></td>
</tr>
<tr><td class="cellrowborder" valign="top" width="16.14838516148385%" headers="mcps1.1.4.1.1 "><p>constexpr uint64_t ASC_L1_SIZE</p></td>
<td class="cellrowborder" valign="top" width="25.847415258474154%" headers="mcps1.1.4.1.2 "><p>The value is determined by the current AI processor. If the AI processor does not have this block of space, the default configuration is 0.</p></td>
<td class="cellrowborder" valign="top" width="58.00419958004199%" headers="mcps1.1.4.1.3 "><p>Indicates the capacity of the L1 Buffer under the current AI processor architecture, and can be used to obtain the L1 Buffer size at compile time.</p></td>
</tr>
<tr><td class="cellrowborder" valign="top" width="16.14838516148385%" headers="mcps1.1.4.1.1 "><p>constexpr uint64_t ASC_L0A_SIZE</p></td>
<td class="cellrowborder" valign="top" width="25.847415258474154%" headers="mcps1.1.4.1.2 "><p>The value is determined by the current AI processor. If the AI processor does not have this block of space, the default configuration is 0.</p></td>
<td class="cellrowborder" valign="top" width="58.00419958004199%" headers="mcps1.1.4.1.3 "><p>Indicates the capacity of the L0A Buffer under the current AI processor architecture, and can be used to obtain the L0A Buffer size at compile time.</p></td>
</tr>
<tr><td class="cellrowborder" valign="top" width="16.14838516148385%" headers="mcps1.1.4.1.1 "><p>constexpr uint64_t ASC_L0B_SIZE</p></td>
<td class="cellrowborder" valign="top" width="25.847415258474154%" headers="mcps1.1.4.1.2 "><p>The value is determined by the current AI processor. If the AI processor does not have this block of space, the default configuration is 0.</p></td>
<td class="cellrowborder" valign="top" width="58.00419958004199%" headers="mcps1.1.4.1.3 "><p>Indicates the capacity of the L0B Buffer under the current AI processor architecture, and can be used to obtain the L0B Buffer size at compile time.</p></td>
</tr>
<tr><td class="cellrowborder" valign="top" width="16.14838516148385%" headers="mcps1.1.4.1.1 "><p>constexpr uint64_t ASC_L0C_SIZE</p></td>
<td class="cellrowborder" valign="top" width="25.847415258474154%" headers="mcps1.1.4.1.2 "><p>The value is determined by the current AI processor. If the AI processor does not have this block of space, the default configuration is 0.</p></td>
<td class="cellrowborder" valign="top" width="58.00419958004199%" headers="mcps1.1.4.1.3 "><p>Indicates the capacity of the L0C Buffer under the current AI processor architecture. It can be used to obtain the L0C resource size at compile time.</p></td>
</tr>
<tr><td class="cellrowborder" valign="top" width="16.14838516148385%" headers="mcps1.1.4.1.1 "><p>constexpr uint64_t ASC_BT_SIZE</p></td>
<td class="cellrowborder" valign="top" width="25.847415258474154%" headers="mcps1.1.4.1.2 "><p>The value is determined by the current AI processor. If the AI processor does not have this block of space, the default configuration is 0.</p></td>
<td class="cellrowborder" valign="top" width="58.00419958004199%" headers="mcps1.1.4.1.3 "><p>Indicates the capacity of the BiasTable Buffer under the current AI processor architecture. It can be used to obtain the BiasTable resource size at compile time.</p></td>
</tr>
</tbody>
</table>

## Built-in Variables<a name="section199434523343"></a>

<a name="table169229153513"></a>
<table><thead align="left"><tr id="row769289183515"><th class="cellrowborder" valign="top" width="15.988401159884011%" id="mcps1.1.4.1.1"><p id="p1669269163510"><a name="p1669269163510"></a><a name="p1669269163510"></a>Variable Name</p></th>
<th class="cellrowborder" valign="top" width="25.977402259774017%" id="mcps1.1.4.1.2"><p id="p116921597358"><a name="p116921597358"></a><a name="p116921597358"></a>Related API</p></th>
<th class="cellrowborder" valign="top" width="58.03419658034197%" id="mcps1.1.4.1.3"><p id="p14692294358"><a name="p14692294358"></a><a name="p14692294358"></a>Function</p></th>
</tr>
</thead>
<tbody><tr id="row06921195350"><td class="cellrowborder" valign="top" width="15.988401159884011%" headers="mcps1.1.4.1.1 "><p id="p156921796359"><a name="p156921796359"></a><a name="p156921796359"></a>block_num</p></td>
<td class="cellrowborder" valign="top" width="25.977402259774017%" headers="mcps1.1.4.1.2 "><p id="p369213993514"><a name="p369213993514"></a><a name="p369213993514"></a>GetBlockNum</p></td>
<td class="cellrowborder" valign="top" width="58.03419658034197%" headers="mcps1.1.4.1.3 "><p id="p969217912357"><a name="p969217912357"></a><a name="p969217912357"></a>Number of cores configured for the current task, used for multi-core logic control within the code.<br>The value of the built-in variable block_num is consistent with the value obtained through the related API GetBlockNum.</p></td>
</tr>
<tr id="row1669214993515"><td class="cellrowborder" valign="top" width="15.988401159884011%" headers="mcps1.1.4.1.1 "><p id="p1769219913351"><a name="p1769219913351"></a><a name="p1769219913351"></a>block_idx</p></td>
<td class="cellrowborder" valign="top" width="25.977402259774017%" headers="mcps1.1.4.1.2 "><p id="p116921990355"><a name="p116921990355"></a><a name="p116921990355"></a>GetBlockIdx</p></td>
<td class="cellrowborder" valign="top" width="58.03419658034197%" headers="mcps1.1.4.1.3 "><p id="p169211933512"><a name="p169211933512"></a><a name="p169211933512"></a>Index of the current core, used for multi-core logic control and multi-core offset calculation within the code.<br>Note that the value of the built-in variable block_idx is not consistent with the value obtained through the related API GetBlockIdx in some scenarios. For details and scenario examples, see the following description.</p></td>
</tr>
</tbody>
</table>
Generally, you are advised to use the APIs corresponding to the built-in variables to obtain the required values, rather than directly using the built-in variables. This is because a built-in variable reflects the configuration information of a single hardware resource. For the software stack that integrates hardware resources and extends hardware functions, the value of a built-in variable may not match its actual semantics.

In the Mix scenario (using the `__mix__` function execution space qualifier), the logical position of a Vector core (AIV) must be determined by block_idx and sub_block_idx together: `logic_idx = block_idx * sub_block_num + sub_block_idx`. Here, block_idx identifies the position of the current combination in the entire grid, and sub_block_idx identifies the position of the current AIV within the combination. Developers should use the GetBlockIdx and GetSubBlockIdx APIs together to obtain the complete logical position information, rather than directly using the built-in variables.

**Example:**

For example, in a scenario where the number of cores configured for the task is 3 (block_num = 3), when `__mix__(1, 2)` is used as the function execution space qualifier, the task starts 3 Cube Cores and 6 Vector Cores. The values of the built-in variable block_idx on the 6 Vector Cores are 0, 0, 1, 1, 2, 2 respectively, that is, the two Vector Cores on the same core have the same value, while the values obtained through the GetBlockIdx API are 0, 1, 2, 3, 4, 5 respectively, which are different. On the 3 Cube Cores, the values of both are the same, namely 0, 1, 2.

For example, in Atlas inference products, when KERNEL_TYPE_MIX_VECTOR_CORE is enabled, the operator runs on both AI Core and Vector Core. In this case, block_idx starts counting from 0 on both types of cores, so users cannot directly use block_idx to split data and control multi-core logic. However, GetBlockIdx adds an offset (the block_num of AI Core) to block_idx on Vector Core, ensuring that the returned value correctly reflects the actual logic in the multi-core environment.

## Kernel Function Configuration<a name="section97005415463"></a>

When calling a function modified by the __global__ qualifier, you must specify the execution configuration. The execution configuration is specified by inserting an expression of the following form between the function name and the parenthesized parameter list:

```
<<<numBlocks, dynUBufSize, stream>>>
```

Where:
-   **numBlocks**: specifies on how many cores the kernel function will execute. Each core that executes the kernel function is assigned a logical ID, that is, **block_idx**, which can be obtained in the kernel function implementation using the built-in variable [block_idx](../language_extension/SIMD-BuiltIn_keyword.md#built-in-variables);

    >[!NOTE] Note
    >**numBlocks** is a logical core concept, with a value range of \[1,65535\]. To fully utilize hardware resources, it is generally set to the number of physical cores or a multiple thereof.
    >- For coupled mode and decoupled mode, the meaning and setting rules of **numBlocks** at runtime differ slightly, as described below:
    >    - Coupled mode: Because its Vector and Cube units are integrated together, **numBlocks** is used to set the number of AI Core instances to start for execution, without distinguishing between Vector and Cube. The number of AI Cores can be obtained through [GetCoreNumAiv](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAiv.md) or [GetCoreNumAic](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAic.md).
    >    - Decoupled mode
    >        - For operators that contain only Vector computation, **numBlocks** is used to set how many Vector (AIV) instances to start for execution. For example, if a certain AI processor has 40 Vector cores, the recommended setting is 40.
    >        - For operators that contain only Cube computation, **numBlocks** is used to set how many Cube (AIC) instances to start for execution. For example, if a certain AI processor has 20 Cube cores, the recommended setting is 20.
    >        - For operators with fused Vector/Cube computation, AIV and AIC are started in combination at startup. **numBlocks** is used to set how many combinations to start for execution. For example, if a certain AI processor has 40 Vector cores and 20 Cube cores, and one combination consists of 2 Vector cores and 1 Cube core, the recommended setting is 20. In this case, 20 combinations are started, that is, 40 Vector cores and 20 Cube cores. **Note: In this scenario, the number of logical cores set for **numBlocks** cannot exceed the number of physical cores (2 Vector cores and 1 Cube core combined form 1 physical core).**
    >        - The numbers of AIC and AIV cores are obtained through the [GetCoreNumAic](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAic.md) and [GetCoreNumAiv](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAiv.md) APIs, respectively.
    >- If the developer uses the device resource limiting feature, the **numBlocks** set for the operator must not exceed the number of cores returned by the APIs provided by [PlatformAscendC](../../../api/Utils-API/platform_info/PlatformAscendC/PlatformAscendC.md) for obtaining the number of cores (GetCoreNum/GetCoreNumAic/GetCoreNumAiv, etc.). For example, if **aclrtSetStreamResLimit** is used to set the number of Vector cores at the stream level to 8, then the **GetCoreNumAiv** API returns 8, and the **numBlocks** set for a Vector operator must not exceed 8. Otherwise, resources of other streams will be preempted, causing the resource limit to become ineffective.

-   **dynUBufSize**: Dynamic Unified Buffer Size, which configures the size of the space for UB dynamic memory allocation (UB only, excluding L1 and others), in bytes. The default value is **0**.
-   **stream**: The type is **aclrtStream**. A stream is used to maintain the execution order of asynchronous operations, ensuring that they are executed on the device in the order in which they are called in the application code. The default value is **nullptr**. For stream creation and other management APIs, see [*Runtime APIs*](https://hiascend.com/document/redirect/CannCommunityRuntimeApi).

The following example shows how to declare and call a kernel function.

```
// Declaration
__global__ __vector__ void add_custom(float* x, float* y, float* z);
// Call
add_custom<<<numBlocks, dynUBufSize, stream>>>(x, y, z);
```
