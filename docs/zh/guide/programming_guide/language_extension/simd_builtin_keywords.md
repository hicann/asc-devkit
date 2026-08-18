# SIMD BuiltIn关键字<a name="ZH-CN_TOPIC_0000002509863893"></a>

## 预定义宏<a name="section2035104120360"></a>

如其他语言一样，会提供一些内置的宏方便用户编写程序。预定义宏一节着重介绍一些用户在做异构编程时会经常用到的宏，以及宏的解释。

-   <a id="npu-arch"></a>**\_\_NPU\_ARCH\_\_**

    \_\_NPU\_ARCH\_\_是Device侧AI Core代码中的预处理宏，用于标识AI处理器的架构版本。该宏由四位数字组成，其中前三位数字用于标识AI Core的IP核\(Intellectual Property Core\)类型，第四位数字标识该AI Core同一个IP核的配置版本。通过该宏，开发者可以针对不同AI处理器，差异化进行代码适配和优化。产品型号和NPU架构版本的对应关系如下：

    <!-- npu="950" id1 -->
    - Ascend 950PR/Ascend 950DT：3510
    <!-- end id1 -->
    <!-- npu="A3" id2 -->
    - Atlas A3 训练系列产品/Atlas A3 推理系列产品：2201
    <!-- end id2 -->
    <!-- npu="910b" id3 -->
    - Atlas A2 训练系列产品/Atlas A2 推理系列产品：2201
    <!-- end id3 -->
    <!-- npu="310b" id4 -->
    - Atlas 200I/500 A2 推理产品：3002
    <!-- end id4 -->
    <!-- npu="310p" id5 -->
    - Atlas 推理系列产品：2002
    <!-- end id5 -->
    <!-- npu="910" id6 -->
    - Atlas 训练系列产品：1001
    <!-- end id6 -->

    以下为通过\_\_NPU\_ARCH\_\_控制在不同AI处理器上算子输出值舍入模式的示例。

    ```
    __aicore__ static inline void CopyOut(uint64_t mulLen)
    {
    #if __NPU_ARCH__ == 2002
        Cast(dstLocal, srcLocal, RoundMode::CAST_NONE, mulLen); // CAST_NONE表示舍入模式在转换有精度损失时使用CAST_RINT模式，在不涉及精度损失时不进行舍入
    #elif __NPU_ARCH__ == 2201
        Cast(dstLocal, srcLocal, RoundMode::CAST_RINT, mulLen); // CAST_RINT表示舍入模式为四舍六入五成双舍入
    #endif
        event_t eventVToMTE3 = static_cast<event_t>(GetTPipePtr()->FetchEventID(HardEvent::V_MTE3));
        SetFlag<HardEvent::V_MTE3>(eventVToMTE3);
        WaitFlag<HardEvent::V_MTE3>(eventVToMTE3);
        CommonCopyOut<float>(dstLocal, mulLen);  // 拷贝LocalTensor至GlobalTensor
    }
    ```

