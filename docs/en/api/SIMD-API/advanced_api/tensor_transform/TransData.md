# TransData

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T16:12:10.217Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported

- Atlas A3 training products/Atlas A3 inference products: Supported

- Atlas A2 training products/Atlas A2 inference products: Supported

- Atlas 200I/500 A2 inference products: Not supported

- Atlas inference products AI Core: Not supported

- Atlas inference products Vector Core: Not supported

- Atlas training products: Not supported

## Description

Converts the layout format of the input data to the target layout format.

The data format conversion scenarios supported by this API include the following four. In addition to dimension order changes, they involve splitting the C axis and the N axis. Specifically, the C axis is split into the C1 axis and the C0 axis, and the N axis is split into the N1 axis and the N0 axis. For data with a bit width of 16, C0 and N0 are fixed to 16, and the calculation formulas for C1 and N1 are as follows.

![](../../../figures/zh-cn_formulaimage_0000002355948405.png)

![](../../../figures/zh-cn_formulaimage_0000002356028741.png)

-   Scenario 1: NCDHW -\> NDC1HWC0

    Input Tensor \{shape:\[N, C, D, H, W\]\}, and output Tensor \{shape:\[N, D, C/16, H, W, 16\]\}. Note that C0 is actually equal to 16. For ease of illustration, C0 is set to 2 in the following figure.

    **Figure 1**  Converting NCDHW format to NDC1HWC0 format
    ![](../../../figures/NCDHW-to-NDC1HWC0-format-conversion.png "Converting NCDHW format to NDC1HWC0 format")

-   Scenario 2: NDC1HWC0 -\> NCDHW

    The input Tensor is \{shape:\[N, D, C/16, H, W, 16\]\}, and the output Tensor is \{shape:\[N, C, D, H, W\]\}. Note that C0 is actually 16; for ease of illustration, C0 is set to 2 in the following figure.

    **Figure 2**  Converting NDC1HWC0 format to NCDHW format
    ![](../../../figures/NDC1HWC0_to_NCDHW_format_conversion.png "Converting NDC1HWC0 format to NCDHW format")

-   Scenario 3: NCDHW -\> FRACTAL\_Z\_3D

    The input Tensor is \{shape:\[N, C, D, H, W\]\}, and the output Tensor is \{shape:\[D, C/16, H, W, N/16, 16, 16\]\}. Note that C0 and N0 are actually 16; for ease of illustration, C0 and N0 are set to 2 in the following figure.

    **Figure 3**  Converting NCDHW format to FRACTAL\_Z\_3D format
    ![](../../../figures/NCDHW_converted_to_FRACTAL_Z_3D.png "Converting NCDHW format to FRACTAL_Z_3D format")

-   Scenario 4: FRACTAL\_Z\_3D -\> NCDHW

    The input tensor \{shape:\[D, C/16, H, W, N/16, 16, 16\]\}, and the output tensor \{shape:\[N, C, D, H, W\]\}. Note that C0 and N0 are actually equal to 16. For ease of illustration, C0 and N0 are set to 2 in the following figure.

    **Figure 4**  Converting FRACTAL\_Z\_3D format to the NCDHW format
    ![](../../../figures/FRACTAL_Z_3D_converted_to_NCDHW.png "Converting FRACTAL_Z_3D format to the NCDHW format")

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <const TransDataConfig& config, typename T, typename U, typename S>
    __aicore__ inline void TransData(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const TransDataParams<U, S>& params)
    ```

-   The API framework applies for temporary space.

    ```
    template <const TransDataConfig& config, typename T, typename U, typename S>
    __aicore__ inline void TransData(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const TransDataParams<U, S>& params)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **passing it via the sharedTmpBuffer input parameter** and **allocation by the API framework**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

-   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

