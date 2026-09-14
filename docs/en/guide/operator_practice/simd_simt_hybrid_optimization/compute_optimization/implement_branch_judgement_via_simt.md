# Implementing Branch Judgment Through SIMT<a name="ZH-CN_TOPIC_0000002552896537"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:39:04.921Z -->

>[!NOTE] Note
>This performance optimization suggestion applies to the following models:
>-   Ascend 950PR/Ascend 950DT

[Priority] High

[Description] Batch data computation implemented based on the SIMD programming model delivers high performance. However, when branch judgment is involved in the operator implementation logic, SIMD-based computation operations become relatively complex, leading to performance degradation. In this case, consider using the SIMT approach, because SIMT programming is more flexible and better suited to scenarios involving branch judgment.

[Example Introduction] Take the floor\_mod operator as an example. The operator divides each element of input x by the corresponding element of input y and obtains the remainder. The remainder should have the same sign as the divisor y, and its absolute value should be smaller than the absolute value of y. During computation, it is necessary to determine the sign of each element in y and the magnitude relationship between the remainder and the absolute value of that element. For the complete operator implementation code, see [SIMT and SIMD Hybrid Programming High-Performance Optimization Example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/05_simd_simt_hybrid/02_best_practices/simd_simt_high_performance).

**Table 1**  Example specifications

<table>
<thead>
<tr>
<th>Name</th>
<th>Name</th>
<th>Shape</th>
<th>Data Type</th>
<th>Format</th>
</tr>
</thead>
<tbody>
<tr>
<td rowspan="2">Operator Input</td>
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
<td>Operator Output</td>
<td>z</td>
<td>[8192, 8192]</td>
<td>int32</td>
<td>ND</td>
</tr>
</tbody>
</table>

The SIMT thread hierarchy is as follows:

-   Kernel startup core count: 64
-   Number of threads per SIMT VF call: 1024

[Negative Example]

SIMD RegBase-based floor\_mod operator implementation: corresponding to scenario 1 (SCENARIO\_NUM=1) in the sample. This scenario uses DataCopy to move data between GM and UB, and uses the RegBase VF function on UB for computation. Because SIMD cannot directly express element-wise branch judgment through ordinary if else statements, multiple Reg vector computation APIs are required to complete sign comparison, condition combination, and result selection. The relevant code is as follows.

```cpp
template <typename T>
__simd_vf__ inline void floor_mod_simd(__ubuf__ T* zAddr, __ubuf__ T* xAddr, __ubuf__ T* yAddr, const uint32_t count)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(T);
    uint16_t loopTimes = AscendC::CeilDivision(count, oneRepeatSize);
    AscendC::Reg::RegTensor<T> xValue;
    AscendC::Reg::RegTensor<T> yValue;
    AscendC::Reg::RegTensor<T> modValue;
    AscendC::Reg::RegTensor<T> tempValue;
    AscendC::Reg::RegTensor<T> defaultValue;
    AscendC::Reg::RegTensor<T> signValue;

    AscendC::Reg::MaskReg mask;
    AscendC::Reg::MaskReg selectMask;
    AscendC::Reg::MaskReg adjustMask;
    uint32_t maskCount = count;

    AscendC::Reg::Duplicate(defaultValue, T(-1));
    AscendC::Reg::Duplicate(signValue, FMOD_B32_SIGN);

    for (uint16_t i = 0; i < loopTimes; i++) {
        mask = AscendC::Reg::UpdateMask<T>(maskCount);
        AscendC::Reg::LoadAlign(xValue, xAddr + i * oneRepeatSize);
        AscendC::Reg::LoadAlign(yValue, yAddr + i * oneRepeatSize);

        AscendC::Reg::Div(tempValue, xValue, yValue, mask);
        AscendC::Reg::Mul(tempValue, yValue, tempValue, mask);
        AscendC::Reg::Sub(modValue, xValue, tempValue, mask);

        AscendC::Reg::Compares<T, AscendC::CMPMODE::NE>(selectMask, yValue, T(0), mask);
        AscendC::Reg::Select(tempValue, modValue, defaultValue, selectMask);

        AscendC::Reg::Add(modValue, tempValue, yValue, mask);
        AscendC::Reg::Compares<T, AscendC::CMPMODE::NE>(adjustMask, tempValue, T(0), mask);
        AscendC::Reg::And(xValue, tempValue, signValue, mask);
        AscendC::Reg::And(yValue, yValue, signValue, mask);
        AscendC::Reg::Compare<T, AscendC::CMPMODE::NE>(selectMask, xValue, yValue, mask);
        AscendC::Reg::MaskAnd(adjustMask, selectMask, adjustMask, mask);
        AscendC::Reg::Select(modValue, modValue, tempValue, adjustMask);
        AscendC::Reg::StoreAlign(zAddr + i * oneRepeatSize, modValue, mask);
    }
}
```

[Positive Example]

Implementation of the floor\_mod operator based on SIMT: This corresponds to scenario 3 (SCENARIO\_NUM=3) in the sample. This scenario also uses DataCopy to move data between GM and UB, implements the computation on UB using the SIMT programming model, and performs branch judgment through if else statements. The code is as follows.

```cpp
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
```

[Performance Comparison]

With the same core count, the same input and output specifications, and DataCopy used for data movement between GM and UB, the performance data of scenario 1 and scenario 3 are compared as follows.

| Scenario | Implementation | Core Count | Task Duration\(μs\) | aiv\_vec\_time\(μs\) | aiv\_vec\_ratio | aiv\_mte2\_time\(μs\) | aiv\_mte2\_ratio |
|:---|:---|:---:|---:|---:|---:|---:|---:|
| Scenario 1 | SIMD RegBase | 64 | 532.144 | 523.082 | 0.985 | 237.127 | 0.446 |
| Scenario 3 | SIMT continuous UB access | 64 | 457.402 | 319.948 | 0.701 | 437.758 | 0.959 |

Compared with scenario 1, scenario 3 uses SIMT to directly express the branch judgment logic in floor\_mod. The aiv\_vec\_time decreases from 523.082 μs to 319.948 μs, a reduction of about 38.8%, indicating that the Vector computation time is significantly reduced. The end-to-end Task Duration decreases from 532.144 μs to 457.402 μs, a reduction of about 14.0%. The aiv\_mte2\_ratio of scenario 3 reaches 0.959, meaning the main bottleneck has shifted to MTE2 Bound, that is, the greatest impact on operator performance has shifted to the efficiency of data movement from GM to UB.
