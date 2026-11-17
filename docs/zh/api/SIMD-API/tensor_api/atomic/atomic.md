# 原子操作

DMA（Direct Memory Access，直接内存访问单元）原子操作用于在数据搬运过程中对Global Memory上的已有数据执行原子读改写，再写入搬运结果。Tensor API提供以下接口配置后续目的地址为Global Memory的受支持搬运指令：

- **[set_atomic_add](set_atomic_add.md)**：将待搬运数据与Global Memory上的已有数据求和，并将结果写入Global Memory。
- **[set_atomic_max](set_atomic_max.md)**：比较待搬运数据与Global Memory上的已有数据，将两者中的最大值写入Global Memory。
- **[set_atomic_min](set_atomic_min.md)**：比较待搬运数据与Global Memory上的已有数据，将两者中的最小值写入Global Memory。
- **[disable_dma_atomic](disable_dma_atomic.md)**：关闭DMA原子模式，避免原子配置影响后续搬运。

`set_atomic_add`、`set_atomic_max`和`set_atomic_min`通过模板参数指定参与原子操作的数据类型。

Scalar原子操作用于在Global Memory的指定地址上完成单个元素的原子读改写：

- **[atomic_add](atomic_add.md)**：执行原子加，并返回旧值。
- **[atomic_sub](atomic_sub.md)**：执行原子减，并返回旧值。
- **[atomic_min](atomic_min.md)**：执行原子取小，并返回旧值。
- **[atomic_max](atomic_max.md)**：执行原子取大，并返回旧值。
- **[atomic_inc](atomic_inc.md)**：执行原子递增，并返回旧值。
- **[atomic_dec](atomic_dec.md)**：执行原子递减，并返回旧值。
- **[atomic_and](atomic_and.md)**：执行原子按位与，并返回旧值。
- **[atomic_or](atomic_or.md)**：执行原子按位或，并返回旧值。
- **[atomic_xor](atomic_xor.md)**：执行原子按位异或，并返回旧值。
- **[atomic_exch](atomic_exch.md)**：执行原子交换，并返回旧值。
- **[atomic_cas](atomic_cas.md)**：执行原子比较交换，并返回旧值。
