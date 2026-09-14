# How to Use Temporary Space on the Kernel Side<a name="ZH-CN_TOPIC_0000002522740421"></a>

<!-- md-trans-meta sourceCommit=b68361bf94b609fef8765a3982175a9d58850963 translatedAt=2026-08-26T12:21:05.124Z pushedAt=2026-09-03T11:49:17.271Z -->

The internal implementation of kernel-side APIs generally involves complex mathematical computation and requires additional temporary space to store intermediate variables during computation. Except for cube computation, HCCL communication, and convolution computation, for temporary space handling in most high-level APIs, developers can pass pre-allocated temporary space through the sharedTmpBuffer input parameter of the kernel-side API, or allocate temporary space through the API framework.

-   Pass temporary space through the **sharedTmpBuffer** input parameter. The kernel-side API uses the passed tensor as the temporary space. In this mode, developers can manage the sharedTmpBuffer memory space by themselves and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   Allocate temporary space through the API framework. Developers do not need to allocate temporary space on the kernel side, but they need to reserve the size of the temporary space. That is, when allocating memory space, the size of the temporary space to be reserved should be subtracted from the available space size.

Regardless of which mode developers use, when allocating tensor space or reserving temporary space, they need to obtain the temporary space size BufferSize required by the kernel-side API in advance. For this purpose, the **GetxxxMaxMinTmpSize** API is provided in the corresponding category of APIs to obtain the size range of the space to be reserved, where xxx is the corresponding kernel-side API. Developers call the **GetxxxMaxMinTmpSize** API on the host side to obtain the maximum and minimum temporary space sizes in bytes to be reserved/allocated, and select an appropriate space size within this range as the tiling parameter to be passed to the kernel side.

-   To ensure functional correctness, the reserved/allocated temporary space size must not be smaller than the minimum temporary space size.
-   Within the range from the minimum temporary space size to the maximum temporary space size, as the temporary space increases, the computation performance of the kernel-side API is optimized to a certain extent. To achieve better performance, developers can reserve/allocate space based on the actual memory usage.

Take the Asin API as an example:

```
// The data type T of the operator input is half, and isReuseSource uses the default value false.
auto shape_input = context->GetInputTensor(0)->GetOriginShape();    
std::vector<int64_t> srcDims = {shape_input.GetDim(0), shape_input.GetDim(1)};
uint32_t srcSize = 1;
for (auto dim : srcDims) {
    srcSize *= dim;
}
uint32_t typeSize = 2;
ge::Shape shape(srcDims);
uint32_t minValue = 0;
uint32_t maxValue = 0;
AscendC::GetAsinMaxMinTmpSize(shape, typeSize, false, maxValue, minValue);

auto platformInfo = context->GetPlatformInfo();
auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
uint64_t tailSize = 0; // Size of the remaining space in UB.
ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, tailSize); // This sample uses the full UB space. In actual scenarios, tailSize needs to subtract the UB space already used by the user.
auto tmpSize = tailSize >= maxValue ? maxValue : tailSize;

AsinCustomTilingData tiling;
tiling.set_tmpBufferSize(tmpSize); // Set the temporary space size as a tiling parameter.
```

In addition, most high-level APIs provide the **GetxxxTmpBufferFactorSize** API, which is used to obtain **maxLiveNodeCnt** and **extraBuf**. **maxLiveNodeCnt** indicates how many times the temporary space is the space occupied by the data volume of a single computation; **extraBuf** indicates the number of bytes of temporary space required by the kernel-side API. With a fixed space size, **maxLiveNodeCnt** and **extraBuf** can be used to infer the maximum number of elements that the operator can compute at a time.

The following is an example of the inference:

-   The operator implementation needs to call the **Mean** API. The developer reserves a space of currBuff size (that is, the total available space) for it, and uses the **GetMeanTmpBufferFactorSize** API to obtain the output values **maxLiveNodeCnt** and **extraBuf**. The maximum number of elements that the operator can compute at a time can be inferred as follows:

    currentShapeSize = \(currBuff - extraBuf\) / maxLiveNodeCnt / typeSize

-   The operator implementation needs to call two kernel-side APIs **KernelIntf1** and **KernelIntf2**. Using the two sets of output values \(maxLiveNodeCnt, extraBuf\) from the two **GetXxxTmpBufferFactorSize** APIs (where Xxx is the two high-level APIs to be called) and the current temporary space currBuff, the maximum number of elements **currentShapeSize** that can be computed at a time is inferred as follows:

    currentShapeSize1 = \(currBuff - extraBuf1\) / maxLiveNodeCnt1 / typeSize

    currentShapeSize2 = \(currBuff - extraBuf2\) / maxLiveNodeCnt2 / typeSize

    currentShapeSize = min\(currentShapeSize1 , currentShapeSize2\)

Note that currBuff in the preceding text indicates the space available for API computation, from which the space occupied by user input and output must be excluded.

Take an operator that needs to call both the **Asin** and **Acos** APIs as an example:

```
// The input data type T of the operator is half.
auto shape_input = context->GetInputTensor(0)->GetOriginShape();
std::vector<int64_t> srcDims = { shape_input.GetDim(0), shape_input.GetDim(1) };
uint32_t srcSize = 1;
uint32_t srcCurSize = 1;
for (auto dim : srcDims) {
    srcSize *= dim;
}
uint32_t typeSize = 2;

auto platformInfo = context->GetPlatformInfo();
auto ascendcPlatform = platform_ascendc::PlatformAscendC(platformInfo);
uint64_t tailSize = 0; // Remaining space size of UB
ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, tailSize);

uint32_t asinMaxLiveNodeCount = 0;
uint32_t asinExtraBuf = 0;

uint32_t acosMaxLiveNodeCount = 0;
uint32_t acosExtraBuf = 0;

AscendC::GetAsinTmpBufferFactorSize(typeSize, asinMaxLiveNodeCount, asinExtraBuf);
AscendC::GetAcosTmpBufferFactorSize(typeSize, acosMaxLiveNodeCount, acosExtraBuf);
// The size of tmp must subtract the space occupied by the input and output of the API called on UB.
// In this example, the input and output of the Asin API and the input and output of the Acos API are included. The output of the Asin API serves as the input of the Acos API, so a total of three copies of src space size are required.
auto tmpSize = tailSize - srcSize * typeSize * 3;
assert(tmpSize >= asinExtraBuf);
assert(tmpSize >= acosExtraBuf);
// Calculate the maximum number of elements that can be computed in a single call of the Asin operator.
if (asinMaxLiveNodeCount != 0) {
    srcAsinCurSize = (tmpSize - asinExtraBuf) / asinMaxLiveNodeCount / typeSize;
} else {
    srcAsinCurSize = srcSize;
}
// Calculate the maximum number of elements that can be computed in a single call of the Acos operator.
if (acosMaxLiveNodeCount != 0) {
    srcAcosCurSize = (tmpSize - acosExtraBuf) / acosMaxLiveNodeCount / typeSize; 
} else {
    srcAcosCurSize = srcSize;
}
srcCurSize = std::min(srcAsinCurSize, srcAcosCurSize);

AsinCustomTilingData tiling;
tiling.set_srcCurSize(srcCurSize); // Set the maximum number of elements that can be computed in a single call as a tiling parameter.
```
