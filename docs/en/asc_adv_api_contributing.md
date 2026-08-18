# API Contribution Guide
## Overview
High-level API (Advanced API) is an abstraction and encapsulation of common algorithms based on single-core, used to improve operator development efficiency. High-level API is implemented by encapsulating basic APIs, mainly including math library, Matmul, quantization/dequantization, data normalization, and other APIs.

Developers contribute to this repository mainly in two scenarios: one is custom development of brand-new high-level APIs, and the other is advanced development by modifying existing high-level APIs in the repository. After completing API development based on either scenario, developers can compile and deploy the API to the CANN software environment, then use it in operator programs.

Before reading this document, ensure you have understood the Ascend AI processor hardware architecture and have studied Ascend C programming related documents or courses. Recommend mastering the knowledge in [Ascend C Programming Guide](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC). This document uses the `Axpy` interface in the high-level API math library as an example to introduce API development for both scenarios.

----

## Development Process
Ascend C high-level API development process mainly includes the following steps:
- Design API
- Develop API
  - Write API external interface
  - Write API internal implementation
- Test
  - UT testing
  - Single operator testing
---
## Custom Development of Brand-new API
Below uses high-level API `Axpy` as an example to introduce how to develop a high-level API from scratch. This case removes some unnecessary code. You can view all code in the repository files [axpy.h](../../include/adv_api/math/axpy.h), [axpy_tiling.h](../../include/adv_api/math/axpy_tiling.h), [axpy_tiling_intf.h](../../include/adv_api/math/axpy_tiling_intf.h), [axpy_common_impl.h](../../impl/adv_api/detail/math/axpy/axpy_common_impl.h), [axpy_tiling_impl.cpp](../../impl/adv_api/tiling/math/axpy_tiling_impl.cpp).
### Design API
Axpy's function is to multiply each element in source operand `srcTensor` with a scalar, then add it with the corresponding element in destination operand `dstTensor`. The calculation formula is as follows.

$$
dstTensor_i = srcTensor_i \times scalarValue+dstTensor_i
$$

- Kernel Function Side

    Consider the data flow during operator execution. Host side applies for GlobalMemory space and writes data into it. On the kernel function side, data is moved from GlobalMemory to LocalMemory. Compute unit fetches data from LocalMemory for computation, writes results back to LocalMemory, and finally moves computation results from LocalMemory to GlobalMemory. High-level API completes the computation part, so high-level API source operands and destination operands are both in LocalMemory. That is, high-level API interface parameters include LocalTensor with already moved-in data and other parameters.

    From `Axpy` formula, the interface needs input parameters `srcTensor` and `scalarValue`, output parameter `dstTensor`, and also needs an input parameter `calCount` indicating the number of elements participating in computation. Since intermediate results of multiplication are generated during this computation, extra LocalMemory space needs to be allocated for storing intermediate computation results. To pass this temporary buffer space in the interface, add a `shardTmpBuffer` input parameter. To extend API supported data types, define two template parameters as source operand and destination operand data types. The template parameter `isReuseSource` is a reserved parameter and can be omitted during development. After analyzing and determining all interface parameters based on API algorithm function or formula, the following kernel function prototype is obtained.

    After defining the function prototype, analyze and determine the basic APIs used in API implementation. The multiplication operation between Tensor and Scalar in `Axpy` formula can be implemented using Ascend C provided basic API `Muls`. The addition operation between two Tensors can be implemented using Ascend C provided basic API `Add`.

    ```c++
    template <typename T, typename U, bool isReuseSource = false>
    __aicore__ inline void Axpy(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```
    Template parameter description
    | Parameter Name | Description |
    | ----------- | ----------- |
    | T | Destination operand data type, supported data types are half/float. |
    | U | Source operand data type, supported data types are half/float. |
    | isReuseSource | This parameter is reserved, pass default value false. |

    Interface parameter description
    | Parameter Name | Input/Output | Description |
    | ----------- | ----------- |----------- |
    | dstTensor | Output | Destination operand. |
    | srcTensor | Input | Source operand. |
    | scalarValue | Input | Scalar value. Supported data types are half/float. Scalar operand type needs to match srcTensor. |
    | sharedTmpBuffer | Input | Temporary buffer. Type is LocalTensor, supported TPosition is VECIN/VECCALC/VECOUT. Since the internal implementation of this interface needs extra temporary space to store intermediate variables during computation. Temporary space needs to be passed by developer through sharedTmpBuffer parameter. |
    | calCount | Input | Number of elements participating in computation. |
