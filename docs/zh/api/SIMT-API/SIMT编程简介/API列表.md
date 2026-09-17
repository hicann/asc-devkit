# API列表

## 同步与内存栅栏

**表1**  同步接口

| 接口名 | 功能描述 |
| --- | --- |
| [asc_syncthreads](../同步与内存栅栏/同步接口/asc_syncthreads.md) | 等待当前thread block内所有thread代码都执行到该函数位置。 |

**表2**  内存栅栏接口

| 接口名 | 功能描述 |
| --- | --- |
| [asc_threadfence](../同步与内存栅栏/内存栅栏接口/asc_threadfence.md) | 用于保证不同核对同一份全局、共享内存的访问过程中，写入操作的时序性。 |
| [asc_threadfence_block](../同步与内存栅栏/内存栅栏接口/asc_threadfence_block.md) | 用于协调同一线程块（Thread Block）内线程之间的内存操作顺序，确保某一线程在调用asc_threadfence_block()之前的所有内存读写操作对同一线程块内的其他线程可见。 |

## 原子操作

**表3**  原子操作

| 接口名 | 功能描述 |
| --- | --- |
| [asc_atomic_add](../原子操作/asc_atomic_add.md) | 对Unified Buffer或Global Memory上的数据与指定数据执行原子加操作，即将指定数据累加到Unified Buffer或Global Memory的数据中。 |
| [asc_atomic_sub](../原子操作/asc_atomic_sub.md) | 对Unified Buffer或Global Memory上的数据与指定数据执行原子减操作，即在Unified Buffer或Global Memory的数据上减去指定数据。 |
| [asc_atomic_exch](../原子操作/asc_atomic_exch.md) | 对Unified Buffer或Global Memory地址做原子赋值操作，即将指定数据赋值到Unified Buffer或Global Memory地址中。 |
| [asc_atomic_max](../原子操作/asc_atomic_max.md) | 对Unified Buffer或Global Memory数据做原子求最大值操作，即将Unified Buffer或Global Memory的数据与指定数据中的最大值赋值到Unified Buffer或Global Memory地址中。 |
| [asc_atomic_min](../原子操作/asc_atomic_min.md) | 对Unified Buffer或Global Memory数据做原子求最小值操作，即将Unified Buffer或Global Memory的数据与指定数据中的最小值赋值到Unified Buffer或Global Memory地址中。 |
| [asc_atomic_inc](../原子操作/asc_atomic_inc.md) | 对Unified Buffer或Global Memory上address的数值进行原子加1操作，如果address上的数值大于等于指定数值val，则对address赋值为0，否则将address上数值加1。 |
| [asc_atomic_dec](../原子操作/asc_atomic_dec.md) | 对Unified Buffer或Global Memory上address的数值进行原子减1操作，如果address上的数值等于0或大于指定数值val，则对address赋值为val，否则将address上数值减1。 |
| [asc_atomic_cas](../原子操作/asc_atomic_cas.md) | 对Unified Buffer或Global Memory上address的数值进行原子比较赋值操作，如果address上的数值等于指定数值compare，则对address赋值为指定数值val，否则address的数值不变。 |
| [asc_atomic_and](../原子操作/asc_atomic_and.md) | 对Unified Buffer或Global Memory上address的数值与指定数值val进行原子与（&）操作，即将address数值与（&）val的结果赋值到Unified Buffer或Global Memory上。 |
| [asc_atomic_or](../原子操作/asc_atomic_or.md) | 对Unified Buffer或Global Memory上address的数值与指定数值val进行原子或（\|）操作，即将address数值或（\|）val的结果赋值到Unified Buffer或Global Memory上。 |
| [asc_atomic_xor](../原子操作/asc_atomic_xor.md) | 对Unified Buffer或Global Memory上address的数值与指定数值val进行原子异或（^）操作，即将address数值异或（^）val的结果赋值到Unified Buffer或Global Memory上。 |

## Warp函数

**表4**  Warp Vote类函数

| 接口名 | 功能描述 |
| --- | --- |
| [asc_all](../Warp函数/Warp-Vote类函数/asc_all.md) | 判断是否所有活跃线程的输入均不为0。 |
| [asc_any](../Warp函数/Warp-Vote类函数/asc_any.md) | 判断是否有活跃线程的输入不为0。 |
| [asc_ballot](../Warp函数/Warp-Vote类函数/asc_ballot.md) | 判断Warp内每个活跃线程的输入是否不为0。 |
| [asc_activemask](../Warp函数/Warp-Vote类函数/asc_activemask.md) | 查看Warp内所有线程是否为活跃状态。 |

**表5**  Warp Shfl类函数

| 接口名 | 功能描述 |
| --- | --- |
| [asc_shfl](../Warp函数/Warp-Shfl类函数/asc_shfl.md) | 获取Warp内指定线程srcLane输入的用于交换的var值。 |
| [asc_shfl_up](../Warp函数/Warp-Shfl类函数/asc_shfl_up.md) | 获取Warp内当前线程向前偏移delta（当前线程Lane ID - delta）的线程输入的用于交换的var值。 |
| [asc_shfl_down](../Warp函数/Warp-Shfl类函数/asc_shfl_down.md) | 获取Warp内当前线程向后偏移delta（当前线程Lane ID + delta）的线程输入的用于交换的var值。 |
| [asc_shfl_xor](../Warp函数/Warp-Shfl类函数/asc_shfl_xor.md) | 获取Warp内当前线程Lane ID与输入laneMask做异或操作（Lane ID ^ laneMask）得到的dstLaneId对应线程输入的用于交换的var值。 |

**表6**  Warp Reduce类函数

| 接口名 | 功能描述 |
| --- | --- |
| [asc_reduce_add](../Warp函数/Warp-Reduce类函数/asc_reduce_add.md) | 对Warp内所有活跃线程输入的val求和。 |
| [asc_reduce_max](../Warp函数/Warp-Reduce类函数/asc_reduce_max.md) | 对Warp内所有活跃线程输入的val求最大值。 |
| [asc_reduce_min](../Warp函数/Warp-Reduce类函数/asc_reduce_min.md) | 对Warp内所有活跃线程输入val求最小值。 |

**表7** Lane ID类函数

| 接口名 | 功能描述 |
| --- | --- |
| [laneid](../Warp函数/Lane-ID类函数/laneid.md) | 获取线程在其线程束内的索引。 |
| [lanemask_eq](../Warp函数/Lane-ID类函数/lanemask_eq.md) | 提供当前线程的一个32位掩码，在当前线程所属的Warp中，只有当前线程所在的lane位被置为1，其余位为0。 |
| [lanemask_le](../Warp函数/Lane-ID类函数/lanemask_le.md) | 提供当前线程的一个32位掩码，在当前线程所属的Warp中，将“Lane ID小于等于当前线程”的线程的对应位设为1，其余位为0。 |
| [lanemask_lt](../Warp函数/Lane-ID类函数/lanemask_lt.md) | 提供当前线程的一个32位掩码，在当前线程所属的Warp中，将“Lane ID严格小于当前线程”的线程的对应位设为1，其余位为0。 |
| [lanemask_ge](../Warp函数/Lane-ID类函数/lanemask_ge.md) | 提供当前线程的一个32位掩码，在当前线程所属的Warp中，将“Lane ID大于等于当前线程”的线程的对应位设为1，其余位为0。 |
| [lanemask_gt](../Warp函数/Lane-ID类函数/lanemask_gt.md) | 提供当前线程的一个32位掩码，在当前线程所属的Warp中，将“Lane ID严格大于当前线程”的线程的对应位设为1，其余位为0。 |

## 数学函数

**表8**  half类型算术函数

| 接口名 | 功能描述 |
| --- | --- |
| [__habs](../数学函数/half类型/half类型算术函数/__habs.md) | 获取输入数据的绝对值。 |
| [__hfma](../数学函数/half类型/half类型算术函数/__hfma.md) | 对输入数据x、y、z，计算x与y相乘加上z的结果。 |
| [__hadd](../数学函数/half类型/half类型算术函数/__hadd.md) | 计算两个half类型数据的相加结果，并遵循CAST_RINT模式舍入。 |
| [__hsub](../数学函数/half类型/half类型算术函数/__hsub.md) | 计算两个half类型数据的相减结果，并遵循CAST_RINT模式舍入。 |
| [__hmul](../数学函数/half类型/half类型算术函数/__hmul.md) | 计算两个half类型数据的相乘结果，并遵循CAST_RINT模式舍入。 |
| [__hdiv](../数学函数/half类型/half类型算术函数/__hdiv.md) | 计算两个half类型数据的相除结果，并遵循CAST_RINT模式舍入。 |
| [__hneg](../数学函数/half类型/half类型算术函数/__hneg.md) | 获取输入half类型数据的负值。 |
| [__hfma_relu](../数学函数/half类型/half类型算术函数/__hfma_relu.md) | 对输入half类型数据x、y、z，计算x与y相乘加上z的结果，并遵循CAST_RINT模式舍入。负数结果置为0。 |

**表9**  half类型比较函数

| 接口名 | 功能描述 |
| --- | --- |
| [__hmax](../数学函数/half类型/half类型比较函数/__hmax.md) | 获取两个输入数据中的最大值。 |
| [__hmin](../数学函数/half类型/half类型比较函数/__hmin.md) | 获取两个输入数据中的最小值。 |
| [__hisnan](../数学函数/half类型/half类型比较函数/__hisnan.md) | 判断浮点数是否为nan。 |
| [__hisinf](../数学函数/half类型/half类型比较函数/__hisinf.md) | 判断浮点数是否为无穷。 |
| [__heq](../数学函数/half类型/half类型比较函数/__heq.md) | 比较两个half类型数据是否相等，相等时返回true。 |
| [__hne](../数学函数/half类型/half类型比较函数/__hne.md) | 比较两个half类型数据是否不相等，不相等时返回true。 |
| [__hle](../数学函数/half类型/half类型比较函数/__hle.md) | 比较两个half类型数据，仅当第一个数小于或等于第二个数时返回true。 |
| [__hge](../数学函数/half类型/half类型比较函数/__hge.md) | 比较两个half类型数据，仅当第一个数大于或等于第二个数时返回true。 |
| [__hlt](../数学函数/half类型/half类型比较函数/__hlt.md) | 比较两个half类型数据，仅当第一个数小于第二个数时返回true。 |
| [__hgt](../数学函数/half类型/half类型比较函数/__hgt.md) | 比较两个half类型数据，仅当第一个数大于第二个数时返回true。 |
| [__hequ](../数学函数/half类型/half类型比较函数/__hequ.md) | 比较两个half类型数据是否相等，相等时返回true。若任一输入为nan，返回true。 |
| [__hneu](../数学函数/half类型/half类型比较函数/__hneu.md) | 比较两个half类型数据是否不相等，不相等时返回true。若任一输入为nan，返回true。 |
| [__hleu](../数学函数/half类型/half类型比较函数/__hleu.md) | 比较两个half类型数据，当第一个数小于或等于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hgeu](../数学函数/half类型/half类型比较函数/__hgeu.md) | 比较两个half类型数据，当第一个数大于或等于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hltu](../数学函数/half类型/half类型比较函数/__hltu.md) | 比较两个half类型数据，当第一个数小于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hgtu](../数学函数/half类型/half类型比较函数/__hgtu.md) | 比较两个half类型数据，当第一个数大于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hmax_nan](../数学函数/half类型/half类型比较函数/__hmax_nan.md) | 获取两个输入数据中的最大值。任一输入为nan时返回nan。 |
| [__hmin_nan](../数学函数/half类型/half类型比较函数/__hmin_nan.md) | 获取两个输入数据中的最小值。任一输入为nan时返回nan。 |

