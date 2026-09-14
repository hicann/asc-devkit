# Pad

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T16:04:08.421Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

For a two-dimensional Tensor of height \* width, pads the Tensor in the width direction to 32B alignment. If the Tensor width is already 32B-aligned and all data is valid, this API does not support alignment. The specific functional scenarios of this API are as follows:

-   Scenario 1

    The Tensor width is not 32B-aligned. Taking half as an example, for 16\*15, padding is performed by adding one column on the right, resulting in 16\*16.

-   Scenario 2

    The Tensor width is already 32B-aligned but contains some redundant data. Taking half as an example, for 16\*16 (the last two columns are redundant data), padding is performed and the result remains 16\*16, but the last two columns of redundant data can be filled with a specified value.

## Prototype

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation. To obtain the temporary space size **BufferSize**, use the **GetPadMaxMinTmpSize** API provided in [Pad Tiling](Pad-Tiling.md) to obtain the required maximum and minimum temporary space sizes. The minimum space ensures functional correctness, while the maximum space is used to improve performance.

The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it in via the sharedTmpBuffer input parameter**. Therefore, the Pad API provides two function prototypes:

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T>
    __aicore__ inline void Pad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams, const LocalTensor<uint8_t>& sharedTmpBuffer, PadTiling& tiling)
    ```

    In this method, you must allocate and manage the temporary memory space, and reuse this memory after the API call is completed. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.

-   The API framework allocates temporary space.

    ```
    template <typename T>
    __aicore__ inline void Pad(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, PadParams& padParams, PadTiling& tiling)
    ```

    In this method, you do not need to allocate the space, but must reserve the temporary space size.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are int16_t, uint16_t, half, int32_t, uint32_t, and float. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand, with a two-dimensional shape. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand, with a two-dimensional shape. For the definition of the LocalTensor data structure, see [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| padParams | Input | Pad parameters, of the PadParams type. The PadParams structure is defined in the following code, with the specific parameters as follows:<br>leftPad: Amount of data padded on the left. Unit: column.<br>rightPad: Amount of data padded on the right. Unit: column.<br>padValue: Value to be padded, supporting int32_t. |
| sharedTmpBuffer | Input | Shared buffer used to store temporary data generated during internal API computation. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization. For how to obtain the shared buffer size, see [Pad Tiling](Pad-Tiling.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
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
-   The total width after padding does not exceed the width after the original width is aligned to the nearest 32B.
-   The data volume of the source operand must be 32B-aligned.

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

## Examples

This example is a scenario 1 example: the width of the Tensor is not 32B-aligned. Taking half as an example, such as 16\*15, perform Pad and pad one column on the right to obtain 16\*16. The input data type is half.

```
// dstLocal: Output Tensor.
// srcLocal: Input Tensor.
// padParams: Parameters that control padding.
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

Output data:
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
