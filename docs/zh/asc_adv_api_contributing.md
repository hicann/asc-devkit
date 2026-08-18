# API贡献指南
## 概述
高阶API（Advanced API）是基于单核对常见算法的抽象和封装，用于提高算子开发效率。高阶API通过封装基础API实现，主要包括数学库、Matmul、量化反量化、数据归一化等API。

开发者对本仓库的贡献主要存在两种场景，一是自定义开发全新的高阶API，二是修改仓中原有高阶API进行进阶开发。开发者基于任意一种场景完成API的开发后，可以将该API编译部署到CANN软件环境中，然后在算子程序中使用。

在阅读本文档前，请确保您已了解昇腾AI处理器硬件架构，学习过Ascend C编程相关文档或课程，推荐掌握[Ascend C编程指南](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC)中的知识。本文档以高阶API数学库中的`Axpy`接口为例，分别对上述两种场景的API开发进行介绍。

----

## 开发流程
Ascend C高阶API的开发流程主要包括如下步骤：
- 设计API
- 开发API
  - 编写API对外接口
  - 编写API内部实现
- 测试
  - UT测试
  - 单算子测试
---
## 自定义开发全新API
下面以高阶API `Axpy`为例，介绍如何从零开始，开发一个高阶API。本案例删除了部分非必要代码，您可以在代码仓的[axpy.h](../../include/adv_api/math/axpy.h)、[axpy_tiling.h](../../include/adv_api/math/axpy_tiling.h)、[axpy_tiling_intf.h](../../include/adv_api/math/axpy_tiling_intf.h)、[axpy_common_impl.h](../../impl/adv_api/detail/math/axpy/axpy_common_impl.h)、[axpy_tiling_impl.cpp](../../impl/adv_api/tiling/math/axpy_tiling_impl.cpp)文件中查看全部代码。
### 设计API
axpy的功能为源操作数`srcTensor`中每个元素与标量求积后，与目的操作数`dstTensor`中的对应元素相加，计算公式如下。

$$
dstTensor_i = srcTensor_i \times scalarValue+dstTensor_i
$$

- 核函数（Kernel）侧

    考虑算子执行过程中的数据流向，Host侧申请GlobalMemory空间并向其中写入数据，核函数（Kernel）侧从GlobalMemory搬运数据到LocalMemory，计算单元从LocalMemory中获取数据进行计算，将结果写回LocalMemory中，最后LocalMemory中的计算结果搬出到GlobalMemory。高阶API用于完成其中的计算，因此高阶API的源操作数与目的操作数均位于LocalMemory内存中，即高阶API的接口参数包括已搬入数据的LocalTensor以及其它参数。

    从`Axpy`的公式可知，接口需要输入参数`srcTensor`和`scalarValue`，输出参数`dstTensor`，还需要一个输入参数`calCount`表示参与计算的元素个数。由于该计算中产生了乘积的中间结果，因此需要额外分配LocalMemory的空间，用于存储中间计算结果。为在接口中传入该临时缓存空间，增加一个`shardTmpBuffer`输入参数。为了扩展API支持的数据类型，定义两个模版参数，分别作为源操作数和目的操作数的数据类型。模板参数中的`isReuseSource`为预留参数，在开发时可以不添加该参数。根据API的算法功能或公式，分析得出接口的所有参数后，得到如下核函数（Kernel）侧函数原型。

    定义出函数原型后，分析并确定API实现中使用的基础API。`Axpy`公式中Tensor与Scalar的相乘运算，可以使用Ascend C提供的基础API `Muls`实现；两个Tensor的加法运算，可以使用Ascend C提供的基础API `Add`实现。

    ```c++
    template <typename T, typename U, bool isReuseSource = false>
    __aicore__ inline void Axpy(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```
    模板参数说明
    | 参数名      | 描述 |
    | ----------- | ----------- |
    | T      |    目的操作数数据类型，支持数据类型为half/float。    |
    | U      |    源操作数数据类型，支持数据类型为half/float。   |
    | isReuseSource   |   该参数预留，传入默认值false即可。      |

    接口参数说明
    | 参数名      | 输入/输出 |  描述   |
    | ----------- | ----------- |----------- |
    | dstTensor      | 输出  | 目的操作数。    |
    | srcTensor      |  输入  |源操作数。   |
    | scalarValue   | 输入  |scalar标量。支持的数据类型为half/float。scalar操作数的类型需要和srcTensor保持一致。      |
    | sharedTmpBuffer   | 输入  | 临时缓存。类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。由于该接口的内部实现需要额外的临时空间来存储计算过程中的中间变量。临时空间需要开发者通过sharedTmpBuffer入参传入。      |
    | calCount   | 输入 | 参与计算的元素个数。     | 