-   <a name="li19530175294118"></a>**ASCEND\_IS\_AIV、ASCEND\_IS\_AIC**

    ASCEND\_IS\_AIV和ASCEND\_IS\_AIC是通过C++宏实现的条件判断语句，用于在\_\_aicore\_\_修饰的函数中实现代码的条件编译。基于分离模式（AIC核和AIV核分离）开发融合算子时，算子逻辑中同时涉及AIV核和AIC核的处理逻辑，并需要进行核间同步，此时需要通过ASCEND\_IS\_AIV/ ASCEND\_IS\_AIC进行AIV和AIC核代码的隔离。

    >[!NOTE]说明
    >当使用高阶API Matmul时，其内部已通过REGIST\_MATMUL\_OBJ宏方式实现了AIV与AIC核代码的隔离，用户无需再使用该宏进行处理。

    以MatmulNzCustom算子为例，该算子在分离模式下需要分别在AIV核和AIC核上实现不同的逻辑。具体而言，AIV核负责将矩阵数据搬入Unified Buffer（UB），完成数据的重排（将矩阵数据转换为NZ格式），并将其写入Global Memory。而AIC核则直接从Global Memory读取已经重排好的NZ格式数据，并执行矩阵乘法（Matmul）计算。由于AIV核和AIC核的代码逻辑不同，需要通过ASCEND\_IS\_AIV和ASCEND\_IS\_AIC宏进行代码隔离，确保在编译时分别生成适用于AIV核和AIC核的代码。

    示例伪码如下：

    ```
    template <typename AType, typename BType, typename CType, typename BiasType>
    __aicore__ inline void MatmulKernel<AType, BType, CType, BiasType>::Process(AscendC::TPipe *pipe)
    {
        // 利用AIV核的Vector计算单元实现ND2NZ格式转换。如下代码中MatrixBtoNZ为将B矩阵进行ND2NZ格式转换的函数。
        if ASCEND_IS_AIV {
            pipe->InitBuffer(ubBuf, TOTAL_UB_SIZE);
            MatrixBtoNZ<typename B_TYPE::T>(tempGM,
                bGMNZ,
                tiling,
                isTransB,
                ubBuf,
                tiling.baseK,
                tiling.baseN);  // Vector侧实现的ND2NZ函数
            SyncAll();
            // AIC核和AIV核同步
            AscendC::CrossCoreSetFlag<0x2, PIPE_MTE3>(0x4);
            return;
        }
        if ASCEND_IS_AIC {
            AscendC::CrossCoreWaitFlag(0x4);   // 等待AIV核完成ND2NZ格式转换
        }
        ... ...
        // 设置左矩阵A、右矩阵B、Bias。
        matmulObj.SetTail(tailM, tailN);
        matmulObj.SetTensorA(aGlobal, false);
        matmulObj.SetTensorB(bGlobal, false);
        if (tiling.isBias) {
            matmulObj.SetBias(biasGlobal);
        }
        // 完成矩阵乘操作
        matmulObj.IterateAll(cGlobal);
        // 结束矩阵乘操作
        matmulObj.End();
    }
    ```

-   **ASCENDC\_CUBE\_ONLY**

    ASCENDC\_CUBE\_ONLY是通过C++宏实现的条件判断语句，用于在\_\_aicore\_\_修饰的函数中实现代码的条件编译。

    基于分离模式开发非融合算子时，在只有矩阵计算的算子场景下，可以通过设置ASCENDC\_CUBE\_ONLY，开启纯Cube模式完成Matmul计算，减少消息通信的性能开销，提升算子性能。

    >[!CAUTION]注意
    >ASCENDC\_CUBE\_ONLY宏必须在\#include "lib/matmul\_intf.h"之前设置。

    以matmul\_custom算子为例，高阶API Matmul默认使用MIX模式，即用户从AIV侧发起消息，通过消息通信框架中转消息后，在AIC侧执行Matmul计算。这套消息处理机制会带来额外的Scalar性能开销。相较于MIX模式，纯Cube模式可以直接跳过消息通信框架，完成Matmul计算，提升算子性能。

    示例伪码如下：

    ```
    #define ASCENDC_CUBE_ONLY
    #include "lib/matmul_intf.h"

    using A_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, AType>;
    using B_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BType>;
    using C_TYPE = AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, CType>;
    using BIAS_TYPE =  AscendC::MatmulType<AscendC::TPosition::GM, CubeFormat::ND, BiasType>;
    AscendC::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CFG_NORM> matmulObj;
    ```

## 函数执行空间限定符<a name="section1074418132518"></a>

函数执行空间限定符（Function Execution Space Qualifier）指示函数是在Host侧执行还是在Device侧执行，以及它是否可从Host侧或Device侧调用。

-   **\_\_global\_\_**

    \_\_global\_\_执行空间限定符声明一个核函数（Kernel）。核函数（Kernel）有如下性质：在Device上执行；只能被Host侧函数调用；\_\_global\_\_只是表示这是Device侧函数的入口，并不表示具体的设备类型，具体的设备类型由\_\_aicore\_\_标记。具有如下使用约束：

    -   一个\_\_global\_\_函数必须返回void类型，并且不能是class的成员函数。
    -   主机侧调用\_\_global\_\_函数必须使用<<<\>\>\>异构调用语法。
    -   \_\_global\_\_的调用是异步的，意味着函数返回，并不表示kernel函数在device侧已经执行完成，如果需要同步，需要使用Runtime同步接口显式同步，如**aclrtSynchronizeStream**接口。

