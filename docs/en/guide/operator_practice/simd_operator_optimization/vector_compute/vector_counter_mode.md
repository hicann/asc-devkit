# Flexibly Using the Counter Mode in Vector Operators<a name="ZH-CN_TOPIC_0000001989723914"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:55:40.979Z pushedAt=2026-11-03T07:05:13.354Z nl=2 -->

[Priority] High

[Description] In Normal mode, the amount of data computed by the vector computation APIs in a Vector operator is controlled by the iteration count repeatTimes and the mask. When you want to specify the total element count for an API, you first need to determine whether there are different main blocks and tail blocks. For the main block, you need to set the mask so that all elements participate in the computation, and calculate the number of iterations required for the main block. Then, you reset the mask based on the number of remaining elements in the tail block and perform the tail block computation. This process involves a large amount of Scalar computation.

In counter mode, you do not need to calculate the iteration count or determine whether a tail block exists. After setting the mask mode to Counter mode, you only need to set the mask to \{0, total element count \} and then call the corresponding API. The processing logic is simpler, which reduces the number of instructions and the amount of Scalar computation, and more efficiently utilizes the concurrency capability of a single instruction execution, thereby improving performance.

Note: For details about Normal mode, Counter mode, and masks, see [How to Use Mask APIs](../../../programming_guide/library_api/basic_api/quick_reference/how_to_use_mask_api.md).

The code in the following negative example and positive example uses the [AddCustom](../../../../../../examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op) operator as an example, and modifies the call code of the Add API to illustrate the advantages of Counter mode.

```
AscendC::Add(zLocal, xLocal, yLocal, this->tileLength);
```

[Negative example]

The input data types are half for xLocal and yLocal, and the data volume of each is 15000. In Normal mode, the maximum number of elements participating in the computation in each iteration is 256B/sizeof\(half\)=128, so the 15000 Add computations are divided into: the main block performs 15000/128=117 iterations, with 128 elements participating in the computation in each iteration; the tail block performs 1 iteration, with 15000-117\*128=24 elements participating in the computation. From the code perspective, you need to calculate the repeatTimes of the main block and the number of elements in the tail block. When computing the main block, set the mask value to 128. When computing the tail block, set the mask value to the number of elements in the tail block, which is 24. These processes all involve Scalar computation.

```
uint32_t ELE_SIZE = 15000;
AscendC::BinaryRepeatParams binaryParams;

uint32_t numPerRepeat = 256 / sizeof(DTYPE_X);  // DTYPE_X is of the half data type.
uint32_t mainRepeatTimes = ELE_SIZE / numPerRepeat;
uint32_t tailEleNum = ELE_SIZE % numPerRepeat;

AscendC::SetMaskNorm();
AscendC::SetVectorMask<DTYPE_X, AscendC::MaskMode::NORMAL>(numPerRepeat); // Set the mask in Normal mode so that each iteration computes 128 elements.
AscendC::Add<DTYPE_X, false>(zLocal, xLocal, yLocal, AscendC::MASK_PLACEHOLDER, mainRepeatTimes, binaryParams);   // The value of MASK_PLACEHOLDER is 0. It is a mask placeholder here, and the actual mask value is subject to the one set by SetVectorMask.
if (tailEleNum > 0) {
     AscendC::SetVectorMask<DTYPE_X, AscendC::MaskMode::NORMAL>(tailEleNum); // Set the normal mode mask so that 24 numbers are calculated in each iteration.
     // Offset the start address of the tensor to the 14976th element of xLocal and yLocal for tail block calculation.
     AscendC::Add<DTYPE_X, false>(zLocal[mainRepeatTimes * numPerRepeat], xLocal[mainRepeatTimes * numPerRepeat], 
           yLocal[mainRepeatTimes * numPerRepeat], AscendC::MASK_PLACEHOLDER, 1, binaryParams);  
}
AscendC::ResetMask();  // Restore the mask value.
```

[Positive example]

The input data type is half for xLocal and yLocal, and the data volume is 15000 for both. In Counter mode, you only need to set the mask to 15000, the total number of elements involved in the calculation, and then directly call the Add instruction to complete all calculations. There is no need for the cumbersome main block and tail block calculation, making the code more concise.

When vector calculation of up to 15000 elements is to be processed, the advantages of Counter mode are more obvious. It eliminates the need to repeatedly modify the different mask values of the main block and the tail block, reduces the number of instructions and the amount of Scalar computation, and fully utilizes the concurrency capability of a single instruction execution.

```
uint32_t ELE_SIZE = 15000;
AscendC::BinaryRepeatParams binaryParams;
AscendC::SetMaskCount();
AscendC::SetVectorMask<DTYPE_X, AscendC::MaskMode::COUNTER>(ELE_SIZE);  // Set the counter mode mask to calculate 15000 numbers in total.
AscendC::Add<DTYPE_X, false>(zLocal, xLocal, yLocal, AscendC::MASK_PLACEHOLDER, 1, binaryParams);                // The value of MASK_PLACEHOLDER is 0. It is a mask placeholder here, and the actual mask value is subject to the one set by SetVectorMask.
AscendC::ResetMask();  // Restore the mask value.
```

[Performance comparison]

**Figure 1**  Comparison of scalar execution time between normal mode and counter mode<a name="fig7838115891613"></a>  
![](../../../figures/scalar_time.png "Comparison of Scalar execution time between Normal mode and Counter mode")

**Figure 2**  Comparison of vector execution time between normal mode and counter mode<a name="fig1223382352110"></a>  
![](../../../figures/vector_time.png "Comparison of Vector execution time between Normal mode and Counter mode")

The preceding performance data is obtained by running the negative example and positive example code 1000 times each to measure the Scalar and Vector execution time. As shown in the two performance comparison figures and the sample code, the Counter mode greatly simplifies the code and makes it easier to maintain, while reducing the Scalar and Vector computation time and improving performance.