- Tiling侧

    核函数（Kernel）侧接口的计算需要开发者预留/申请临时空间，该临时空间的大小需要在Tiling侧根据获取到的源操作数shape大小，计算高阶API所需的最大(maxValue)临时空间和最小临时空间(minValue)的大小。因此在Tiling侧提供一个用于计算maxValue和minValue的接口。接口的输入参数包括源操作数Tensor的shape大小和源操作数数据类型所占字节数，shape大小的参数使用AscendC::TensorShape类型，数据类型所占的字节数使用`uint32_t`类型，输出参数包括minValue和maxValue。与Axpy接口中的isReuseSource参数系统，Tiling接口中的isReuseSource为预留参数。
    ```c++
    void GetAxpyMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);
    ```
    接口参数说明
    | 参数名      | 输入/输出 |  描述   |
    | ----------- | ----------- |----------- |
    | srcShape      | 输入  | 输入的shape信息。   |
    | typeSize      |  输入  | 算子输入的数据类型大小，单位为字节。比如算子输入的数据类型为half，此处应该传入2。   |
    | isReuseSource   | 输入  |  预留参数。     |
    | maxValue   | 输出  | Axpy接口能完成计算所需的最大临时空间大小，超出该值的空间不会被接口使用。在最小临时空间-最大临时空间范围内，随着临时空间增大，kernel侧接口计算性能会有一定程度的优化提升。为了达到更好的性能，开发者可以根据实际的内存使用情况进行空间申请，最大空间大小为0表示计算不需要临时空间。      |
    | minValue   | 输出 | Axpy接口能完成计算所需的最小临时空间大小。为保证功能正确，接口计算时申请的临时空间不能小于该数值。最小空间大小为0表示计算不需要临时空间。   | 
### 开发API
#### 编写API对外接口
- 核函数（Kernel）侧接口
    
    在`include/adv_api/`对应分类的目录下，新增[axpy.h](../../include/adv_api/math/axpy.h)文件。根据上述分析设计的API函数原型，编写对外接口的代码，函数实现中调用AxpyImpl的实现。
    ```c++
    template <typename T, typename U, bool isReuseSource = false>
    __aicore__ inline void Axpy(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U scalarValue,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    {
        AxpyImpl<T, U, isReuseSource>(dstTensor, srcTensor, scalarValue, sharedTmpBuffer, calCount);
    }
    ```
- Tiling侧接口
    
    在`include/adv_api/`对应分类的目录下，新增[axpy_tiling.h](../../include/adv_api/math/axpy_tiling.h)文件。根据上述分析设计的Tiling侧接口，编写函数声明。
    ```c++
    #include "../utils/types.h"

    void GetAxpyMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);
    ```
- 在公共文件中引入头文件。

    [include/adv_api/kernel_api.h](../../include/adv_api/kernel_api.h)文件中包含了所有高阶API头文件，建议在该文件中引入新增API的头文件，这样在调用高阶API时，只需要引入`"kernel_api.h"`。
    ```c++
    #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    // ...
    #include "math/axpy.h"
    // ...
    #endif // __NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201
    ```
    
    [include/adv_api/tiling_api.h](../../include/adv_api/tiling_api.h)文件中包含了所有高阶API的TIling接口头文件，建议在该文件中引入新增的Tiling接口头文件，这样在调用高阶API的Tiling函数时，只需要引入`"tiling_api.h"`。
    ```c++
    #include "math/axpy_tiling.h"
    ```