-   **\_\_aicore\_\_**

    \_\_aicore\_\_执行空间限定符声明一个函数，它具有如下属性：

    -   在Device侧执行
    -   只能被\_\_global\_\_函数，或者其他\_\_aicore\_\_函数调用

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

    \_\_host\_\_执行空间限定符声明一个函数，它具有如下属性：

    -   只能在Host侧执行
    -   只能被Host侧函数调用
    -   \_\_global\_\_ 和\_\_host\_\_不能一起使用

    \_\_host\_\_限定符是可选项，无函数执行空间限定符定义的函数，默认是host函数。

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

    AI CPU函数执行空间限定符\_\_aicpu\_\_用于指示函数是否为AI CPU核函数（Kernel），它具有如下属性：

    -   在Device侧执行且只能被Host侧函数调用，因此必须与\_\_global\_\_同时声明。
    -   一个\_\_global\_\_ \_\_aicpu\_\_函数不能是void返回类型，并且入参只能是一个指针。
    -   一个\_\_global\_\_ \_\_aicpu\_\_函数不能在.asc文件中进行定义，只能声明，且需要使用extern。
    -   Host侧调用\_\_global\_\_ \_\_aicpu\_\_函数时必须使用<<<\>\>\>异构调用语法，输入的函数入参在入参指针的基础上需要输入从指针中读取的数据大小。
    -   \_\_global\_\_的调用是异步的，意味着函数返回，并不表示kernel函数在Device侧已经执行完成，如果需要同步，需要使用Runtime同步接口显式同步，如**aclrtSynchronizeStream**接口

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
    ```

-   **\_\_inline\_\_**

    \_\_inline\_\_限定符声明一个函数，它具有如下属性：

    -   标识Device侧函数强制内联，可以减少函数频繁调用产生的指令压栈、出栈的开销，但可能会导致算子二进制增加。
    -   和C++函数修饰符inline的主要区别是Device侧\_\_inline\_\_是强制内联，C++的inline则是根据编译器优化选择性内联。
    -   AI Core对函数嵌套深度有限制，一般推荐嵌套深度不超过4层。使用强制内联可以减少调用层次。

-   **\_\_cube\_\_**

    标识该核函数（Kernel）仅在Cube核执行。针对耦合模式的硬件架构，该修饰符不生效。

    ```
    extern "C" __global__ __cube__ void mmad_custom(GM_ADDR a, GM_ADDR b, GM_ADDR c)
    {
        KernelMmad op;
        op.Init(a, b, c);
        op.Process();
    }
    ```

-   **\_\_vector\_\_**

    标识该核函数（Kernel）仅在Vector核执行。针对耦合模式的硬件架构，该修饰符不生效。

    ```
     __vector__ __global__ __aicore__ void add_custom(){}
    ```

-   **\_\_mix\_\_\(cube, vec\)**

    标识该核函数（Kernel）同时在Cube核和Vector核上执行。\(cube, vec\)分别表示核函数（Kernel）启动的Cube核和Vector核的配比，支持的配比为\(1, 0\)、\(0, 1\)、\(1, 1\)和\(1, 2\)。在该修饰符生效的非耦合架构上，`__mix__(1, 0)`等价于`__cube__`，`__mix__(0, 1)`等价于`__vector__`，可分别使用__cube__和__vector__替代。针对耦合模式的硬件架构，该修饰符不生效。

-   **\_\_schedmode\_\_\(mode\)**

    标识该核函数（Kernel）的执行调度模式。如下图所示：

    -   mode = 0 : normal mode，尽可能选择空闲物理核下发执行核函数（Kernel），若空闲物理核数无法满足当前核函数（Kernel）的需要，没有下发的部分等待核心空闲后执行。此时OP1和OP2算子会存在交叠执行（overlap）的情况。

    -   mode = 1 : batch mode，在下发核函数（Kernel）时先进行判断，若空闲物理核数无法满足当前核函数（Kernel）的需要，则等待至空闲物理核数满足该核函数（Kernel）所需要的所有物理核时，同时下发执行，OP1和OP2的执行被切分（split）开，不会出现交叠执行的情况。

    ![](../../figures/batchmode.png)

    在多流并发场景，多算子并行执行时，若执行总核数超过最大物理核数，且多个算子逻辑使用SyncALL等核间同步接口时，建议设置mode为1，防止多个算子之间互相等待空闲核调度，导致死锁。默认值mode为0。

    ```
     __schedmode__(1) __global__ __mix__(1, 2) void OP1() // OP1使用了SyncAll接口，且存在多流并发的可能，需要设置batch mode（mode 1）
    {
        AscendC::SyncAll();
        ....
    }
     __schedmode__(1) __global__ __mix__(1, 2) void OP2() // OP2使用了SyncAll接口，且存在多流并发的可能，需要设置batch mode（mode 1）
    {
        AscendC::SyncAll();
        ....
    }
    __schedmode__(0) __global__ __vector__ void OP3() {...} // OP3没有使用SyncAll接口，可以设置为normal mode(mode 0)，按照正常规则执行算子。
    or
    __global__ __vector__ void OP3() {...} // 不设置__schedmode__，默认为normal mode。
    ```

## 函数标记宏<a name="section192521344610"></a>

-   \_\_simd\_vf\_\_

    函数标记宏，用于标记SIMD VF入口函数，函数无返回值。使用[asc\_vf\_call](../../../api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md)调用SIMD VF入口函数，启动VF子任务。

    ```
    __simd_vf__ inline void KernelAdd(__ubuf__ float* x, __ubuf__ float* y, __ubuf__ float* z)
    ```

    \_\_simd\_vf\_\_标记的SIMD VF有以下入参约束：

    -   支持指针传参（Pass-by-Pointer），指针变量必须用\_\_ubuf\_\_地址空间限定符修饰。
    -   不支持引用传参（Pass-by-Reference）。
    -   不支持函数指针传参。

        \_\_simd\_vf\_\_使用的示例如下：

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

    函数标记宏，函数可以有返回值，允许被SIMD VF入口函数或其他非入口函数调用。

    ```
    __simd_callee__ inline float add(float x, float y)
    ```

## 地址空间限定符<a name="section1624210295308"></a>

AI Core具备多级独立片上存储，各个地址空间独立编址，具备各自的访存指令，根据架构差异，有些存储空间具备统一地址空间（Generic Address Space），有些则没有。设备侧编程基于语法扩展允许地址空间作为合法的类型限定符，以提供针对不同地址空间的访问能力和地址空间合法性检查。

**表2**  地址空间映射关系

| 地址空间限定符 | AI Core物理存储空间 |
|----------------|---------------------|
| __gm__ | 设备侧内存GM |
| __ubuf__ | Vector UB |
| __ca__ | Cube L0A Buffer |
| __cb__ | Cube L0B Buffer |
| __cc__ | Cube L0C Buffer |
| __cbuf__ | Cube L1 Buffer |
| __fbuf__ | Fixpipe Buffer |
| __ssbuf__ | SSBuffer |

地址空间限定符可以在变量声明中使用，用于指定对象分配的区域。如果对象的类型被地址空间名称限定，那么该对象将被分配在指定的地址空间中。同样地，对于指针，指向的类型可以通过地址空间进行限定，以指示所指向的对象所在的地址空间。

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

地址空间限定符不能用于非指针返回类型，非指针函数参数，函数类型，同一个类型上不允许使用多个地址空间限定符。

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

>[!NOTE]说明
>**重要**：不同地址空间指针的大小可能不同。例如，不能认为sizeof\(\_\_gm\_\_ int \*\)总是等于sizeof\(\_\_ubuf\_\_ int \*\)，譬如编译器或许可能在某些系统上以32bit存储\_\_ubuf\_\_指针。

-   **private地址空间**

    private地址空间是大多数变量的默认地址空间，特别是局部变量。

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

-   **\_\_gm\_\_地址空间**

    \_\_gm\_\_地址空间限定符用来表示分配于设备侧全局内存的对象，全局内存对象可以声明为标量、用户自定义结构体的指针。

    ```
    __gm__ int *var; // var point to an array of int elements

    typedef struct {
        float a[3];
        int b[2];
    } foo_t;

    __gm__ foo_t *info; // info point to an array of foo_t elements
    ```

-   **\_\_ubuf\_\_地址空间**

    \_\_ubuf\_\_地址空间用来描述存储于AI Core核内UB存储空间的变量。

    ```
    __global__ __aicore__ void foo() {
      // ptr is in private address space, point to __ubuf__
      __ubuf__ int *ptr;
    }
    ```

-   **\_\_ca\_\_, \_\_cb\_\_, \_\_cc\_\_, \_\_cbuf\_\_地址空间**

    上述几个地址空间主要用于特定的DMA指令访问，不具备标量直接访问能力。

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

## 内置常量<a name="section784531219338"></a>

| 常量名 | 取值 | 功能 |
|--------|------|------|
| constexpr int32_t g_coreType | AscendC::AIC<br>AscendC::AIV | 常量值由框架自动设置，AIC核下，配置为AscendC::AIC，AIV核下，配置为AscendC::AIV。可以通过对该常量值的判断，来实现了AIV与AIC核代码的区分和隔离。功能等同于直接使用ASCEND_IS_AIV、ASCEND_IS_AIC。 |
| constexpr uint64_t ASC_UB_SIZE | 取值由当前AI处理器决定，若该AI处理器不存在这块空间，则默认配置为0。 | 表示当前AI处理器架构下UB的容量，可用于编译期获取UB资源大小。 |
| constexpr uint64_t ASC_L1_SIZE | 取值由当前AI处理器决定，若该AI处理器不存在这块空间，则默认配置为0。 | 表示当前AI处理器架构下L1 Buffer的容量，可用于编译期获取L1 Buffer大小。 |
| constexpr uint64_t ASC_L0A_SIZE | 取值由当前AI处理器决定，若该AI处理器不存在这块空间，则默认配置为0。 | 表示当前AI处理器架构下L0A Buffer的容量，可用于编译期获取L0A Buffer大小。 |
| constexpr uint64_t ASC_L0B_SIZE | 取值由当前AI处理器决定，若该AI处理器不存在这块空间，则默认配置为0。 | 表示当前AI处理器架构下L0B Buffer的容量，可用于编译期获取L0B Buffer大小。 |
| constexpr uint64_t ASC_L0C_SIZE | 取值由当前AI处理器决定，若该AI处理器不存在这块空间，则默认配置为0。 | 表示当前AI处理器架构下L0C Buffer的容量，可用于编译期获取L0C资源大小。 |
| constexpr uint64_t ASC_BT_SIZE | 取值由当前AI处理器决定，若该AI处理器不存在这块空间，则默认配置为0。 | 表示当前AI处理器架构下BiasTable Buffer的容量，可用于编译期获取BiasTable资源大小。 |

## 内置变量<a name="section199434523343"></a>

| 变量名 | 相关API | 功能 |
|-------|---------|------|
| block_num | GetBlockNum | 当前任务配置的核数，用于代码内部的多核逻辑控制等。内置变量block_num的值与通过相关API GetBlockNum获取的值保持一致。 |
| block_idx | GetBlockIdx | 当前核的索引，用于代码内部的多核逻辑控制及多核偏移量计算等。注意，内置变量block_idx的值与通过相关API GetBlockIdx获取的值在部分场景下并不一致，具体说明及场景示例请参考下文描述。 |
通常，建议用户使用内置变量对应的API获取所需值，不建议用户直接使用内置变量。因为内置变量反映的是单个硬件资源的配置信息，对于软件栈整合硬件资源、扩展硬件的功能，内置变量的值与实际语义可能不符。

在Mix场景（使用`__mix__`函数执行空间限定符）下，Vector核（AIV）的逻辑位置需要通过block_idx和sub_block_idx共同确定：`logic_idx = block_idx * sub_block_num + sub_block_idx`。其中block_idx标识当前组合在整个grid中的位置，sub_block_idx标识当前AIV在组合内的位置。开发者应使用GetBlockIdx和GetSubBlockIdx API组合获取完整的逻辑位置信息，而非直接使用内置变量。

**示例：**

例如，在任务配置的核数为3（block_num = 3）的场景下，当使用`__mix__(1, 2)`作为函数执行空间限定符时，任务会启动3个Cube Core和6个Vector Core，内置变量block_idx在6个Vector Core上的取值分别为：0、0、1、1、2、2，即位于同一核上的两个Vector Core取值相同，而GetBlockIdx API获取的值分别为：0、1、2、3、4、5，两者并不相同。在3个Cube Core上两者的取值相同，均为：0、1、2。

<!-- npu="310p" id7 -->
例如，在Atlas 推理系列产品中，当启用KERNEL_TYPE_MIX_VECTOR_CORE时，算子会同时运行在AI Core和Vector Core上。此时，block_idx在这两种核心上都是从0开始计数，用户无法直接通过block_idx来切分数据和控制多核逻辑。而GetBlockIdx在Vector Core上对block_idx增加偏移量（AI Core的block_num），从而保证返回的值能够正确反映多核环境下的实际逻辑。
<!-- end id7 -->

## 核函数（Kernel）配置<a name="核函数配置"></a><a name="section97005415463"></a>

在调用\_\_global\_\_限定符修饰的函数时必须指定执行配置。执行配置通过在函数名和带括号的参数列表之间插入如下形式的表达式来指定：

```
<<<numBlocks, dynUBufSize, stream>>>
```

其中：
-   numBlocks：规定了核函数（Kernel）将会在几个核上执行。每个执行该核函数（Kernel）的核会被分配一个逻辑ID，即block\_idx，可以在核函数（Kernel）的实现中使用内置变量[block_idx](../language_extension/simd_builtin_keywords.md#内置变量)获取；

    >[!NOTE]说明 
    >numBlocks是逻辑核的概念，取值范围为\[1,65535\]。为了充分利用硬件资源，一般设置为物理核的核数或其倍数。
    >- 对于耦合模式和分离模式，numBlocks在运行时的意义和设置规则有一些区别，具体说明如下：
    >    - 耦合模式：由于其Vector、Cube单元是集成在一起的，numBlocks用于设置启动多个AI Core核实例执行，不区分Vector、Cube。AI Core的核数可以通过[GetCoreNumAiv](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAiv.md)或者[GetCoreNumAic](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAic.md)获取。
    >    - 分离模式
    >        - 针对仅包含Vector计算的算子，numBlocks用于设置启动多少个Vector（AIV）实例执行，比如某款AI处理器上有40个Vector核，建议设置为40。
    >        - 针对仅包含Cube计算的算子，numBlocks用于设置启动多少个Cube（AIC）实例执行，比如某款AI处理器上有20个Cube核，建议设置为20。
    >        - 针对Vector/Cube融合计算的算子，启动时，按照AIV和AIC组合启动，numBlocks用于设置启动多少个组合执行，比如某款AI处理器上有40个Vector核和20个Cube核，一个组合是2个Vector核和1个Cube核，建议设置为20，此时会启动20个组合，即40个Vector核和20个Cube核。**注意：该场景下，设置的numBlocks逻辑核的核数不能超过物理核（2个Vector核和1个Cube核组合为1个物理核）的核数。**
    >        - AIC/AIV的核数分别通过[GetCoreNumAic](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAic.md)和[GetCoreNumAiv](../../../api/Utils-API/platform_info/PlatformAscendC/GetCoreNumAiv.md)接口获取。
    >- 如果开发者使用了Device资源限制特性，那么算子设置的numBlocks不应超过[PlatformAscendC](../../../api/Utils-API/platform_info/PlatformAscendC/PlatformAscendC.md)提供核数的API（GetCoreNum/GetCoreNumAic/GetCoreNumAiv等）返回的核数。例如，使用aclrtSetStreamResLimit设置Stream级别的Vector核数为8，那么GetCoreNumAiv接口返回值为8，针对Vector算子设置的numBlocks不应超过8，否则会抢占其他Stream的资源，导致资源限制失效。

-   dynUBufSize：Dynamic UB Size，是配置UB动态内存分配的空间的大小（仅限UB，不包括L1等），单位为Byte，默认设置为0；
-   stream：类型为aclrtStream，stream用于维护一些异步操作的执行顺序，确保按照应用程序中的代码调用顺序在device上执行，默认设置为nullptr。stream创建等管理接口请参考[《Runtime运行时API》](https://hiascend.com/document/redirect/CannCommunityRuntimeApi)。

以下示例展示了核函数（Kernel）的声明与调用方式。

```
// 声明
__global__ __vector__ void add_custom(float* x, float* y, float* z);
// 调用
add_custom<<<numBlocks, dynUBufSize, stream>>>(x, y, z);
```
