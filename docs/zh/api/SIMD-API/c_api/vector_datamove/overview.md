# 概述

## API列表

**表1** 矢量数据搬运接口

| 接口名称 | 接口描述 | 最细粒度公共头文件 |
| --- | --- | --- |
| [asc_copy](asc_copy.md) | 将数据从UB搬运到UB。支持高维切分。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_ub2l1](asc_copy_ub2l1.md) | 将数据从UB搬运到L1 Buffer。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_ub2ub](asc_copy_ub2ub.md) | 将数据从UB搬运到UB。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md) | 多维数据搬运接口，将数据从Global Memory（GM）搬运到UB。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_ndim_copy_dci](asc_ndim_copy_dci.md) | 执行N维搬运场景所需的数据缓存失效（Data Cache Invalidate，DCI）操作。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_set_copy_pad_val](asc_set_copy_pad_val.md) | 与`asc_copy_gm2ub_align`配合使用，设置搬运数据块左右两侧的填充值。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_set_gm2ub_loop1_stride](asc_set_gm2ub_loop1_stride.md) | 使用[asc_copy_gm2ub_align](asc_copy_gm2ub_align/asc_copy_gm2ub_align.md)将数据从Global Memory（GM）搬运到UB时，设置内层循环中相邻迭代数据块间的间隔。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_set_gm2ub_loop2_stride](asc_set_gm2ub_loop2_stride.md) | 使用[asc_copy_gm2ub_align](asc_copy_gm2ub_align/asc_copy_gm2ub_align.md)将数据从Global Memory（GM）搬运到UB时，设置外层循环中相邻迭代数据块间的间隔。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_set_gm2ub_loop_size](asc_set_gm2ub_loop_size.md) | 使用[asc_copy_gm2ub_align](asc_copy_gm2ub_align/asc_copy_gm2ub_align.md)将数据从Global Memory（GM）搬运到UB时，设置数据搬运流程中的循环次数。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_set_ndim_loop_stride](asc_set_ndim_loop_stride.md) | 设置[asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md)在进行多维搬运时每个维度内的源操作数与目的操作数的元素之间的间隔，最多设置5个维度。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_set_ndim_pad_count](asc_set_ndim_pad_count.md) | 设置[asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md)接口各个维度左右侧的Padding元素个数。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_set_ndim_pad_value](asc_set_ndim_pad_value.md) | 设置[asc_ndim_copy_gm2ub](asc_ndim_copy_gm2ub.md)接口Padding的填充固定值。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_set_ub2gm_loop1_stride](asc_set_ub2gm_loop1_stride.md) | 使用[asc_copy_ub2gm_align](asc_copy_ub2gm_align/asc_copy_ub2gm_align.md)将数据从UB搬运到Global Memory（GM）时，设置内层循环中源操作数在相邻迭代间的数据块间隔，以及目的操作数在相邻迭代间的数据块间隔。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_set_ub2gm_loop2_stride](asc_set_ub2gm_loop2_stride.md) | 使用[asc_copy_ub2gm_align](asc_copy_ub2gm_align/asc_copy_ub2gm_align.md)将数据从UB搬运到Global Memory（GM）时，设置外层循环中源操作数在相邻迭代间的数据块间隔，以及目的操作数在相邻迭代间的数据块间隔。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_set_ub2gm_loop_size](asc_set_ub2gm_loop_size.md) | 使用[asc_copy_ub2gm_align](asc_copy_ub2gm_align/asc_copy_ub2gm_align.md)将数据从UB搬运到Global Memory（GM）时，设置内层循环和外层循环的次数。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_gm2ub](asc_copy_gm2ub/asc_copy_gm2ub.md) | 将GM中的数据搬运到UB，具体接口形式按NPU架构版本区分。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_gm2ub（NPU架构版本2201）](asc_copy_gm2ub/asc_copy_gm2ub_arch_2201.md) | 将数据从Global Memory（GM）搬运到Unified Buffer（UB）。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_gm2ub（NPU架构版本3510）](asc_copy_gm2ub/asc_copy_gm2ub_arch_3510.md) | 将GM数据连续或按高维切分方式搬运到UB；有效长度非32字节对齐时，在UB侧补齐写入。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_gm2ub_align](asc_copy_gm2ub_align/asc_copy_gm2ub_align.md) | 将GM中的数据搬运到UB，支持非32字节整数倍长度的搬运及对齐补齐，具体接口形式按NPU架构版本区分。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_gm2ub_align（NPU架构版本2201）](asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_2201.md) | 将数据从Global Memory（GM）搬运到Unified Buffer（UB），支持搬运长度不是32字节整数倍的数据，并在搬运过程中进行32字节对齐补齐。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_gm2ub_align（NPU架构版本3510）](asc_copy_gm2ub_align/asc_copy_gm2ub_align_arch_3510.md) | 将数据从Global Memory（GM）搬运到Unified Buffer（UB），支持搬运长度不是32字节整数倍的数据，并在搬运过程中进行32字节对齐补齐。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_ub2gm](asc_copy_ub2gm/asc_copy_ub2gm.md) | 将UB中的数据搬运到GM，具体接口形式按NPU架构版本区分。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_ub2gm（NPU架构版本2201）](asc_copy_ub2gm/asc_copy_ub2gm_arch_2201.md) | 将数据从Unified Buffer（UB）搬运到Global Memory（GM）。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_ub2gm（NPU架构版本3510）](asc_copy_ub2gm/asc_copy_ub2gm_arch_3510.md) | 将UB数据连续或按高维切分方式搬运到GM；有效长度非32字节对齐时，补齐读取UB，仅将有效数据写入GM。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_ub2gm_align](asc_copy_ub2gm_align/asc_copy_ub2gm_align.md) | 将UB中的数据搬运到GM，支持非32字节整数倍长度的搬运，具体接口形式按NPU架构版本区分。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_ub2gm_align（NPU架构版本2201）](asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_2201.md) | 将UB数据搬运到GM，支持长度非32字节对齐的搬运，仅将有效数据写入GM。 | `c_api/vector_datamove/vector_datamove.h` |
| [asc_copy_ub2gm_align（NPU架构版本3510）](asc_copy_ub2gm_align/asc_copy_ub2gm_align_arch_3510.md) | 将UB数据搬运到GM，支持长度非32字节对齐的搬运，仅将有效数据写入GM。 | `c_api/vector_datamove/vector_datamove.h` |