#### 编写API内部实现
- 核函数（Kernel）侧实现

    在核函数（Kernel）接口实现文件路径[impl/adv_api/detail](../../impl/adv_api/detail)的相应类别目录（本案例为`math`）下新增`axpy`目录，在该目录下新增接口实现文件[axpy_common_impl.h](../../impl/adv_api/detail/math/axpy/axpy_common_impl.h)，然后在该实现文件中编写接口实现代码。

    首先，引入必要的头文件。
    ```c++
    #include "kernel_tensor.h"
    ```
    编写AxpyImpl函数实现代码。如果源操作数的数据类型为float，则调用基础API的Axpy接口，否则，调用自定义函数`AxpySub`，通过Muls和Add组合计算，提供更优的计算精度。
    ```c++
    template <typename T, typename U, bool isReuseSource>
    __aicore__ inline void AxpyImpl(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U scalarValue,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    {
        CHECK_FUNC_HIGHLEVEL_API(
            Axpy, (T, U, isReuseSource), (dstTensor, srcTensor, scalarValue, sharedTmpBuffer, calCount));

        if constexpr (sizeof(U) == sizeof(float)) {
            Axpy<T, U>(dstTensor, srcTensor, scalarValue, calCount);
        } else {
            AxpySub<T, U, isReuseSource>(dstTensor, srcTensor, scalarValue, sharedTmpBuffer, calCount);
        }
    }
    ```
    `AxpySub`函数根据临时空间大小制定计算策略，主要包括确定计算循环次数round、整块计算量stackSize、尾块计算量tail，具体代码如下。由于传入接口的sharedTmpBuffer是uint8类型，而源操作数元素是float类型，因此需要将sharedTmpBuffer的元素类型转化为float，获取其中的元素个数。根据硬件架构的特点，一次计算的最小单元为一个Block，即32字节，`axpyTmpCalc`函数将获取的临时空间能够存储的元素个数向下32字节对齐，得到一次计算的整块计算量stackSize。同时，当目的操作数元素类型为half时，为了提升精度，计算过程中会对源操作数和目的操作数分别做一次Cast精度转换，二分之一的临时空间被Cast结果占用，所以该场景下，计算的数据量为临时空间能够存储的元素个数向下32字节对齐后的大小的一半。已知总计算量calCount、单次整块计算量stackSize，计算出循环次数round和尾块计算量tail。计算公式如下：
   $$stackSize = \begin{cases}
    Align32(tmpbufferSize \div 2) &T = half
    \\ 
    Align32(tmpbufferSize) &T = float
    \end{cases}$$

    $$
    round = calCount/stackSize
    $$

    $$
    tail = calCount%stackSize
    $$
    如果tail不为0，即存在尾块，则需要额外处理尾块数据。在尾块数据计算前，通过setMaskCount接口设置计算单元的工作方式为Count模式。
    ```c++
    template <typename T, typename U, bool isReuseSource = false>
    __aicore__ inline void AxpySub(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U& scalarValue,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    {
        uint32_t bufferSize = sharedTmpBuffer.GetSize();
        CheckTmpBufferSize(bufferSize, 0, bufferSize);

        LocalTensor<float> tmpBuffer = sharedTmpBuffer.ReinterpretCast<float>();
        uint32_t tmpBufferSize = tmpBuffer.GetSize();

        uint32_t stackSize = axpyTmpCalc<T>(tmpBufferSize);

        const uint32_t round = calCount / stackSize;
        const uint32_t tail = calCount % stackSize;

        SetMaskCount();
        SetVectorMask<T, MaskMode::COUNTER>(0, stackSize);

        uint32_t offset = 0;
        for (uint32_t i = 0; i < round; i++) {
            AxpyIntrinsicsImpl(dstTensor[offset], srcTensor[offset], scalarValue, tmpBuffer, stackSize);
            offset = offset + stackSize;
        }

        if (tail != 0) {
            SetVectorMask<T, MaskMode::COUNTER>(0, tail);
            AxpyIntrinsicsImpl(dstTensor[offset], srcTensor[offset], scalarValue, tmpBuffer, stackSize);
        }

        SetMaskNorm();
        ResetMask();
    }
    ```
    `AxpyIntrinsicsImpl`函数调用基础API实现Axpy具体的算法。对于不同数据类型的目的操作数，重载实现`AxpyIntrinsicsImpl`函数。如下函数实现代码为源操作数和目的操作数的元素类型均为half的场景，如上文所述，此时临时空间stackBuffer能容纳$2 * stackSize$个元素，二分之一的临时空间用于保存源操作数和目的操作数的Cast结果。因此保存目的操作数Cast结果的Tensor `tmpDst`的起始偏移量为stackSize。在算法实现时，对源操作数和目的操作数分别做一次half到float类型的Cast转换，然后调用Muls、Add接口进行计算，最后将计算结果做一次float到half类型的Cast转换。
    ```c++
    template <>
    __aicore__ inline void AxpyIntrinsicsImpl(const LocalTensor<half>& dstTensor, const LocalTensor<half>& srcTensor,
        const half& scalarValue, LocalTensor<float> stackBuffer, uint32_t stackSize)
    {
        LocalTensor<float> tmpSrc = stackBuffer[0];
        LocalTensor<float> tmpDst = stackBuffer[stackSize];

        const UnaryRepeatParams unaryParams;
        const BinaryRepeatParams binaryParams;

        Cast<float, half, false>(tmpSrc, srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
            {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
        PipeBarrier<PIPE_V>();

        Cast<float, half, false>(tmpDst, dstTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
            {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
        PipeBarrier<PIPE_V>();

        Muls<float, false>(tmpSrc, tmpSrc, (float)scalarValue, MASK_PLACEHOLDER, 1, unaryParams);
        PipeBarrier<PIPE_V>();

        Add<float, false>(tmpDst, tmpSrc, tmpDst, MASK_PLACEHOLDER, 1, binaryParams);
        PipeBarrier<PIPE_V>();

        Cast<half, float, false>(dstTensor, tmpDst, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
            {1, 1, HALF_DEFAULT_REPEAT_STRIDE, DEFAULT_REPEAT_STRIDE});
        PipeBarrier<PIPE_V>();
    }
    ```
