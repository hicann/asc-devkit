# PhiloxRandom

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T12:18:15.058Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Not supported
- Atlas A2 training products/Atlas A2 inference products: Not Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Not supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Based on the Philox random number generation algorithm, generates a number of random numbers given a random seed.

The core algorithm of Philox random number generation is a counter-based pseudo-random number generation algorithm. It takes a 128-bit counter C and two 32-bit keys (k0 and k1) as input, and outputs four 32-bit integers.

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
| Rounds | Number of iterations implemented internally by the Philox algorithm. Supported values are 7 or 10. |
| T | Data type of the destination operand. Supported data types are uint32_t, int32_t, and float.<br><br>uint32_t and int32_t follow a uniform distribution over their data type ranges, and float follows a uniform distribution over the range 0 to 1. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The start address of the LocalTensor must be 32-byte aligned. |
| philoxKey | Input | Random seed. Two 32-bit keys, defined as follows:<br>using PhiloxKey = uint32_t[2]; |
| philoxCounter | Input | Random seed. A 128-bit counter C (composed of four 32-bit values), defined as follows:<br>using PhiloxCounter = uint32_t[4]; |
| count | Input | Number of elements to generate in the destination operand. |
| params | Input | Parameter information required for stride mode computation. The type is PhiloxRandomParams, defined as shown in the following code, where the parameters mean:<br>stride: Interval between two rows of elements.<br>row: Number of rows to generate.<br>column: Number of elements in each generated row.<br>Note:<br>row * column must be greater than 0 and not greater than the size of the LocalTensor.<br>column % 4 == 0, stride % 4 == 0, and stride >= column. |

```
struct PhiloxRandomParams {
   uint32_t stride; 
   uint32_t row; 
   uint32_t column; 
}
```

**Figure 1**  PhiloxRandom diagram  
![](../../../figures/PhiloxRandom_diagram.png "PhiloxRandom diagram")

The preceding figure is a schematic diagram of random number generation.

-   In continuous mode, use philoxCounter=\{0, 0, 0, 0\} with count=32 to generate 32 random numbers.
-   In stride mode, generation can be performed in two column-wise passes by calling the API twice. The first call uses philoxCounter=\{0, 0, 0, 0\} with stride=8, row=4, and column=4; the second call uses philoxCounter=\{1, 0, 0, 0\} (each counter C increment generates a 128-bit random number) with stride=8, row=4, and column=4.

## Return Value

None

## Constraints

None

## Examples

For the complete operator sample, see the [philoxrandom sample](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/09_random/philoxrandom).

```
// dstLocal: Tensor that stores the computation result.
// philoxKey={0,0}, philoxCounter={0,0,0,0}

// Stride mode, generating 32*32 elements.
PhiloxRandom<10>(dstLocal, philoxKey, philoxCounter, params);
// Continuous mode, generating 1024 elements.
PhiloxRandom<10>(dstLocal, philoxKey, philoxCounter, 1024);
```

The results are as follows:

```
[0.31179297 0.8263413  0.6849456 ... 0.10521233 0.29894042 0.96700084]
```