- Tiling Side

    The kernel function interface requires developers to reserve/apply for temporary space. The size of this temporary space needs to be calculated on Tiling side based on obtained source operand shape size, computing high-level API required maximum (maxValue) temporary space and minimum temporary space (minValue) sizes. Therefore, Tiling side provides an interface for computing maxValue and minValue. Interface input parameters include source operand Tensor shape size and source operand data type byte size. Shape size parameter uses AscendC::TensorShape type, data type byte size uses `uint32_t` type. Output parameters include minValue and maxValue. Similar to isReuseSource parameter in Axpy interface, isReuseSource in Tiling interface is a reserved parameter.
    ```c++
    void GetAxpyMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);
    ```
    Interface parameter description
    | Parameter Name | Input/Output | Description |
    | ----------- | ----------- |----------- |
    | srcShape | Input | Input shape information. |
    | typeSize | Input | Operator input data type size, in bytes. For example, if operator input data type is half, pass 2 here. |
    | isReuseSource | Input | Reserved parameter. |
    | maxValue | Output | Maximum temporary space size required by Axpy interface to complete computation. Space exceeding this value will not be used by the interface. Within the minimum temporary space to maximum temporary space range, as temporary space increases, the computation performance of the kernel function interface will improve. To achieve better performance, developers can apply for space based on actual memory usage. Maximum space size of 0 indicates computation does not need temporary space. |
    | minValue | Output | Minimum temporary space size required by Axpy interface to complete computation. To ensure correct functionality, temporary space applied for during interface computation cannot be smaller than this value. Minimum space size of 0 indicates computation does not need temporary space. |
### Develop API
#### Write API External Interface
- Kernel Function Side Interface
    
    In the corresponding category directory under `include/adv_api/`, add new file [axpy.h](../../include/adv_api/math/axpy.h). Based on the above analyzed and designed API function prototype, write external interface code. Function implementation calls AxpyImpl implementation.
    ```c++
    template <typename T, typename U, bool isReuseSource = false>
    __aicore__ inline void Axpy(const LocalTensor<T>& dstTensor, const LocalTensor<U>& srcTensor, const U scalarValue,
        const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    {
        AxpyImpl<T, U, isReuseSource>(dstTensor, srcTensor, scalarValue, sharedTmpBuffer, calCount);
    }
    ```
- Tiling Side Interface
    
    In the corresponding category directory under `include/adv_api/`, add new file [axpy_tiling.h](../../include/adv_api/math/axpy_tiling.h). Based on the above analyzed and designed Tiling side interface, write function declaration.
    ```c++
    #include "../utils/types.h"

    void GetAxpyMaxMinTmpSize(const AscendC::TensorShape& srcShape, const uint32_t typeSize, const bool isReuseSource, uint32_t& maxValue, uint32_t& minValue);
    ```
- Include header files in common files.

    [include/adv_api/kernel_api.h](../../include/adv_api/kernel_api.h) file includes all high-level API header files. Recommend including new API header files in this file, so when calling high-level APIs, only need to include `"kernel_api.h"`.
    ```c++
    #if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201)
    // ...
    #include "math/axpy.h"
    // ...
    #endif // __NPU_ARCH__ == 1001 || __NPU_ARCH__ == 2002 || __NPU_ARCH__ == 2201
    ```
    
    [include/adv_api/tiling_api.h](../../include/adv_api/tiling_api.h) file includes all high-level API Tiling interface header files. Recommend including new Tiling interface header files in this file, so when calling high-level API Tiling functions, only need to include `"tiling_api.h"`.
    ```c++
    #include "math/axpy_tiling.h"
    ```