**表10**  half类型数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [htanh](../数学函数/half类型/half类型数学库函数/htanh.md) | 获取输入数据的双曲正切值。 |
| [hexp](../数学函数/half类型/half类型数学库函数/hexp.md) | 指定输入x，获取e的x次方。 |
| [hexp2](../数学函数/half类型/half类型数学库函数/hexp2.md) | 指定输入x，获取2的x次方。 |
| [hexp10](../数学函数/half类型/half类型数学库函数/hexp10.md) | 指定输入x，获取10的x次方。 |
| [hlog](../数学函数/half类型/half类型数学库函数/hlog.md) | 获取以e为底，输入数据的对数。 |
| [hlog2](../数学函数/half类型/half类型数学库函数/hlog2.md) | 获取以2为底，输入数据的对数。 |
| [hlog10](../数学函数/half类型/half类型数学库函数/hlog10.md) | 获取以10为底，输入数据的对数。 |
| [hcos](../数学函数/half类型/half类型数学库函数/hcos.md) | 获取输入数据的三角函数余弦值。 |
| [hsin](../数学函数/half类型/half类型数学库函数/hsin.md) | 获取输入数据的三角函数正弦值。 |
| [hsqrt](../数学函数/half类型/half类型数学库函数/hsqrt.md) | 获取输入数据x的平方根。 |
| [hrsqrt](../数学函数/half类型/half类型数学库函数/hrsqrt.md) | 获取输入数据x的平方根的倒数。 |
| [hrcp](../数学函数/half类型/half类型数学库函数/hrcp.md) | 获取输入数据x的倒数。 |
| [hrint](../数学函数/half类型/half类型数学库函数/hrint.md) | 获取与输入数据最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [hfloor](../数学函数/half类型/half类型数学库函数/hfloor.md) | 获取小于或等于输入数据的最大整数值。 |
| [hceil](../数学函数/half类型/half类型数学库函数/hceil.md) | 获取大于或等于输入数据的最小整数值。 |
| [htrunc](../数学函数/half类型/half类型数学库函数/htrunc.md) | 获取对输入数据的浮点数截断后的整数。 |

**表11**  half类型精度转换函数

