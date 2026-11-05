# Transpose<a name="ZH-CN_TOPIC_0000001498796777"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:29:59.956Z pushedAt=2026-10-28T03:14:39.485Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_layout_conversion/Transpose_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

The header file path is `"basic_api/kernel_operator_vec_transpose_intf.h"`.

The Transpose API is used to transpose a 16\*16 two-dimensional matrix data block or to convert between the \[N,C,H,W\] and \[N,H,W,C\] data formats.

The computation principle and reference pseudocode of the normal transpose API for 16\*16 are as follows:

```python
import numpy as np

src = np.random.randn(16, 16).astype(np.float16)
dst = src.T
```

The computation principle and reference pseudocode of the enhanced transpose for mutual conversion between the \[N,C,H,W\] and \[N,H,W,C\] data formats are as follows:

```python
import numpy as np

# transposeParams.transposeType : TRANSPOSE_NCHW2NHWC
src_nchw = np.random.randn(transposeParams.nSize, transposeParams.cSize, transposeParams.hSize, transposeParams.wSize).astype(np.float16)
dst_nhwc = np.transpose(src_nchw, axes=(0,2,3,1))

# transposeParams.transposeType : TRANSPOSE_NHWC2NCHW
src_nhwc = np.random.randn(transposeParams.nSize, transposeParams.hSize, transposeParams.wSize, transposeParams.cSize).astype(np.float16)
dst_nchw = np.transpose(src_nhwc, axes=(0,3,1,2))
```

## Prototype<a name="section620mcpsimp"></a>

- Normal transpose, which supports transposing a 16\*16 two-dimensional matrix data block.

  ```cpp
  template <typename T>
  __aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T>& src)
  ```

- Enhanced transpose, which supports transposing a 16\*16 two-dimensional matrix data block and supports mutual conversion between \[N,C,H,W\] and \[N,H,W,C\].

  ```cpp
  template <typename T>
  __aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T> &src, const LocalTensor<uint8_t> &sharedTmpBuffer, const TransposeParamsExt &transposeParams)
  ```

## Parameters

**Table**  Template parameter description

| Parameter | Description |
| :-- | :-- |
| T | Data type of the operand. |

**Table**  API parameter description

