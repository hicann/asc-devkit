# Data Layout Format<a name="ZH-CN_TOPIC_0000001700139572"></a>

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T12:34:57.739Z pushedAt=2026-09-10T12:11:18.490Z -->

A data layout format describes how a multidimensional tensor is stored in memory in deep learning.

Common data formats include ND, NHWC, and NCHW, which assign specific business semantics to each axis of a tensor.

In addition to the NHWC and NCHW formats, there are also some special private data formats, such as FRACTAL\_NZ (also abbreviated as NZ), NC1HWC0, FRACTAL\_Z, NDC1HWC0, and FRACTAL\_Z\_3D. These formats are introduced to meet the high-performance computing requirements of the Cube Compute unit in the AI Core. By optimizing the memory layout, these formats can improve computing efficiency. You can see the specific applications of these formats when developing operators using the matrix multiplication and convolution APIs.

## Common Formats<a name="section138780167211"></a>

-   **ND, NHWC, and NCHW**

    Data layout formats were originally used to represent how images are stored in memory. Common ones include ND, NHWC, and NCHW. In general, all tensors are N-dimensional (ND), while NHWC and NCHW assign specific business semantics to each axis of a four-dimensional tensor, such as height, width, and the number of channels.

    The main difference between NHWC and NCHW lies in the position of the channel dimension:

    -   In the NHWC format, the channel dimension is at the last position.
    -   In the NCHW format, the channel dimension comes before the height and width.

    The meaning of each axis is explained as follows:

    -   **N**: Batch, indicating the number of images.
    -   **H**: Height, the height of the image, that is, the number of pixels in the vertical direction.
    -   **W**: Width, the width of the image, that is, the number of pixels in the horizontal direction.
    -   **C**: Channels, the number of channels of an image. For example, the number of channels of a color RGB image is 3.

    As shown in [Figure 1](#zh-cn_topic_0000001265237150_zh-cn_topic_0228422146_zh-cn_topic_0187054064_fig146585121118), taking an RGB image as an example, in NCHW, C is arranged in the outer layer, and the actual storage is "RRRRRRGGGGGGBBBBBB", that is, all pixel values of the same channel are stored together in sequence. In NHWC, C is arranged in the innermost layer, and the actual storage is "RGBRGBRGBRGBRGBRGB", that is, the pixel values at the same position across multiple channels are stored together in sequence.

    **Figure 1**  NCHW and NHWC storage example<a name="zh-cn_topic_0000001265237150_zh-cn_topic_0228422146_zh-cn_topic_0187054064_fig146585121118"></a>  
    ![](../../../figures/nchw_and_nhwc_storage_example.png "NCHW and NHWC storage example")

    Although the stored data is the same, different storage orders lead to different data access characteristics. Therefore, even for the same operation, the corresponding compute performance differs.

-   **NDHWC and NCDHW**

    NDHWC and NCDHW are five-dimensional tensors. Compared with NHWC and NCHW, they add a D dimension, where D represents feature depth, indicating the extension of data in the depth direction, such as the time steps of a video or the depth layers of a medical image. Therefore, these formats facilitate convolution operations in the time dimension. Taking NDHWC as an example, its data format is shown in the following figure:

    ![](../../../figures/NCDHW.png)

## Special Formats Related to Cube Multiplication<a name="section184471251122117"></a>

When the Mmad basic API is used for matrix multiplication, the data layout of the matrix input and output must meet certain requirements. As shown in the following figure, matrix A (in the L0A Buffer) must be in FRACTAL\_ZZ format, matrix B (in the L0B Buffer) must be in FRACTAL\_ZN format, and matrix C (in the L0C Buffer) must be in FRACTAL\_NZ format. These formats divide a matrix into several fractal matrices, adapting to the hardware feature that the Cube Compute unit reads \(16, 16\) × \(16, 16\) data at a time for computation (using the half data type as an example), thereby improving the matrix computation efficiency. The size of a fractal is related to the data type and the storage location. For details, see the following sections.

![](../../../figures/zn_abc.png)

-   **FRACTAL\_NZ/NZ**
<a name="li19960204116136"></a>

    The FRACTAL\_NZ format, abbreviated as NZ, is obtained by performing padding (pad), reshaping (reshape), and transposing (transpose) on the lowest two dimensions of a Tensor (for all dimensions of a Tensor, the right side is the low dimension and the left side is the high dimension). The specific conversion process is as follows:

    A matrix of size \(M, N\) is divided into M1 \* N1 fractals, which are arranged in column-major order and shaped like the letter N. Each fractal contains M0 \* N0 elements, which are arranged in row-major order and shaped like the letter Z. Therefore, this data format is called the NZ format. Here, \(M0, N0\) indicates the size of a fractal.

    It is expressed by the following formula:

    ```
    (…, B, M, N)->pad->(…, B, M1 * M0, N1 * N0)->reshape->(…, B, M1, M0, N1, N0)->transpose->(…, B, N1, M1, M0, N0)
    ```

    >[!NOTE] Note
    >Generally, the NZ format is used in different scenarios in the L0C Buffer and L1 Buffer:
    >- In the L0C Buffer, the NZ format is used to store the result of matrix multiplication. Its fractal shape is 16x16, containing 256 elements. This structure is ideal for the Cube Compute unit to perform efficient matrix multiplication.
    >- In the L1 Buffer, the NZ format is adopted so that data can be conveniently converted to the corresponding ZZ and ZN formats when being moved to the L0A Buffer and L0B Buffer. In this case, the fractal shape is 16 x \(32B / sizeof\(Datatype\)\), with a size of 512 bytes.
    >Therefore, when data is moved from the L0C Buffer to the L1 Buffer, its fractal size may change.

    The following uses a specific example to describe the process of converting the ND format to the NZ format.

    The shape of the original tensor is \(20, 28\):

    ```
    data = [x for x in range(20 * 28)]
    data_a = data * np.ones((20 * 28), dtype="float16")
    tensor_a = data_a.reshape((20, 28))
    print(tensor_a)
    ```

    The original tensor data is printed as follows:

    ```
    [[  0.   1.   2.   3.   4.   5.   6.   7.   8.   9.  10.  11.  12.  13.
       14.  15.  16.  17.  18.  19.  20.  21.  22.  23.  24.  25.  26.  27.]
     [ 28.  29.  30.  31.  32.  33.  34.  35.  36.  37.  38.  39.  40.  41.
       42.  43.  44.  45.  46.  47.  48.  49.  50.  51.  52.  53.  54.  55.]
     [ 56.  57.  58.  59.  60.  61.  62.  63.  64.  65.  66.  67.  68.  69.
       70.  71.  72.  73.  74.  75.  76.  77.  78.  79.  80.  81.  82.  83.]
     [ 84.  85.  86.  87.  88.  89.  90.  91.  92.  93.  94.  95.  96.  97.
       98.  99. 100. 101. 102. 103. 104. 105. 106. 107. 108. 109. 110. 111.]
     [112. 113. 114. 115. 116. 117. 118. 119. 120. 121. 122. 123. 124. 125.
      126. 127. 128. 129. 130. 131. 132. 133. 134. 135. 136. 137. 138. 139.]
     [140. 141. 142. 143. 144. 145. 146. 147. 148. 149. 150. 151. 152. 153.
      154. 155. 156. 157. 158. 159. 160. 161. 162. 163. 164. 165. 166. 167.]
     [168. 169. 170. 171. 172. 173. 174. 175. 176. 177. 178. 179. 180. 181.
      182. 183. 184. 185. 186. 187. 188. 189. 190. 191. 192. 193. 194. 195.]
     [196. 197. 198. 199. 200. 201. 202. 203. 204. 205. 206. 207. 208. 209.
      210. 211. 212. 213. 214. 215. 216. 217. 218. 219. 220. 221. 222. 223.]
     [224. 225. 226. 227. 228. 229. 230. 231. 232. 233. 234. 235. 236. 237.
      238. 239. 240. 241. 242. 243. 244. 245. 246. 247. 248. 249. 250. 251.]
     [252. 253. 254. 255. 256. 257. 258. 259. 260. 261. 262. 263. 264. 265.
      266. 267. 268. 269. 270. 271. 272. 273. 274. 275. 276. 277. 278. 279.]
     [280. 281. 282. 283. 284. 285. 286. 287. 288. 289. 290. 291. 292. 293.
      294. 295. 296. 297. 298. 299. 300. 301. 302. 303. 304. 305. 306. 307.]
     [308. 309. 310. 311. 312. 313. 314. 315. 316. 317. 318. 319. 320. 321.
      322. 323. 324. 325. 326. 327. 328. 329. 330. 331. 332. 333. 334. 335.]
     [336. 337. 338. 339. 340. 341. 342. 343. 344. 345. 346. 347. 348. 349.
      350. 351. 352. 353. 354. 355. 356. 357. 358. 359. 360. 361. 362. 363.]
     [364. 365. 366. 367. 368. 369. 370. 371. 372. 373. 374. 375. 376. 377.
      378. 379. 380. 381. 382. 383. 384. 385. 386. 387. 388. 389. 390. 391.]
     [392. 393. 394. 395. 396. 397. 398. 399. 400. 401. 402. 403. 404. 405.
      406. 407. 408. 409. 410. 411. 412. 413. 414. 415. 416. 417. 418. 419.]
     [420. 421. 422. 423. 424. 425. 426. 427. 428. 429. 430. 431. 432. 433.
      434. 435. 436. 437. 438. 439. 440. 441. 442. 443. 444. 445. 446. 447.]
     [448. 449. 450. 451. 452. 453. 454. 455. 456. 457. 458. 459. 460. 461.
      462. 463. 464. 465. 466. 467. 468. 469. 470. 471. 472. 473. 474. 475.]
     [476. 477. 478. 479. 480. 481. 482. 483. 484. 485. 486. 487. 488. 489.
      490. 491. 492. 493. 494. 495. 496. 497. 498. 499. 500. 501. 502. 503.]
     [504. 505. 506. 507. 508. 509. 510. 511. 512. 513. 514. 515. 516. 517.
      518. 519. 520. 521. 522. 523. 524. 525. 526. 527. 528. 529. 530. 531.]
     [532. 533. 534. 535. 536. 537. 538. 539. 540. 541. 542. 543. 544. 545.
      546. 547. 548. 549. 550. 551. 552. 553. 554. 555. 556. 557. 558. 559.]]
    ```

    The conversion process is expressed in pseudocode as follows:

    ```
    N0 = 16
    N1 = (28 + N0 - 1) // N0
    pad_n = N1 * N0 - 28
    M0 = 16
    M1 = (20 + M0 - 1) // M0
    pad_m = M1 * M0 - 20
    tensor_b = np.pad(tensor_a, [[0, pad_m], [0, pad_n]])
    tensor_b = tensor_b.reshape((M1, M0, N1, N0))
    tensor_b = tensor_b.transpose((2, 0, 1, 3))
    print(tensor_b)
    ```

    The schematic diagram of the conversion process is as follows:

    ![](../../../figures/call_example_1.png)

    The converted tensor is printed as follows:

    ```
    [[[[  0.   1.   2. ...  13.  14.  15.]
       [ 28.  29.  30. ...  41.  42.  43.]
       [ 56.  57.  58. ...  69.  70.  71.]
       ...
       [364. 365. 366. ... 377. 378. 379.]
       [392. 393. 394. ... 405. 406. 407.]
       [420. 421. 422. ... 433. 434. 435.]]
    
      [[448. 449. 450. ... 461. 462. 463.]
       [476. 477. 478. ... 489. 490. 491.]
       [504. 505. 506. ... 517. 518. 519.]
       ...
       [  0.   0.   0. ...   0.   0.   0.]
       [  0.   0.   0. ...   0.   0.   0.]
       [  0.   0.   0. ...   0.   0.   0.]]]
    
    
     [[[ 16.  17.  18. ...   0.   0.   0.]
       [ 44.  45.  46. ...   0.   0.   0.]
       [ 72.  73.  74. ...   0.   0.   0.]
       ...
       [380. 381. 382. ...   0.   0.   0.]
       [408. 409. 410. ...   0.   0.   0.]
       [436. 437. 438. ...   0.   0.   0.]]
    
      [[464. 465. 466. ...   0.   0.   0.]
       [492. 493. 494. ...   0.   0.   0.]
       [520. 521. 522. ...   0.   0.   0.]
       ...
       [  0.   0.   0. ...   0.   0.   0.]
       [  0.   0.   0. ...   0.   0.   0.]
       [  0.   0.   0. ...   0.   0.   0.]]]]
    ```

-   **FRACTAL\_ZZ/ZZ**

    The FRACTAL\_ZZ format, abbreviated as ZZ, is obtained by performing padding (pad), splitting (reshape), and transposition (transpose) on the lowest two dimensions of a tensor (for all dimensions of a tensor, the right side is the low dimension and the left side is the high dimension). The specific conversion process is as follows:

    A matrix of size \(M,  K\) is divided into M1 \* K1 fractals, which are arranged in row major order and shaped like a zigzag. Each fractal contains M0 \* K0 elements, which are arranged in row major order and shaped like a zigzag. Therefore, this data format is called the ZZ format. Here, \(M0, K0\) indicates the size of a fractal. The fractal shape is 16 x \(32B / sizeof\(Datatype\)\), with a size of 512 bytes.

    ![](../../../figures/zz.png)

    The conversion process is expressed by the following formula:

    ```
    (…, B, M, K)->pad->(…, B, M1 * M0, K1 * K0)->reshape->(…, B, M1, M0, K1, K0)->transpose->(…, B, M1, K1, M0, K0)
    ```

    For different data types, the values of M0 and K0 are different:

    -   For a data type with a bit width of 4: M0=16, K0=64.
    -   For a data type with a bit width of 8: M0=16, K0=32.
    -   For a data type with a bit width of 16: M0=16, K0=16.
    -   For a data type with a bit width of 32: M0=16, K0=8.

-   **FRACTAL\_ZN/ZN**

    The FRACTAL\_ZN format, abbreviated as ZN, is obtained by performing padding (pad), reshaping (reshape), and transposing (transpose) operations on the lowest two dimensions of a Tensor (for all dimensions of a Tensor, the right side is the low dimension and the left side is the high dimension). The specific conversion process is as follows:

    A matrix of size \(K,  N\) is divided into K1 \* N1 fractals, which are arranged in row-major order with a zigzag shape. Each fractal contains K0 \* N0 elements arranged in column-major order with an N shape, hence the name ZN format. Here, \(K0, N0\) indicates the size of a fractal. The fractal shape is \(32B / sizeof\(Datatype\)\) x 16, with a size of 512 bytes.

    ![](../../../figures/Zn_format_half.png)

    The conversion process is expressed by the following formula:

    ```
    (…, B, K, N)->pad->(…, B, K1 * K0, N1 * N0)->reshape->(…, B, K1, K0, N1, N0)->transpose->(…, B, K1, N1, N0, K0)
    ```

    For different data types, the values of K0 and N0 differ:

    -   For a data type with a bit width of 4: K0=64, N0=16;
    -   For a data type with a bit width of 8: K0=32, N0=16;
    -   For a data type with a bit width of 16: K0=16, N0=16;
    -   For a data type with a bit width of 32: K0=8, N0=16.

## Special Formats Related to Convolution<a name="section0205348165318"></a>

-   **NC1HWC0**

    On AI processors, to improve the access efficiency of data blocks in general matrix multiplication (GEMM) operations, all tensor data uses the five-dimensional NC1HWC0 data format. C0 is strongly related to the microarchitecture and equals the size of the matrix Compute unit in the AI Core.

    C1=\(C+C0-1\)/C0. If the result is not divisible, it is rounded down.

    The conversion from NHWC/NCHW to NC1HWC0 is as follows: split the data along the C dimension into C1 parts of NHWC0/NC0HW, and then arrange the C1 parts of NHWC0/NC0HW contiguously in memory as NC1HWC0. The following figure shows the format conversion schematic diagram.

    ![](../../../figures/nc1hwc0_format_conversion_diagram.png)

    -   The conversion formula from NHWC to NC1HWC0 is as follows:

        ```
        Tensor.reshape( [N, H, W, C1, C0]).transpose( [0, 3, 1, 2, 4] )
        ```

    -   The conversion formula from NCHW to NC1HWC0 is as follows:

        ```
        Tensor.reshape( [N, C1, C0, H, W]).transpose( [0, 1, 3, 4, 2] )
        ```

-   **FRACTAL\_Z**

    FRACTAL\_Z is a data format used to define convolution weights, obtained by transforming the FT Matrix (FT: Filter, convolution kernel). FRACTAL\_Z is the final data format sent to the Cube, using the four-dimensional data layout of "C1HW,N1,N0,C0".

    The data has two layers of tiling, as shown in the following figure:

    ![](../../../figures/zh-cn_image_0000002335908976.png)

    The first layer is related to the Cube size, and the data is continuous along the column direction (small n). The second layer is related to the matrix size, and the data is continuous along the row direction (large Z).

    For example, HWCN = \(2, 2, 32, 32\) is converted to FRACTAL\_Z\(C1HW, N1, N0, C0\) = \(8, 2, 16, 16\).

    The process of converting HWCN to FRACTAL\_Z is as follows:

    ```
    Tensor.padding([ [0,0], [0,0], [0,(C0-C%C0)%C0], [0,(N0-N%N0)%N0] ]).reshape( [H, W, C1, C0, N1, N0]).transpose( [2, 0, 1, 4, 5, 3] ).reshape( [C1*H*W, N1, N0, C0])
    ```

    The process of converting NCHW to FRACTAL\_Z is as follows:

    ```
    Tensor.padding([ [0,(N0-N%N0)%N0], [0,(C0-C%C0)%C0], [0,0], [0,0] ]).reshape( [N1, N0, C1, C0, H, W,]).transpose( [2, 4, 5, 0, 1, 3] ).reshape( [C1*H*W, N1, N0, C0])
    ```

-   **NDC1HWC0**

    To improve the access efficiency of data blocks in matrix multiplication operations, NDHWC is converted to the NDC1HWC0 format. C0 is strongly related to the microarchitecture and equals the size of the matrix Compute unit in the AI Core. It is 16 for the float16\_t type and 32 for the int8\_t type. This part of data needs to be stored continuously.

    C1=\(C+C0-1\)/C0. If the result is not divisible, it is rounded down.

    The conversion process from NDHWC to NDC1HWC0 is as follows: split the data along the C dimension into C1 NDHWC0 blocks, and then arrange the C1 NDHWC0 blocks continuously in memory to form NDC1HWC0. The schematic diagram of this format conversion is shown in the following figure.

    ![](../../../figures/NDC1HWC0.png)

-   **FRACTAL\_Z\_3D**

    FRACTAL\_Z\_3D is a 3D convolution weight format. For example, the Conv3D operator uses this format to express the weights of 3D convolution.

    The transformation from NDHWC to FRACTAL\_Z\_3D is expressed by the following formula:

    ```
    (…, N, D, H, W, C)->pad->(…, N1 * N0, D, H, W, C1 * C0)->reshape->(…, N1, N0, D, H, W, C1, C0)->transpose->(D, C1, H, W, N1, N0, C0)->reshape->(…, D * C1 * H * W, N1, N0, C0)
    ```

    For different data types, the values of C0 and N0 differ:

    -   For a data type with a bit width of 4: C0=64, N0=16;
    -   For a data type with a bit width of 8: C0=32, N0=16;
    -   For a data type with a bit width of 16: C0=16, N0=16;
    -   For a data type with a bit width of 32: C0=8, N0=16.

    Input a Tensor in NDHWC format with a shape of \(48, 2, 2, 2, 32\):

    ![](../../../figures/3D_1.png)

    After conversion, the FRACTAL\_Z\_3D format is obtained as follows:

    ![](../../../figures/3D_2.png)

## Matmul High-Level API Related Formats<a name="section34223918570"></a>

-   **BSH/SBH**: B: Batch, the batch size; S: sequence length; H = N \* D, where N is the number of heads and D is the head size. This format is usually used for Matmul matrix multiplication. The data layout is shown in the following figure:

    ![](../../../figures/skip_write_mode_diagram.png)

    ![](../../../figures/skip_write_mode_diagram_27.png)

-   **BMNK**: a general data format; B: Batch, the batch size; M, N, and K are the matrix dimensions of the matrix multiplication \[M, K\]\*\[K, N\]. Its data layout is as follows:

    ![](../../../figures/bmnk.png)

-   **BSNGD**: the shape obtained by reshaping the original BSH shape. S and D are the M axis (or N axis) and K axis of the matrix multiplication of a single batch, and one SD is the computation data of one batch. This format is usually used for Matmul matrix multiplication. The data layout is shown in the following figure:

    ![](../../../figures/bsngd.png)

-   **SBNGD**: the shape obtained by reshaping the original SBH shape. S and D are the M axis (or N axis) and K axis of the matrix multiplication of a single batch, and one SD is the computation data of one batch. This format is usually used for Matmul matrix multiplication. The data layout is shown in the following figure:

    ![](../../../figures/sbngd.png)

-   **BNGS1S2**: generally the output of matrix multiplication in the preceding two data layouts. S1S2 data is stored contiguously, and one S1S2 is the computation data of one batch. This format is usually used for Matmul matrix multiplication. The data layout is shown in the following figure:

    ![](../../../figures/bngs1s2.png)

-   **ND\_ALIGN**: ND\_ALIGN is a transformed data format of the ND data format
<a name="li075920427155"></a>. When outputting the result matrix C of matrix multiplication, it is used to configure matrix C to be output according to the rule of 32-byte alignment in the N direction.

    The ND-\>ND\_ALIGN transformation process is shown in the following figure. Assume that the data type of the result matrix C of matrix multiplication is int32\_t and it is output to VECOUT. The original matrix is not 32-byte aligned in the N direction. After ND\_ALIGN is set, zeros are padded after it to align it to 32 bytes.

    ![](../../../figures/nd2nz-conversion-schematic-diagram.png)

-   **VECTOR**: VECTOR is a data format used in the
<a name="li51557161818"></a>[GEMV](../../../operator_practice/simd_operator_impl/matrix_advanced_api/feature_scenarios/cube_vector_multiplication.md) (General Matrix-Vector Multiply) scenario. Configuring the matrix in the VECTOR data layout means that the input data is a vector.

    **Figure 2**  Schematic diagram of matrix A in the Vector format for GEMV scenario input<a name="fig172015331814"></a>  
    ![](../../../figures/gemv-scenario-input-vector-format-matrix-a-schematic-diagram.png "Schematic diagram of matrix A in the Vector format for GEMV scenario input")

## COLUMN\_MAJOR<a name="section14365456143720"></a>

The ND data layout format is also called ROW\_MAJOR (row-major), and correspondingly there is COLUMN\_MAJOR (column-major). The difference between these two data layout formats lies in which direction of consecutive elements in an array or matrix is contiguous in memory.

-   ROW\_MAJOR: Consecutive elements in the row direction are contiguous in memory.
-   COLUMN\_MAJOR: Consecutive elements in the column direction are contiguous in memory.

The schematic diagrams of the two data layout formats are as follows.

![](../../../figures/row_major_layout.png)


![](../../../figures/column_major_layout.png)