When **sharedTmpBuffer** is passed, you need to apply for space for the tensor; when the API framework applies for temporary space, you need to reserve temporary space. The temporary space size **BufferSize** is obtained as follows: use the API provided in [GetTransDataMaxMinTmpSize](GetTransDataMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| config | Specifies the data format conversion scenario. The following four conversion scenarios are currently supported: NCDHW -> NDC1HWC0, NDC1HWC0 -> NCDHW, NCDHW -> FRACTAL_Z_3D, and FRACTAL_Z_3D -> NCDHW. This parameter is of the **TransDataConfig** type, whose definition is shown in the following code.<br><br>A configuration example is as follows.<br>constexpr AscendC::TransDataConfig config1 = {AscendC::DataFormat::NCDHW, AscendC::DataFormat::FRACTAL_Z_3D}; |
| T | Data type of the operand. The supported data types are int16_t, uint16_t, half, and bfloat16_t. |
| U | Shape information of the source operand, of the [Layout](../../../basic_api/aux_data_structures/Layout/Layout.md) type.<br>AscendC::Layout ncdhwLayout = AscendC::MakeLayout(AscendC::MakeShape(n, c, d, h, w), AscendC::MakeStride()); |
| S | Shape information of the destination operand, of the [Layout](../../../basic_api/aux_data_structures/Layout/Layout.md) type.<br>AscendC::Layout fractalzLayout = AscendC::MakeLayout(AscendC::MakeShape(d, c1, h, w, n1, n0, c0), AscendC::MakeStride()); |

```
struct TransDataConfig {
    DataFormat srcFormat;
    DataFormat dstFormat;
};

enum class DataFormat : uint8_t {
    ND = 0,
    NZ,
    NCHW,
    NC1HWC0,
    NHWC,
    NCDHW,
    NDC1HWC0,
    FRACTAL_Z_3D,
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside TransData, provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetTransDataMaxMinTmpSize](GetTransDataMaxMinTmpSize.md). |
| params | Input | Shape information of the source operand and the destination operand. This parameter is of the **TransDataParams** data type, whose definition and configuration example are as follows, where the template parameters T and U must be of the [Layout](../../../basic_api/aux_data_structures/Layout/Layout.md) type. The Shape dimensions specified by this parameter must be consistent with the dimensions corresponding to the Format in **config**. |







































```
template <typename T, typename U>
struct TransDataParams {
    T srcLayout;
    U dstLayout;
};

AscendC::Layout ncdhwLayout = AscendC::MakeLayout(AscendC::MakeShape(n, c, d, h, w), AscendC::MakeStride());
AscendC::Layout fractalzLayout = AscendC::MakeLayout(AscendC::MakeShape(d, c1, h, w, n1, n0, c0), AscendC::MakeStride());
AscendC::TransDataParams&lt;decltype(ncdhwLayout), decltype(fractalzLayout)&gt; params = {ncdhwLayout, fractalzLayout};
```

## Return Value

None

## Constraints

-   For the alignment requirements of operand addresses, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).

-   Overlap between source and destination operand addresses is not supported.







































-   Overlap between **sharedTmpBuffer** and source or destination operand addresses is not supported.

-   For NCDHW-format input, if the combined axis of the H axis and W axis is not 32-byte aligned, you need to pad data on the combined axis to make it 32-byte aligned before calling this API. When calling this API, you should pass the original shape (that is, the shape before axis combination) in the parameter that specifies shape information. For example, if the original shape of the input is \[1, 16, 2, 3, 5\], you need to pad the input data to the shape \[1, 16, 2, 16\], and the padded data is invalid data.

-   For NCDHW-format output, the API implementation combines the H axis and W axis and pads data on the combined axis to achieve 32-byte alignment. When calling this API, you should pass the original shape (that is, the shape before axis combination) in the parameter that specifies shape information. For example, if the original target shape in NCDHW format is \[1, 16, 2, 3, 5\], the actual output is data of the shape \[1, 16, 2, 16\], where the data padded by the API is invalid data.

## Examples

To learn more, see the [TransData example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/08_transpose/transdata).

```
// dstLocal: Output tensor.
// srcLocal: Input tensor.
// tmp: Temporary buffer.
// params: Shape information of the source and destination operands.
// Construct the layout.
AscendC::Layout ncdhwLayout = AscendC::MakeLayout(AscendC::MakeShape(1, 32, 2, 2, 8), AscendC::MakeStride());
AscendC::Layout ndc1hwc0Layout = AscendC::MakeLayout(AscendC::MakeShape(1, 2, 2, 2, 8, 16), AscendC::MakeStride());
static constexpr AscendC::TransDataConfig config = {DataFormat::NCDHW, DataFormat::NDC1HWC0};
AscendC::TransDataParams<decltype(ncdhwLayout), decltype(ndc1hwc0Layout)> params = {ncdhwLayout, ndc1hwc0Layout};
AscendC::TransData<config>(dstLocal, srcLocal, tmp, params);
```

The following is an example result:

```
The data type of the input and output is half.
Input data (src):
[[[[[  0   1   2   3   4   5   6   7]
    [  8   9  10  11  12  13  14  15]]]

  [[[ 16  17  18  19  20  21  22  23]
    [ 24  25  26  27  28  29  30  31]]]

  [[[ 32  33  34  35  36  37  38  39]
    [ 40  41  42  43  44  45  46  47]]]

  [[[ 48  49  50  51  52  53  54  55]
    [ 56  57  58  59  60  61  62  63]]]

  [[[ 64  65  66  67  68  69  70  71]
    [ 72  73  74  75  76  77  78  79]]]

  [[[ 80  81  82  83  84  85  86  87]
    [ 88  89  90  91  92  93  94  95]]]

  [[[ 96  97  98  99 100 101 102 103]
    [104 105 106 107 108 109 110 111]]]

  [[[112 113 114 115 116 117 118 119]
    [120 121 122 123 124 125 126 127]]]

  [[[128 129 130 131 132 133 134 135]
    [136 137 138 139 140 141 142 143]]]

  [[[144 145 146 147 148 149 150 151]
    [152 153 154 155 156 157 158 159]]]

  [[[160 161 162 163 164 165 166 167]
    [168 169 170 171 172 173 174 175]]]

  [[[176 177 178 179 180 181 182 183]
    [184 185 186 187 188 189 190 191]]]

  [[[192 193 194 195 196 197 198 199]
    [200 201 202 203 204 205 206 207]]]

  [[[208 209 210 211 212 213 214 215]
    [216 217 218 219 220 221 222 223]]]

  [[[224 225 226 227 228 229 230 231]
    [232 233 234 235 236 237 238 239]]]

  [[[240 241 242 243 244 245 246 247]
    [248 249 250 251 252 253 254 255]]]

  [[[256 257 258 259 260 261 262 263]
    [264 265 266 267 268 269 270 271]]]

  [[[272 273 274 275 276 277 278 279]
    [280 281 282 283 284 285 286 287]]]

  [[[288 289 290 291 292 293 294 295]
    [296 297 298 299 300 301 302 303]]]

  [[[304 305 306 307 308 309 310 311]
    [312 313 314 315 316 317 318 319]]]

  [[[320 321 322 323 324 325 326 327]
    [328 329 330 331 332 333 334 335]]]

  [[[336 337 338 339 340 341 342 343]
    [344 345 346 347 348 349 350 351]]]

  [[[352 353 354 355 356 357 358 359]
    [360 361 362 363 364 365 366 367]]]

  [[[368 369 370 371 372 373 374 375]
    [376 377 378 379 380 381 382 383]]]

  [[[384 385 386 387 388 389 390 391]
    [392 393 394 395 396 397 398 399]]]

  [[[400 401 402 403 404 405 406 407]
    [408 409 410 411 412 413 414 415]]]

  [[[416 417 418 419 420 421 422 423]
    [424 425 426 427 428 429 430 431]]]

  [[[432 433 434 435 436 437 438 439]
    [440 441 442 443 444 445 446 447]]]

  [[[448 449 450 451 452 453 454 455]
    [456 457 458 459 460 461 462 463]]]

  [[[464 465 466 467 468 469 470 471]
    [472 473 474 475 476 477 478 479]]]

  [[[480 481 482 483 484 485 486 487]
    [488 489 490 491 492 493 494 495]]]

  [[[496 497 498 499 500 501 502 503]
    [504 505 506 507 508 509 510 511]]]]]

Input config: {DataFormat::NCDHW, DataFormat::NDC1HWC0}
Input params: {(1, 32, 2, 2, 8), (1, 2, 2, 2, 8, 16)}

Output data (dst):
[[[[[[  0  16  32  48  64  80  96 112 128 144 160 176
      192 208 224 240]
     [  1  17  33  49  65  81  97 113 129 145 161 177
      193 209 225 241]
     [  2  18  34  50  66  82  98 114 130 146 162 178
      194 210 226 242]
     [  3  19  35  51  67  83  99 115 131 147 163 179
      195 211 227 243]]

    [[  4  20  36  52  68  84 100 116 132 148 164 180
      196 212 228 244]
     [  5  21  37  53  69  85 101 117 133 149 165 181
      197 213 229 245]
     [  6  22  38  54  70  86 102 118 134 150 166 182
      198 214 230 246]
     [  7  23  39  55  71  87 103 119 135 151 167 183
      199 215 231 247]]

    [[  8  24  40  56  72  88 104 120 136 152 168 184
      200 216 232 248]
     [  9  25  41  57  73  89 105 121 137 153 169 185
      201 217 233 249]
     [ 10  26  42  58  74  90 106 122 138 154 170 186
      202 218 234 250]
     [ 11  27  43  59  75  91 107 123 139 155 171 187
      203 219 235 251]]

    [[ 12  28  44  60  76  92 108 124 140 156 172 188
      204 220 236 252]
     [ 13  29  45  61  77  93 109 125 141 157 173 189
      205 221 237 253]
     [ 14  30  46  62  78  94 110 126 142 158 174 190
      206 222 238 254]
     [ 15  31  47  63  79  95 111 127 143 159 175 191
      207 223 239 255]]]

   [[[256 272 288 304 320 336 352 368 384 400 416 432
      448 464 480 496]
     [257 273 289 305 321 337 353 369 385 401 417 433
      449 465 481 497]
     [258 274 290 306 322 338 354 370 386 402 418 434
      450 466 482 498]
     [259 275 291 307 323 339 355 371 387 403 419 435
      451 467 483 499]]

    [[260 276 292 308 324 340 356 372 388 404 420 436
      452 468 484 500]
     [261 277 293 309 325 341 357 373 389 405 421 437
      453 469 485 501]
     [262 278 294 310 326 342 358 374 390 406 422 438
      454 470 486 502]
     [263 279 295 311 327 343 359 375 391 407 423 439
      455 471 487 503]]

    [[264 280 296 312 328 344 360 376 392 408 424 440
      456 472 488 504]
     [265 281 297 313 329 345 361 377 393 409 425 441
      457 473 489 505]
     [266 282 298 314 330 346 362 378 394 410 426 442
      458 474 490 506]
     [267 283 299 315 331 347 363 379 395 411 427 443
      459 475 491 507]]

    [[268 284 300 316 332 348 364 380 396 412 428 444
      460 476 492 508]
     [269 285 301 317 333 349 365 381 397 413 429 445
      461 477 493 509]
     [270 286 302 318 334 350 366 382 398 414 430 446
      462 478 494 510]
     [271 287 303 319 335 351 367 383 399 415 431 447
      463 479 495 511]]]]]]

```