#### Write API Internal Implementation
- Kernel Function Side Implementation

    In the kernel function interface implementation path [impl/adv_api/detail](../../impl/adv_api/detail), add a new `axpy` directory under the corresponding category directory (`math` in this case). In this directory, add new interface implementation file [axpy_common_impl.h](../../impl/adv_api/detail/math/axpy/axpy_common_impl.h), then write interface implementation code in this implementation file.

    First, include necessary header files.
    ```c++
    #include "kernel_tensor.h"
    ```
    Write AxpyImpl function implementation code. If source operand data type is float, call basic API Axpy interface. Otherwise, call custom function `AxpySub`, using Muls and Add combination computation to provide better computation precision.
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
    `AxpySub` function determines computation strategy based on temporary space size, mainly includes determining computation loop count round, full block computation amount stackSize, tail block computation amount tail. Specific code is as follows. Since sharedTmpBuffer passed to interface is uint8 type while source operand elements are float type, need to convert sharedTmpBuffer element type to float to get element count. Based on hardware architecture characteristics, minimum computation unit is one Block, that is 32 bytes. `axpyTmpCalc` function aligns down the element count that temporary space can store to 32 bytes, getting full block computation amount stackSize for one computation. Meanwhile, when destination operand element type is half, to improve precision, Cast precision conversion is done for source operand and destination operand respectively during computation. Half of temporary space is occupied by Cast results. So in this scenario, computation data amount is half of the element count that temporary space can store after aligning down to 32 bytes. Given total computation amount calCount, single full block computation amount stackSize, compute loop count round and tail block computation amount tail. Calculation formula is as follows:
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
    If tail is not 0, that is tail block exists, need extra processing for tail block data. Before tail block data computation, set compute unit working mode to Count mode through setMaskCount interface.
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
    `AxpyIntrinsicsImpl` function calls basic API to implement Axpy specific algorithm. For different destination operand data types, overload implement `AxpyIntrinsicsImpl` function. The following function implementation code is for source operand and destination operand element type both being half scenario. As described above, temporary space stackBuffer can hold $2 * stackSize$ elements, half of temporary space is used to save source operand and destination operand Cast results. Therefore, Tensor `tmpDst` saving destination operand Cast result has starting offset stackSize. During algorithm implementation, Cast conversion from half to float type is done for source operand and destination operand respectively, then call Muls, Add interfaces for computation, finally Cast conversion from float to half type for computation results.
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
- Tiling Side Implementation

    In Tiling interface implementation file path [impl/adv_api/tiling](../../impl/adv_api/tiling) corresponding category directory (this case is `math`), add new interface implementation file [axpy_tiling_impl.cpp](../../impl/adv_api/tiling/math/axpy_tiling_impl.cpp), then write interface implementation code in this implementation file.

    First, include necessary header files.
    ```c++
    #include "lib/math/axpy_tiling.h"  // Tiling interface header file
    #include <cstdint>  // Type library
    #include "impl/host_log.h" // Log library
    ```
    Define constants used in Tiling computation.
    ```c++
    constexpr uint32_t AXPY_HALF_CALC_PROC = 4;
    constexpr uint32_t AXPY_FLOAT_CALC_PROC = 1;
    constexpr uint32_t AXPY_ONE_REPEAT_BYTE_SIZE = 256;
    ```
    In `GetAxpyMaxMinTmpSize` interface, call `GetAxpyMaxTmpSize` interface to get required temporary space maximum value, call `GetAxpyMinTmpSize` interface to get required temporary space minimum value.
   
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
    Analyze the computation of required temporary space maximum value. When destination operand data type is float, only source operand needs Cast conversion. `Axpy` pseudo code is as follows, using one times element count inputSize of temporary space.
    ```
    Cast(tmpSrc, srcTensor)
    Muls(tmpSrc, dstTensor, scalar)
    Add(dstTensor, tmpSrc, dstTensor)
    ```
    When destination operand data type is half, both destination operand and source operand need Cast conversion. `Axpy` pseudo code is as follows, using four times element count inputSize of temporary space.
    ```
    Cast(tmp1, srcTensor)
    Cast(tmp2, dstTensor)
    Muls(tmp3, tmp1, scalar)
    Add(tmp4, tmp2, tmp3)
    Cast(dstTensor, tmp4)
    ```
    Considering Vector compute unit single Repeat computes 256 bytes of data, the maximum temporary space calculation formula is as follows:

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
    Analyze the computation of required temporary space minimum value. If temporary space is minimum, single computation data amount is minimum, that is each computation data amount is fixed as single Repeat processed data size, 256 bytes. The minimum temporary space calculation formula is as follows:

    $$
    MinValue=\begin{cases}\tiny AXPY\_ONE\_REPEAT\_BYTE\_SIZE*AXPY\_HALF\_CALC\_PROC &dstType = half\\\tiny AXPY\_ONE\_REPEAT\_BYTE\_SIZE*AXPY\_FLOAT\_CALC\_PROC &dstType = float\end{cases}
    $$
    ```c++
    inline uint32_t GetAxpyMinTmpSize(const uint32_t typeSize)
    {
        return AXPY_ONE_REPEAT_BYTE_SIZE * (typeSize == sizeof(float) ? AXPY_FLOAT_CALC_PROC : AXPY_HALF_CALC_PROC);
    }
    ```
    After writing Tiling side implementation file, need to include this file in [impl/adv_api/tiling/CMakeLists.txt](../../impl/adv_api/tiling/CMakeLists.txt). Specifically: add file path `${CMAKE_CURRENT_SOURCE_DIR}/math/axpy_tiling_impl.cpp` in `add_library(tiling_api STATIC ...)` statement.