- Tiling侧实现

     在Tiling接口实现文件路径[impl/adv_api/tiling](../../impl/adv_api/tiling)的相应类别目录（本案例为`math`）下新增接口实现文件[axpy_tiling_impl.cpp](../../impl/adv_api/tiling/math/axpy_tiling_impl.cpp)，然后在该实现文件中编写接口实现代码。

    首先，引入必要的头文件。
    ```c++
    #include "lib/math/axpy_tiling.h"  // Tiling接口头文件
    #include <cstdint>  // 类型库
    #include "impl/host_log.h" // 日志库
    ```
    定义Tiling计算中需要使用的常量。
    ```c++
    constexpr uint32_t AXPY_HALF_CALC_PROC = 4;
    constexpr uint32_t AXPY_FLOAT_CALC_PROC = 1;
    constexpr uint32_t AXPY_ONE_REPEAT_BYTE_SIZE = 256;
    ```
    在`GetAxpyMaxMinTmpSize`接口中，调用`GetAxpyMaxTmpSize`接口获取所需临时空间最大值，调用`GetAxpyMinTmpSize`接口获取所需临时空间最小值。
   
    ```c++
    void GetAxpyMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource,
    uint32_t& maxValue, uint32_t& minValue)
    {
        (void)isReuseSource;
        const uint32_t inputSize = srcShape.GetShapeSize();
        ASCENDC_HOST_ASSERT(inputSize > 0, return, "Input Shape size must be greater than 0.");
        if (typeSize == sizeof(float)) {
            minValue = 0;
            maxValue = 0;
            return;
        }
        minValue = GetAxpyMinTmpSize(typeSize);
        maxValue = GetAxpyMaxTmpSize(inputSize, typeSize);
    }
    ```
    分析所需临时空间最大值的计算，当目的操作数的数据类型为float时，只有源操作数需要Cast转换，`Axpy`伪代码如下所示，此时使用了一倍元素个数inputSize的临时空间。
    ```
    Cast(tmpSrc, srcTensor)
    Muls(tmpSrc, dstTensor, scalar)
    Add(dstTensor, tmpSrc, dstTensor)
    ```
    当目的操作数的数据类型为half时，目的操作数和源操作数都需要Cast转换，`Axpy`伪代码如下所示，此时使用了四倍元素个数inputSize的临时空间。
    ```
    Cast(tmp1, srcTensor)
    Cast(tmp2, dstTensor)
    Muls(tmp3, tmp1, scalar)
    Add(tmp4, tmp2, tmp3)
    Cast(dstTensor, tmp4)
    ```
    考虑到Vector计算单元单次Repeat计算256字节的数据，那么最大临时空间的计算公式如下：

    $$
    MaxValue=\begin{cases}\max(inputSize*typeSize,\tiny AXPY\_ONE\_REPEAT\_BYTE\_SIZE \normalsize)*\tiny AXPY\_HALF\_CALC\_PROC &dstType = half\\\max(inputSize*typeSize,\tiny AXPY\_ONE\_REPEAT\_BYTE\_SIZE \normalsize)*\tiny AXPY\_FLOAT\_CALC\_PROC &dstType = float\end{cases}
    $$
    ```c++
   inline uint32_t GetAxpyMaxTmpSize(const uint32_t inputSize, const uint32_t typeSize)
    {
        const uint8_t calcPro = typeSize == sizeof(float) ? AXPY_FLOAT_CALC_PROC : AXPY_HALF_CALC_PROC;
        return inputSize * typeSize > AXPY_ONE_REPEAT_BYTE_SIZE ?
                calcPro * inputSize * typeSize :
                calcPro * AXPY_ONE_REPEAT_BYTE_SIZE; // All temporary variables are float.
    }
    ```
    分析所需临时空间最小值的计算，若临时空间最小，则单次计算数据量最小，即每次计算数据量固定为单次Repeat处理的数据大小，256字节，那么临时空间最小值的计算公式如下:

    $$
    MinValue=\begin{cases}\tiny AXPY\_ONE\_REPEAT\_BYTE\_SIZE*AXPY\_HALF\_CALC\_PROC &dstType = half\\\tiny AXPY\_ONE\_REPEAT\_BYTE\_SIZE*AXPY\_FLOAT\_CALC\_PROC &dstType = float\end{cases}
    $$
    ```c++
    inline uint32_t GetAxpyMinTmpSize(const uint32_t typeSize)
    {
        return AXPY_ONE_REPEAT_BYTE_SIZE * (typeSize == sizeof(float) ? AXPY_FLOAT_CALC_PROC : AXPY_HALF_CALC_PROC);
    }
    ```
    编写完Tiling侧实现文件后，需要在[impl/adv_api/tiling/CMakeLists.txt](../../impl/adv_api/tiling/CMakeLists.txt)中引入该文件。具体方式为：在`add_library(tiling_api STATIC ...)`语句中新增文件路径`${CMAKE_CURRENT_SOURCE_DIR}/math/axpy_tiling_impl.cpp`。
