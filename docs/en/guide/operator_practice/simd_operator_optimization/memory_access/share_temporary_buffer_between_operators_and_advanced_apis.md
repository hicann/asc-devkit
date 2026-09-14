# Sharing Temporary Buffers Between Operators and Advanced APIs<a name="ZH-CN_TOPIC_0000001893038929"></a>

<!-- md-trans-meta sourceCommit=51eb188d02e4be270d9dc56141153e3f75345dc0 translatedAt=2026-08-26T14:21:46.255Z -->

[Priority] High

[Description] If a high-level API used by an operator requires a temporary buffer, such as SoftMax, the temporary space occupies the space used for other computations of the operator, reducing the amount of data moved per computation and increasing the number of moves. In this scenario, sharing the temporary buffer space increases the amount of data moved per move, reduces the number of moves, and improves memory usage efficiency.

[Negative Example]

The SoftMax high-level API requires temporary buffer space for computation, and the operator has an independent temporary buffer when performing other computations. The UB space is fixed. Assume that the temporary space that can be allocated to SoftMax and Add is 64 KB, and the temporary buffer space tmpSoftmaxBuf required by SoftMax occupies 32 KB. In this case, the LocalTensor tmpSumBuf that stores the Add computation result can be allocated at most 32 KB. If the amount of data computed by src0Tensor is 512 KB, the data needs to be moved 512 / 32 = **16** times.

```
...
constexpr int32_t blockLen = 32 * 1024;
TBuf<TPosition::VECCALC> tmpSoftmaxBuf; 
pipe.InitBuffer(tmpSoftmaxBuf, softmaxBufSize * sizeof(uint8_t));  // Allocate a 32 KB temporary buffer for Softmax separately.
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

[Positive Example]

The SoftMax high-level API requires temporary buffer space for computation, and the operator can share this temporary buffer when performing other computations. Based on the preceding assumption, the data needs to be moved only 512 / 64 = **8** times.

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