## Advanced Development Based on Existing API
If developers perform advanced feature development based on existing APIs in current repository, the design API process is the same as described above. When developing code, consider reloading existing function implementations or adding new code branches. Based on advanced feature algorithm function, determine needed basic APIs and complete coding. For example, if existing API does not support some data types, suppose Axpy does not support destination operand element as float type, then need to overload `AxpyIntrinsicsImpl` function to implement dstTensor data type as float function.
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

### Test
After completing API coding, developers can write corresponding test cases and test code to verify compilation is normal and API function is correct. Testing includes UT testing and single operator testing by building simple custom operator project.
#### UT Testing
UT testing uses gTest as testing framework, generally verifies interface compilation is normal and cannot guard interface function.
##### UT Coding
###### Kernel Function Side

In UT directory [tests/api/adv_api/math](../../tests/api/adv_api/math), add new directory `axpy`, file [test_operator_axpy.cpp](../../tests/api/adv_api/math/axpy/test_operator_axpy.cpp). UT implementation mainly includes the following three parts:
1. Include header files
    ```c++
    #include <gtest/gtest.h>
    #include "kernel_operator.h"
    ``` 
2. Implement operator
    - Initialize variables.
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
    - Copy data from GlobalMemory to LocalMemory.
        ```c++
        LocalTensor<U> inputLocal = vecInQue.AllocTensor<U>();
        LocalTensor<T> outputLocal = vecOutQue.AllocTensor<T>();
        LocalTensor<uint8_t> tmpLocal = vecTmpQue.AllocTensor<uint8_t>();

        SetVectorMask<uint8_t, MaskMode::NORMAL>(256);
        DataCopy(inputLocal, inputGlobal, dataSize);
        ```
    - Call high-level API for computation.
        ```c++
        SetFlag<HardEvent::MTE2_V>(EVENT_ID0);
        SetVectorMask<uint8_t, MaskMode::NORMAL>(128);
        WaitFlag<HardEvent::MTE2_V>(EVENT_ID0);
        U scalar = 4;

        Axpy<T, U, false>(outputLocal, inputLocal, scalar, tmpLocal, dataSize);
        ```
    - Copy data from LocalMemory to GlobalMemory.
        ```c++
        SetFlag<HardEvent::V_MTE3>(EVENT_ID0);
        WaitFlag<HardEvent::V_MTE3>(EVENT_ID0);

        DataCopy(outputGlobal, outputLocal, dataSize);
        PipeBarrier<PIPE_ALL>();
        ```
    - Free LocalMemory space.
        ```c++
        vecInQue.FreeTensor(inputLocal);
        vecOutQue.FreeTensor(outputLocal);
        vecTmpQue.FreeTensor(tmpLocal);
        ```
