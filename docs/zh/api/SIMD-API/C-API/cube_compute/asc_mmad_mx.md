# asc_mmad_mx

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

完成包含放缩功能的矩阵乘加操作。计算公式如下：

$$
c_{matrix} = (a_{matrix} * b_{matrix}) + c_{matrix}
$$

## 函数原型

- 常规计算

    ```cpp
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val) 
    ```

- 同步计算

    ```cpp
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e1m2_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e1m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp4x2_e2m1_t* a_matrix, __cb__ fp4x2_e2m1_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e4m3fn_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e4m3fn_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    __aicore__ inline void asc_mmad_mx_sync(__cc__ float* c_matrix, __ca__ fp8_e5m2_t* a_matrix, __cb__ fp8_e5m2_t* b_matrix, uint16_t left_height, uint16_t n_dim, uint16_t right_width, uint8_t unit_flag, bool disable_gemv, bool c_matrix_source, bool c_matrix_init_val)
    ```

## 参数说明

| 参数名       | 输入/输出 | 描述               |
| --------- | ----- | ---------------- |
| c_matrix      | 输出    | 	目的操作数，结果矩阵。            |
| a_matrix      | 输入    |   源操作数，左矩阵A。|
| b_matrix      | 输入    | 	源操作数，右矩阵B。      |
| left_height   | 输入    | 	左矩阵height ，取值范围为[0,4095]。      |
| n_dim         | 输入    | 	左矩阵width、右矩阵height，取值范围为[0,4095]。      |
| right_width   | 输入    | 	右矩阵width，取值范围为[0,4095]。      |
| unit_flag     | 输入    | 	unit_flag是一种asc_mmad_mx接口细粒度的并行，开启该功能后，硬件每计算完一个分形，计算结果就会被搬出，该功能不适用于L0C Buffer累加的场景。取值说明如下： <br>&bull; 0：保留值； <br>&bull; 2：开启unit_flag，硬件执行完指令后，不会关闭unit_flag功能；<br>&bull; 3：开启unit_flag，硬件执行完指令后，会关闭unit_flag功能。<br> 开启该功能时，矩阵计算的unit_flag在最后一个分形设置为3，其余分形计算设置为2即可。     |
| disable_gemv  | 输入    | 是否关闭GEMV模式，false表示开启GEMV模式，true表示关闭GEMV模式。<br>GEMV(General Matrix-Vector Multiplication)表示实现矩阵和向量的乘积。当left_height=1时，开启GEMV后，从L0A Buffer读取数据时，将以ND格式进行读取，而不会将其视为ZZ格式。 |
| c_matrix_source       | 输入    | 	配置C矩阵初始值是否来源于BiasTable（存放Bias的硬件缓存区）。取值说明如下：  <br>&bull; true：来源于BiasTable。 <br>&bull; false：来源于L0C。 |
| c_matrix_init_val      | 输入    | 	配置C矩阵初始值是否为0。取值说明如下：  <br>&bull; true：C矩阵初始值为0。 <br>&bull; false：C矩阵初始值通过c_matrix_source参数进行配置。     |

## 返回值说明

无

## 流水类型

PIPE_M

## 约束说明

- 当left_height、right_width、n_dim中的任意一个值为0时，该指令不会被执行。
- 当开启GEMV模式，即disable_gemv=false时，必须要满足left_height=1。
- 操作数地址对齐约束请参考[通用地址对齐约束](../通用说明和约束.md#通用地址对齐约束)。

## 调用示例

```cpp
// total_length指参与搬运的数据总长度
constexpr uint64_t total_length = 128;
// 以下三个参数分别对应矩阵c,a,b的地址
__cc__ float c_matrix[total_length];
__ca__ fp8_e4m3fn_t a_matrix[total_length];
__cb__ fp8_e4m3fn_t b_matrix[total_length];

uint16_t left_height = 16;
uint16_t n_dim = 16;
uint16_t right_width =16;
uint8_t unit_flag = 0;
bool disable_gemv = false;
bool c_matrix_source = false;
bool c_matrix_init_val = true;
// 函数调用
asc_mmad_mx_sync(c_matrix, a_matrix, b_matrix, left_height, n_dim, right_width, unit_flag, disable_gemv, c_matrix_source, c_matrix_init_val);
```
