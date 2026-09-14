# Improving Memory Access Efficiency with Unified Buffer<a name="ZH-CN_TOPIC_0000002511242528"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:39:20.716Z -->

>[!NOTE] Note
>This performance optimization suggestion applies to the following models:
>-   Ascend 950PR/Ascend 950DT

[Priority] High

[Description] When SIMT threads directly access Global Memory, the memory access requests go through the GM access path, and data transfer and thread computation are coupled in the same SIMT logic, which tends to reduce the overall memory access efficiency. When the amount of data processed at a time can fit into the maximum available Unified Buffer space (256KB - 8KB reserved by the system - 32KB minimum Dcache), you can use the SIMD transfer API to continuously move data from Global Memory to Unified Buffer, so that SIMT programming can directly access the data in Unified Buffer. This improves memory access efficiency and the overall performance of the operator.

[Example Introduction] Take the floor\_mod operator implemented with SIMD and SIMT hybrid programming as an example. The input x and y of this operator both have a shape of \[8192, 8192\] and a data type of int32, and the output z has a shape of \[8192, 8192\]. For the complete example, see the [SIMT and SIMD hybrid programming high-performance optimization example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance).

**Table 1**  Example specifications

<table>
<thead>
<tr>
<th>Name</th>
<th>Name</th>
<th>Shape</th>
<th>Data Yype</th>
<th>Format</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">Operator input</td>
<td>x</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
<tr>
<td>y</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
<tr>
<td>Operator output</td>
<td>z</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
</tbody>
</table>

The SIMT thread hierarchy is as follows:

-   Number of cores started by the kernel: 64
-   Number of threads per SIMT VF call: 1024

[Negative Example]

SIMT directly accesses the x, y, and z data in Global Memory, corresponding to scenario 0 in the sample (SCENARIO\_NUM=0). In this scenario, data is not transferred between GM and UB through DataCopy. Instead, SIMT threads directly read input from GM and write results back to GM. The code is as follows.

```cpp
template <typename T>
__simt_vf__ inline void floor_mod_simt_gm_contiguous(
    __gm__ T* x, __gm__ T* y, __gm__ T* z, uint32_t inputTotalLength)
{
    for (uint32_t index = static_cast<uint32_t>(threadIdx.x); index < inputTotalLength;
         index += static_cast<uint32_t>(blockDim.x)) {
        T yValue = y[index];
        const auto rem = x[index] % yValue;
        bool signsDiffer = ((rem < 0) != (yValue < 0));
        if (signsDiffer && (rem != 0)) {
            z[index] = rem + yValue;
        } else {
            z[index] = rem;
        }
    }
}

__aicore__ inline void ProcessGmSimt()
{
    asc_vf_call<floor_mod_simt_gm_contiguous<T>>(
        dim3(THREAD_COUNT), const_cast<__gm__ T*>(xGm.GetPhyAddr()),
        const_cast<__gm__ T*>(yGm.GetPhyAddr()), const_cast<__gm__ T*>(zGm.GetPhyAddr()),
        DataLenPerCore);
}
```

[Positive Example]

Use the SIMD API DataCopy to continuously copy x and y from Global Memory to Unified Buffer, read data from and write data to Unified Buffer directly in the SIMT programming mode, and then write the results back to Global Memory through DataCopy. This corresponds to scenario 3 (SCENARIO\_NUM=3) in the sample. The code is as follows.

```cpp
__aicore__ inline void CopyIn(
    uint32_t tileIdx, uint32_t count, AscendC::LocalTensor<T>& xLocal, AscendC::LocalTensor<T>& yLocal)
{
    uint32_t tileOffset = tileIdx * TileLength;
    AscendC::DataCopy(xLocal, xGm[tileOffset], count);
    AscendC::DataCopy(yLocal, yGm[tileOffset], count);
}

template <typename T>
__simt_vf__ inline void floor_mod_simt_contiguous(
    __ubuf__ T* x, __ubuf__ T* y, __ubuf__ T* z, uint32_t inputTotalLength)
{
    for (uint32_t index = static_cast<uint32_t>(threadIdx.x); index < inputTotalLength;
         index += static_cast<uint32_t>(blockDim.x)) {
        T yValue = y[index];
        const auto rem = x[index] % yValue;
        bool signsDiffer = ((rem < 0) != (yValue < 0));
        if (signsDiffer && (rem != 0)) {
            z[index] = rem + yValue;
        } else {
            z[index] = rem;
        }
    }
}

__aicore__ inline void Compute(
    uint32_t count, AscendC::LocalTensor<T>& xLocal, AscendC::LocalTensor<T>& yLocal,
    AscendC::LocalTensor<T>& zLocal)
{
    __ubuf__ T* xAddr = (__ubuf__ T*)xLocal.GetPhyAddr();
    __ubuf__ T* yAddr = (__ubuf__ T*)yLocal.GetPhyAddr();
    __ubuf__ T* zAddr = (__ubuf__ T*)zLocal.GetPhyAddr();

    asc_vf_call<floor_mod_simt_contiguous<T>>(dim3(THREAD_COUNT), xAddr, yAddr, zAddr, count);
    AscendC::DataSyncBarrier<AscendC::MemDsbT::UB>();
}

__aicore__ inline void CopyOut(uint32_t tileIdx, uint32_t count, AscendC::LocalTensor<T>& zLocal)
{
    uint32_t tileOffset = tileIdx * TileLength;
    AscendC::DataCopy(zGm[tileOffset], zLocal, count);
}
```

[Performance Comparison]

The following table compares the performance of scenario 0 and scenario 3.

| Scenario | Implementation | Core Count | Task Duration\(μs\) | aiv\_vec\_time\(μs\) | aiv\_mte2\_time\(μs\) | aiv\_mte3\_time\(μs\) |
|:---|:---|:---:|---:|---:|---:|---:|
| Scenario 0 | SIMT direct access to GM | 64 | 867.222 | 863.175 | 0.005 | 0.003 |
| Scenario 3 | SIMT contiguous access to UB | 64 | 457.402 | 319.948 | 437.758 | 110.623 |

In scenario 0, aiv\_mte2\_time and aiv\_mte3\_time are close to 0, indicating that data reads and writes do not go through the DataCopy path from GM to UB or from UB to GM, and the time is mainly consumed by SIMT direct access to GM and computation. In scenario 3, inputs and outputs are continuously copied between GM and UB through DataCopy, and SIMT threads directly access UB to complete the computation. As a result, Task Duration decreases from 867.222 μs to 457.402 μs, and the end-to-end time drops by about 47.3%.
