# Flexible Use of Counter Mode in Vector Operators<a name="ZH-CN_TOPIC_0000001989723914"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:28:14.480Z -->

[Priority] High

[Description] In Normal mode, the amount of data computed by the vector computation APIs in Vector operators is controlled by the iteration count repeatTimes and the mask. When you want to specify the total element count to be computed by an API, you first need to determine whether there are distinct main blocks and tail blocks. For the main block, you need to set the mask so that all elements participate in the computation, and calculate the number of iterations required for the main block. Then, based on the number of remaining elements in the tail block, you reset the mask and perform the tail block computation. This process involves a large amount of Scalar computation.

In Counter mode, you do not need to calculate the iteration count or determine whether a tail block exists. After setting the mask mode to Counter mode, you only need to set the mask to \{0, total element count\}, and then call the corresponding API. The processing logic is simpler, which reduces the number of instructions and the amount of Scalar computation, while making more efficient use of the concurrency capability of a single instruction execution, thereby improving performance.

Note: For details about Normal mode, Counter mode, and masks, see [How to Use Mask Operation APIs](../../../programming_guide/library_api/basic_api/quick_reference/how_to_use_mask_operation_api.md).

The code in the following counterexample and positive example uses the [AddCustom](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/02_features/99_acl_based/00_acl_compilation/custom_op) operator as an example. The code that calls the Add API is modified to illustrate the advantages of Counter mode.

```
AscendC::Add(zLocal, xLocal, yLocal, this->tileLength);
```

[Negative Example]

The input data type is half for xLocal and yLocal, and the data volume is 15000 for each. In Normal mode, the maximum number of elements participating in the computation in each iteration is 256B/sizeof\(half\)=128. Therefore, the 15000 Add computations are divided as follows: the main block performs 15000/128=117 iterations, with 128 elements participating in the computation in each iteration; the tail block performs 1 iteration, with 15000-117\*128=24 elements participating in the computation. From the code perspective, you need to calculate the repeatTimes of the main block and the element count of the tail block. When computing the main block, set the mask value to 128; when computing the tail block, set the mask value to the tail block element count 24. All these processes involve Scalar computation.

```
uint32_t ELE_SIZE = 15000;
AscendC::BinaryRepeatParams binaryParams;

uint32_t numPerRepeat = 256 / sizeof(DTYPE_X);  // DTYPE_X is the half data type
uint32_t mainRepeatTimes = ELE_SIZE / numPerRepeat;
uint32_t tailEleNum = ELE_SIZE % numPerRepeat;

AscendC::SetMaskNorm();
AscendC::SetVectorMask<DTYPE_X, AscendC::MaskMode::NORMAL>(numPerRepeat); // Set the Normal mode mask so that each iteration computes 128 elements
AscendC::Add<DTYPE_X, false>(zLocal, xLocal, yLocal, AscendC::MASK_PLACEHOLDER, mainRepeatTimes, binaryParams);   // The value of MASK_PLACEHOLDER is 0. It is a mask placeholder here, and the actual mask value is determined by SetVectorMask.
if (tailEleNum > 0) {
     AscendC::SetVectorMask<DTYPE_X, AscendC::MaskMode::NORMAL>(tailEleNum); // Set the mask in Normal mode so that 24 elements are computed in each iteration.
     // Offset the start address of the tensor to the 14976th element of xLocal and yLocal to perform the tail block computation.
     AscendC::Add<DTYPE_X, false>(zLocal[mainRepeatTimes * numPerRepeat], xLocal[mainRepeatTimes * numPerRepeat], 
           yLocal[mainRepeatTimes * numPerRepeat], AscendC::MASK_PLACEHOLDER, 1, binaryParams);  
}
AscendC::ResetMask();  // Restore the mask value.
```

[Positive Example]

The input data type of xLocal and yLocal is half, and both contain 15000 elements. In Counter mode, you only need to set the mask to 15000, the total number of elements involved in the computation, and then directly call the Add instruction to complete all computations. There is no need for the cumbersome main block and tail block computation, making the code more concise.

When a vector computation involving up to 15000 elements is required, the advantage of Counter mode becomes more evident. It eliminates the need to repeatedly modify the different mask values for the main block and tail block, reduces the number of instructions and the amount of Scalar computation, and fully utilizes the concurrency capability of a single instruction execution.

```
uint32_t ELE_SIZE = 15000;
AscendC::BinaryRepeatParams binaryParams;
AscendC::SetMaskCount();
AscendC::SetVectorMask<DTYPE_X, AscendC::MaskMode::COUNTER>(ELE_SIZE);  // Set the mask in Counter mode to compute 15000 elements in total.
AscendC::Add<DTYPE_X, false>(zLocal, xLocal, yLocal, AscendC::MASK_PLACEHOLDER, 1, binaryParams);                // The value of MASK_PLACEHOLDER is 0. It is a mask placeholder here, and the actual mask value is determined by SetVectorMask.
AscendC::ResetMask();  // Restore the mask value.
```

[Performance Comparison]

**Figure 1**  Scalar execution time comparison between Normal mode and Counter mode<a name="fig7838115891613"></a>  
![](../../../figures/scalar_execution_time_comparison_between_normal_mode_and_counter_mode.png "Scalar execution time comparison between Normal mode and Counter mode")

**Figure 2**  Vector execution time comparison between Normal mode and Counter mode<a name="fig1223382352110"></a>  
![](../../../figures/vector_execution_time_comparison_between_normal_mode_and_counter_mode.png "Vector execution time comparison between Normal mode and Counter mode")

The preceding performance data is the Scalar and Vector execution time obtained by running the counterexample and positive example code 1000 times in a loop. As shown in the two performance comparison figures and the sample code, using Counter mode greatly simplifies the code and makes it easier to maintain, while reducing the Scalar and Vector computation time and improving performance.