| 接口名 | 功能描述 |
| --- | --- |
| [__float2half](../数学函数/half类型/half类型精度转换函数/__float2half.md) | 获取输入遵循CAST_RINT模式转换成的半精度浮点数。 |
| [__float2half_rn](../数学函数/half类型/half类型精度转换函数/__float2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的半精度浮点数。 |
| [__float2half_rn_sat](../数学函数/half类型/half类型精度转换函数/__float2half_rn_sat.md) | 饱和模式下获取输入遵循CAST_RINT模式转换成的半精度浮点数。 |
| [__float22half2_rn_sat](../数学函数/half类型/half类型精度转换函数/__float22half2_rn_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_RINT模式转换成的half2类型数据。 |
| [__float2half_rz](../数学函数/half类型/half类型精度转换函数/__float2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的半精度浮点数。 |
| [__float2half_rz_sat](../数学函数/half类型/half类型精度转换函数/__float2half_rz_sat.md) | 饱和模式下获取输入遵循CAST_TRUNC模式转换成的半精度浮点数。 |
| [__float22half2_rz](../数学函数/half类型/half类型精度转换函数/__float22half2_rz.md) | 获取输入的两个分量遵循CAST_TRUNC模式转换成的half2类型数据。 |
| [__float22half2_rz_sat](../数学函数/half类型/half类型精度转换函数/__float22half2_rz_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_TRUNC模式转换成的half2类型数据。 |
| [__float2half_rd](../数学函数/half类型/half类型精度转换函数/__float2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的半精度浮点数。 |
| [__float2half_rd_sat](../数学函数/half类型/half类型精度转换函数/__float2half_rd_sat.md) | 饱和模式下获取输入遵循CAST_FLOOR模式转换成的半精度浮点数。 |
| [__float22half2_rd](../数学函数/half类型/half类型精度转换函数/__float22half2_rd.md) | 获取输入的两个分量遵循CAST_FLOOR模式转换成的half2类型数据。 |
| [__float22half2_rd_sat](../数学函数/half类型/half类型精度转换函数/__float22half2_rd_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_FLOOR模式转换成的half2类型数据。 |
| [__float2half_ru](../数学函数/half类型/half类型精度转换函数/__float2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的半精度浮点数。 |
| [__float2half_ru_sat](../数学函数/half类型/half类型精度转换函数/__float2half_ru_sat.md) | 饱和模式下获取输入遵循CAST_CEIL模式转换成的半精度浮点数。 |
| [__float22half2_ru](../数学函数/half类型/half类型精度转换函数/__float22half2_ru.md) | 获取输入的两个分量遵循CAST_CEIL模式转换成的half2类型数据。 |
| [__float22half2_ru_sat](../数学函数/half类型/half类型精度转换函数/__float22half2_ru_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_CEIL模式转换成的half2类型数据。 |
| [__float2half_rna](../数学函数/half类型/half类型精度转换函数/__float2half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的半精度浮点数。 |
| [__float2half_rna_sat](../数学函数/half类型/half类型精度转换函数/__float2half_rna_sat.md) | 饱和模式下获取输入遵循CAST_ROUND模式转换成的半精度浮点数。 |
| [__float22half2_rna](../数学函数/half类型/half类型精度转换函数/__float22half2_rna.md) | 获取输入的两个分量遵循CAST_ROUND模式转换成的half2类型数据。 |
| [__float22half2_rna_sat](../数学函数/half类型/half类型精度转换函数/__float22half2_rna_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_ROUND模式转换成的half2类型数据。 |
| [__float2half_ro](../数学函数/half类型/half类型精度转换函数/__float2half_ro.md) | 获取输入遵循CAST_ODD模式转换成的半精度浮点数。 |
| [__float2half_ro_sat](../数学函数/half类型/half类型精度转换函数/__float2half_ro_sat.md) | 饱和模式下获取输入遵循CAST_ODD模式转换成的半精度浮点数。 |
| [__float22half2_ro](../数学函数/half类型/half类型精度转换函数/__float22half2_ro.md) | 获取输入的两个分量遵循CAST_ODD模式转换成的half2类型数据。 |
| [__float22half2_ro_sat](../数学函数/half类型/half类型精度转换函数/__float22half2_ro_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_ODD模式转换成的half2类型数据。 |
| [__half2float](../数学函数/half类型/half类型精度转换函数/__half2float.md) | 获取输入转换成的浮点数。 |
| [__half2half_rn](../数学函数/half类型/half类型精度转换函数/__half2half_rn.md) | 获取输入遵循CAST_RINT模式取整后的half类型数据。 |
| [__half2half_rz](../数学函数/half类型/half类型精度转换函数/__half2half_rz.md) | 获取输入遵循CAST_TRUNC模式取整后的half类型数据。 |
| [__half2half_rd](../数学函数/half类型/half类型精度转换函数/__half2half_rd.md) | 获取输入遵循CAST_FLOOR模式取整后的half类型数据。 |
| [__half2half_ru](../数学函数/half类型/half类型精度转换函数/__half2half_ru.md) | 获取输入遵循CAST_CEIL模式取整后的half类型数据。 |
| [__half2half_rna](../数学函数/half类型/half类型精度转换函数/__half2half_rna.md) | 获取输入遵循CAST_ROUND模式取整后的half类型数据。 |
| [__half2uint_rn](../数学函数/half类型/half类型精度转换函数/__half2uint_rn.md) | 获取输入遵循CAST_RINT模式转换成的无符号整数。 |
| [__half2uint_rz](../数学函数/half类型/half类型精度转换函数/__half2uint_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的无符号整数。 |
| [__half2uint_rd](../数学函数/half类型/half类型精度转换函数/__half2uint_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的无符号整数。 |
| [__half2uint_ru](../数学函数/half类型/half类型精度转换函数/__half2uint_ru.md) | 获取输入遵循CAST_CEIL模式转换成的无符号整数。 |
| [__half2uint_rna](../数学函数/half类型/half类型精度转换函数/__half2uint_rna.md) | 获取输入遵循CAST_ROUND模式转换成的无符号整数。 |
| [__half2int_rn](../数学函数/half类型/half类型精度转换函数/__half2int_rn.md) | 获取输入遵循CAST_RINT模式转换成的有符号整数。 |
| [__half2int_rz](../数学函数/half类型/half类型精度转换函数/__half2int_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的有符号整数。 |
| [__half2int_rd](../数学函数/half类型/half类型精度转换函数/__half2int_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的有符号整数。 |
| [__half2int_ru](../数学函数/half类型/half类型精度转换函数/__half2int_ru.md) | 获取输入遵循CAST_CEIL模式转换成的有符号整数。 |
| [__half2int_rna](../数学函数/half类型/half类型精度转换函数/__half2int_rna.md) | 获取输入遵循CAST_ROUND模式转换成的有符号整数。 |
| [__half2ull_rn](../数学函数/half类型/half类型精度转换函数/__half2ull_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位无符号整数。 |
| [__half2ull_rz](../数学函数/half类型/half类型精度转换函数/__half2ull_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位无符号整数。 |
| [__half2ull_rd](../数学函数/half类型/half类型精度转换函数/__half2ull_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位无符号整数。 |
| [__half2ull_ru](../数学函数/half类型/half类型精度转换函数/__half2ull_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位无符号整数。 |
| [__half2ull_rna](../数学函数/half类型/half类型精度转换函数/__half2ull_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位无符号整数。 |
| [__half2ll_rn](../数学函数/half类型/half类型精度转换函数/__half2ll_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位有符号整数。 |
| [__half2ll_rz](../数学函数/half类型/half类型精度转换函数/__half2ll_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位有符号整数。 |
| [__half2ll_rd](../数学函数/half类型/half类型精度转换函数/__half2ll_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位有符号整数。 |
| [__half2ll_ru](../数学函数/half类型/half类型精度转换函数/__half2ll_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位有符号整数。 |
| [__half2ll_rna](../数学函数/half类型/half类型精度转换函数/__half2ll_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位有符号整数。 |
| [__bfloat162half_rn](../数学函数/half类型/half类型精度转换函数/__bfloat162half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__bfloat162half_rn_sat](../数学函数/half类型/half类型精度转换函数/__bfloat162half_rn_sat.md) | 饱和模式下获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__bfloat162half_rz](../数学函数/half类型/half类型精度转换函数/__bfloat162half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__bfloat162half_rz_sat](../数学函数/half类型/half类型精度转换函数/__bfloat162half_rz_sat.md) | 饱和模式下获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__bfloat162half_rd](../数学函数/half类型/half类型精度转换函数/__bfloat162half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__bfloat162half_rd_sat](../数学函数/half类型/half类型精度转换函数/__bfloat162half_rd_sat.md) | 饱和模式下获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__bfloat162half_ru](../数学函数/half类型/half类型精度转换函数/__bfloat162half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__bfloat162half_ru_sat](../数学函数/half类型/half类型精度转换函数/__bfloat162half_ru_sat.md) | 饱和模式下获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__bfloat162half_rna](../数学函数/half类型/half类型精度转换函数/__bfloat162half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__bfloat162half_rna_sat](../数学函数/half类型/half类型精度转换函数/__bfloat162half_rna_sat.md) | 饱和模式下获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__uint2half_rn](../数学函数/half类型/half类型精度转换函数/__uint2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__uint2half_rn_sat](../数学函数/half类型/half类型精度转换函数/__uint2half_rn_sat.md) | 饱和模式下获取输入的uint32数据转换成的half数据，并遵循CAST_RINT模式。 |
| [__uint2half_rz](../数学函数/half类型/half类型精度转换函数/__uint2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__uint2half_rz_sat](../数学函数/half类型/half类型精度转换函数/__uint2half_rz_sat.md) | 饱和模式下获取输入的uint32数据转换成的half数据，并遵循CAST_TRUNC模式。 |
| [__uint2half_rd](../数学函数/half类型/half类型精度转换函数/__uint2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__uint2half_rd_sat](../数学函数/half类型/half类型精度转换函数/__uint2half_rd_sat.md) | 饱和模式下获取输入的uint32数据转换成的half数据，并遵循CAST_FLOOR模式。 |
| [__uint2half_ru](../数学函数/half类型/half类型精度转换函数/__uint2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__uint2half_ru_sat](../数学函数/half类型/half类型精度转换函数/__uint2half_ru_sat.md) | 饱和模式下获取输入的uint32数据转换成的half数据，并遵循CAST_CEIL模式。 |
| [__uint2half_rna](../数学函数/half类型/half类型精度转换函数/__uint2half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__uint2half_rna_sat](../数学函数/half类型/half类型精度转换函数/__uint2half_rna_sat.md) | 饱和模式下获取输入的uint32数据转换成的half数据，并遵循CAST_ROUND模式。 |
| [__int2half_rn](../数学函数/half类型/half类型精度转换函数/__int2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__int2half_rn_sat](../数学函数/half类型/half类型精度转换函数/__int2half_rn_sat.md) | 饱和模式下获取输入的int32数据转换成的half数据，并遵循CAST_RINT模式。 |
| [__int2half_rz](../数学函数/half类型/half类型精度转换函数/__int2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__int2half_rz_sat](../数学函数/half类型/half类型精度转换函数/__int2half_rz_sat.md) | 饱和模式下获取输入的int32数据转换成的half数据，并遵循CAST_TRUNC模式。 |
| [__int2half_rd](../数学函数/half类型/half类型精度转换函数/__int2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__int2half_rd_sat](../数学函数/half类型/half类型精度转换函数/__int2half_rd_sat.md) | 饱和模式下获取输入的int32数据转换成的half数据，并遵循CAST_FLOOR模式。 |
| [__int2half_ru](../数学函数/half类型/half类型精度转换函数/__int2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__int2half_ru_sat](../数学函数/half类型/half类型精度转换函数/__int2half_ru_sat.md) | 饱和模式下获取输入的int32数据转换成的half数据，并遵循CAST_CEIL模式。 |
| [__int2half_rna](../数学函数/half类型/half类型精度转换函数/__int2half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__int2half_rna_sat](../数学函数/half类型/half类型精度转换函数/__int2half_rna_sat.md) | 饱和模式下获取输入的int32数据转换成的half数据，并遵循CAST_ROUND模式。 |
| [__ull2half_rn](../数学函数/half类型/half类型精度转换函数/__ull2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__ull2half_rz](../数学函数/half类型/half类型精度转换函数/__ull2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__ull2half_rd](../数学函数/half类型/half类型精度转换函数/__ull2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__ull2half_ru](../数学函数/half类型/half类型精度转换函数/__ull2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__ull2half_rna](../数学函数/half类型/half类型精度转换函数/__ull2half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__ll2half_rn](../数学函数/half类型/half类型精度转换函数/__ll2half_rn.md) | 获取输入遵循CAST_RINT模式转换成的half类型数据。 |
| [__ll2half_rz](../数学函数/half类型/half类型精度转换函数/__ll2half_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的half类型数据。 |
| [__ll2half_rd](../数学函数/half类型/half类型精度转换函数/__ll2half_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的half类型数据。 |
| [__ll2half_ru](../数学函数/half类型/half类型精度转换函数/__ll2half_ru.md) | 获取输入遵循CAST_CEIL模式转换成的half类型数据。 |
| [__ll2half_rna](../数学函数/half类型/half类型精度转换函数/__ll2half_rna.md) | 获取输入遵循CAST_ROUND模式转换成的half类型数据。 |
| [__floats2half2_rn](../数学函数/half类型/half类型精度转换函数/__floats2half2_rn.md) | 将输入的数据x，y遵循CAST_RINT模式分别转换为half类型并填充到half2的前后两部分，返回转换后的half2类型数据。 |
| [__float22half2_rn](../数学函数/half类型/half类型精度转换函数/__float22half2_rn.md) | 将float2类型数据遵循CAST_RINT模式转换为half2类型，返回转换后的half2类型数据。 |
| [__low2half](../数学函数/half类型/half类型精度转换函数/__low2half.md) | 返回输入数据的低16位。 |
| [__low2half2](../数学函数/half类型/half类型精度转换函数/__low2half2.md) | 将输入数据的低16位填充到half2并返回。 |
| [__low2float](../数学函数/half类型/half类型精度转换函数/__low2float.md) | 将输入数据的低16位转换为浮点数并返回结果。 |
| [__lowhigh2highlow](../数学函数/half类型/half类型精度转换函数/__lowhigh2highlow.md) | 将输入数据的高低16位进行交换并返回。 |
| [__high2half](../数学函数/half类型/half类型精度转换函数/__high2half.md) | 提取输入half2的高16位，并返回。 |
| [__high2half2](../数学函数/half类型/half类型精度转换函数/__high2half2.md) | 将输入数据的高16位填充到half2并返回结果。 |
| [__high2float](../数学函数/half类型/half类型精度转换函数/__high2float.md) | 将输入数据的高16位转换为float类型并返回结果。 |
| [__highs2half2](../数学函数/half类型/half类型精度转换函数/__highs2half2.md) | 分别提取两个half2输入的高16位，并填充到half2中。返回填充后的数据。 |
| [__lows2half2](../数学函数/half类型/half类型精度转换函数/__lows2half2.md) | 分别提取两个half2输入的低16位，并填充到half2中。返回填充后的数据。 |
| [__halves2half2](../数学函数/half类型/half类型精度转换函数/__halves2half2.md) | 将输入的数据分别填充为half2前后两个分量，返回填充后数据。 |
| [__half22float2](../数学函数/half类型/half类型精度转换函数/__half22float2.md) | 将half2的两个分量分别转换为float，并填充到float2返回。 |
| [__ushort_as_half](../数学函数/half类型/half类型精度转换函数/__ushort_as_half.md) | 将unsigned short int的按位重新解释为half，即将unsigned short int的数据存储的位按照half的格式进行读取。 |

**表12**  half2类型算术函数

| 接口名 | 功能描述 |
| --- | --- |
| [__haddx2](../数学函数/half类型/half2类型算术函数/__haddx2.md) | 计算两个half2类型数据各分量的相加结果，并遵循CAST_RINT模式舍入。 |
| [__hsubx2](../数学函数/half类型/half2类型算术函数/__hsubx2.md) | 计算两个half2类型数据各分量的相减结果，并遵循CAST_RINT模式舍入。 |
| [__hmulx2](../数学函数/half类型/half2类型算术函数/__hmulx2.md) | 计算两个half2类型数据各分量的相乘结果，并遵循CAST_RINT模式舍入。 |
| [__hdivx2](../数学函数/half类型/half2类型算术函数/__hdivx2.md) | 计算两个half2类型数据各分量的相除结果，并遵循CAST_RINT模式舍入。 |
| [__habsx2](../数学函数/half类型/half2类型算术函数/__habsx2.md) | 计算输入half2类型数据各分量的绝对值。 |
| [__hfmax2](../数学函数/half类型/half2类型算术函数/__hfmax2.md) | 计算两个half2类型数据各分量的乘加的结果（前两个输入相乘后与第三个输入相加），并遵循CAST_RINT模式舍入。 |
| [__hnegx2](../数学函数/half类型/half2类型算术函数/__hnegx2.md) | 获取输入half2类型数据各分量的负值。 |
| [__hfmax2_relu](../数学函数/half类型/half2类型算术函数/__hfmax2_relu.md) | 计算两个half2类型数据各分量的乘加的结果（前两个输入相乘后与第三个输入相加），并遵循CAST_RINT模式舍入。负数结果置为0。 |
| [__hcmadd](../数学函数/half类型/half2类型算术函数/__hcmadd.md) | 将三个half2输入视为复数（第一个分量为实部，第二个分量为虚部），执行复数乘加运算x*y+z。 |

**表13**  half2类型比较函数

| 接口名 | 功能描述 |
| --- | --- |
| [__hbeqx2](../数学函数/half类型/half2类型比较函数/__hbeqx2.md) | 比较两个half2类型数据的两个分量是否相等，仅当两个分量均相等时返回true。 |
| [__hbnex2](../数学函数/half类型/half2类型比较函数/__hbnex2.md) | 比较两个half2类型数据的两个分量是否不相等，仅当两个分量均不相等时返回true。 |
| [__hblex2](../数学函数/half类型/half2类型比较函数/__hblex2.md) | 比较两个half2类型数据的两个分量，仅当两个分量均满足第一个数小于或等于第二个数时返回true。 |
| [__hbgex2](../数学函数/half类型/half2类型比较函数/__hbgex2.md) | 比较两个half2类型数据的两个分量，仅当两个分量均满足第一个数大于或等于第二个数时返回true。 |
| [__hbltx2](../数学函数/half类型/half2类型比较函数/__hbltx2.md) | 比较两个half2类型数据的两个分量，仅当两个分量均满足第一个数小于第二个数时返回true。 |
| [__hbgtx2](../数学函数/half类型/half2类型比较函数/__hbgtx2.md) | 比较两个half2类型数据的两个分量，仅当两个分量均满足第一个数大于第二个数时返回true。 |
| [__hbequx2](../数学函数/half类型/half2类型比较函数/__hbequx2.md) | 比较两个half2类型数据的两个分量是否相等，当两个分量均相等时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbneux2](../数学函数/half类型/half2类型比较函数/__hbneux2.md) | 比较两个half2类型数据的两个分量是否不相等，当两个分量均不相等时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbleux2](../数学函数/half类型/half2类型比较函数/__hbleux2.md) | 比较两个half2类型数据的两个分量，当两个分量均满足第一个数小于或等于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbgeux2](../数学函数/half类型/half2类型比较函数/__hbgeux2.md) | 比较两个half2类型数据的两个分量，当两个分量均满足第一个数大于或等于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbltux2](../数学函数/half类型/half2类型比较函数/__hbltux2.md) | 比较两个half2类型数据的两个分量，当两个分量均满足第一个数小于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbgtux2](../数学函数/half类型/half2类型比较函数/__hbgtux2.md) | 比较两个half2类型数据的两个分量，当两个分量均满足第一个数大于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__heqx2](../数学函数/half类型/half2类型比较函数/__heqx2.md) | 比较两个half2类型数据的两个分量，如果分量相等，则对应比较结果为1.0，否则为0.0。 |
| [__hnex2](../数学函数/half类型/half2类型比较函数/__hnex2.md) | 比较两个half2类型数据的两个分量，如果分量不相等，则对应比较结果为1.0，否则为0.0。 |
| [__hlex2](../数学函数/half类型/half2类型比较函数/__hlex2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数小于或等于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hgex2](../数学函数/half类型/half2类型比较函数/__hgex2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数大于或等于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hltx2](../数学函数/half类型/half2类型比较函数/__hltx2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数小于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hgtx2](../数学函数/half类型/half2类型比较函数/__hgtx2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数大于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hequx2](../数学函数/half类型/half2类型比较函数/__hequx2.md) | 比较两个half2类型数据的两个分量，如果分量相等，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hneux2](../数学函数/half类型/half2类型比较函数/__hneux2.md) | 比较两个half2类型数据的两个分量，如果分量不相等，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hleux2](../数学函数/half类型/half2类型比较函数/__hleux2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数小于或等于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hgeux2](../数学函数/half类型/half2类型比较函数/__hgeux2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数大于或等于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hltux2](../数学函数/half类型/half2类型比较函数/__hltux2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数小于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hgtux2](../数学函数/half类型/half2类型比较函数/__hgtux2.md) | 比较两个half2类型数据的两个分量，如果分量满足第一个数大于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__heqx2_mask](../数学函数/half类型/half2类型比较函数/__heqx2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量相等，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hnex2_mask](../数学函数/half类型/half2类型比较函数/__hnex2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量不相等，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hlex2_mask](../数学函数/half类型/half2类型比较函数/__hlex2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hgex2_mask](../数学函数/half类型/half2类型比较函数/__hgex2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hltx2_mask](../数学函数/half类型/half2类型比较函数/__hltx2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hgtx2_mask](../数学函数/half类型/half2类型比较函数/__hgtx2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hequx2_mask](../数学函数/half类型/half2类型比较函数/__hequx2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量相等，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hneux2_mask](../数学函数/half类型/half2类型比较函数/__hneux2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量不相等，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hleux2_mask](../数学函数/half类型/half2类型比较函数/__hleux2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hgeux2_mask](../数学函数/half类型/half2类型比较函数/__hgeux2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hltux2_mask](../数学函数/half类型/half2类型比较函数/__hltux2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hgtux2_mask](../数学函数/half类型/half2类型比较函数/__hgtux2_mask.md) | 比较两个half2类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__isnanx2](../数学函数/half类型/half2类型比较函数/__isnanx2.md) | 判断half2类型数据的两个分量是否为nan。 |
| [__hmaxx2](../数学函数/half类型/half2类型比较函数/__hmaxx2.md) | 获取两个half2类型数据各分量的最大值。 |
| [__hmaxx2_nan](../数学函数/half类型/half2类型比较函数/__hmaxx2_nan.md) | 获取两个half2类型数据各分量的最大值。任一分量为nan时对应结果为nan。 |
| [__hminx2](../数学函数/half类型/half2类型比较函数/__hminx2.md) | 获取两个half2类型数据各分量的最小值。 |
| [__hminx2_nan](../数学函数/half类型/half2类型比较函数/__hminx2_nan.md) | 获取两个half2类型数据各分量的最小值。任一分量为nan时对应结果为nan。 |

**表14**  half2类型数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [h2tanh](../数学函数/half类型/half2类型数学库函数/h2tanh.md) | 获取输入数据各元素的双曲正切值。 |
| [h2exp](../数学函数/half类型/half2类型数学库函数/h2exp.md) | 指定输入x，对x的各元素，获取e的该元素次方。 |
| [h2exp2](../数学函数/half类型/half2类型数学库函数/h2exp2.md) | 指定输入x，对x的各元素，获取2的该元素次方。 |
| [h2exp10](../数学函数/half类型/half2类型数学库函数/h2exp10.md) | 指定输入x，对x的各元素，获取10的该元素次方。 |
| [h2log](../数学函数/half类型/half2类型数学库函数/h2log.md) | 获取以e为底，输入数据各元素的对数。 |
| [h2log2](../数学函数/half类型/half2类型数学库函数/h2log2.md) | 获取以2为底，输入数据各元素的对数。 |
| [h2log10](../数学函数/half类型/half2类型数学库函数/h2log10.md) | 获取以10为底，输入数据各元素的对数。 |
| [h2cos](../数学函数/half类型/half2类型数学库函数/h2cos.md) | 获取输入数据各元素的三角函数余弦值。 |
| [h2sin](../数学函数/half类型/half2类型数学库函数/h2sin.md) | 获取输入数据各元素的三角函数正弦值。 |
| [h2sqrt](../数学函数/half类型/half2类型数学库函数/h2sqrt.md) | 获取输入数据x各元素的平方根。 |
| [h2rsqrt](../数学函数/half类型/half2类型数学库函数/h2rsqrt.md) | 获取输入数据x各元素的平方根的倒数。 |
| [h2rcp](../数学函数/half类型/half2类型数学库函数/h2rcp.md) | 获取输入数据x各元素的倒数。 |
| [h2rint](../数学函数/half类型/half2类型数学库函数/h2rint.md) | 获取与输入数据各元素最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [h2floor](../数学函数/half类型/half2类型数学库函数/h2floor.md) | 获取小于或等于输入数据各元素的最大整数值。 |
| [h2ceil](../数学函数/half类型/half2类型数学库函数/h2ceil.md) | 获取大于或等于输入数据各元素的最小整数值。 |
| [h2trunc](../数学函数/half类型/half2类型数学库函数/h2trunc.md) | 获取对输入数据各元素的浮点数截断后的整数。 |

**表15**  bfloat16类型算术函数

| 接口名 | 功能描述 |
| --- | --- |
| [__habs](../数学函数/bfloat16类型/bfloat16类型算术函数/__habs-150.md) | 获取输入数据的绝对值。 |
| [__hfma](../数学函数/bfloat16类型/bfloat16类型算术函数/__hfma-151.md) | 对输入数据x、y、z，计算x与y相乘加上z的结果。 |
| [__hadd](../数学函数/bfloat16类型/bfloat16类型算术函数/__hadd-152.md) | 计算两个bfloat16类型数据的相加结果，并遵循CAST_RINT模式舍入。 |
| [__hsub](../数学函数/bfloat16类型/bfloat16类型算术函数/__hsub-153.md) | 计算两个bfloat16类型数据的相减结果，并遵循CAST_RINT模式舍入。 |
| [__hmul](../数学函数/bfloat16类型/bfloat16类型算术函数/__hmul-154.md) | 计算两个bfloat16类型数据的相乘结果，并遵循CAST_RINT模式舍入。 |
| [__hdiv](../数学函数/bfloat16类型/bfloat16类型算术函数/__hdiv-155.md) | 计算两个bfloat16类型数据的相除结果，并遵循CAST_RINT模式舍入。 |
| [__hneg](../数学函数/bfloat16类型/bfloat16类型算术函数/__hneg-156.md) | 获取输入bfloat16类型数据的负值。 |
| [__hfma_relu](../数学函数/bfloat16类型/bfloat16类型算术函数/__hfma_relu-157.md) | 对输入bfloat16类型数据x、y、z，计算x与y相乘加上z的结果，并遵循CAST_RINT模式舍入。负数结果置为0。 |

**表16**  bfloat16类型比较函数

| 接口名 | 功能描述 |
| --- | --- |
| [__hmax](../数学函数/bfloat16类型/bfloat16类型比较函数/__hmax-158.md) | 获取两个输入数据中的最大值。 |
| [__hmin](../数学函数/bfloat16类型/bfloat16类型比较函数/__hmin-159.md) | 获取两个输入数据中的最小值。 |
| [__hisnan](../数学函数/bfloat16类型/bfloat16类型比较函数/__hisnan-160.md) | 判断浮点数是否为nan。 |
| [__hisinf](../数学函数/bfloat16类型/bfloat16类型比较函数/__hisinf-161.md) | 判断浮点数是否为无穷。 |
| [__heq](../数学函数/bfloat16类型/bfloat16类型比较函数/__heq-162.md) | 比较两个bfloat16类型数据是否相等，相等时返回true。 |
| [__hne](../数学函数/bfloat16类型/bfloat16类型比较函数/__hne-163.md) | 比较两个bfloat16类型数据是否不相等，不相等时返回true。 |
| [__hle](../数学函数/bfloat16类型/bfloat16类型比较函数/__hle-164.md) | 比较两个bfloat16类型数据，仅当第一个数小于或等于第二个数时返回true。 |
| [__hge](../数学函数/bfloat16类型/bfloat16类型比较函数/__hge-165.md) | 比较两个bfloat16类型数据，仅当第一个数大于或等于第二个数时返回true。 |
| [__hlt](../数学函数/bfloat16类型/bfloat16类型比较函数/__hlt-166.md) | 比较两个bfloat16类型数据，仅当第一个数小于第二个数时返回true。 |
| [__hgt](../数学函数/bfloat16类型/bfloat16类型比较函数/__hgt-167.md) | 比较两个bfloat16类型数据，仅当第一个数大于第二个数时返回true。 |
| [__hequ](../数学函数/bfloat16类型/bfloat16类型比较函数/__hequ-168.md) | 比较两个bfloat16类型数据是否相等，相等时返回true。若任一输入为nan，返回true。 |
| [__hneu](../数学函数/bfloat16类型/bfloat16类型比较函数/__hneu-169.md) | 比较两个bfloat16类型数据是否不相等，不相等时返回true。若任一输入为nan，返回true。 |
| [__hleu](../数学函数/bfloat16类型/bfloat16类型比较函数/__hleu-170.md) | 比较两个bfloat16类型数据，当第一个数小于或等于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hgeu](../数学函数/bfloat16类型/bfloat16类型比较函数/__hgeu-171.md) | 比较两个bfloat16类型数据，当第一个数大于或等于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hltu](../数学函数/bfloat16类型/bfloat16类型比较函数/__hltu-172.md) | 比较两个bfloat16类型数据，当第一个数小于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hgtu](../数学函数/bfloat16类型/bfloat16类型比较函数/__hgtu-173.md) | 比较两个bfloat16类型数据，当第一个数大于第二个数时返回true。若任一输入为nan，返回true。 |
| [__hmax_nan](../数学函数/bfloat16类型/bfloat16类型比较函数/__hmax_nan-174.md) | 获取两个输入数据中的最大值。任一输入为nan时返回nan。 |
| [__hmin_nan](../数学函数/bfloat16类型/bfloat16类型比较函数/__hmin_nan-175.md) | 获取两个输入数据中的最小值。任一输入为nan时返回nan。 |

**表17**  bfloat16数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [htanh](../数学函数/bfloat16类型/bfloat16类型数学库函数/htanh-176.md) | 获取输入数据的双曲正切值。 |
| [hexp](../数学函数/bfloat16类型/bfloat16类型数学库函数/hexp-177.md) | 指定输入x，获取e的x次方。 |
| [hexp2](../数学函数/bfloat16类型/bfloat16类型数学库函数/hexp2-178.md) | 指定输入x，获取2的x次方。 |
| [hexp10](../数学函数/bfloat16类型/bfloat16类型数学库函数/hexp10-179.md) | 指定输入x，获取10的x次方。 |
| [hlog](../数学函数/bfloat16类型/bfloat16类型数学库函数/hlog-180.md) | 获取以e为底，输入数据的对数。 |
| [hlog2](../数学函数/bfloat16类型/bfloat16类型数学库函数/hlog2-181.md) | 获取以2为底，输入数据的对数。 |
| [hlog10](../数学函数/bfloat16类型/bfloat16类型数学库函数/hlog10-182.md) | 获取以10为底，输入数据的对数。 |
| [hcos](../数学函数/bfloat16类型/bfloat16类型数学库函数/hcos-183.md) | 获取输入数据的三角函数余弦值。 |
| [hsin](../数学函数/bfloat16类型/bfloat16类型数学库函数/hsin-184.md) | 获取输入数据的三角函数正弦值。 |
| [hsqrt](../数学函数/bfloat16类型/bfloat16类型数学库函数/hsqrt-185.md) | 获取输入数据x的平方根。 |
| [hrsqrt](../数学函数/bfloat16类型/bfloat16类型数学库函数/hrsqrt-186.md) | 获取输入数据x的平方根的倒数。 |
| [hrcp](../数学函数/bfloat16类型/bfloat16类型数学库函数/hrcp-187.md) | 获取输入数据x的倒数。 |
| [hrint](../数学函数/bfloat16类型/bfloat16类型数学库函数/hrint-188.md) | 获取与输入数据最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [hfloor](../数学函数/bfloat16类型/bfloat16类型数学库函数/hfloor-189.md) | 获取小于或等于输入数据的最大整数值。 |
| [hceil](../数学函数/bfloat16类型/bfloat16类型数学库函数/hceil-190.md) | 获取大于或等于输入数据的最小整数值。 |
| [htrunc](../数学函数/bfloat16类型/bfloat16类型数学库函数/htrunc-191.md) | 获取对输入数据的浮点数截断后的整数。 |

**表18**  bfloat16类型精度转换函数

| 接口名 | 功能描述 |
| --- | --- |
| [__float2bfloat16](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_rn_sat](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rn_sat.md) | 饱和模式下获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__float22bfloat162_rn_sat](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rn_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_RINT模式转换成的bfloat16x2_t类型数据。 |
| [__float2bfloat16_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_rz_sat](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rz_sat.md) | 饱和模式下获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__float22bfloat162_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rz.md) | 获取输入的两个分量遵循CAST_TRUNC模式转换成的bfloat16x2_t类型数据。 |
| [__float22bfloat162_rz_sat](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rz_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_TRUNC模式转换成的bfloat16x2_t类型数据。 |
| [__float2bfloat16_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_rd_sat](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rd_sat.md) | 饱和模式下获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__float22bfloat162_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rd.md) | 获取输入的两个分量遵循CAST_FLOOR模式转换成的bfloat16x2_t类型数据。 |
| [__float22bfloat162_rd_sat](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rd_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_FLOOR模式转换成的bfloat16x2_t类型数据。 |
| [__float2bfloat16_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_ru_sat](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_ru_sat.md) | 饱和模式下获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__float22bfloat162_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_ru.md) | 获取输入的两个分量遵循CAST_CEIL模式转换成的bfloat16x2_t类型数据。 |
| [__float22bfloat162_ru_sat](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_ru_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_CEIL模式转换成的bfloat16x2_t类型数据。 |
| [__float2bfloat16_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__float2bfloat16_rna_sat](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat16_rna_sat.md) | 饱和模式下获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__float22bfloat162_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rna.md) | 获取输入的两个分量遵循CAST_ROUND模式转换成的bfloat16x2_t类型数据。 |
| [__float22bfloat162_rna_sat](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rna_sat.md) | 饱和模式下获取输入的两个分量遵循CAST_ROUND模式转换成的bfloat16x2_t类型数据。 |
| [__half2bfloat16_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__half2bfloat16_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__half2bfloat16_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__half2bfloat16_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__half2bfloat16_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__half2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__bfloat162float](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162float.md) | 获取输入转换为浮点数的结果。 |
| [__bfloat162bfloat16_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rn.md) | 获取输入遵循CAST_RINT模式取整后的bfloat16_t类型数据。 |
| [__bfloat162bfloat16_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式取整后的bfloat16_t类型数据。 |
| [__bfloat162bfloat16_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式取整后的bfloat16_t类型数据。 |
| [__bfloat162bfloat16_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式取整后的bfloat16_t类型数据。 |
| [__bfloat162bfloat16_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式取整后的bfloat16_t类型数据。 |
| [__bfloat162uint_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rn.md) | 获取输入遵循CAST_RINT模式转换成的无符号整数。 |
| [__bfloat162uint_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的无符号整数。 |
| [__bfloat162uint_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的无符号整数。 |
| [__bfloat162uint_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_ru.md) | 获取输入遵循CAST_CEIL模式转换成的无符号整数。 |
| [__bfloat162uint_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162uint_rna.md) | 获取输入遵循CAST_ROUND模式转换成的无符号整数。 |
| [__bfloat162int_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rn.md) | 获取输入遵循CAST_RINT模式转换成的有符号整数。 |
| [__bfloat162int_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的有符号整数。 |
| [__bfloat162int_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的有符号整数。 |
| [__bfloat162int_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_ru.md) | 获取输入遵循CAST_CEIL模式转换成的有符号整数。 |
| [__bfloat162int_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162int_rna.md) | 获取输入遵循CAST_ROUND模式转换成的有符号整数。 |
| [__bfloat162ull_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位无符号整数。 |
| [__bfloat162ull_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位无符号整数。 |
| [__bfloat162ull_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位无符号整数。 |
| [__bfloat162ull_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位无符号整数。 |
| [__bfloat162ull_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ull_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位无符号整数。 |
| [__bfloat162ll_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位有符号整数。 |
| [__bfloat162ll_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位有符号整数。 |
| [__bfloat162ll_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位有符号整数。 |
| [__bfloat162ll_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位有符号整数。 |
| [__bfloat162ll_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162ll_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位有符号整数。 |
| [__uint2bfloat16_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__uint2bfloat16_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__uint2bfloat16_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__uint2bfloat16_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__uint2bfloat16_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__uint2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__int2bfloat16_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__int2bfloat16_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__int2bfloat16_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__int2bfloat16_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__int2bfloat16_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__int2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__ull2bfloat16_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__ull2bfloat16_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__ull2bfloat16_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__ull2bfloat16_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__ull2bfloat16_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ull2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__ll2bfloat16_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rn.md) | 获取输入遵循CAST_RINT模式转换成的bfloat16类型数据。 |
| [__ll2bfloat16_rz](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的bfloat16类型数据。 |
| [__ll2bfloat16_rd](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的bfloat16类型数据。 |
| [__ll2bfloat16_ru](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_ru.md) | 获取输入遵循CAST_CEIL模式转换成的bfloat16类型数据。 |
| [__ll2bfloat16_rna](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ll2bfloat16_rna.md) | 获取输入遵循CAST_ROUND模式转换成的bfloat16类型数据。 |
| [__float2bfloat162_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float2bfloat162_rn.md) | 将float类型数据遵循CAST_RINT模式转换为bfloat16类型并填充到bfloat16x2的前后两部分，返回填充后的bfloat16x2类型数据。 |
| [__floats2bfloat162_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__floats2bfloat162_rn.md) | 将输入的数据x，y遵循CAST_RINT模式分别转换为bfloat16类型并填充到bfloat16x2的前后两部分，返回转换后的bfloat16x2类型数据。 |
| [__float22bfloat162_rn](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__float22bfloat162_rn.md) | 将float2类型数据遵循CAST_RINT模式转换为bfloat16x2类型，返回转换后的bfloat16x2类型数据。 |
| [__bfloat162bfloat162](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat162bfloat162.md) | 将输入的数据填充为bfloat16x2前后两个分量，返回转换后的bfloat16x2类型数据。 |
| [__halves2bfloat162](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__halves2bfloat162.md) | 将输入的数据分别填充为bfloat16x2前后两个分量，返回填充后数据。 |
| [__high2bfloat16](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__high2bfloat16.md) | 提取输入bfloat16x2的高16位，并返回。 |
| [__high2bfloat162](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__high2bfloat162.md) | 将输入数据的高16位填充到bfloat16x2并返回结果。 |
| [__high2float](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__high2float.md) | 将输入数据的高16位转换为float类型并返回结果。 |
| [__highs2bfloat162](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__highs2bfloat162.md) | 分别提取两个bfloat162输入的高16位，并填充到bfloat162中。返回填充后的数据。 |
| [__low2bfloat16](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__low2bfloat16.md) | 返回输入数据的低16位。 |
| [__low2bfloat162](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__low2bfloat162.md) | 将输入数据的低16位填充到bfloat16x2并返回。 |
| [__low2float](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__low2float.md) | 将输入数据的低16位转换为浮点数并返回结果。 |
| [__lowhigh2highlow](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__lowhigh2highlow.md) | 将输入数据的高低16位进行交换并返回。 |
| [__lows2bfloat162](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__lows2bfloat162.md) | 分别提取两个bfloat162输入的低16位，并填充到bfloat162中。返回填充后的数据。 |
| [__bfloat1622float2](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__bfloat1622float2.md) | 将bfloat16x2的两个分量分别转换为float，并填充到float2返回。 |
| [__ushort_as_bfloat16](../数学函数/bfloat16类型/bfloat16类型精度转换函数/__ushort_as_bfloat16.md) | 将unsigned short int的按位重新解释为bfloat16，即将unsigned short int的数据存储的位按照bfloat16的格式进行读取。 |

**表19**  bfloat16x2类型算术函数

| 接口名 | 功能描述 |
| --- | --- |
| [__haddx2](../数学函数/bfloat16类型/bfloat16x2类型算术函数/__haddx2-192.md) | 计算两个bfloat16x2_t类型数据各分量的相加结果，并遵循CAST_RINT模式舍入。 |
| [__hsubx2](../数学函数/bfloat16类型/bfloat16x2类型算术函数/__hsubx2-193.md) | 计算两个bfloat16x2_t类型数据各分量的相减结果，并遵循CAST_RINT模式舍入。 |
| [__hmulx2](../数学函数/bfloat16类型/bfloat16x2类型算术函数/__hmulx2-194.md) | 计算两个bfloat16x2_t类型数据各分量的相乘结果，并遵循CAST_RINT模式舍入。 |
| [__hdivx2](../数学函数/bfloat16类型/bfloat16x2类型算术函数/__hdivx2-195.md) | 计算两个bfloat16x2_t类型数据各分量的相除结果，并遵循CAST_RINT模式舍入。 |
| [__habsx2](../数学函数/bfloat16类型/bfloat16x2类型算术函数/__habsx2-196.md) | 计算输入bfloat16x2_t类型数据各分量的绝对值。 |
| [__hfmax2](../数学函数/bfloat16类型/bfloat16x2类型算术函数/__hfmax2-197.md) | 计算两个bfloat16x2_t类型数据各分量的乘加的结果（前两个输入相乘后与第三个输入相加），并遵循CAST_RINT模式舍入。 |
| [__hnegx2](../数学函数/bfloat16类型/bfloat16x2类型算术函数/__hnegx2-198.md) | 获取输入bfloat16x2_t类型数据各分量的负值。 |
| [__hfmax2_relu](../数学函数/bfloat16类型/bfloat16x2类型算术函数/__hfmax2_relu-199.md) | 计算两个bfloat16x2_t类型数据各分量的乘加的结果（前两个输入相乘后与第三个输入相加），并遵循CAST_RINT模式舍入。负数结果置为0。 |
| [__hcmadd](../数学函数/bfloat16类型/bfloat16x2类型算术函数/__hcmadd-200.md) | 将三个bfloat16x2_t输入视为复数（第一个分量为实部，第二个分量为虚部），执行复数乘加运算x*y+z。 |

**表20**  bfloat16x2类型比较函数

| 接口名 | 功能描述 |
| --- | --- |
| [__hbeqx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbeqx2-201.md) | 比较两个bfloat16x2_t类型数据的两个分量是否相等，仅当两个分量均相等时返回true。 |
| [__hbnex2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbnex2-202.md) | 比较两个bfloat16x2_t类型数据的两个分量是否不相等，仅当两个分量均不相等时返回true。 |
| [__hblex2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hblex2-203.md) | 比较两个bfloat16x2_t类型数据的两个分量，仅当两个分量均满足第一个数小于或等于第二个数时返回true。 |
| [__hbgex2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgex2-204.md) | 比较两个bfloat16x2_t类型数据的两个分量，仅当两个分量均满足第一个数大于或等于第二个数时返回true。 |
| [__hbltx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbltx2-205.md) | 比较两个bfloat16x2_t类型数据的两个分量，仅当两个分量均满足第一个数小于第二个数时返回true。 |
| [__hbgtx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgtx2-206.md) | 比较两个bfloat16x2_t类型数据的两个分量，仅当两个分量均满足第一个数大于第二个数时返回true。 |
| [__hbequx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbequx2-207.md) | 比较两个bfloat16x2_t类型数据的两个分量是否相等，当两个分量均相等时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbneux2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbneux2-208.md) | 比较两个bfloat16x2_t类型数据的两个分量是否不相等，当两个分量均不相等时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbleux2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbleux2-209.md) | 比较两个bfloat16x2_t类型数据的两个分量，当两个分量均满足第一个数小于或等于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbgeux2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgeux2-210.md) | 比较两个bfloat16x2_t类型数据的两个分量，当两个分量均满足第一个数大于或等于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbltux2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbltux2-211.md) | 比较两个bfloat16x2_t类型数据的两个分量，当两个分量均满足第一个数小于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__hbgtux2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hbgtux2-212.md) | 比较两个bfloat16x2_t类型数据的两个分量，当两个分量均满足第一个数大于第二个数时返回true。若任一输入的分量为nan，该分量的比较结果为true。 |
| [__heqx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__heqx2-213.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量相等，则对应比较结果为1.0，否则为0.0。 |
| [__hnex2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hnex2-214.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量不相等，则对应比较结果为1.0，否则为0.0。 |
| [__hlex2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hlex2-215.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数小于或等于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hgex2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgex2-216.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数大于或等于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hltx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltx2-217.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数小于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hgtx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtx2-218.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数大于第二个数，则对应比较结果为1.0，否则为0.0。 |
| [__hequx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hequx2-219.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量相等，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hneux2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hneux2-220.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量不相等，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hleux2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hleux2-221.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数小于或等于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hgeux2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgeux2-222.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数大于或等于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hltux2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltux2-223.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数小于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__hgtux2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtux2-224.md) | 比较两个bfloat16x2_t类型数据的两个分量，如果分量满足第一个数大于第二个数，则对应比较结果为1.0，否则为0.0。若任一输入的分量为nan，该分量的比较结果为1.0。 |
| [__heqx2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__heqx2_mask-225.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量相等，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hnex2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hnex2_mask-226.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量不相等，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hlex2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hlex2_mask-227.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hgex2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgex2_mask-228.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hltx2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltx2_mask-229.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hgtx2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtx2_mask-230.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于第二个数，则对应16位掩码为0xFFFF，否则为0x0。 |
| [__hequx2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hequx2_mask-231.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量相等，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hneux2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hneux2_mask-232.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量不相等，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hleux2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hleux2_mask-233.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hgeux2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgeux2_mask-234.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于或等于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hltux2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hltux2_mask-235.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数小于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__hgtux2_mask](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hgtux2_mask-236.md) | 比较两个bfloat16x2_t类型数据的两个分量，结果以unsigned int形式返回，低16位为第一个分量的掩码结果，高16位为第二个分量的掩码结果。如果分量满足第一个数大于第二个数，则对应16位掩码为0xFFFF，否则为0x0。若任一输入的分量为nan，对应16位掩码为0xFFFF。 |
| [__isnanx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__isnanx2-237.md) | 判断bfloat16x2_t类型数据的两个分量是否为nan。 |
| [__hmaxx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hmaxx2-238.md) | 获取两个bfloat16x2_t类型数据各分量的最大值。 |
| [__hmaxx2_nan](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hmaxx2_nan-239.md) | 获取两个bfloat16x2_t类型数据各分量的最大值。任一分量为nan时对应结果为nan。 |
| [__hminx2](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hminx2-240.md) | 获取两个bfloat16x2_t类型数据各分量的最小值。 |
| [__hminx2_nan](../数学函数/bfloat16类型/bfloat16x2类型比较函数/__hminx2_nan-241.md) | 获取两个bfloat16x2_t类型数据各分量的最小值。任一分量为nan时对应结果为nan。 |

**表21**  bfloat16x2类型数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [h2tanh](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2tanh-242.md) | 获取输入数据各元素的双曲正切值。 |
| [h2exp](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp-243.md) | 指定输入x，对x的各元素，获取e的该元素次方。 |
| [h2exp2](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp2-244.md) | 指定输入x，对x的各元素，获取2的该元素次方。 |
| [h2exp10](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2exp10-245.md) | 指定输入x，对x的各元素，获取10的该元素次方。 |
| [h2log](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log-246.md) | 获取以e为底，输入数据各元素的对数。 |
| [h2log2](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log2-247.md) | 获取以2为底，输入数据各元素的对数。 |
| [h2log10](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2log10-248.md) | 获取以10为底，输入数据各元素的对数。 |
| [h2cos](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2cos-249.md) | 获取输入数据各元素的三角函数余弦值。 |
| [h2sin](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2sin-250.md) | 获取输入数据各元素的三角函数正弦值。 |
| [h2sqrt](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2sqrt-251.md) | 获取输入数据x各元素的平方根。 |
| [h2rsqrt](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2rsqrt-252.md) | 获取输入数据x各元素的平方根的倒数。 |
| [h2rcp](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2rcp-253.md) | 获取输入数据x各元素的倒数。 |
| [h2rint](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2rint-254.md) | 获取与输入数据各元素最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [h2floor](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2floor-255.md) | 获取小于或等于输入数据各元素的最大整数值。 |
| [h2ceil](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2ceil-256.md) | 获取大于或等于输入数据各元素的最小整数值。 |
| [h2trunc](../数学函数/bfloat16类型/bfloat16x2类型数学库函数/h2trunc-257.md) | 获取对输入数据各元素的浮点数截断后的整数。 |

**表22**  float类型数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [tanf](../数学函数/float类型数学库函数/tanf.md) | 获取输入数据的三角函数正切值。 |
| [tanhf](../数学函数/float类型数学库函数/tanhf.md) | 获取输入数据的双曲正切值。 |
| [tanpif](../数学函数/float类型数学库函数/tanpif.md) | 获取输入数据与π相乘的正切值。 |
| [atanf](../数学函数/float类型数学库函数/atanf.md) | 获取输入数据的反正切值。 |
| [atan2f](../数学函数/float类型数学库函数/atan2f.md) | 获取输入数据y/x的反正切值。 |
| [atanhf](../数学函数/float类型数学库函数/atanhf.md) | 获取输入数据的反双曲正切值。 |
| [expf](../数学函数/float类型数学库函数/expf.md) | 指定输入x，获取e的x次方。 |
| [exp2f](../数学函数/float类型数学库函数/exp2f.md) | 指定输入x，获取2的x次方。 |
| [exp10f](../数学函数/float类型数学库函数/exp10f.md) | 指定输入x，获取10的x次方。 |
| [expm1f](../数学函数/float类型数学库函数/expm1f.md) | 指定输入x，获取e的x次方减1。 |
| [logf](../数学函数/float类型数学库函数/logf.md) | 获取以e为底，输入数据的对数。 |
| [log2f](../数学函数/float类型数学库函数/log2f.md) | 获取以2为底，输入数据的对数。 |
| [log10f](../数学函数/float类型数学库函数/log10f.md) | 获取以10为底，输入数据的对数。 |
| [log1pf](../数学函数/float类型数学库函数/log1pf.md) | 获取以e为底，输入数据加1的对数。 |
| [logbf](../数学函数/float类型数学库函数/logbf.md) | 计算以2为底，输入数据的对数，并对结果向下取整，返回浮点数。 |
| [ilogbf](../数学函数/float类型数学库函数/ilogbf.md) | 计算以2为底，输入数据的对数，并对结果向下取整，返回整数。 |
| [cosf](../数学函数/float类型数学库函数/cosf.md) | 获取输入数据的三角函数余弦值。 |
| [coshf](../数学函数/float类型数学库函数/coshf.md) | 获取输入数据的双曲余弦值。 |
| [cospif](../数学函数/float类型数学库函数/cospif.md) | 获取输入数据与π相乘的余弦值。 |
| [acosf](../数学函数/float类型数学库函数/acosf.md) | 获取输入数据的反余弦值。 |
| [acoshf](../数学函数/float类型数学库函数/acoshf.md) | 获取输入数据的双曲反余弦值。 |
| [sinf](../数学函数/float类型数学库函数/sinf.md) | 获取输入数据的三角函数正弦值。 |
| [sinhf](../数学函数/float类型数学库函数/sinhf.md) | 获取输入数据的双曲正弦值。 |
| [sinpif](../数学函数/float类型数学库函数/sinpif.md) | 获取输入数据与π相乘的正弦值。 |
| [asinf](../数学函数/float类型数学库函数/asinf.md) | 获取输入数据的反正弦值。 |
| [asinhf](../数学函数/float类型数学库函数/asinhf.md) | 获取输入数据的双曲反正弦值。 |
| [sincosf](../数学函数/float类型数学库函数/sincosf.md) | 获取输入数据的三角函数正弦值和余弦值。 |
| [sincospif](../数学函数/float类型数学库函数/sincospif.md) | 获取输入数据与π相乘的三角函数正弦值和余弦值。 |
| [frexpf](../数学函数/float类型数学库函数/frexpf.md) | 将x转换为归一化[1/2, 1)的有符号数乘以2的积分幂。 |
| [ldexpf](../数学函数/float类型数学库函数/ldexpf.md) | 获取输入x乘以2的exp次幂的结果。 |
| [sqrtf](../数学函数/float类型数学库函数/sqrtf.md) | 获取输入数据x的平方根。 |
| [rsqrtf](../数学函数/float类型数学库函数/rsqrtf.md) | 获取输入数据x的平方根的倒数。 |
| [hypotf](../数学函数/float类型数学库函数/hypotf.md) | 获取输入数据x、y的平方和x^2 + y^2的平方根。 |
| [rhypotf](../数学函数/float类型数学库函数/rhypotf.md) | 获取输入数据x、y的平方和x^2 + y^2的平方根的倒数。 |
| [powf](../数学函数/float类型数学库函数/powf.md) | 获取输入数据x的y次幂。 |
| [norm3df](../数学函数/float类型数学库函数/norm3df.md) | 获取输入数据a、b、c的平方和a^2 + b^2 + c^2的平方根。 |
| [rnorm3df](../数学函数/float类型数学库函数/rnorm3df.md) | 获取输入数据a、b、c的平方和a^2 + b^2 + c^2的平方根的倒数。 |
| [norm4df](../数学函数/float类型数学库函数/norm4df.md) | 获取输入数据a、b、c、d的平方和a^2 + b^2+ c^2+ d^2的平方根。 |
| [rnorm4df](../数学函数/float类型数学库函数/rnorm4df.md) | 获取输入数据a、b、c、d的平方和a^2 + b^2 + c^2 + d^2的平方根的倒数。 |
| [normf](../数学函数/float类型数学库函数/normf.md) | 获取输入数据a中前n个元素的平方和a[0]^2 + a[1]^2 +...+ a[n-1]^2的平方根。 |
| [rnormf](../数学函数/float类型数学库函数/rnormf.md) | 获取输入数据a中前n个元素的平方和a[0]^2 + a[1]^2 + ...+ a[n-1]^2的平方根的倒数。 |
| [cbrtf](../数学函数/float类型数学库函数/cbrtf.md) | 获取输入数据x的立方根。 |
| [rcbrtf](../数学函数/float类型数学库函数/rcbrtf.md) | 获取输入数据x的立方根的倒数。 |
| [erff](../数学函数/float类型数学库函数/erff.md) | 获取输入数据的误差函数值。 |
| [erfcf](../数学函数/float类型数学库函数/erfcf.md) | 获取输入数据的互补误差函数值。 |
| [erfinvf](../数学函数/float类型数学库函数/erfinvf.md) | 获取输入数据的逆误差函数值。 |
| [erfcinvf](../数学函数/float类型数学库函数/erfcinvf.md) | 获取输入数据的逆互补误差函数值。 |
| [erfcxf](../数学函数/float类型数学库函数/erfcxf.md) | 获取输入数据的缩放互补误差函数值。 |
| [tgammaf](../数学函数/float类型数学库函数/tgammaf.md) | 获取输入数据x的伽马函数值。 |
| [lgammaf](../数学函数/float类型数学库函数/lgammaf.md) | 获取输入数据x伽马值的绝对值并求自然对数。 |
| [cyl_bessel_i0f](../数学函数/float类型数学库函数/cyl_bessel_i0f.md) | 获取输入数据x的0阶常规修正圆柱贝塞尔函数的值。 |
| [cyl_bessel_i1f](../数学函数/float类型数学库函数/cyl_bessel_i1f.md) | 获取输入数据x的1阶常规修正圆柱贝塞尔函数的值。 |
| [normcdff](../数学函数/float类型数学库函数/normcdff.md) | 获取输入数据x的标准正态分布的累积分布函数值。 |
| [normcdfinvf](../数学函数/float类型数学库函数/normcdfinvf.md) | 获取输入数据x的标准正态累积分布的逆函数。 |
| [j0f](../数学函数/float类型数学库函数/j0f.md) | 获取输入数据x的0阶第一类贝塞尔函数j0的值。 |
| [j1f](../数学函数/float类型数学库函数/j1f.md) | 获取输入数据x的1阶第一类贝塞尔函数j1的值。 |
| [jnf](../数学函数/float类型数学库函数/jnf.md) | 获取输入数据x的n阶第一类贝塞尔函数jn的值。 |
| [y0f](../数学函数/float类型数学库函数/y0f.md) | 获取输入数据x的0阶第二类贝塞尔函数y0的值。 |
| [y1f](../数学函数/float类型数学库函数/y1f.md) | 获取输入数据x的1阶第二类贝塞尔函数y1的值。 |
| [ynf](../数学函数/float类型数学库函数/ynf.md) | 获取输入数据x的n阶第二类贝塞尔函数yn的值。 |
| [fabsf](../数学函数/float类型数学库函数/fabsf.md) | 获取输入数据的绝对值。 |
| [fmaf](../数学函数/float类型数学库函数/fmaf.md) | 对输入数据x、y、z，计算x与y相乘加上z的结果。 |
| [fmaxf](../数学函数/float类型数学库函数/fmaxf.md) | 获取两个输入数据中的最大值。 |
| [fminf](../数学函数/float类型数学库函数/fminf.md) | 获取两个输入数据中的最小值。 |
| [fdimf](../数学函数/float类型数学库函数/fdimf.md) | 获取输入数据的差值，差值小于0时，返回0。 |
| [remquof](../数学函数/float类型数学库函数/remquof.md) | 获取输入数据x除以y的余数。求余数时，商取最接近x除以y浮点数结果的整数，当x除以y的浮点数结果与左右最接近的整数距离相等时，商取偶数，同时将商赋值给指针变量quo。 |
| [fmodf](../数学函数/float类型数学库函数/fmodf.md) | 获取输入数据x除以y的余数。求余数时，商取x除以y浮点数结果的整数部分。 |
| [remainderf](../数学函数/float类型数学库函数/remainderf.md) | 获取输入数据x除以y的余数。求余数时，商取最接近x除以y浮点数结果的整数，当x除以y的浮点数结果与左右最接近的整数距离相等时，商取偶数。 |
| [copysignf](../数学函数/float类型数学库函数/copysignf.md) | 获取由第一个输入x的数值部分和第二个输入y的符号部分拼接得到的浮点数。 |
| [nearbyintf](../数学函数/float类型数学库函数/nearbyintf.md) | 获取与输入浮点数最接近的整数，输入浮点数与左右整数的距离相等时，返回偶数。 |
| [nextafterf](../数学函数/float类型数学库函数/nextafterf.md) | 如果y大于x，返回比x大的下一个可表示的浮点值，即浮点数二进制最低位加1。<br><br>如果y小于x，返回比x小的下一个可表示的浮点值，即浮点数二进制最低位减1。<br><br>如果y等于x，返回x。 |
| [scalbnf](../数学函数/float类型数学库函数/scalbnf.md) | 获取输入数据x与2的n次方的乘积。 |
| [scalblnf](../数学函数/float类型数学库函数/scalblnf.md) | 获取输入数据x与2的n次方的乘积。 |
| [modff](../数学函数/float类型数学库函数/modff.md) | 将输入数据分解为小数部分和整数部分。 |
| [fdividef](../数学函数/float类型数学库函数/fdividef.md) | 获取两个输入数据相除的结果。 |
| [signbit](../数学函数/float类型数学库函数/signbit.md) | 获取输入数据的符号位。 |
| [__saturatef](../数学函数/float类型数学库函数/__saturatef.md) | 将输入数据钳位到[0.0, 1.0]区间。 |
| [__fdividef](../数学函数/float类型数学库函数/__fdividef.md) | 获取两个输入数据相除的结果。 |
| [rintf](../数学函数/float类型数学库函数/rintf.md) | 获取与输入数据最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [lrintf](../数学函数/float类型数学库函数/lrintf.md) | 获取与输入数据最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [llrintf](../数学函数/float类型数学库函数/llrintf.md) | 获取与输入数据最接近的整数，若存在两个同样接近的整数，则获取其中的偶数。 |
| [roundf](../数学函数/float类型数学库函数/roundf.md) | 获取对输入数据四舍五入后的整数。 |
| [lroundf](../数学函数/float类型数学库函数/lroundf.md) | 获取对输入数据四舍五入后的整数。 |
| [llroundf](../数学函数/float类型数学库函数/llroundf.md) | 获取对输入数据四舍五入后的整数。 |
| [floorf](../数学函数/float类型数学库函数/floorf.md) | 获取小于或等于输入数据的最大整数值。 |
| [ceilf](../数学函数/float类型数学库函数/ceilf.md) | 获取大于或等于输入数据的最小整数值。 |
| [truncf](../数学函数/float类型数学库函数/truncf.md) | 获取对输入数据的浮点数截断后的整数。 |
| [isfinite](../数学函数/float类型数学库函数/isfinite1.md) | 判断浮点数是否为有限数（非inf、非nan）。 |
| [isnan](../数学函数/float类型数学库函数/isnan1.md) | 判断浮点数是否为nan。 |
| [isinf](../数学函数/float类型数学库函数/isinf1.md) | 判断浮点数是否为无穷。 |

**表23**  类型转换函数

| 接口名 | 功能描述 |
| --- | --- |
| [__float2float_rn](../数学函数/数据类型转换/类型转换函数/__float2float_rn.md) | 获取输入遵循CAST_RINT模式取整后的浮点数。 |
| [__float2float_rz](../数学函数/数据类型转换/类型转换函数/__float2float_rz.md) | 获取输入遵循CAST_TRUNC模式取整后的浮点数。 |
| [__float2float_rd](../数学函数/数据类型转换/类型转换函数/__float2float_rd.md) | 获取输入遵循CAST_FLOOR模式取整后的浮点数。 |
| [__float2float_ru](../数学函数/数据类型转换/类型转换函数/__float2float_ru.md) | 获取输入遵循CAST_CEIL模式取整后的浮点数。 |
| [__float2float_rna](../数学函数/数据类型转换/类型转换函数/__float2float_rna.md) | 获取输入遵循CAST_ROUND模式取整后的浮点数。 |
| [__float2uint_rn](../数学函数/数据类型转换/类型转换函数/__float2uint_rn.md) | 获取输入遵循CAST_RINT模式转换成的无符号整数。 |
| [__float2uint_rz](../数学函数/数据类型转换/类型转换函数/__float2uint_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的无符号整数。 |
| [__float2uint_rd](../数学函数/数据类型转换/类型转换函数/__float2uint_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的无符号整数。 |
| [__float2uint_ru](../数学函数/数据类型转换/类型转换函数/__float2uint_ru.md) | 获取输入遵循CAST_CEIL模式转换成的无符号整数。 |
| [__float2uint_rna](../数学函数/数据类型转换/类型转换函数/__float2uint_rna.md) | 获取输入遵循CAST_ROUND模式转换成的无符号整数。 |
| [__float2int_rn](../数学函数/数据类型转换/类型转换函数/__float2int_rn.md) | 获取输入遵循CAST_RINT模式转换成的有符号整数。 |
| [__float2int_rz](../数学函数/数据类型转换/类型转换函数/__float2int_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的有符号整数。 |
| [__float2int_rd](../数学函数/数据类型转换/类型转换函数/__float2int_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的有符号整数。 |
| [__float2int_ru](../数学函数/数据类型转换/类型转换函数/__float2int_ru.md) | 获取输入遵循CAST_CEIL模式转换成的有符号整数。 |
| [__float2int_rna](../数学函数/数据类型转换/类型转换函数/__float2int_rna.md) | 获取输入遵循CAST_ROUND模式转换成的有符号整数。 |
| [__float2ull_rn](../数学函数/数据类型转换/类型转换函数/__float2ull_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位无符号整数。 |
| [__float2ull_rz](../数学函数/数据类型转换/类型转换函数/__float2ull_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位无符号整数。 |
| [__float2ull_rd](../数学函数/数据类型转换/类型转换函数/__float2ull_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位无符号整数。 |
| [__float2ull_ru](../数学函数/数据类型转换/类型转换函数/__float2ull_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位无符号整数。 |
| [__float2ull_rna](../数学函数/数据类型转换/类型转换函数/__float2ull_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位无符号整数。 |
| [__float2ll_rn](../数学函数/数据类型转换/类型转换函数/__float2ll_rn.md) | 获取输入遵循CAST_RINT模式转换成的64位有符号整数。 |
| [__float2ll_rz](../数学函数/数据类型转换/类型转换函数/__float2ll_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的64位有符号整数。 |
| [__float2ll_rd](../数学函数/数据类型转换/类型转换函数/__float2ll_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的64位有符号整数。 |
| [__float2ll_ru](../数学函数/数据类型转换/类型转换函数/__float2ll_ru.md) | 获取输入遵循CAST_CEIL模式转换成的64位有符号整数。 |
| [__float2ll_rna](../数学函数/数据类型转换/类型转换函数/__float2ll_rna.md) | 获取输入遵循CAST_ROUND模式转换成的64位有符号整数。 |
| [__uint2float_rn](../数学函数/数据类型转换/类型转换函数/__uint2float_rn.md) | 获取输入遵循CAST_RINT模式转换成的浮点数。 |
| [__uint2float_rz](../数学函数/数据类型转换/类型转换函数/__uint2float_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的浮点数。 |
| [__uint2float_rd](../数学函数/数据类型转换/类型转换函数/__uint2float_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的浮点数。 |
| [__uint2float_ru](../数学函数/数据类型转换/类型转换函数/__uint2float_ru.md) | 获取输入遵循CAST_CEIL模式转换成的浮点数。 |
| [__uint2float_rna](../数学函数/数据类型转换/类型转换函数/__uint2float_rna.md) | 获取输入遵循CAST_ROUND模式转换成的浮点数。 |
| [__int2float_rn](../数学函数/数据类型转换/类型转换函数/__int2float_rn.md) | 获取输入遵循CAST_RINT模式转换成的浮点数。 |
| [__int2float_rz](../数学函数/数据类型转换/类型转换函数/__int2float_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的浮点数。 |
| [__int2float_rd](../数学函数/数据类型转换/类型转换函数/__int2float_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的浮点数。 |
| [__int2float_ru](../数学函数/数据类型转换/类型转换函数/__int2float_ru.md) | 获取输入遵循CAST_CEIL模式转换成的浮点数。 |
| [__int2float_rna](../数学函数/数据类型转换/类型转换函数/__int2float_rna.md) | 获取输入遵循CAST_ROUND模式转换成的浮点数。 |
| [__ull2float_rn](../数学函数/数据类型转换/类型转换函数/__ull2float_rn.md) | 获取输入遵循CAST_RINT模式转换成的浮点数。 |
| [__ull2float_rz](../数学函数/数据类型转换/类型转换函数/__ull2float_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的浮点数。 |
| [__ull2float_rd](../数学函数/数据类型转换/类型转换函数/__ull2float_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的浮点数。 |
| [__ull2float_ru](../数学函数/数据类型转换/类型转换函数/__ull2float_ru.md) | 获取输入遵循CAST_CEIL模式转换成的浮点数。 |
| [__ull2float_rna](../数学函数/数据类型转换/类型转换函数/__ull2float_rna.md) | 获取输入遵循CAST_ROUND模式转换成的浮点数。 |
| [__ll2float_rn](../数学函数/数据类型转换/类型转换函数/__ll2float_rn.md) | 获取输入遵循CAST_RINT模式转换成的浮点数。 |
| [__ll2float_rz](../数学函数/数据类型转换/类型转换函数/__ll2float_rz.md) | 获取输入遵循CAST_TRUNC模式转换成的浮点数。 |
| [__ll2float_rd](../数学函数/数据类型转换/类型转换函数/__ll2float_rd.md) | 获取输入遵循CAST_FLOOR模式转换成的浮点数。 |
| [__ll2float_ru](../数学函数/数据类型转换/类型转换函数/__ll2float_ru.md) | 获取输入遵循CAST_CEIL模式转换成的浮点数。 |
| [__ll2float_rna](../数学函数/数据类型转换/类型转换函数/__ll2float_rna.md) | 获取输入遵循CAST_ROUND模式转换成的浮点数。 |
| [__int_as_float](../数学函数/数据类型转换/类型转换函数/__int_as_float.md) | 将整数中的位重新解释为浮点数。 |
| [__uint_as_float](../数学函数/数据类型转换/类型转换函数/__uint_as_float.md) | 将无符号整数中的位重新解释为浮点数。 |
| [__float_as_int](../数学函数/数据类型转换/类型转换函数/__float_as_int.md) | 将浮点数中的位重新解释为有符号整数。 |
| [__float_as_uint](../数学函数/数据类型转换/类型转换函数/__float_as_uint.md) | 将浮点数中的位重新解释为无符号整数。 |

**表24**  整型数学库函数

| 接口名 | 功能描述 |
| --- | --- |
| [labs](../数学函数/整型数学库函数/labs.md) | 获取输入数据的绝对值。 |
| [llabs](../数学函数/整型数学库函数/llabs.md) | 获取输入数据的绝对值。 |
| [llmax](../数学函数/整型数学库函数/llmax.md) | 获取两个输入数据中的最大值。 |
| [ullmax](../数学函数/整型数学库函数/ullmax.md) | 获取两个输入数据中的最大值。 |
| [umax](../数学函数/整型数学库函数/umax.md) | 获取两个输入数据中的最大值。 |
| [llmin](../数学函数/整型数学库函数/llmin.md) | 获取两个输入数据中的最小值。 |
| [ullmin](../数学函数/整型数学库函数/ullmin.md) | 获取两个输入数据中的最小值。 |
| [umin](../数学函数/整型数学库函数/umin.md) | 获取两个输入数据中的最小值。 |
| [__mulhi](../数学函数/整型数学库函数/__mulhi.md) | 获取输入int32类型数据x和y乘积的高32位。 |
| [__umulhi](../数学函数/整型数学库函数/__umulhi.md) | 获取输入uint32类型数据x和y乘积的高32位。 |
| [__mul64hi](../数学函数/整型数学库函数/__mul64hi.md) | 获取输入int64类型数据x和y乘积的高64位。 |
| [__umul64hi](../数学函数/整型数学库函数/__umul64hi.md) | 获取输入uint64类型数据x和y乘积的高64位。 |
| [__mul_i32toi64](../数学函数/整型数学库函数/__mul_i32toi64.md) | 计算输入32位整数x和y的乘积，返回64位结果。 |
| [__brev](../数学函数/整型数学库函数/__brev.md) | 将输入数据的位序反转，返回反转后的值。 |
| [__clz](../数学函数/整型数学库函数/__clz.md) | 从输入数据的二进制最高有效位开始，返回连续的前导零的位数。 |
| [__ffs](../数学函数/整型数学库函数/__ffs.md) | 从二进制输入数据的最低位开始，查找第一个值为1的比特位的位置，并返回该位置的索引，索引从1开始计数；如果二进制数据中没有1，则返回0。 |
| [__popc](../数学函数/整型数学库函数/__popc.md) | 统计输入数据从二进制的高位到低位比特位为1的数量。 |
| [__byte_perm](../数学函数/整型数学库函数/__byte_perm.md) | 由输入的两个4字节的uint32_t类型数据组成一个8个字节的64比特位的整数，通过选择器s指定选取其中的4个字节，将这4个字节从低位到高位拼成一个uint32_t类型的整数。 |
| [__sad](../数学函数/整型数学库函数/__sad.md) | 对输入数据x、y、z，计算\|x - y\|+z的结果，即第一个入参和第二个入参之差的绝对值与第三个入参的和。 |
| [__usad](../数学函数/整型数学库函数/__usad.md) | 对输入数据x、y、z，计算\|x - y\|+z的结果，即第一个入参和第二个入参之差的绝对值与第三个入参的和。 |
| [__mul24](../数学函数/整型数学库函数/__mul24.md) | 获取输入int32类型数据x和y低24位乘积的低32位结果。x和y的高8位被忽略。 |
| [__umul24](../数学函数/整型数学库函数/__umul24.md) | 获取输入uint32类型数据x和y低24位乘积的低32位结果。x和y的高8位被忽略。 |
| [__hadd](../数学函数/整型数学库函数/__hadd-259.md) | 获取输入int32类型数据x和y的平均值，避免中间求和溢出。 |
| [__rhadd](../数学函数/整型数学库函数/__rhadd.md) | 获取输入int32类型数据x和y的向上取整平均值，避免中间求和溢出。 |
| [__uhadd](../数学函数/整型数学库函数/__uhadd.md) | 获取输入uint32类型数据x和y的平均值，避免中间求和溢出。 |
| [__urhadd](../数学函数/整型数学库函数/__urhadd.md) | 获取输入uint32类型数据x和y的向上取整平均值，避免中间求和溢出。 |
| [max](../数学函数/整型数学库函数/max1.md) | 获取两个输入数据中的最大值。 |
| [min](../数学函数/整型数学库函数/min1.md) | 获取两个输入数据中的最小值。 |

## 访存函数<a name="section1064618511375"></a>

**表25**  访存函数

| 接口名 | 功能描述 |
| --- | --- |
| [asc_ldcg](../访存函数/asc_ldcg.md) | 从L2 Cache加载缓存的数据，如果缓存命中，则直接返回数据。若未命中，则从Global Memory地址预加载数据缓存至L2 Cache，并返回数据。 |
| [asc_ldca](../访存函数/asc_ldca.md) | 首先从Data Cache加载缓存数据，若未命中，则尝试从L2 Cache加载。如果Data Cache和L2 Cache中均未找到所需数据，则从Global Memory中读取数据，然后将其缓存到L2 Cache和Data Cache中。 |
| [asc_stcg](../访存函数/asc_stcg.md) | 将指定数据存储到Global Memory的地址address中，并缓存到L2 Cache，但不缓存至Data Cache。 |
| [asc_stwt](../访存函数/asc_stwt.md) | 将指定数据存储到Global Memory的地址address中，并缓存至Data Cache和L2 Cache。 |
| [asc_dcci_single](../访存函数/asc_dcci_single.md) | 刷新指定地址所在的Cache Line，保证数据读取时Cache的一致性。 |
| [asc_dcci_entire](../访存函数/asc_dcci_entire.md) | 刷新核内的整个Data Cache，保证数据读取时Cache的一致性。 |
| [asc_nop](../访存函数/asc_nop.md) | 生成一条空操作指令，占用当前线程15个时钟周期，不执行任何实际计算与访存操作。 |

## 地址空间谓词函数<a name="section97001946144014"></a>

**表26**  地址空间谓词函数

| 接口名 | 功能描述 |
| --- | --- |
| [__isGlobal](../地址空间谓词函数/__isGlobal.md) | 判断输入的指针是否指向Global Memory内存空间的地址。 |
| [__isUbuf](../地址空间谓词函数/__isUbuf.md) | 判断输入的指针是否指向Unified Buffer内存空间的地址。 |
| [__isLocal](../地址空间谓词函数/__isLocal.md) | 判断输入的指针是否指向栈空间的地址。 |

## 地址空间转换函数

**表27**  地址空间转换函数

| 接口名 | 功能描述 |
| --- | --- |
| [__cvta_generic_to_global](../地址空间转换函数/__cvta_generic_to_global.md) | 将输入的指针转换为其指向的Global Memory内存空间的地址值并返回。 |
| [__cvta_generic_to_ubuf](../地址空间转换函数/__cvta_generic_to_ubuf.md) | 将输入的指针转换为其指向的Unified Buffer内存空间的地址值并返回。 |
| [__cvta_generic_to_local](../地址空间转换函数/__cvta_generic_to_local.md) | 将输入的指针转换为其指向的栈空间地址的值并返回。 |
| [__cvta_global_to_generic](../地址空间转换函数/__cvta_global_to_generic.md) | 将Global Memory内存空间的地址值转换为对应的指针。 |
| [__cvta_ubuf_to_generic](../地址空间转换函数/__cvta_ubuf_to_generic.md) | 将Unified Buffer内存空间的地址值转换为对应的指针。 |
| [__cvta_local_to_generic](../地址空间转换函数/__cvta_local_to_generic.md) | 将栈空间的地址值转换为对应的指针。 |

## 协作组

**表28**  thread_block接口

| 接口名 | 功能描述 |
| --- | --- |
| [this_thread_block](../协作组/thread_block/thread_block构造函数.md) | 获取当前线程块协作组对象。 |
| [sync](../协作组/thread_block/sync.md) | 同步线程块内所有线程。 |
| [size](../协作组/thread_block/size.md) | 获取线程块内线程总数。 |
| [num_threads](../协作组/thread_block/num_threads.md) | 获取线程块内线程总数。 |
| [thread_rank](../协作组/thread_block/thread_rank.md) | 获取当前线程在线程块内的排名。 |
| [thread_index](../协作组/thread_block/thread_index.md) | 获取当前线程在线程块内的三维索引。 |
| [group_index](../协作组/thread_block/group_index.md) | 获取当前线程块在网格中的三维索引。 |
| [group_dim](../协作组/thread_block/group_dim.md) | 获取线程块的维度。 |
| [dim_threads](../协作组/thread_block/dim_threads.md) | 获取线程块内线程的三维维度。 |

**表29**  coalesced_group接口

| 接口名 | 功能描述 |
| --- | --- |
| [coalesced_threads](../协作组/coalesced_group/coalesced_group构造函数.md) | 获取当前活跃线程组成的协作组对象。 |
| [sync](../协作组/coalesced_group/sync.md) | 同步组内所有线程。 |
| [size](../协作组/coalesced_group/size.md) | 获取组内线程总数。 |
| [num_threads](../协作组/coalesced_group/num_threads.md) | 获取组内线程总数。 |
| [thread_rank](../协作组/coalesced_group/thread_rank.md) | 获取当前线程在组内的排名。 |
| [meta_group_rank](../协作组/coalesced_group/meta_group_rank.md) | 获取当前协作组在父组的排名。 |
| [meta_group_size](../协作组/coalesced_group/meta_group_size.md) | 获取父组被划分时创建的子组数量。 |
| [shfl](../协作组/coalesced_group/shfl.md) | 组内线程的数据交换，直接读取组内指定线程的数据。 |
| [shfl_up](../协作组/coalesced_group/shfl_up.md) | 获取组内当前线程向前偏移delta的线程的数据。 |
| [shfl_down](../协作组/coalesced_group/shfl_down.md) | 获取组内当前线程向后偏移delta的线程的数据。 |
| [ballot](../协作组/coalesced_group/ballot.md) | 判断组内每个活跃线程的输入是否非零。 |
| [any](../协作组/coalesced_group/any.md) | 判断是否有组内线程的输入不为0。 |
| [all](../协作组/coalesced_group/all.md) | 判断是否所有组内线程的输入均不为0。 |

**表30**  thread_block_tile接口

| 接口名 | 功能描述 |
| --- | --- |
| [tiled_partition](../协作组/thread_block_tile/thread_block_tile构造函数.md) | 创建指定大小的thread_block_tile协作组。 |
| [sync](../协作组/thread_block_tile/sync.md) | 同步组内所有线程。 |
| [size](../协作组/thread_block_tile/size.md) | 获取组内线程总数。 |
| [num_threads](../协作组/thread_block_tile/num_threads.md) | 获取组内线程总数。 |
| [thread_rank](../协作组/thread_block_tile/thread_rank.md) | 获取当前线程在组内的排名。 |
| [meta_group_rank](../协作组/thread_block_tile/meta_group_rank.md) | 获取当前协作组在父组的排名。 |
| [meta_group_size](../协作组/thread_block_tile/meta_group_size.md) | 获取父组被划分时创建的子组数量。 |
| [shfl](../协作组/thread_block_tile/shfl.md) | 组内线程的数据交换，直接读取组内指定线程的数据。 |
| [shfl_up](../协作组/thread_block_tile/shfl_up.md) | 获取组内当前线程向前偏移delta的线程的数据。 |
| [shfl_down](../协作组/thread_block_tile/shfl_down.md) | 获取组内当前线程向后偏移delta的线程的数据。 |
| [shfl_xor](../协作组/thread_block_tile/shfl_xor.md) | 获取组内与当前线程rank做异或运算后的线程的数据。 |
| [ballot](../协作组/thread_block_tile/ballot.md) | 判断组内每个活跃线程的输入是否非零。 |
| [any](../协作组/thread_block_tile/any.md) | 判断是否有组内线程的输入不为0。 |
| [all](../协作组/thread_block_tile/all.md) | 判断是否所有组内线程的输入均不为0。 |

**表31**  协作组划分接口

| 接口名 | 功能描述 |
| --- | --- |
| [tiled_partition](../协作组/tiled_partition.md) | 将一个线程组按指定大小划分为多个子组。 |
| [binary_partition](../协作组/binary_partition.md) | 根据一个标签（0或1）将父组划分为两个子组。 |
