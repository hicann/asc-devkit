# Selecting Low-Latency Instructions to Optimize Reduction Operation Performance<a name="ZH-CN_TOPIC_0000002049326341"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T14:27:54.368Z -->

[Priority] High

[Description]

Instruction execution latency refers to the time consumed from the start of an instruction's execution to its complete finish (that is, when all operations end and the result becomes available). It directly affects the response speed and real-time performance of a program. In latency-sensitive scenarios, reducing instruction execution latency is key to improving performance. The following uses reduction operations as an example to introduce the performance comparison of several reduction approaches, helping developers select a higher-performance approach based on the specific data scale and scenario when using reduction instructions.

## Comparison Between the Binary Accumulation Approach and the Reduction Instruction Approach<a name="section179410573522"></a>

According to the single-instruction performance test data (which developers can test by themselves), the latency of reduction instructions such as ReduceRepeat is about 2 to 5 times that of the Add instruction. Therefore, for reduction operations on contiguous data, a combination of the Add instruction and the ReduceRepeat instruction can be used to optimize overall performance. This approach is referred to as the **binary accumulation approach**, and it is described as follows:

-   Binary accumulation: Split the data into two halves and use the Add instruction to add the two halves together. Split the resulting data into two halves again and continue accumulating with the Add instruction. Repeat this process.
-   When the amount of data after binary accumulation is less than or equal to 256 bytes (the amount of data processed by one Repeat in a single instruction), use the ReduceRepeat instruction to obtain the reduction result in a single execution.

Assume that the input data type is float and the shape is \(5, 256\). The following figure shows the execution process for one row of data:

**Figure 1**  Diagram of the binary accumulation approach<a name="fig181871717152010"></a>  
![](../../../figures/reduce_best_practice.png "Diagram of the binary accumulation approach")

Perform the preceding process for each row separately to obtain the final reduction result. That is, for data with a shape of \(m, k\), the shape becomes \(m, 1\) after reduction.

Because the ReduceSum API is implemented by combining multiple instructions, in general, for scenarios with a large amount of data and many loops, the performance of the binary accumulation approach \> the performance of the single-instruction ReduceRepeat operation \> the performance of the ReduceSum API. For scenarios with a small amount of data or special shapes, the approaches need to be analyzed separately based on conditions such as instruction execution time and the number of executed instructions.

The following provides the core code snippets and performance data comparison between the binary accumulation approach and the reduction instruction approach. For the complete sample, see [ReduceCustom](../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce).

[Performance Data]

When the input shape is 30000 and the data type is float, the performance data comparison is as follows. The data unit is cycle, obtained by calling the GetSystemCycle API.

<a name="table1240852315316"></a>
<table><thead align="left"><tr id="row140811238537"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p12319202885319"><a name="p12319202885319"></a><a name="p12319202885319"></a>Binary Accumulation Approach</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p1788511331538"><a name="p1788511331538"></a><a name="p1788511331538"></a>ReduceRepeat Single-Instruction Operation</p></th>
</tr>
</thead>
<tbody><tr id="row54086232538"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1329224035317"><a name="p1329224035317"></a><a name="p1329224035317"></a>172</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1129116401539"><a name="p1129116401539"></a><a name="p1129116401539"></a>242</p></td>
</tr>
</tbody>
</table>

[Binary Accumulation Approach]