| Parameter | Input/Output | Meaning |
| :------- | :-------- | :--- |
| dst | Output | Destination operand.<br/>Type: [LocalTensor](../../data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br/>The start address of LocalTensor must be 32-byte aligned. |
| src | Input | Source operand.<br/>Type: LocalTensor. Supported TPosition: VECIN/VECCALC/VECOUT.<br/>The start address of LocalTensor must be 32-byte aligned.<br/>The data type must be consistent with dst. |
| sharedTmpBuffer | Input | Shared temporary buffer. For the size of sharedTmpBuffer, see [Table: Memory required by sharedTmpBuffer](#table4). |
| transposeParams | Input | Data structure that controls Transpose. The structure contains the input shape information and the transposeType parameter. For the definition of this data structure, see [Table: Parameter description of the TransposeParamsExt structure](#table3).<br>struct TransposeParamsExt {<br/>    \_\_aicore\_\_ TransposeParamsExt() {}<br/>    \_\_aicore\_\_ TransposeParamsExt(const uint16_t nSizeIn, const uint16_t cSizeIn, const uint16_t hSizeIn,<br/>        const uint16_t wSizeIn, const TransposeType transposeTypeIn)<br/>        : nSize(nSizeIn),<br/>          cSize(cSizeIn),<br/>          hSize(hSizeIn),<br/>          wSize(wSizeIn),<br/>          transposeType(transposeTypeIn)<br/>    {}<br/>    uint16_t nSize = 0;<br/>    uint16_t cSize = 0;<br/>    uint16_t hSize = 0;<br/>    uint16_t wSize = 0;<br/>    TransposeType transposeType = TransposeType::TRANSPOSE\_ND2ND\_B16;<br/>}; |

**Table**  Parameter description of the TransposeParamsExt structure<a id="table3"></a>

| Parameter | Meaning |
| :--- | :--- |
| nSize | n-axis length. Default value: 0.<br/>•For two-dimensional matrix data block transpose, this parameter is not required; the passed value is invalid.<br/>•For [N,C,H,W] and [N,H,W,C] data format mutual conversion, value range: nSize∈[0, 65535]. |
| cSize | c-axis length. Default value: 0.<br/>•For two-dimensional matrix data block transpose, this parameter is not required; the passed value is invalid.<br/>•For [N,C,H,W] and [N,H,W,C] data format mutual conversion, value range: cSize∈[0, 4095]. |
| hSize | h-axis length. Default value: 0.<br/>•For two-dimensional matrix data block transpose, fixed to 16.<br/>•For [N,C,H,W] and [N,H,W,C] data format mutual conversion, value range: hSize \* wSize ∈[0, 4095], and hSize \* wSize \* sizeof(T) must be 32B-aligned. |
| wSize | w-axis length. Default value: 0.<br/>•For two-dimensional matrix data block transpose, fixed to 16.<br/>•For [N,C,H,W] and [N,H,W,C] data format mutual conversion, value range: hSize \* wSize ∈[0, 4095], and hSize \* wSize \* sizeof(T) must be 32B-aligned. |
| transposeType | Type of data layout and reshape, of the TransposeType enum class. Default value: TRANSPOSE_ND2ND_B16.<br>enum class TransposeType : uint8_t {<br/>    TRANSPOSE_TYPE_NONE,           // The API does not perform any processing.<br/>    TRANSPOSE_NZ2ND_0213,          // Currently not supported.<br/>    TRANSPOSE_NZ2NZ_0213,          // Currently not supported.<br/>    TRANSPOSE_NZ2NZ_012_WITH_N,    // Currently not supported.<br/>    TRANSPOSE_NZ2ND_012_WITH_N,    // Currently not supported.<br/>    TRANSPOSE_NZ2ND_012_WITHOUT_N, // Currently not supported.<br/>    TRANSPOSE_NZ2NZ_012_WITHOUT_N, // Currently not supported.<br/>    TRANSPOSE_ND2ND_ONLY,          // Currently not supported.<br/>    TRANSPOSE_ND_UB_GM,            // Currently not supported.<br/>    TRANSPOSE_GRAD_ND_UB_GM,       // Currently not supported.<br/>    TRANSPOSE_ND2ND_B16,           // [16,16] two-dimensional matrix transpose.<br/>    TRANSPOSE_NCHW2NHWC,           // [N,C,H,W]->[N,H,W,C],<br/>    TRANSPOSE_NHWC2NCHW            // [N,H,W,C]->[N,C,H,W]<br/>}; |

<!-- npu="950" id10 -->

**Table**  Memory required by sharedTmpBuffer for Ascend 950PR&950DT products<a id="table4"></a>

| transposeType | Size required by sharedTmpBuffer |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | No temporary buffer is required. |
| TRANSPOSE_NCHW2NHWC | The size of the temporary buffer is calculated according to the following calculation rule (pseudocode).<br>auto h0 = 16; // When the bit width of the data type is 8, h0 = 32; otherwise, h0 = 16.<br/>auto w0 = 32 / sizeof(type);  // type represents the data type.<br/>auto tmpBufferSize = (cSize + 2)  * h0 \* w0 \* sizeof(type); |
| TRANSPOSE_NHWC2NCHW | The size of the temporary buffer is calculated according to the following calculation rule (pseudocode).<br>auto h0 = 16; // When the bit width of the data type is 8, h0 = 32; otherwise, h0 = 16.<br/>auto w0 = 32 / sizeof(type);  // type represents the data type.<br/>auto tmpBufferSize = (cSize \* 2 + 1) \* h0 \* w0 \* sizeof(type); |

<!-- end id10 -->

<!-- npu="A3" id11 -->

**Table**  Memory required by sharedTmpBuffer for Atlas A3 products<a id="table4"></a>

| transposeType | Size required by sharedTmpBuffer |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | No temporary buffer is required. |
| TRANSPOSE_NCHW2NHWC | The size of the temporary buffer is calculated according to the following calculation rule (pseudocode).<br>auto h0 = 16; // When the bit width of the data type is 8, h0 = 32; otherwise, h0 = 16.<br/>auto w0 = 32 / sizeof(type);  // type represents the data type.<br/>auto tmpBufferSize = (cSize + 2)  * h0 \* w0 \* sizeof(type); |
| TRANSPOSE_NHWC2NCHW | The size of the temporary buffer is calculated according to the following calculation rule (pseudocode).<br>auto h0 = 16; // When the bit width of the data type is 8, h0 = 32; otherwise, h0 = 16.<br/>auto w0 = 32 / sizeof(type);  // type represents the data type.<br/>auto tmpBufferSize = (cSize \* 2 + 1) \* h0 \* w0 \* sizeof(type); |

<!-- end id11 -->

<!-- npu="910b" id12 -->

**Table**  Memory required by sharedTmpBuffer for Atlas A2 products<a id="table4"></a>

| transposeType | Size required by sharedTmpBuffer |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | No temporary buffer is required. |
| TRANSPOSE_NCHW2NHWC | The size of the temporary buffer is calculated according to the following calculation rule (pseudocode).<br>auto h0 = 16; // When the bit width of the data type is 8, h0 = 32; otherwise, h0 = 16.<br/>auto w0 = 32 / sizeof(type);  // type represents the data type.<br/>auto tmpBufferSize = (cSize + 2)  * h0 \* w0 \* sizeof(type); |
| TRANSPOSE_NHWC2NCHW | The size of the temporary buffer is calculated according to the following calculation rule (pseudocode).<br>auto h0 = 16; // When the bit width of the data type is 8, h0 = 32; otherwise, h0 = 16.<br/>auto w0 = 32 / sizeof(type);  // type represents the data type.<br/>auto tmpBufferSize = (cSize \* 2 + 1) \* h0 \* w0 \* sizeof(type); |

<!-- end id12 -->

<!-- npu="310b" id13 -->

**Table**  Memory required by sharedTmpBuffer for Atlas 200I/500 A2 inference products<a id="table4"></a>

| transposeType | Size required for sharedTmpBuffer |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | No temporary buffer is required. |

<!-- end id13 -->

<!-- npu="310p" id14 -->

**Table**  Memory required for sharedTmpBuffer on the Atlas inference products AI Core<a id="table4"></a>

| transposeType | Size required for sharedTmpBuffer |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | No temporary buffer is required. |
| TRANSPOSE_NCHW2NHWC | No temporary buffer is required. |
| TRANSPOSE_NHWC2NCHW | No temporary buffer is required. |

<!-- end id14 -->

## Data Types

- Normal transpose:

  <!-- npu="950" id17 -->
  Ascend 950PR&950DT products support the following operand data types: int16_t, uint16_t, half.
  <!-- end id17 -->

  <!-- npu="A3" id18 -->
  Atlas A3 products support the following operand data types: int16_t, uint16_t, half.
  <!-- end id18 -->

  <!-- npu="910b" id19 -->
  Atlas A2 products support the following operand data types: int16_t, uint16_t, half.
  <!-- end id19 -->

  <!-- npu="310b" id20 -->
  Atlas 200I/500 A2 inference products support the following operand data types: int16_t, uint16_t, half.
  <!-- end id20 -->

  <!-- npu="310p" id21 -->
  Atlas inference products AI Core supports the following operand data types: int16_t, uint16_t, half.
  <!-- end id21 -->

  <!-- npu="910" id22 -->
  Atlas training products supports the following operand data types: int16_t, uint16_t, half.
  <!-- end id22 -->

- Enhanced transpose:
  - transposeType is TRANSPOSE\_ND2ND\_B16:

    <!-- npu="950" id25 -->
    For the Ascend 950PR&950DT products, the supported data types of the operand are: int16_t, uint16_t, half.
    <!-- end id25 -->

    <!-- npu="A3" id26 -->
    For the Atlas A3 products, the supported data types of the operand are: uint16_t.
    <!-- end id26 -->

    <!-- npu="910b" id27 -->
    For the Atlas A2 products, the supported data types of the operand are: uint16_t.
    <!-- end id27 -->

    <!-- npu="310b" id28 -->
    For the Atlas 200I/500 A2 inference products, the supported data types of the operand are: uint16_t.
    <!-- end id28 -->

    <!-- npu="310p" id29 -->
    For the Atlas inference products AI Core, the supported data types of the operand are: uint16_t.
    <!-- end id29 -->

  - When transposeType is TRANSPOSE\_NCHW2NHWC or TRANSPOSE\_NHWC2NCHW:

    <!-- npu="950" id30 -->
    For the Ascend 950PR&950DT products, the supported data types of the operand are: int8_t, uint8_t, fp4x2_e2m1_t, fp4x2_e1m2_t, hifloat8_t, fp8_e8m0_t, fp8_e5m2_t, fp8_e4m3fn_t, int4x2_t, int16_t, uint16_t, half, bfloat16_t, int32_t, uint32_t, float, complex32.
    <!-- end id30 -->

    <!-- npu="A3" id31 -->
    For the Atlas A3 products, the supported data types of the operand are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
    <!-- end id31 -->

    <!-- npu="910b" id32 -->
    For the Atlas A2 products, the supported data types of the operand are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
    <!-- end id32 -->

    <!-- npu="310p" id33 -->
    For the Atlas inference products AI Core, the supported data types of the operand are: int8_t, uint8_t, int16_t, uint16_t, half, int32_t, uint32_t, float.
    <!-- end id33 -->

## Return Value

None.

## Constraints

- For operand address alignment requirements, see [Unified Buffer Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- The normal transpose API supports src and dst reuse.
- For the enhanced transpose API, src and dst reuse is supported when transposeType is TRANSPOSE\_ND2ND\_B16, and is not supported when transposeType is TRANSPOSE\_NCHW2NHWC or TRANSPOSE\_NHWC2NCHW.
- When transposing a two-dimensional matrix data block, nSize and cSize do not need to be passed, and the passed values are invalid; hSize and wSize are fixed at 16.
- For the enhanced transpose API, when transposeType is TRANSPOSE\_NCHW2NHWC or TRANSPOSE\_NHWC2NCHW, if nSize, cSize, hSize, and wSize are 0, no computation is performed and no data is written to the destination operand.
- For mutual conversion between \[N,C,H,W\] and \[N,H,W,C\] data formats, the parameter value ranges are as follows: nSize∈[0, 65535], cSize∈[0, 4095], hSize \* wSize ∈[0, 4095], and hSize \* wSize \* sizeof(T) must be 32B aligned.
- In the enhanced transpose API, the size of the sharedTmpBuffer input parameter must not be smaller than the minimum threshold required for computation.

## Example

- For a snippet of the normal API example, see [Transpose class example](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/transpose) scenario 1 for the complete snippet. This example transposes a half-type matrix of \[16,16\].

  ```cpp
  // dstLocal: destination operand tensor
  // srcLocal: source operand tensor
  AscendC::Transpose<half>(dstLocal, srcLocal);
  ```

  ```plain
  Input data src_gm:
  [[  0.   1.   2.   3.   4.   5.   6.   7.   8.   9.  10.  11.  12.  13.
     14.  15.]
   [ 16.  17.  18.  19.  20.  21.  22.  23.  24.  25.  26.  27.  28.  29.
     30.  31.]
   [ 32.  33.  34.  35.  36.  37.  38.  39.  40.  41.  42.  43.  44.  45.
     46.  47.]
   [ 48.  49.  50.  51.  52.  53.  54.  55.  56.  57.  58.  59.  60.  61.
     62.  63.]
   [ 64.  65.  66.  67.  68.  69.  70.  71.  72.  73.  74.  75.  76.  77.
     78.  79.]
   [ 80.  81.  82.  83.  84.  85.  86.  87.  88.  89.  90.  91.  92.  93.
     94.  95.]
   [ 96.  97.  98.  99. 100. 101. 102. 103. 104. 105. 106. 107. 108. 109.
    110. 111.]
   [112. 113. 114. 115. 116. 117. 118. 119. 120. 121. 122. 123. 124. 125.
    126. 127.]
   [128. 129. 130. 131. 132. 133. 134. 135. 136. 137. 138. 139. 140. 141.
    142. 143.]
   [144. 145. 146. 147. 148. 149. 150. 151. 152. 153. 154. 155. 156. 157.
    158. 159.]
   [160. 161. 162. 163. 164. 165. 166. 167. 168. 169. 170. 171. 172. 173.
    174. 175.]
   [176. 177. 178. 179. 180. 181. 182. 183. 184. 185. 186. 187. 188. 189.
    190. 191.]
   [192. 193. 194. 195. 196. 197. 198. 199. 200. 201. 202. 203. 204. 205.
    206. 207.]
   [208. 209. 210. 211. 212. 213. 214. 215. 216. 217. 218. 219. 220. 221.
    222. 223.]
   [224. 225. 226. 227. 228. 229. 230. 231. 232. 233. 234. 235. 236. 237.
    238. 239.]
   [240. 241. 242. 243. 244. 245. 246. 247. 248. 249. 250. 251. 252. 253.
    254. 255.]]
  
  Output data dst_gm:
  [[  0.  16.  32.  48.  64.  80.  96. 112. 128. 144. 160. 176. 192. 208.
    224. 240.]
   [  1.  17.  33.  49.  65.  81.  97. 113. 129. 145. 161. 177. 193. 209.
    225. 241.]
   [  2.  18.  34.  50.  66.  82.  98. 114. 130. 146. 162. 178. 194. 210.
    226. 242.]
   [  3.  19.  35.  51.  67.  83.  99. 115. 131. 147. 163. 179. 195. 211.
    227. 243.]
   [  4.  20.  36.  52.  68.  84. 100. 116. 132. 148. 164. 180. 196. 212.
    228. 244.]
   [  5.  21.  37.  53.  69.  85. 101. 117. 133. 149. 165. 181. 197. 213.
    229. 245.]
   [  6.  22.  38.  54.  70.  86. 102. 118. 134. 150. 166. 182. 198. 214.
    230. 246.]
   [  7.  23.  39.  55.  71.  87. 103. 119. 135. 151. 167. 183. 199. 215.
    231. 247.]
   [  8.  24.  40.  56.  72.  88. 104. 120. 136. 152. 168. 184. 200. 216.
    232. 248.]
   [  9.  25.  41.  57.  73.  89. 105. 121. 137. 153. 169. 185. 201. 217.
    233. 249.]
   [ 10.  26.  42.  58.  74.  90. 106. 122. 138. 154. 170. 186. 202. 218.
    234. 250.]
   [ 11.  27.  43.  59.  75.  91. 107. 123. 139. 155. 171. 187. 203. 219.
    235. 251.]
   [ 12.  28.  44.  60.  76.  92. 108. 124. 140. 156. 172. 188. 204. 220.
    236. 252.]
   [ 13.  29.  45.  61.  77.  93. 109. 125. 141. 157. 173. 189. 205. 221.
    237. 253.]
   [ 14.  30.  46.  62.  78.  94. 110. 126. 142. 158. 174. 190. 206. 222.
    238. 254.]
   [ 15.  31.  47.  63.  79.  95. 111. 127. 143. 159. 175. 191. 207. 223.
    239. 255.]]
  ```

- For the enhanced API example snippet, see [Transpose class sample](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/transpose) scenario 2 for the complete code, which implements the \[N,C,H,W\]->\[N,H,W,C\] transpose of the half type.

  ```cpp
  AscendC::TransposeParamsExt transposeParams;
  transposeParams.nSize = N; // N axis length
  transposeParams.cSize = C; // C axis length
  transposeParams.hSize = H; // H axis length
  transposeParams.wSize = W; // W axis length
  transposeParams.transposeType = transposeType; 
  AscendC::Transpose(dstLocal, srcLocal, stackBuffer, transposeParams);
  ```
