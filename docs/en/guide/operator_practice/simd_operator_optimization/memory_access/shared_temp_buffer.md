# Sharing Temporary Buffers Between Operators and Advanced APIs<a name="ZH-CN_TOPIC_0000001893038929"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T12:42:42.849Z pushedAt=2026-11-03T07:05:13.337Z nl=2 -->

[Priority] High

[Description] If a advanced API used by an operator requires a temporary buffer, such as SoftMax, the temporary space occupies the space used for other computations of the operator, reducing the amount of data moved in a single computation and increasing the number of moves. In this scenario, you can share the temporary buffer space to increase the amount of data moved in a single move, reduce the number of moves, and improve memory usage efficiency.

[Negative example]

The SoftMax advanced API requires temporary buffer space for computation, and the operator has an independent temporary buffer when performing other computations. The Unified Buffer (UB) space is fixed. Assume that the temporary space that can be allocated to SoftMax and Add is 64 KB, and the temporary buffer space tmpSoftmaxBuf required by SoftMax occupies 32 KB. In this case, the LocalTensor tmpSumBuf that stores the Add computation result can be allocated at most 32 KB. If the amount of data computed by src0Tensor is 512 KB, the data needs to be moved 512 / 32 = **16** times.

```
...
constexpr int32_t blockLen = 32 * 1024;
TBuf<TPosition::VECCALC> tmpSoftmaxBuf; 
pipe.InitBuffer(tmpSoftmaxBuf, softmaxBufSize * sizeof(uint8_t));  // Allocate a 32 KB temporary buffer for SoftMax separately.
TBuf<TPosition::VECCALC> tmpSumBuf;
pipe.InitBuffer(tmpSumBuf, sumBufSize * sizeof(T)); // Allocate a temporary buffer for Add separately, where softmaxBufSize * sizeof(uint8_t) + sumBufSize * sizeof(T) <= 64 KB.
...
for (int i = 0; i < 16; i++) {
    ...
    LocalTensor<uint8_t> tmpSoftmaxTensor = tmpSoftmaxBuf.Get<uint8_t>(softmaxBufSize);
    SoftMax<T, true, true>(dstTensor, expSumTensor, dstMaxTensor, srcTensor, tmpSoftmaxTensor, tiling);
    ...
    DataCopy(src0Tensor, src0Gm[i * blockLen / sizeof(T)], Params);
    ...
    LocalTensor<T> tmpSumTensor = tmpSumBuf.Get<T>(sumBufSize);
    Add<T>(tmpSumTensor, src0Tensor, src1Tensor, count);
    ...
}
...
```

[Positive example]

The SoftMax advanced API requires temporary buffer space for computation, and the operator can share this temporary buffer when performing other computations. Based on the preceding assumptions, the data needs to be moved only 512 / 64 = **8** times.

```
...
constexpr int32_t blockLen = 64 * 1024;
TBuf<TPosition::VECCALC> tmpSharedBuf;
pipe.InitBuffer(tmpSharedBuf, bufferSize); // Allocate a shared buffer where bufferSize = MAX(softmaxBufSize * sizeof(uint8_t), sumBufSize * sizeof(T)) <= 64 KB.
...
for (int i = 0; i < 8; i++) {
    ...
    LocalTensor<uint8_t> tmpSharedTensor = tmpSharedBuf.Get<uint8_t>(softmaxBufSize);
    SoftMax<T, true, true>(dstTensor, expSumTensor, dstMaxTensor, srcTensor, tmpSharedTensor, tiling);
    ...
    DataCopy(src0Tensor, src0Gm[i * blockLen / sizeof(T)], Params);
    ...
    LocalTensor<T> tmpSumTensor = tmpSharedBuf.Get<T>(sumBufSize);
    Add<T>(tmpSumTensor, src0Tensor, src1Tensor, count);
    ...
}
...
```

