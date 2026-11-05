# PhiloxRandom

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T10:49:26.492Z pushedAt=2026-10-29T02:14:34.541Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/random_functions/PhiloxRandom_res.md#id1 -->

## Description

Generates a specified number of random numbers based on the Philox random number generation algorithm and a given random seed.

The core of the Philox random number generation algorithm is a counter-based pseudo-random number generation algorithm. It takes a 128-bit counter C and two 32-bit keys (k0 and k1) as input and outputs four 32-bit integers.

## Prototype

-   Continuous mode

    ```
    template <uint16_t Rounds = 7, typename T>
    __aicore__ inline void PhiloxRandom(const LocalTensor<T>& dstLocal, const PhiloxKey& philoxKey, const PhiloxCounter& philoxCounter, uint16_t count)
    ```

-   Stride mode

    ```
    template <uint16_t Rounds = 7, typename T>
    __aicore__ inline void PhiloxRandom(const LocalTensor<T>& dstLocal, const PhiloxKey& philoxKey, const PhiloxCounter& philoxCounter, const PhiloxRandomParams& params)
    ```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| Rounds | Number of iterations in the internal implementation of the Philox algorithm. Supported values are 7 and 10. |
| T | Data type of the destination operand. Supported data types are uint32_t, int32_t, and float.<br><br>uint32_t and int32_t follow a uniform distribution over their respective data type ranges, and float follows a uniform distribution in the range of 0 to 1. |

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of the LocalTensor must be 32-byte aligned. |
| philoxKey | Input | Random seed. Two 32-bit keys, defined as follows:<br>using PhiloxKey = uint32_t[2]; |
| philoxCounter | Input | Random seed. A 128-bit counter C (consisting of four 32-bit values), defined as follows:<br>using PhiloxCounter = uint32_t[4]; |
| count | Input | Number of elements to generate in the destination operand. |
| params | Input | Parameter information required for stride mode computation. The type is PhiloxRandomParams, defined in the following code. The parameters are described as follows:<br>stride: interval between elements of two rows.<br>row: number of rows to generate.<br>column: number of elements in each generated row.<br>Note:<br>row * column must be greater than 0 and not greater than the size of the LocalTensor.<br>column % 4 == 0, stride % 4 == 0, and stride >= column. |

```
struct PhiloxRandomParams {
    uint32_t stride;
    uint32_t row;
    uint32_t column;
};
```

**Figure 1**  PhiloxRandom diagram  
![](../../../figures/philoxrandom_diagram.png "PhiloxRandom diagram")

The preceding figure is a diagram of random number generation.

-   In continuous mode, use philoxCounter=\{0, 0, 0, 0\} with count=32 to generate 32 random numbers.
-   In stride mode, you can generate data in two passes by column, calling the API twice. The first call uses philoxCounter=\{0, 0, 0, 0\}, with stride=8, row=4, and column=4; the second call uses philoxCounter=\{1, 0, 0, 0\} (each auto-increment of counter C generates a 128-bit random number), with stride=8, row=4, and column=4.

## Return Value

None

## Constraints

None

## Examples

For the complete operator example, see [PhiloxRandom Examples](../../../../../../examples/01_simd_cpp_api/04_advanced_api/09_random/philoxrandom).

```
// dstLocal: Tensor for storing the computation result.
// philoxKey={0,0}, philoxCounter={0,0,0,0}

// stride mode, generating 32*32 elements.
PhiloxRandom<10>(dstLocal, philoxKey, philoxCounter, params);
// Continuous mode, generating 1024 elements.
PhiloxRandom<10>(dstLocal, philoxKey, philoxCounter, 1024);
```

The result is as follows:

```
[0.31179297 0.8263413  0.6849456 ... 0.10521233 0.29894042 0.96700084]
```
