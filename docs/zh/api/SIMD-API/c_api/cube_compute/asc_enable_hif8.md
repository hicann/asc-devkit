# asc_enable_hif8

## 产品支持情况

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT：支持
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 训练系列产品/Atlas A3 推理系列产品：不支持
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 训练系列产品/Atlas A2 推理系列产品：不支持
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 推理产品：不支持
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas 推理系列产品AI Core：不支持
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas 推理系列产品Vector Core：不支持
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas 训练系列产品：不支持
<!-- end id7 -->

## 功能说明

用于设置Mmad计算开启HiF8模式，开启该模式后L0A Buffer/L0B Buffer中的8bit数据将在参与矩阵乘法运算前被转化为hifloat8_t类型数据。

## 函数原型

 ```cpp
__aicore__ inline void asc_enable_hif8()
```

## 返回值说明

无

## 流水类型

PIPE_S

## 约束说明

无

## 调用示例

```cpp
constexpr uint64_t total_length = 128;
// 左/右矩阵以 int8_t 存放于 L0A/L0B（需先完成 GM/L1→L0 搬运）
__cc__ int32_t c_matrix[total_length];
__ca__ int8_t a_matrix[total_length];
__cb__ int8_t b_matrix[total_length];

uint16_t left_height = 16;  // M
uint16_t n_dim = 16;        // K
uint16_t right_width = 16;  // N

// 在 asc_mmad 前调用：将 L0 中的 8bit 操作数转换为 hifloat8_t 后参与矩阵运算
asc_enable_hif8();

uint8_t unit_flag = 0;
bool disable_gemv = false;
bool c_matrix_source = false;
bool c_matrix_init_val = true;
asc_mmad_sync(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width,
              unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
```
