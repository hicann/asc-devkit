# Selecting Low-Latency Instructions to Optimize Reduction Operation Performance<a name="ZH-CN_TOPIC_0000002049326341"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:53:59.801Z pushedAt=2026-11-03T07:05:13.354Z nl=2 -->

[Priority] High

[Description]

Instruction execution latency refers to the time consumed from the start of an instruction's execution to its complete finish (that is, when all operations end and the result becomes available). It directly affects the response speed and real-time performance of a program. In latency-sensitive scenarios, reducing instruction execution latency is the key to improving performance. Taking the reduction operation as an example, the following describes the performance comparison of several reduction schemes, so that developers can select a higher-performance scheme based on the specific data scale and scenario when using reduction instructions.

## Comparison Between the Binary Accumulation Scheme and Reduction Instruction Schemes<a name="section179410573522"></a>

According to the single-instruction performance test data (which developers can test by themselves), the latency of reduction instructions such as ReduceRepeat is about 2 to 5 times that of the Add instruction. Therefore, for reduction operations on contiguous data, a combination of the Add instruction and the ReduceRepeat instruction can be used to optimize the overall performance. This scheme is referred to as the **binary accumulation scheme**, and is described as follows:

-   Binary accumulation: Divide the data into two parts and use the Add instruction to add the two parts. Divide the result into two parts again and continue to use the Add instruction for accumulation. Repeat this process.
-   When the amount of data after binary accumulation is less than or equal to 256 bytes (the data operation amount of one Repeat in one instruction), use the ReduceRepeat instruction to obtain the reduction result in a single execution.

Assume that the input data type is float and the shape is \(5, 256\). The following figure shows the execution process of one row of data:

**Figure 1** Schematic diagram of the binary accumulation scheme<a name="fig181871717152010"></a>  
![](../../../figures/reduce_best_practice.png "Schematic diagram of the binary accumulation scheme")

Execute the preceding process for each row separately to obtain the final reduction result, that is, data with the shape \(m, k\). After the reduction is complete, the shape is \(m, 1\).

Because the ReduceSum API is implemented by combining multiple instructions, generally, in scenarios with a large amount of data and many loops, the performance of the binary accumulation scheme is \> the performance of the ReduceRepeat single-instruction operation \> the performance of the ReduceSum API. However, in scenarios with a small amount of data or a special shape, the schemes need to be analyzed separately based on conditions such as the instruction execution time and the number of executed instructions.

The following provides the core code snippets and performance data comparison between the binary accumulation scheme and the reduction instruction schemes. For the complete sample, see [ReduceCustom](../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce).

[Performance data]

When the input shape is 30000 and the data type is float, the performance data in the following example is compared as follows. The data unit is cycle, obtained by using the **GetSystemCycle** API.

<a name="table1240852315316"></a>
<table><thead align="left"><tr id="row140811238537"><th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.1"><p id="p12319202885319"><a name="p12319202885319"></a><a name="p12319202885319"></a>Binary Accumulation Scheme</p></th>
<th class="cellrowborder" valign="top" width="50%" id="mcps1.1.3.1.2"><p id="p1788511331538"><a name="p1788511331538"></a><a name="p1788511331538"></a>ReduceRepeat Single-Instruction Operation</p></th>
</tr>
</thead>
<tbody><tr id="row54086232538"><td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.1 "><p id="p1329224035317"><a name="p1329224035317"></a><a name="p1329224035317"></a>172</p></td>
<td class="cellrowborder" valign="top" width="50%" headers="mcps1.1.3.1.2 "><p id="p1129116401539"><a name="p1129116401539"></a><a name="p1129116401539"></a>242</p></td>
</tr>
</tbody>
</table>

[Binary accumulation scheme]

```
__aicore__ inline void BinaryReduceSumImpl(const AscendC::LocalTensor<float>& dst, const AscendC::LocalTensor<float>& src, const uint32_t bsLength, const uint32_t hLength)
{
    // src is two-dimensional data with shape (bsLength, hLength), and dst has shape (bsLength, 1)
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

[ReduceRepeat single-instruction operation]

```
__aicore__ inline void ReduceRepeatSumImpl(const AscendC::LocalTensor<float>& dst, const AscendC::LocalTensor<float>& src, const uint32_t bsLength, const uint32_t hLength)
{ 
    // src is two-dimensional data with shape (bsLength, hLength), and dst has shape (bsLength, 1)
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

## Comparison Between ReduceDataBlock and ReduceRepeat Reduction Schemes<a name="section18620646762"></a>

Further testing and analysis show that the execution efficiency of the single-instruction ReduceDataBlock is higher than that of ReduceRepeat. Therefore, depending on the shape, different instruction combinations can achieve better execution performance.

For example, for data of the float type with a shape size of 256, the reduction result can be obtained in the following three ways:

-   Use ReduceRepeat twice;
-   Use ReduceDataBlock three times;
-   Use one ReduceDataBlock operation plus one ReduceRepeat operation.

By analyzing the single-instruction performance data (which developers can test on their own), it can be seen that the performance of one ReduceDataBlock operation plus one ReduceRepeat operation is better than that of using ReduceRepeat twice, and also better than the scheme of using ReduceDataBlock three times.

The following provides the core code snippets and performance data comparison of the three methods above. For the complete sample, see [ReduceCustom](../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/reduce).

[Performance data]

The input shape is 256 and the data type is float. The performance data of the following example is as follows:

**Table 1** Performance data of three reduction operations: two ReduceRepeat operations, three ReduceDataBlock operations, and one ReduceDataBlock operation plus one ReduceRepeat operation (total time of 100 loops)

<a name="table174575114592"></a>
<table><thead align="left"><tr id="row3758185125915"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.1"><p id="p57581151175915"><a name="p57581151175915"></a><a name="p57581151175915"></a>Two ReduceRepeat Operations</p></th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.2"><p id="p1960391014012"><a name="p1960391014012"></a><a name="p1960391014012"></a>Three ReduceDataBlock Operations</p></th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.2.4.1.3"><p id="p2758451105915"><a name="p2758451105915"></a><a name="p2758451105915"></a>One ReduceDataBlock Operation Plus One ReduceRepeat Operation</p></th>
</tr>
</thead>
<tbody><tr id="row275855115912"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.1 "><p id="p075810518599"><a name="p075810518599"></a><a name="p075810518599"></a>13us</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.2 "><p id="p8758155165918"><a name="p8758155165918"></a><a name="p8758155165918"></a>13.94us</p></td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.2.4.1.3 "><p id="p4758165105910"><a name="p4758165105910"></a><a name="p4758165105910"></a>8.44us</p></td>
</tr>
</tbody>
</table>

[Two ReduceRepeat operations]

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

[Three ReduceDataBlock operations]

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

[ReduceDataBlock + ReduceRepeat operation]

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
