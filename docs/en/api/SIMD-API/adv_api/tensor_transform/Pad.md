# Pad

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-08T11:10:46.181Z pushedAt=2026-10-27T11:55:25.716Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/tensor_transform/Pad_res.md#id1 -->

## Description

Pads a two-dimensional Tensor of height \* width to 32B alignment in the width direction. If the width of the Tensor is already 32B-aligned and all data is valid, calling this API for alignment is not supported. The specific functional scenarios of this API are as follows:

-   Scenario 1

    The width of the Tensor is not 32B-aligned. Taking half as an example, for a 16\*15 tensor, pad one column on the right to make it 16\*16.

-   Scenario 2

    The width of the Tensor is already 32B-aligned, but there is some redundant data. Taking half as an example, for a 16\*16 tensor (the last two columns are redundant data), after padding it remains 16\*16, but the last two columns of redundant data can be filled with the configured value.

## Prototype

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation. To obtain the temporary space size **BufferSize**: use the **GetPadMaxMinTmpSize** API provided in [Pad Tiling](Pad-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space guarantees functional correctness, while the maximum space is used to improve performance.

Temporary space supports two methods: **application by the API framework** and **passing in by the developer through the sharedTmpBuffer input parameter**. Therefore, the Pad API has two prototypes:

-   Pass in temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T>
    __aicore__ inline void Pad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams, const LocalTensor<uint8_t>& sharedTmpBuffer, PadTiling& tiling)
    ```

    In this method, the developer must apply for and manage the temporary memory space, and reuse this memory after the API call is completed. The memory is not repeatedly applied for and released, which provides higher flexibility and higher memory utilization.

-   The API framework applies for temporary space

    ```
    template <typename T>
    __aicore__ inline void Pad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams, PadTiling& tiling)
    ```

    In this method, the developer does not need to apply for the space, but must reserve the size of the temporary space.

## Parameters

**Table 1** Template parameters

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: int16_t, uint16_t, half, int32_t, uint32_t, float. |

**Table 2** API parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand, with a two-dimensional shape. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand, with a two-dimensional shape. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| padParams | Input | Pad parameters, of the PadParams type. The PadParams structure is defined in the following code, and its specific parameters are as follows:<br>leftPad: the amount of data padded on the left. Unit: column.<br>rightPad: the amount of data padded on the right. Unit: column.<br>padValue: the padding value, which supports int32_t. |
| sharedTmpBuffer | Input | Shared buffer used to store temporary data generated during internal API computation. With this method, developers can manage the sharedTmpBuffer memory space by themselves and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization. For how to obtain the shared buffer size, see [Pad Tiling](Pad-Tiling.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| tiling | Input | Tiling information required for computation. For how to obtain the tiling information, see [Pad Tiling](Pad-Tiling.md). |

```
struct PadParams {
    uint16_t leftPad = 0;
    uint16_t rightPad = 0;
    int32_t padValue = 0;
};
```

## Return Value

None

## Constraints

-   For scenario 1, padding is supported on both the left and right sides simultaneously.
-   For scenario 2, padding is supported only on the right side.
-   The total width after padding does not exceed the width obtained by aligning the original width to the nearest 32B.
-   The data volume of the source operand must be 32B-aligned.

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

## Examples

This sample demonstrates scenario 1: the Tensor width is not 32B-aligned. Taking half as an example, such as 16\*15, Pad is applied by adding 1 column on the right, resulting in 16\*16. The input data type is half.

```
// dstLocal: output Tensor.
// srcLocal: input Tensor.
// padParams: parameters that control padding.
AscendC::PadParams padParams{0, 1, 321}; // Pad 0 columns on the left, 1 column on the right, with a padding value of 321.
AscendC::Pad(dstLocal, srcLocal, padParams, tilingData.padTilingData);
```

```
Input data:
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14
15 16 17 18 19 20 21 22 23 24 25 26 27 28 29
30 31 32 33 34 35 36 37 38 39 40 41 42 43 44
45 46 47 48 49 50 51 52 53 54 55 56 57 58 59
60 61 62 63 64 65 66 67 68 69 70 71 72 73 74
75 76 77 78 79 80 81 82 83 84 85 86 87 88 89
90 91 92 93 94 95 96 97 98 99 100 101 102 103 104
105 106 107 108 109 110 111 112 113 114 115 116 117 118 119
120 121 122 123 124 125 126 127 128 129 130 131 132 133 134
135 136 137 138 139 140 141 142 143 144 145 146 147 148 149
150 151 152 153 154 155 156 157 158 159 160 161 162 163 164
165 166 167 168 169 170 171 172 173 174 175 176 177 178 179
180 181 182 183 184 185 186 187 188 189 190 191 192 193 194
195 196 197 198 199 200 201 202 203 204 205 206 207 208 209
210 211 212 213 214 215 216 217 218 219 220 221 222 223 224
225 226 227 228 229 230 231 232 233 234 235 236 237 238 239

Output data
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 321
15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 321
30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 321
45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 321
60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 321
75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 321
90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 321
105 106 107 108 109 110 111 112 113 114 115 116 117 118 119 321
120 121 122 123 124 125 126 127 128 129 130 131 132 133 134 321
135 136 137 138 139 140 141 142 143 144 145 146 147 148 149 321
150 151 152 153 154 155 156 157 158 159 160 161 162 163 164 321
165 166 167 168 169 170 171 172 173 174 175 176 177 178 179 321
180 181 182 183 184 185 186 187 188 189 190 191 192 193 194 321
195 196 197 198 199 200 201 202 203 204 205 206 207 208 209 321
210 211 212 213 214 215 216 217 218 219 220 221 222 223 224 321
225 226 227 228 229 230 231 232 233 234 235 236 237 238 239 321
```