3. Write test code
    - Define input parameter data structure.
        ```c++
        struct AxpyTestParams {
        int32_t dataSize;
        int32_t dataBitSize;
        void (*calFunc)(uint8_t*, uint8_t*, int32_t);
        };
        ```
    - Write test class.
        ```c++
        class AxpyTestsuite : public testing::Test, public testing::WithParamInterface<AxpyTestParams> {
        protected:
            void SetUp() {}
            void TearDown() {}
        };
        ``` 
    - Inject case data.
        ```c++
        INSTANTIATE_TEST_CASE_P(TEST_AXPY, AxpyTestsuite,
            ::testing::Values(AxpyTestParams{256, 2, AxpyKernel<half, half>}, 
                AxpyTestParams{256, 4, AxpyKernel<float, float>}));
        ```
    - Write `TEST_P`.
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
###### Tiling Side

Tiling interface UT is currently unified in [tests/api/adv_api/tiling/test_tiling.cpp](../../tests/api/adv_api/tiling/test_tiling.cpp) file. Add corresponding test function in this file.
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
##### Modify cmake file
Before executing UT cases, modify the [CMakeLists.txt](../../tests/api/adv_api/CMakeLists.txt) file. Since kernel function-side UTs and Tiling-side UTs have different test targets, add the test files to their respective targets. Using the kernel function-side UT for Atlas A2 training series products/Atlas A2 inference series products as an example, add the UT test file path to the case source file list `ASCENDC_TEST_ASCEND910B1_AIV_CASE_SRC_PART_FILES`, that is, add the new file path `${ASCENDC_TESTS_DIR}/math/axpy/test_operator_axpy.cpp`. For the Tiling-side UT, add the test file to the `ASCENDC_TILING_TEST_SRC_FILES` list.
##### Execute UT
- Execute all UT cases
  
  Enter repository main directory, execute the following command for testing.
  ```bash
  bash build.sh -t
  ```
- Run only new UT cases

  Open [tests/main_global.cpp](../../tests/main_global.cpp) file, add the following code in the last line before return in main function. Use gTest filter to filter test cases based on unit test unit name.
    ```c++
    ::testing::GTEST_FLAG(filter) = "*Axpy*";
    ```
  Modify [build.sh](../../build.sh), change all to needed UT target. Using the Ascend 910B1 kernel function-side UT as an example, the target is ascendc_utest_ascend910B1_AIV.
  ```
  function build_test() {
    cmake_config
    # build all
    build ascendc_utest_ascend910B1_AIV
  }
  ```
  Execute command `bash build.sh -t` to run only new UT cases.
#### Single Operator Testing
After completing high-level API coding, test API functionality by implementing operator function and calling the API in the operator. Refer to the following steps.
- Compile and install.

  Compile and install new or modified API source code to environment. For specific method, refer to [Compile and Install](./quick_start.md#compile&install).
- Create simple custom operator project and test.

  Custom develop operator, after creating simple custom operator project, test API functionality by calling single operator. For detailed content about operator development and simple custom operator project, refer to [Ascend C Programming Guide](https://www.hiascend.com/document/redirect/CannCommunityOpdevAscendC).


## Merge Code
When developers complete high-level API coding and testing, please refer to [Contribution Guide](../../CONTRIBUTING_en.md) to merge code into this repository.