```
__aicore__ inline void BinaryReduceSumImpl(const AscendC::LocalTensor<float>& dst, const AscendC::LocalTensor<float>& src, const uint32_t bsLength, const uint32_t hLength)
{
    // src is two-dimensional data with the shape (bsLength, hLength), and dst has the shape (bsLength, 1)
    AscendC::BinaryRepeatParams binaryParams;
    AscendC::UnaryRepeatParams unaryParams;
    AscendC::SetMaskCount();
    for (uint32_t i = 0; i < bsLength; i++) {
        AscendC::LocalTensor<float> srcTmp = src[i * hLength];
        AscendC::LocalTensor<float> dstTmp = dst[i * hLength];
        uint32_t totalNum = hLength / 16 * 16;
        uint32_t remaining = hLength - totalNum;
        AscendC::LocalTensor<float> remainingTensor = srcTmp[totalNum];
        while (totalNum > ONE_REPEAT_FLOAT_SIZE) {
            uint32_t halfNum = AscendC::DivCeil(totalNum, 16) * DEFAULT_REP_STRIDE;
            AscendC::SetVectorMask<uint8_t, AscendC::MaskMode::COUNTER>(0, totalNum - halfNum);
            AscendC::Add<float, false>(dstTmp, srcTmp, srcTmp[halfNum], AscendC::MASK_PLACEHOLDER, 1, binaryParams);
            totalNum = halfNum;
            srcTmp = dstTmp;
        }
        if (remaining != 0 && hLength > ONE_REPEAT_FLOAT_SIZE) {
            AscendC::SetVectorMask<uint8_t, AscendC::MaskMode::COUNTER>(0, remaining);
            AscendC::Add<float, false>(dstTmp, dstTmp, remainingTensor, AscendC::MASK_PLACEHOLDER, 1, binaryParams);
        }
        AscendC::SetVectorMask<uint8_t, AscendC::MaskMode::COUNTER>(0, totalNum);
        AscendC::ReduceRepeat<AscendC::ReduceType::SUM, float, float, false>(dstTmp, srcTmp, AscendC::MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE,
            DEFAULT_BLK_STRIDE, DEFAULT_REP_STRIDE);
    }
    AscendC::ResetMask();
    AscendC::SetMaskNorm();
}
```

[ReduceRepeat Single-Instruction Operation]

```
__aicore__ inline void ReduceRepeatSumImpl(const AscendC::LocalTensor<float>& dst, const AscendC::LocalTensor<float>& src, const uint32_t bsLength, const uint32_t hLength)
{ 
    // src is two-dimensional data with the shape (bsLength, hLength), and dst has the shape (bsLength, 1)
    AscendC::SetMaskCount();
    for (uint32_t i = 0; i < bsLength; i++) {
        uint32_t totalNum = hLength;
        AscendC::LocalTensor<float> srcTmp = src[i * hLength];
        AscendC::LocalTensor<float> dstTmp = dst[i * hLength];
        while (totalNum > 1) {
            AscendC::SetVectorMask<uint8_t, AscendC::MaskMode::COUNTER>(0, totalNum);
            AscendC::ReduceRepeat<AscendC::ReduceType::SUM, float, float, false>(dstTmp, srcTmp, AscendC::MASK_PLACEHOLDER, 1, DEFAULT_BLK_STRIDE,
                DEFAULT_BLK_STRIDE, DEFAULT_REP_STRIDE);
            totalNum = AscendC::DivCeil(totalNum, ONE_REPEAT_FLOAT_SIZE);
            srcTmp = dstTmp;
        }
    }
    AscendC::ResetMask();
    AscendC::SetMaskNorm();
}
```

## Comparison Between ReduceDataBlock and ReduceRepeat Reduction Approaches<a name="section18620646762"></a>

Further testing and analysis show that the execution efficiency of a single ReduceDataBlock instruction is higher than that of ReduceRepeat. Therefore, based on different shapes, better execution performance can be achieved by combining different instructions.

For example, for data of the float type with a shape size of 256, the reduction result can be obtained in the following three ways:

-   Use ReduceRepeat twice;
-   Use ReduceDataBlock three times;
-   Use one ReduceDataBlock operation plus one ReduceRepeat operation.

By analyzing the performance data of a single instruction (which developers can test on their own), it can be seen that the performance of one ReduceDataBlock operation plus one ReduceRepeat operation is better than that of using ReduceRepeat twice, and is also better than the approach of using ReduceDataBlock three times.

The following provides the core code snippets and performance data comparison of the three approaches above. For the complete sample, see [ReduceCustom](../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce).

[Performance Data]

The input shape is 256 and the data type is float. The performance data of the following examples is as follows:

**Table 1** Performance data of three reduction operations: twice ReduceRepeat, three times ReduceDataBlock, and once ReduceDataBlock plus once ReduceRepeat (total time of 100 loops)

<a name="table174575114592"></a>
<table><thead align="left"><tr id="row3758185125915"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="p57581151175915"><a name="p57581151175915"></a><a name="p57581151175915"></a>Twice ReduceRepeat</p></th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="p1960391014012"><a name="p1960391014012"></a><a name="p1960391014012"></a>Three Times ReduceDataBlock</p></th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="p2758451105915"><a name="p2758451105915"></a><a name="p2758451105915"></a>Once ReduceDataBlock Plus Once ReduceRepeat</p></th>
</tr>
</thead>
<tbody><tr id="row275855115912"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p075810518599"><a name="p075810518599"></a><a name="p075810518599"></a>13us</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p8758155165918"><a name="p8758155165918"></a><a name="p8758155165918"></a>13.94us</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p4758165105910"><a name="p4758165105910"></a><a name="p4758165105910"></a>8.44us</p></td>
</tr>
</tbody>
</table>

