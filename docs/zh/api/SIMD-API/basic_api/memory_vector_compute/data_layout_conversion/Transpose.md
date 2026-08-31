# Transpose<a name="ZH-CN_TOPIC_0000001498796777"></a>

## 产品支持情况<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：支持
<!-- end id7 -->
<!-- npu="x90" id8 -->
- Kirin X90：支持
<!-- end id8 -->
<!-- npu="9030" id9 -->
- Kirin 9030：支持
<!-- end id9 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/memory_vector_compute/data_layout_conversion/Transpose_res.md#id1 -->

## 功能说明<a name="section618mcpsimp"></a>

头文件路径为：`"basic_api/kernel_operator_vec_transpose_intf.h"`。

Transpose接口用于实现16\*16的二维矩阵数据块转置或者\[N,C,H,W\]与\[N,H,W,C\]数据格式互相转换。

16\*16的普通转置接口计算原理和参考伪代码如下：

```python
import numpy as np

src = np.random.randn(16, 16).astype(np.float16)
dst = src.T
```

\[N,C,H,W\]与\[N,H,W,C\]数据格式互相转换的增强转置计算原理和参考伪代码如下：

```python
import numpy as np

# transposeParams.transposeType : TRANSPOSE_NCHW2NHWC
src_nchw = np.random.randn(transposeParams.nSize, transposeParams.cSize, transposeParams.hSize, transposeParams.wSize).astype(np.float16)
dst_nhwc = np.transpose(src_nchw, axes=(0,2,3,1))

# transposeParams.transposeType : TRANSPOSE_NHWC2NCHW
src_nhwc = np.random.randn(transposeParams.nSize, transposeParams.hSize, transposeParams.wSize, transposeParams.cSize).astype(np.float16)
dst_nchw = np.transpose(src_nhwc, axes=(0,3,1,2))
```

## 函数原型<a name="section620mcpsimp"></a>

- 普通转置，支持16\*16的二维矩阵数据块进行转置。

  ```cpp
  template <typename T>
  __aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T>& src)
  ```

- 增强转置，支持16\*16的二维矩阵数据块转置，支持\[N,C,H,W\]与\[N,H,W,C\]互相转换。

  ```cpp
  template <typename T>
  __aicore__ inline void Transpose(const LocalTensor<T>& dst, const LocalTensor<T> &src, const LocalTensor<uint8_t> &sharedTmpBuffer, const TransposeParamsExt &transposeParams)
  ```

## 参数说明

**表**  模板参数说明

| 参数名 | 描述 |
| :-- | :-- |
| T | 操作数的数据类型。 |

**表**  接口参数说明

