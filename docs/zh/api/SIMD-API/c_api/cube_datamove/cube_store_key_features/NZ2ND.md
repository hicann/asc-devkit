# NZ2ND

## 特性说明

矩阵搬出时，Nz2ND功能即为实现Nz格式到ND格式的矩阵转换。

Nz2ND转化过程可以参考以下伪代码（所有地址按字节计数，src_matrix_stride_64b为统一的64B步长记号，用于统一说明源矩阵间隔）：

```cpp
for (i = 0; i < matrix_num; i++) {
    // fractal_size = 16 * sizeof(L0C_element)
    src_temp_nd_addr = src + src_matrix_stride_64b * fractal_size * i;
    dst_temp_nd_addr = dst + dst_matrix_stride * sizeof(dst_element) * i;
    for (j = 0; j < m_size; j++) {
        src_temp_n_addr = src_temp_nd_addr + j * 16 * sizeof(L0C_element);
        dst_temp_n_addr = dst_temp_nd_addr + j * dst_stride * sizeof(dst_element);
        for (k = 0; k < ((n_size + 15) / 16); k++) {
            src_block_addr = src_temp_n_addr + k * src_stride * 16 * sizeof(L0C_element);
            dst_block_addr = dst_temp_n_addr + k * 16 * sizeof(dst_element);
        }
    }
}
```

伪代码使用的参数具体说明如下：

- src表示源操作数搬运的起始地址。
- dst表示目的操作数搬运的起始地址。
- 源操作数每个Nz矩阵的起始地址为src\_temp\_nd\_addr。
- 目的操作数每个ND矩阵的起始地址为dst\_temp\_nd\_addr。
- 源操作数中每个Nz矩阵中每一行的起始地址为src\_temp\_n\_addr。
- 目的操作数每个ND矩阵中每一行的起始地址为dst\_temp\_n\_addr。
- 在每一行中，以16个数据为一个数据块，源操作数每块数据块的起始地址为src\_block\_addr，目的操作数中每一块数据块的起始地址为dst\_block\_addr。

以下参数为Nz2ND用户可配置参数，含义及取值范围参见[C API搬出参数](../asc_copy_l0c2gm/asc_copy_l0c2gm.md)：

- m_size与n_size分别表示矩阵m方向和n方向上的维度。
- src_stride表示Nz矩阵中不同Z分形列的间隔，单位为C0\_Size。
- dst_stride表示目标ND矩阵中不同行的间隔，单位为元素。
- matrix_num表示需要做Nz2ND转化的矩阵个数。
- src_matrix_stride_64b表示不同Nz矩阵之间的间隔，单位为64B，即16×sizeof(L0C_element)。
- dst_matrix_stride表示不同目标ND矩阵之间的间隔，单位为元素。

## 特性约束

- 当开启Channel Split/Channel Merge功能（Nz2Nz）时，此功能不可用。
- 目标操作数地址之间不能重叠。

## 使用示例

如下图所示，在L0C Buffer中有两个Nz格式的矩阵，每个矩阵N方向上维度为32，M方向上维度为48，经过Nz2ND转换，将数据格式变为右部分所示排布。

**图1** Nz2ND示意图

![](../../../../figures/fixpipe_nz2nd_c_api.png)

对于该场景，图中参数与C API配置的对应关系如下：

- matrix_num = 2，表示需要做Nz2ND转化的矩阵个数为2。
- n_size = 32，表示源Nz矩阵在N方向上的大小为32个元素。
- m_size = 48，表示源Nz矩阵在M方向上的大小为48个元素。
- src_stride = 64，表示源Nz矩阵中相邻Z排布的起始地址之间的间隔为64 \* C0\_Size\(C0\_Size = 16\*sizeof\(T\)，T为操作数的数据类型\)字节。
- dst_stride = 64，表示目的ND矩阵每一行中的元素个数为64。
- src_matrix_stride_64b = 256，表示不同Nz矩阵起始地址之间的间隔为256 \* C0\_Size字节。
- dst_matrix_stride = 4096，表示目的相邻ND矩阵起始地址之间的偏移为4096个元素。