[Twice ReduceRepeat Operations]

```
...
pipe.InitBuffer(calcBuf, totalLength * sizeof(DTYPE));
AscendC::LocalTensor<DTYPE> tempTensor1 = calcBuf.Get<DTYPE>();
const uint32_t repeatNum = (totalLength * sizeof(DTYPE) + REP_LEN - 1) / REP_LEN;
AscendC::SetMaskCount();
AscendC::SetVectorMask<DTYPE>(0, totalLength);
AscendC::ReduceRepeat<AscendC::ReduceType::SUM, DTYPE, DTYPE, false>(tempTensor1, xLocal, AscendC::MASK_PLACEHOLDER, 1,
    DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REP_STRIDE);
AscendC::PipeBarrier<PIPE_V>();
AscendC::SetVectorMask<DTYPE>(0, repeatNum);
AscendC::ReduceRepeat<AscendC::ReduceType::SUM, DTYPE, DTYPE, false>(zLocal, tempTensor1, AscendC::MASK_PLACEHOLDER, 1,
    DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REP_STRIDE);
AscendC::PipeBarrier<PIPE_V>();
AscendC::SetMaskNorm();
...
```

[Three ReduceDataBlock Operations]

```
...
static constexpr uint32_t BLK_LEN = 32;
TBuf<TPosition::VECCALC> calcBuf;
constexpr uint32_t c0Count = BLK_LEN / sizeof(DTYPE_X);
const uint32_t blockNum0 = (totalLength + c0Count - 1) / c0Count;
const uint32_t blockNum1 = (blockNum0 + c0Count - 1) / c0Count;
AscendC::SetMaskCount();
AscendC::SetVectorMask<DTYPE_X>(0, totalLength);
AscendC::ReduceDataBlock<AscendC::ReduceType::SUM, DTYPE_X, DTYPE_X, false>(tempTensor1, xLocal, AscendC::MASK_PLACEHOLDER, 1,
    DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REP_STRIDE);
AscendC::PipeBarrier<PIPE_V>();
AscendC::SetVectorMask<DTYPE_X>(0, blockNum0);
AscendC::ReduceDataBlock<AscendC::ReduceType::SUM, DTYPE_X, DTYPE_X, false>(tempTensor1, tempTensor1, AscendC::MASK_PLACEHOLDER, 1,
    DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REP_STRIDE);
AscendC::PipeBarrier<PIPE_V>();
AscendC::SetVectorMask<DTYPE_X>(0, blockNum1);
AscendC::ReduceDataBlock<AscendC::ReduceType::SUM, DTYPE_X, DTYPE_X, false>(zLocal, tempTensor1, AscendC::MASK_PLACEHOLDER, 1,
    DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REP_STRIDE);
AscendC::PipeBarrier<PIPE_V>();
AscendC::SetMaskNorm();
...
```

[ReduceDataBlock + ReduceRepeat Operations]

```
...
pipe.InitBuffer(calcBuf, totalLength * sizeof(DTYPE));
AscendC::LocalTensor<DTYPE> tempTensor1 = calcBuf.Get<DTYPE>();
constexpr uint32_t c0Count = BLK_LEN / sizeof(DTYPE);
const uint32_t blockNum0 = (totalLength + c0Count - 1) / c0Count;
AscendC::SetMaskCount();
AscendC::SetVectorMask<DTYPE>(0, totalLength);
AscendC::ReduceDataBlock<AscendC::ReduceType::SUM, DTYPE, DTYPE, false>(tempTensor1, xLocal, AscendC::MASK_PLACEHOLDER, 1,
    DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REP_STRIDE);
AscendC::PipeBarrier<PIPE_V>();
AscendC::SetVectorMask<DTYPE>(0, blockNum0);
AscendC::ReduceRepeat<AscendC::ReduceType::SUM, DTYPE, DTYPE, false>(zLocal, tempTensor1, AscendC::MASK_PLACEHOLDER, 1,
    DEFAULT_BLK_STRIDE, DEFAULT_BLK_STRIDE, DEFAULT_REP_STRIDE);
AscendC::PipeBarrier<PIPE_V>();
AscendC::SetMaskNorm();
...
```