## 基于原有API进阶开发
若开发者基于当前仓库已有的API进行进阶特性开发，设计API的过程与前述内容相同。开发代码时，考虑对原有函数实现重载或新增代码分支，根据进阶特性的算法功能，确定需要使用的基础API，并完成编码。例如，原有API对部分数据类型不支持，假设Axpy不支持目的操作数元素为float类型，那么就需要对`AxpyIntrinsicsImpl`函数进行重载，实现dstTensor数据类型为float的函数功能。
```c++
template <>
__aicore__ inline void AxpyIntrinsicsImpl(const LocalTensor<float>& dstTensor, const LocalTensor<half>& srcTensor,
    const half& scalarValue, LocalTensor<float> stackBuffer, uint32_t stackSize)
{
    const UnaryRepeatParams unaryParams;
    const BinaryRepeatParams binaryParams;

    Cast<float, half, false>(stackBuffer, srcTensor, RoundMode::CAST_NONE, MASK_PLACEHOLDER, 1,
        {1, 1, DEFAULT_REPEAT_STRIDE, HALF_DEFAULT_REPEAT_STRIDE});
    PipeBarrier<PIPE_V>();

    Muls<float, false>(stackBuffer, stackBuffer, (float)scalarValue, MASK_PLACEHOLDER, 1, unaryParams);
    PipeBarrier<PIPE_V>();

    Add<float, false>(dstTensor, stackBuffer, dstTensor, MASK_PLACEHOLDER, 1, binaryParams);
    PipeBarrier<PIPE_V>();
}
```