| 参数名称 | 输入/输出 | 含义 |
| :------- | :-------- | :--- |
| dst | 输出 | 目的操作数。<br/>类型为[LocalTensor](../../data_structures/LocalTensor/LocalTensor.md)，支持的TPosition为VECIN/VECCALC/VECOUT。<br/>LocalTensor的起始地址需要32字节对齐。 |
| src | 输入 | 源操作数。<br/>类型为LocalTensor，支持的TPosition为VECIN/VECCALC/VECOUT。<br/>LocalTensor的起始地址需要32字节对齐。<br/>数据类型需要与dst保持一致。 |
| sharedTmpBuffer | 输入 | 共享的临时Buffer，sharedTmpBuffer的大小参考[表sharedTmpBuffer所需的内存](#table4)。 |
| transposeParams | 输入 | 控制Transpose的数据结构。结构体内包含：输入的shape信息和transposeType参数。该数据结构的定义请参考[表TransposeParamsExt结构体内参数说明](#table3)。<br>struct TransposeParamsExt {<br/>    \_\_aicore\_\_ TransposeParamsExt() {}<br/>    \_\_aicore\_\_ TransposeParamsExt(const uint16_t nSizeIn, const uint16_t cSizeIn, const uint16_t hSizeIn,<br/>        const uint16_t wSizeIn, const TransposeType transposeTypeIn)<br/>        : nSize(nSizeIn),<br/>          cSize(cSizeIn),<br/>          hSize(hSizeIn),<br/>          wSize(wSizeIn),<br/>          transposeType(transposeTypeIn)<br/>    {}<br/>    uint16_t nSize = 0;<br/>    uint16_t cSize = 0;<br/>    uint16_t hSize = 0;<br/>    uint16_t wSize = 0;<br/>    TransposeType transposeType = TransposeType::TRANSPOSE\_ND2ND\_B16;<br/>}; |

**表**  TransposeParamsExt结构体内参数说明<a id="table3"></a>

| 参数名称 | 含义 |
| :--- | :--- |
| nSize | n轴长度。默认值为0。<br/>•二维矩阵数据块转置，无需传入，传入数值无效。<br/>•[N,C,H,W]与[N,H,W,C]数据格式互相转换，取值范围：nSize∈[0, 65535]。 |
| cSize | c轴长度。默认值为0。<br/>•二维矩阵数据块转置，无需传入，传入数值无效。<br/>•[N,C,H,W]与[N,H,W,C]数据格式互相转换，取值范围：cSize∈[0, 4095]。 |
| hSize | h轴长度。默认值为0。<br/>•二维矩阵数据块转置，固定传入16。<br/>•[N,C,H,W]与[N,H,W,C]数据格式互相转换，取值范围：hSize \* wSize ∈[0, 4095]，hSize \* wSize \* sizeof(T)需要保证32B对齐。 |
| wSize | w轴长度。默认值为0。<br/>•二维矩阵数据块转置，固定传入16。<br/>•[N,C,H,W]与[N,H,W,C]数据格式互相转换，取值范围：hSize \* wSize ∈[0, 4095]，hSize \* wSize \* sizeof(T)需要保证32B对齐。 |
| transposeType | 数据排布及reshape的类型，类型为TransposeType枚举类。默认值为TRANSPOSE_ND2ND_B16。<br>enum class TransposeType : uint8_t {<br/>    TRANSPOSE_TYPE_NONE,           // API不做任何处理<br/>    TRANSPOSE_NZ2ND_0213,          // 当前不支持<br/>    TRANSPOSE_NZ2NZ_0213,          // 当前不支持<br/>    TRANSPOSE_NZ2NZ_012_WITH_N,    // 当前不支持<br/>    TRANSPOSE_NZ2ND_012_WITH_N,    // 当前不支持<br/>    TRANSPOSE_NZ2ND_012_WITHOUT_N, // 当前不支持<br/>    TRANSPOSE_NZ2NZ_012_WITHOUT_N, // 当前不支持<br/>    TRANSPOSE_ND2ND_ONLY,          // 当前不支持<br/>    TRANSPOSE_ND_UB_GM,            // 当前不支持<br/>    TRANSPOSE_GRAD_ND_UB_GM,       // 当前不支持<br/>    TRANSPOSE_ND2ND_B16,           // [16,16]二维矩阵转置<br/>    TRANSPOSE_NCHW2NHWC,           // [N,C,H,W]-&gt;[N,H,W,C]，<br/>    TRANSPOSE_NHWC2NCHW            // [N,H,W,C]-&gt;[N,C,H,W]<br/>}; |

<!-- npu="950" id10 -->

**表**  Ascend 950PR/Ascend 950DT sharedTmpBuffer所需的内存<a id="table4"></a>

| transposeType | sharedTmpBuffer所需的大小 |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | 不需要临时Buffer。 |
| TRANSPOSE_NCHW2NHWC | 临时Buffer的大小按照下述计算规则（伪代码）进行计算。<br>auto h0 = 16; // 当数据类型的位宽为8时，h0 = 32；其他情况下，h0 = 16<br/>auto w0 = 32 / sizeof(type);  // type代表数据类型<br/>auto tmpBufferSize = (cSize + 2)  * h0 \* w0 \* sizeof(type); |
| TRANSPOSE_NHWC2NCHW | 临时Buffer的大小按照下述计算规则（伪代码）进行计算。<br>auto h0 = 16; // 当数据类型的位宽为8时，h0 = 32；其他情况下，h0 = 16<br/>auto w0 = 32 / sizeof(type);  // type代表数据类型<br/>auto tmpBufferSize = (cSize \* 2 + 1) \* h0 \* w0 \* sizeof(type); |

<!-- end id10 -->

<!-- npu="A3" id11 -->

**表**  Atlas A3 训练系列产品/Atlas A3 推理系列产品sharedTmpBuffer所需的内存<a id="table4"></a>

| transposeType | sharedTmpBuffer所需的大小 |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | 不需要临时Buffer。 |
| TRANSPOSE_NCHW2NHWC | 临时Buffer的大小按照下述计算规则（伪代码）进行计算。<br>auto h0 = 16; // 当数据类型的位宽为8时，h0 = 32；其他情况下，h0 = 16<br/>auto w0 = 32 / sizeof(type);  // type代表数据类型<br/>auto tmpBufferSize = (cSize + 2)  * h0 \* w0 \* sizeof(type); |
| TRANSPOSE_NHWC2NCHW | 临时Buffer的大小按照下述计算规则（伪代码）进行计算。<br>auto h0 = 16; // 当数据类型的位宽为8时，h0 = 32；其他情况下，h0 = 16<br/>auto w0 = 32 / sizeof(type);  // type代表数据类型<br/>auto tmpBufferSize = (cSize \* 2 + 1) \* h0 \* w0 \* sizeof(type); |

<!-- end id11 -->

<!-- npu="910b" id12 -->

**表**  Atlas A2 训练系列产品/Atlas A2 推理系列产品sharedTmpBuffer所需的内存<a id="table4"></a>

| transposeType | sharedTmpBuffer所需的大小 |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | 不需要临时Buffer。 |
| TRANSPOSE_NCHW2NHWC | 临时Buffer的大小按照下述计算规则（伪代码）进行计算。<br>auto h0 = 16; // 当数据类型的位宽为8时，h0 = 32；其他情况下，h0 = 16<br/>auto w0 = 32 / sizeof(type);  // type代表数据类型<br/>auto tmpBufferSize = (cSize + 2)  * h0 \* w0 \* sizeof(type); |
| TRANSPOSE_NHWC2NCHW | 临时Buffer的大小按照下述计算规则（伪代码）进行计算。<br>auto h0 = 16; // 当数据类型的位宽为8时，h0 = 32；其他情况下，h0 = 16<br/>auto w0 = 32 / sizeof(type);  // type代表数据类型<br/>auto tmpBufferSize = (cSize \* 2 + 1) \* h0 \* w0 \* sizeof(type); |

<!-- end id12 -->

<!-- npu="310b" id13 -->

**表**  Atlas 200I/500 A2 推理产品sharedTmpBuffer所需的内存<a id="table4"></a>

| transposeType | sharedTmpBuffer所需的大小 |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | 不需要临时Buffer。 |

<!-- end id13 -->

<!-- npu="310p" id14 -->

**表**  Atlas 推理系列产品AI Core sharedTmpBuffer所需的内存<a id="table4"></a>

| transposeType | sharedTmpBuffer所需的大小 |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | 不需要临时Buffer。 |
| TRANSPOSE_NCHW2NHWC | 不需要临时Buffer。 |
| TRANSPOSE_NHWC2NCHW | 不需要临时Buffer。 |

<!-- end id14 -->

<!-- npu="x90" id15 -->

**表**  Kirin X90 sharedTmpBuffer所需的内存<a id="table4"></a>

| transposeType | sharedTmpBuffer所需的大小 |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | 不需要临时Buffer。 |
| TRANSPOSE_NCHW2NHWC | 临时Buffer的大小按照下述计算规则（伪代码）进行计算。<br>auto h0 = 16; // 当数据类型的位宽为8时，h0 = 32；其他情况下，h0 = 16<br/>auto w0 = 32 / sizeof(type);  // type代表数据类型<br/>auto tmpBufferSize = (cSize + 2)  * h0 \* w0 \* sizeof(type); |
| TRANSPOSE_NHWC2NCHW | 临时Buffer的大小按照下述计算规则（伪代码）进行计算。<br>auto h0 = 16; // 当数据类型的位宽为8时，h0 = 32；其他情况下，h0 = 16<br/>auto w0 = 32 / sizeof(type);  // type代表数据类型<br/>auto tmpBufferSize = (cSize \* 2 + 1) \* h0 \* w0 \* sizeof(type); |

<!-- end id15 -->

<!-- npu="9030" id16 -->

**表**  Kirin 9030 sharedTmpBuffer所需的内存<a id="table4"></a>

| transposeType | sharedTmpBuffer所需的大小 |
| :-- | :-- |
| TRANSPOSE_ND2ND_B16 | 不需要临时Buffer。 |
| TRANSPOSE_NCHW2NHWC | 临时Buffer的大小按照下述计算规则（伪代码）进行计算。<br>auto h0 = 16; // 当数据类型的位宽为8时，h0 = 32；其他情况下，h0 = 16<br/>auto w0 = 32 / sizeof(type);  // type代表数据类型<br/>auto tmpBufferSize = (cSize + 2)  * h0 \* w0 \* sizeof(type); |
| TRANSPOSE_NHWC2NCHW | 临时Buffer的大小按照下述计算规则（伪代码）进行计算。<br>auto h0 = 16; // 当数据类型的位宽为8时，h0 = 32；其他情况下，h0 = 16<br/>auto w0 = 32 / sizeof(type);  // type代表数据类型<br/>auto tmpBufferSize = (cSize \* 2 + 1) \* h0 \* w0 \* sizeof(type); |

<!-- end id16 -->

## 数据类型

- 普通转置：

  <!-- npu="950" id17 -->
  Ascend 950PR/Ascend 950DT，操作数支持的数据类型为：int16_t、uint16_t、half。
  <!-- end id17 -->

  <!-- npu="A3" id18 -->
  Atlas A3 训练系列产品/Atlas A3 推理系列产品，操作数支持的数据类型为：int16_t、uint16_t、half。
  <!-- end id18 -->

  <!-- npu="910b" id19 -->
  Atlas A2 训练系列产品/Atlas A2 推理系列产品，操作数支持的数据类型为：int16_t、uint16_t、half。
  <!-- end id19 -->

  <!-- npu="310b" id20 -->
  Atlas 200I/500 A2 推理产品，操作数支持的数据类型为：int16_t、uint16_t、half。
  <!-- end id20 -->

  <!-- npu="310p" id21 -->
  Atlas 推理系列产品AI Core，操作数支持的数据类型为：int16_t、uint16_t、half。
  <!-- end id21 -->

  <!-- npu="910" id22 -->
  Atlas 训练系列产品，操作数支持的数据类型为：int16_t、uint16_t、half。
  <!-- end id22 -->

  <!-- npu="x90" id23 -->
  Kirin X90，操作数支持的数据类型为：int16_t、uint16_t、half。
  <!-- end id23 -->

  <!-- npu="9030" id24 -->
  Kirin 9030，操作数支持的数据类型为：int16_t、uint16_t、half。
  <!-- end id24 -->

- 增强转置：
  - transposeType为TRANSPOSE\_ND2ND\_B16：

    <!-- npu="950" id25 -->
    Ascend 950PR/Ascend 950DT，操作数支持的数据类型为：int16_t、uint16_t、half。
    <!-- end id25 -->

    <!-- npu="A3" id26 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，操作数支持的数据类型为：uint16_t。
    <!-- end id26 -->

    <!-- npu="910b" id27 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，操作数支持的数据类型为：uint16_t。
    <!-- end id27 -->

    <!-- npu="310b" id28 -->
    Atlas 200I/500 A2 推理产品，操作数支持的数据类型为：uint16_t。
    <!-- end id28 -->

    <!-- npu="310p" id29 -->
    Atlas 推理系列产品AI Core，操作数支持的数据类型为：uint16_t。
    <!-- end id29 -->

  - transposeType为TRANSPOSE\_NCHW2NHWC或TRANSPOSE\_NHWC2NCHW：

    <!-- npu="950" id30 -->
    Ascend 950PR/Ascend 950DT，操作数支持的数据类型为：int8_t、uint8_t、fp4x2_e2m1_t、fp4x2_e1m2_t、hifloat8_t、fp8_e8m0_t、fp8_e5m2_t、fp8_e4m3fn_t、int4x2_t、int16_t、uint16_t、half、bfloat16_t、int32_t、uint32_t、float、complex32。
    <!-- end id30 -->

    <!-- npu="A3" id31 -->
    Atlas A3 训练系列产品/Atlas A3 推理系列产品，操作数支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
    <!-- end id31 -->

    <!-- npu="910b" id32 -->
    Atlas A2 训练系列产品/Atlas A2 推理系列产品，操作数支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
    <!-- end id32 -->

    <!-- npu="310p" id33 -->
    Atlas 推理系列产品AI Core，操作数支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
    <!-- end id33 -->

    <!-- npu="x90" id34 -->
    Kirin X90，操作数支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
    <!-- end id34 -->

    <!-- npu="9030" id35 -->
    Kirin 9030，操作数支持的数据类型为：int8_t、uint8_t、int16_t、uint16_t、half、int32_t、uint32_t、float。
    <!-- end id35 -->

## 返回值说明

无。

## 约束说明

- 操作数地址对齐要求请参见[Unified Buffer地址对齐约束](../../../general_description_and_constraints.md#section796754519912)。
- 普通转置接口支持src和dst复用。
- 增强转置接口，transposeType为TRANSPOSE\_ND2ND\_B16时支持src和dst复用，transposeType为TRANSPOSE\_NCHW2NHWC、TRANSPOSE\_NHWC2NCHW时不支持src和dst复用。
- 二维矩阵数据块转置时，nSize、cSize无需传入，传入数值无效；hSize、wSize固定传入16。
- 增强转置接口，transposeType为TRANSPOSE\_NCHW2NHWC、TRANSPOSE\_NHWC2NCHW时，如果nSize、cSize、hSize、wSize为0，不会执行计算操作，不会对目的操作数进行写入。
- \[N,C,H,W\]与\[N,H,W,C\]数据格式互相转换，参数取值范围：nSize∈[0, 65535]，cSize∈[0, 4095]，hSize \* wSize ∈[0, 4095]，hSize \* wSize \* sizeof(T)需要保证32B对齐。
- 转置增强接口中，入参sharedTmpBuffer的大小不得小于计算所需的最小阈值。

## 调用示例

- 普通接口调用示例片段，完整片段请参考[Transpose类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/transpose)场景一，该示例对\[16,16\]的half类型矩阵进行转置。

  ```cpp
  // dstLocal：目的操作数tensor
  // srcLocal：源操作数tensor
  AscendC::Transpose<half>(dstLocal, srcLocal);
  ```

  ```plain
  输入数据src_gm:
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
  
  输出数据dst_gm:
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

- 增强接口调用示例片段，完整代码请参考[Transpose类样例](../../../../../../../examples/01_simd_cpp_api/03_basic_api/01_memory_vector_compute/transpose)场景二，完成half类型的\[N,C,H,W\]->\[N,H,W,C\]转置。

  ```cpp
  AscendC::TransposeParamsExt transposeParams;
  transposeParams.nSize = N; // N轴长度
  transposeParams.cSize = C; // C轴长度
  transposeParams.hSize = H; // H轴长度
  transposeParams.wSize = W; // W轴长度
  transposeParams.transposeType = transposeType; 
  AscendC::Transpose(dstLocal, srcLocal, stackBuffer, transposeParams);
  ```
