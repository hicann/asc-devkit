# Efficient Use of the Copy API<a name="ZH-CN_TOPIC_0000001846719836"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:19:18.147Z -->

[Priority] High

[Description] When using the copy API, configure the copy control parameters to implement contiguous copy or fixed-interval copy as much as possible, and avoid using a for loop. The efficiency difference between the two approaches is significant. In the following example, each row of the image is 16 KB, and the first 2 KB of each row needs to be copied. In this scenario, using a for loop to traverse each row can copy only **2** KB at a time. If you directly configure the DataCopyParams parameters (including srcStride/dstStride/blockLen/blockCount), all data can be copied at once, with **32** KB copied each time. For details about the relationship between the amount of copied data and the actual bandwidth, see [Copy a Large Data Block at a Time](transfer_larger_data_blocks_at_once.md). It is recommended to copy all data at once.

**Figure 1** Data layout to be copied<a name="fig147153323541"></a>  
![](../../../figures/data_layout_to_be_moved.png "Data layout to be copied")

[Negative Example]

```
// Copy data with intervals: copy 2 KB of data from each 16 KB row in Global Memory, 16 rows in total
LocalTensor<float> tensorIn;
GlobalTensor<float> tensorGM;
...
constexpr int32_t copyWidth = 2 * 1024 / sizeof(float);
constexpr int32_t imgWidth = 16 * 1024 / sizeof(float);
constexpr int32_t imgHeight = 16;
// Use a for loop to copy only 2 KB each time, repeated 16 times
for (int i = 0; i < imgHeight; i++) {
    DataCopy(tensorIn[i * copyWidth], tensorGM[i * imgWidth], copyWidth);
}
```

[Positive Example]

```
LocalTensor<float> tensorIn;
GlobalTensor<float> tensorGM;
...
constexpr int32_t copyWidth = 2 * 1024 / sizeof(float);
constexpr int32_t imgWidth = 16 * 1024 / sizeof(float);
constexpr int32_t imgHeight = 16;
// Copy all data at once through the DataCopy API that includes DataCopyParams
DataCopyParams copyParams;
copyParams.blockCount = imgHeight;
copyParams.blockLen = copyWidth / 8; // The copy unit is DataBlock (32 bytes), and each DataBlock contains 8 floats
copyParams.srcStride = (imgWidth  - copyWidth) / 8; // Indicates the interval between two copy operations on src, in DataBlock
copyParams.dstStride = 0; // Continuous write, the interval between two copy operations on dst is 0, in DataBlock
DataCopy(tensorGM, tensorIn, copyParams);
```