### 测试
API的编码完成后，开发者可以编写对应的测试用例和测试代码来验证编译正常、API功能正确。测试包含UT测试和通过搭建简易自定义算子工程进行的单算子测试。
#### UT测试
UT测试使用gTest作为测试框架，一般验证接口编译是否正常，并不能看护接口功能。
##### UT编码
###### 核函数（Kernel）侧

在UT目录[tests/api/adv_api/math](../../tests/api/adv_api/math)下新增目录`axpy`、文件[test_operator_axpy.cpp](../../tests/api/adv_api/math/axpy/test_operator_axpy.cpp)。UT实现主要包括如下三部分：
1. 引入头文件
    ```c++
    #include <gtest/gtest.h>
    #include "kernel_operator.h"
    ``` 
2. 实现算子
    - 初始化变量。
        ```c++
        TPipe tpipe;
        TQue<TPosition::VECIN, 1> vecInQue;
        TQue<TPosition::VECIN, 1> vecOutQue;
        TQue<TPosition::VECIN, 1> vecTmpQue;
        GlobalTensor<U> inputGlobal;
        GlobalTensor<T> outputGlobal;
        inputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ U*>(srcGm), dataSize);
        outputGlobal.SetGlobalBuffer(reinterpret_cast<__gm__ T*>(dstGm), dataSize);
        tpipe.InitBuffer(vecInQue, 1, dataSize * sizeof(U));
        tpipe.InitBuffer(vecOutQue, 1, dataSize * sizeof(T));
        if (sizeof(U) == sizeof(float)) {
            tpipe.InitBuffer(vecTmpQue, 1, dataSize * sizeof(float));
        } else {
            tpipe.InitBuffer(vecTmpQue, 1, dataSize * 4 * sizeof(half));
        }
        ```
    - 数据从GlobalMemory拷贝到LocalMemory。
        ```c++
        LocalTensor<U> inputLocal = vecInQue.AllocTensor<U>();
        LocalTensor<T> outputLocal = vecOutQue.AllocTensor<T>();
        LocalTensor<uint8_t> tmpLocal = vecTmpQue.AllocTensor<uint8_t>();

        SetVectorMask<uint8_t, MaskMode::NORMAL>(256);
        DataCopy(inputLocal, inputGlobal, dataSize);
        ```
    - 调用高阶API进行计算。
        ```c++
        SetFlag<HardEvent::MTE2_V>(EVENT_ID0);
        SetVectorMask<uint8_t, MaskMode::NORMAL>(128);
        WaitFlag<HardEvent::MTE2_V>(EVENT_ID0);
        U scalar = 4;

        Axpy<T, U, false>(outputLocal, inputLocal, scalar, tmpLocal, dataSize);
        ```
    - 数据从LocalMemory拷贝到GlobalMemory。
        ```c++
        SetFlag<HardEvent::V_MTE3>(EVENT_ID0);
        WaitFlag<HardEvent::V_MTE3>(EVENT_ID0);

        DataCopy(outputGlobal, outputLocal, dataSize);
        PipeBarrier<PIPE_ALL>();
        ```
    - 释放LocalMemory空间。
        ```c++
        vecInQue.FreeTensor(inputLocal);
        vecOutQue.FreeTensor(outputLocal);
        vecTmpQue.FreeTensor(tmpLocal);
        ```
