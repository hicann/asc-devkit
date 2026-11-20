# Using Data Movement APIs Efficiently<a name="ZH-CN_TOPIC_0000001846719836"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-11-09T10:35:47.989Z pushedAt=2026-11-19T06:32:19.773Z nl=2 -->

[Priority] High

[Description] When using data transfer APIs, you should configure the transfer control parameters to implement contiguous transfer or fixed-stride transfer as much as possible, and avoid using for loops, because the efficiency difference between the two approaches is significant. In the following example, each row of the image is 16 KB, and the first 2 KB of each row needs to be transferred. In this scenario, using a for loop to traverse each row transfers only **2** KB at a time. If you directly configure the **DataCopyParams** parameters (including **srcStride**/**dstStride**/**blockLen**/**blockCount**), the entire transfer can be completed at once, transferring **32** KB each time. For details about the relationship between the amount of transferred data and the actual bandwidth, see [Transfer Larger Data Blocks at a Time](transfer_larger_data_blocks.md). It is recommended to complete the transfer at once.

**Figure 1** Data layout to be transferred<a name="fig147153323541"></a>  
![](../../../figures/pending_data.png "Data layout to be transferred")

[Negative example]

```
// Data transfer has intervals. Transfer 2 KB of data from each 16 KB row in GM, 16 rows in total
LocalTensor<float> tensorIn;
GlobalTensor<float> tensorGM;
...
constexpr int32_t copyWidth = 2 * 1024 / sizeof(float);
constexpr int32_t imgWidth = 16 * 1024 / sizeof(float);
constexpr int32_t imgHeight = 16;
// Use a for loop to transfer only 2 KB each time, repeated 16 times
for (int i = 0; i < imgHeight; i++) {
    DataCopy(tensorIn[i * copyWidth], tensorGM[i * imgWidth], copyWidth);
}
```

[Positive example]

```
LocalTensor<float> tensorIn;
GlobalTensor<float> tensorGM;
...
constexpr int32_t copyWidth = 2 * 1024 / sizeof(float);
constexpr int32_t imgWidth = 16 * 1024 / sizeof(float);
constexpr int32_t imgHeight = 16;
// Transfer all data at once through the DataCopy API that includes DataCopyParams
DataCopyParams copyParams;
copyParams.blockCount = imgHeight;
copyParams.blockLen = copyWidth / 8; // The transfer unit is DataBlock (32 bytes), and each DataBlock contains 8 floats
copyParams.srcStride = (imgWidth  - copyWidth) / 8; // Interval between two src transfers, in DataBlock
copyParams.dstStride = 0; // Continuous write. The interval between two dst transfers is 0, in DataBlock
DataCopy(tensorGM, tensorIn, copyParams);
```
