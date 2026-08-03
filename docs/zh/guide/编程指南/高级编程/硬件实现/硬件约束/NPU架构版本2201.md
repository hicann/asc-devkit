# NPU架构版本2201<a name="ZH-CN_TOPIC_0000002477825678"></a>

本节介绍硬件约束以及解决方案建议。对应的产品型号为：

<!-- npu="A3" id1 -->
-   Atlas A3 训练系列产品/Atlas A3 推理系列产品
<!-- end id1 -->
<!-- npu="910b" id2 -->
-   Atlas A2 训练系列产品/Atlas A2 推理系列产品
<!-- end id2 -->

**表1**  硬件约束以及解决方案建议

<a name="table5984016192019"></a>
<table><thead align="left"><tr id="row49841016132015"><th class="cellrowborder" valign="top" width="13.608639136086392%" id="mcps1.2.4.1.1"><p id="p1498411692018"><a name="p1498411692018"></a><a name="p1498411692018"></a>分类</p>
</th>
<th class="cellrowborder" valign="top" width="33.406659334066596%" id="mcps1.2.4.1.2"><p id="p16984816162019"><a name="p16984816162019"></a><a name="p16984816162019"></a>硬件约束描述</p>
</th>
<th class="cellrowborder" valign="top" width="52.98470152984702%" id="mcps1.2.4.1.3"><p id="p681910154818"><a name="p681910154818"></a><a name="p681910154818"></a>解决方案建议</p>
</th>
</tr>
</thead>
<tbody><tr id="row1098410169204"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p73939428116"><a name="p73939428116"></a><a name="p73939428116"></a>内存访问（L0 Buffer/L1 Buffer/UB等）</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p339313421116"><a name="p339313421116"></a><a name="p339313421116"></a>各存储单元的最小访问粒度/地址对齐要求：</p>
<p id="p14393742910"><a name="p14393742910"></a><a name="p14393742910"></a>Unified Buffer：32Byte对齐。</p>
<p id="p1339384218119"><a name="p1339384218119"></a><a name="p1339384218119"></a>L1 Buffer：32Byte对齐。</p>
<p id="p03939425116"><a name="p03939425116"></a><a name="p03939425116"></a>L0A Buffer/L0B Buffer：512Byte对齐。</p>
<p id="p139316421914"><a name="p139316421914"></a><a name="p139316421914"></a>L0C Buffer：64Byte对齐。</p>
<p id="p1739319428114"><a name="p1739319428114"></a><a name="p1739319428114"></a>BiasTable Buffer：64Byte对齐。</p>
<p id="p13932421316"><a name="p13932421316"></a><a name="p13932421316"></a>Fixpipe Buffer：128Byte对齐。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><a name="ul9477161417"></a><a name="ul9477161417"></a><ul id="ul9477161417"><li>进行数据搬运时，需要感知对齐约束。</li><li>针对UB，遇到一些非对齐的场景，可以使用非对齐搬运的接口或者通过一些技巧（比如搬入时包含冗余数据，搬出时去除冗余数据）来解决。详情见<a href="../../../../算子实践参考/SIMD算子实现/矢量编程/非对齐场景.md">非对齐场景</a>。</li></ul>
</td>
</tr>
<tr id="row698415161206"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p153938429117"><a name="p153938429117"></a><a name="p153938429117"></a>内存访问（UB）</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p1393442615"><a name="p1393442615"></a><a name="p1393442615"></a>UB bank访问冲突（Vector计算访问/搬运访问）。</p>
<p id="p73934420118"><a name="p73934420118"></a><a name="p73934420118"></a></p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p123932421311"><a name="p123932421311"></a><a name="p123932421311"></a>需要按照芯片要求，在软件实现时错开处理的地址，从而解决bank冲突。具体解决方案可参考<a href="../../../../算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/avoid_bank_conflict_npu_arch_2201.md">避免UB的bank冲突</a>章节。</p>
</td>
</tr>
<tr id="row798416160209"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p5393154210115"><a name="p5393154210115"></a><a name="p5393154210115"></a>内存访问（GM）</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p11393174214120"><a name="p11393174214120"></a><a name="p11393174214120"></a>多核并行同地址访问GM，会被硬件串行化。</p>
<p id="p14393442116"><a name="p14393442116"></a><a name="p14393442116"></a></p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p9393104218116"><a name="p9393104218116"></a><a name="p9393104218116"></a>对相同地址的访问，会被硬件串行化，性能为排队时间，大约下降10%-20%；</p>
<p id="p539311424118"><a name="p539311424118"></a><a name="p539311424118"></a>多核访问通过错峰访问（调整数据访问顺序和修改切分策略等），使得第一次加载数据到L2 Cache，后续访问性能反而提升。具体解决方案可参考<a href="../../../../算子实践参考/SIMD算子性能优化/内存访问/避免同地址访问.md">避免同地址访问</a>章节。</p>
</td>
</tr>
<tr id="row189843164202"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1339364210115"><a name="p1339364210115"></a><a name="p1339364210115"></a>内存访问（GM）</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p153934427114"><a name="p153934427114"></a><a name="p153934427114"></a>单次搬运数据长度16KB以上时，可发挥带宽的最佳性能。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1839334215112"><a name="p1839334215112"></a><a name="p1839334215112"></a>根据实测经验，单次搬运数据长度16KB以上时，通常能较好地发挥出带宽的最佳性能。因此对于单次搬运，应考虑尽可能的搬运较大的数据块（不同芯片不一样）。</p>
<p id="p14280187181119"><a name="p14280187181119"></a><a name="p14280187181119"></a>具体解决方案可参考<a href="../../../../算子实践参考/SIMD算子性能优化/内存访问/尽量一次搬运较大的数据块.md">尽量一次搬运较大的数据块</a>章节。</p>
</td>
</tr>
<tr id="row10984111617206"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1139394216112"><a name="p1139394216112"></a><a name="p1139394216112"></a>内存访问（GM--&gt;L1）</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p139313427112"><a name="p139313427112"></a><a name="p139313427112"></a>DataCopy中源操作数相邻连续数据块的间隔（前面一个数据块的尾与后面数据块的头的间隔）不要超出65535，单位为DataBlock（32字节）。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p6393842612"><a name="p6393842612"></a><a name="p6393842612"></a>前面一个数据块的尾与后面数据块的头的间隔超出65535时，需要拆分成多条指令来实现。</p>
</td>
</tr>
<tr id="row28355376396"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1839313422120"><a name="p1839313422120"></a><a name="p1839313422120"></a>内存访问（GM）</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p439317429119"><a name="p439317429119"></a><a name="p439317429119"></a>数据搬运会被拆成128B/256B/512B不同长度进行搬运，非对齐会向上取整。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p133931242413"><a name="p133931242413"></a><a name="p133931242413"></a>Tiling尽量让搬运的内轴128B、256B、512B对齐。</p>
<p id="p4131102271514"><a name="p4131102271514"></a><a name="p4131102271514"></a>具体解决方案可参考<a href="../../../../算子实践参考/SIMD算子性能优化/内存访问/GM地址尽量512B对齐.md">GM地址尽量512B对齐</a>章节。</p>
</td>
</tr>
<tr id="row7725134123913"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1639310422014"><a name="p1639310422014"></a><a name="p1639310422014"></a>Cube</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p339311421712"><a name="p339311421712"></a><a name="p339311421712"></a>MTE1和MMAD指令队列的深度为32。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p113937420111"><a name="p113937420111"></a><a name="p113937420111"></a>对应指令队列容易满，会阻塞其他指令下发，引起流水断流。</p>
<p id="p18393742614"><a name="p18393742614"></a><a name="p18393742614"></a>LoadData（2D矩阵搬运）从L1 Buffer搬运到L0 Buffer，需要发射32条指令，LoadData（卷积数据搬运）只需要一条指令，建议使用LoadData（卷积数据搬运）来实现。</p>
</td>
</tr>
<tr id="row10203343193114"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1139319422115"><a name="p1139319422115"></a><a name="p1139319422115"></a>ICache</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p139319421513"><a name="p139319421513"></a><a name="p139319421513"></a>ICache硬件规格限制32KB。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p23931842016"><a name="p23931842016"></a><a name="p23931842016"></a>拆分Tiling_key或使用模板函数来减少代码段。详情见<a href="../../../高级编程/Aclnn算子工程化开发/设计与实现/多分支策略.md">多分支策略</a>。</p>
</td>
</tr>
<tr id="row194173017534"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1839310421316"><a name="p1839310421316"></a><a name="p1839310421316"></a>ICache</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p1139313428113"><a name="p1139313428113"></a><a name="p1139313428113"></a>多核并行同地址访问ICache，会被硬件串行化。</p>
<p id="p8393542217"><a name="p8393542217"></a><a name="p8393542217"></a></p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p639415421613"><a name="p639415421613"></a><a name="p639415421613"></a>小Shape场景尽量减少启动核数，减少多核同地址访问问题。</p>
</td>
</tr>
<tr id="row724416349531"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p4394342218"><a name="p4394342218"></a><a name="p4394342218"></a>DCache</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p11394134215110"><a name="p11394134215110"></a><a name="p11394134215110"></a>DCache硬件规格限制32KB</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p939416422113"><a name="p939416422113"></a><a name="p939416422113"></a>无</p>
</td>
</tr>
<tr id="row6730204410538"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p153942421414"><a name="p153942421414"></a><a name="p153942421414"></a>Scalar</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p73942421015"><a name="p73942421015"></a><a name="p73942421015"></a>标量写GM时，数据会被缓存在Dcache。硬件不保证DCache和GM一致性，需要用户保证。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1139411421118"><a name="p1139411421118"></a><a name="p1139411421118"></a>使用DataCacheCleanAndInvalid来保证一致性。</p>
</td>
</tr>
<tr id="row56187293595"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p113943421110"><a name="p113943421110"></a><a name="p113943421110"></a>Cube</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p439413421616"><a name="p439413421616"></a><a name="p439413421616"></a>L0C Buffer容量128KB。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p439454212111"><a name="p439454212111"></a><a name="p439454212111"></a>无</p>
</td>
</tr>
<tr id="row16184293594"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p939494215115"><a name="p939494215115"></a><a name="p939494215115"></a>Cube</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p12394194219112"><a name="p12394194219112"></a><a name="p12394194219112"></a>BiasTable Buffer为1KB。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1639410425117"><a name="p1639410425117"></a><a name="p1639410425117"></a>无</p>
</td>
</tr>
<tr id="row1361852935918"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p339410421114"><a name="p339410421114"></a><a name="p339410421114"></a>Cube</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p14651181022320"><a name="p14651181022320"></a><a name="p14651181022320"></a>Cube计算场景float算力为half算力的1/4。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p73946421415"><a name="p73946421415"></a><a name="p73946421415"></a>无</p>
</td>
</tr>
<tr id="row196181129135911"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p193948425111"><a name="p193948425111"></a><a name="p193948425111"></a>Cube</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p163941542119"><a name="p163941542119"></a><a name="p163941542119"></a>Cube输出随路量化场景，不支持int32_t到bfloat16_t类型量化。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><a name="ul7869116221"></a><a name="ul7869116221"></a><ul id="ul7869116221"><li>AIV上存在int32_t-&gt;float、float&gt;bfloat16_t的转换。</li><li>AIC支持float-&gt;bfloat16_t的随路量化。</li></ul>
</td>
</tr>
<tr id="row661982915597"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p0394442216"><a name="p0394442216"></a><a name="p0394442216"></a>Vector</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p339412428113"><a name="p339412428113"></a><a name="p339412428113"></a>Reduce接口half比float性能差。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p173946428117"><a name="p173946428117"></a><a name="p173946428117"></a>half写回UB时存在非32Byte对齐，导致性能劣化，建议把half转float计算，此场景建议使用float数据类型。</p>
</td>
</tr>
<tr id="row461911299592"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p14394142119"><a name="p14394142119"></a><a name="p14394142119"></a>Vector</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p93942421517"><a name="p93942421517"></a><a name="p93942421517"></a>Exp/Ln接口处理同样数量的half/float的耗时是一样的</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p1139412421411"><a name="p1139412421411"></a><a name="p1139412421411"></a>内部对float做了优化，故两者性能相当，开发者可以根据实际情况选择合适的精度类型。</p>
</td>
</tr>
<tr id="row13619162955918"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p039544211115"><a name="p039544211115"></a><a name="p039544211115"></a>流水同步（核内）</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p439594219111"><a name="p439594219111"></a><a name="p439594219111"></a>set/wait同步不匹配，状态会残留，影响后续算子。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p10395542819"><a name="p10395542819"></a><a name="p10395542819"></a>通过孪生调试/mssanitizer工具，提前识别此类问题。</p>
</td>
</tr>
<tr id="row96190298591"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p1395242514"><a name="p1395242514"></a><a name="p1395242514"></a>流水同步（核间）</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p12395184212116"><a name="p12395184212116"></a><a name="p12395184212116"></a>CrossCoreSetFlag计数器存在限制，超出15次需要做一次反向同步，否则会出现卡死。</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p11395342314"><a name="p11395342314"></a><a name="p11395342314"></a>通过孪生调试/mssanitizer工具，超出场景提前报错。</p>
</td>
</tr>
<tr id="row13620172910597"><td class="cellrowborder" valign="top" width="13.608639136086392%" headers="mcps1.2.4.1.1 "><p id="p139512427119"><a name="p139512427119"></a><a name="p139512427119"></a>API通用约束</p>
</td>
<td class="cellrowborder" valign="top" width="33.406659334066596%" headers="mcps1.2.4.1.2 "><p id="p1396164216116"><a name="p1396164216116"></a><a name="p1396164216116"></a>使用Ascend C API时，源操作数和目的操作数的地址重叠通用约束</p>
</td>
<td class="cellrowborder" valign="top" width="52.98470152984702%" headers="mcps1.2.4.1.3 "><p id="p143966421117"><a name="p143966421117"></a><a name="p143966421117"></a>使用基础API的Tensor高维切分计算接口时，为了节省地址空间，开发者可以定义一个Tensor，供源操作数与目的操作数同时使用（即地址重叠）。使用时需要注意以下约束：</p>
<a name="ul24282818269"></a><a name="ul24282818269"></a><ul id="ul24282818269"><li>单次迭代内：源操作数与目的操作数必须100%完全重叠，不支持部分重叠。</li><li>多次迭代间：不支持前序迭代的目的操作数与后序迭代的源操作数重叠。例如，第N次迭代的目的操作数是第N+1次的源操作数。在这种情况下，第N次迭代可能会改写覆盖源操作数的数值，导致无法得到预期结果。特别地，对于部分双目计算类的API（Add、Sub、Mul、Max、Min、AddRelu、SubRelu），当数据类型为half、int32_t、float时，支持前序迭代的目的操作数与后序迭代的源操作数重叠：仅针对目的操作数和第二个源操作数重叠的情况，且src1RepStride或者dstRepStride必须为0。</li></ul>
</td>
</tr>
</tbody>
</table>