3. 编写测试代码
    - 定义入参的数据结构。
        ```c++
        struct AxpyTestParams {
        int32_t dataSize;
        int32_t dataBitSize;
        void (*calFunc)(uint8_t*, uint8_t*, int32_t);
        };
        ```
    - 编写测试类。
        ```c++
        class AxpyTestsuite : public testing::Test, public testing::WithParamInterface<AxpyTestParams> {
        protected:
            void SetUp() {}
            void TearDown() {}
        };
        ``` 
    - 注入用例数据。
        ```c++
        INSTANTIATE_TEST_CASE_P(TEST_AXPY, AxpyTestsuite,
            ::testing::Values(AxpyTestParams{256, 2, AxpyKernel<half, half>}, 
                AxpyTestParams{256, 4, AxpyKernel<float, float>}));
        ```
    - 编写`TEST_P`。
        ```c++
        TEST_P(AxpyTestsuite, AxpyTestCase)
        {
            auto param = GetParam();
            uint8_t srcGm[param.dataSize * param.dataBitSize] = {0};
            uint8_t dstGm[param.dataSize * param.dataBitSize] = {0};

            param.calFunc(srcGm, dstGm, param.dataSize);
            for (int32_t i = 0; i < param.dataSize; i++) { EXPECT_EQ(dstGm[i], 0x00); }
        }
        ```
###### Tiling侧

Tiling接口的UT目前统一在[tests/api/adv_api/tiling/test_tiling.cpp](../../tests/api/adv_api/tiling/test_tiling.cpp)文件中，在该文件中添加相应测试函数即可。
```c++
TEST_F(TestTiling, TestAxpyTiling)
{
    uint32_t maxVal = 0;
    uint32_t minVal = 0;
    GetAxpyMaxMinTmpSize(AscendC::TensorShape({128}), 4, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 0);
    EXPECT_EQ(minVal, 0);
    GetAxpyMaxMinTmpSize(AscendC::TensorShape({256}), 2, false, maxVal, minVal);
    EXPECT_EQ(maxVal, 256 * 4 * 2);
    EXPECT_EQ(minVal, 256 * 4);
}
```
##### 修改cmake文件
在执行UT用例前，需要修改[CMakeLists.txt](../../tests/api/adv_api/CMakeLists.txt)文件，由于核函数（Kernel）侧和Tiling侧的UT执行对象不同，需要在不同的target下添加测试文件。以核函数（Kernel）侧验证Atlas A2 训练系列产品/Atlas A2 推理系列产品的UT为例，将UT测试文件路径添加到用例的源文件列表`ASCENDC_TEST_ASCEND910B1_AIV_CASE_SRC_PART_FILES`中，即新增文件路径`${ASCENDC_TESTS_DIR}/math/axpy/test_operator_axpy.cpp`。Tiling侧同理，需要将测试文件添加到`ASCENDC_TILING_TEST_SRC_FILES`列表。
##### 执行UT
- 执行全量UT用例
  
  进入仓库主目录，执行如下命令进行测试。
  ```bash
  bash build.sh -t
  ```
- 仅运行新增UT用例

  打开[tests/main_global.cpp](../../tests/main_global.cpp)文件，在main函数中return前的最后一行添加下面的代码，利用gTest的过滤器根据单元测试的单元名字过滤测试用例。
    ```c++
    ::testing::GTEST_FLAG(filter) = "*Axpy*";
    ```
  修改[build.sh](../../build.sh)，将all修改为需要执行的UT target，以执行核函数（Kernel）侧Ascend 910B1的UT为例，target即为ascendc_utest_ascend910B1_AIV。
  ```
  function build_test() {
    cmake_config
    # build all
    build ascendc_utest_ascend910B1_AIV
  }
  ```
  执行命令`bash build.sh -t`即可仅运行新增UT用例。
#### 单算子测试
完成高阶API编码后，通过实现算子功能，在算子中调用该API，来测试API的功能，具体参考如下步骤。
- 编译安装。

  将新增或修改的API源码编译并安装到环境中，具体方式请参考[编译安装](./quick_start.md#compile&install)。
- 创建简易自定义算子工程并测试。

  自定义开发算子，创建简易自定义算子工程后，通过调用单算子测试API功能。关于算子开发和简易自定义算子工程的详细内容请参考[Ascend C编程指南](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC)。


## 合入代码
当开发者完成高阶API的编码和测试后，请参考[贡献指南](../../CONTRIBUTING.md)将代码合入本仓库。
