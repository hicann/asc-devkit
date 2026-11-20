# Aligning GM Addresses to 512B Wherever Possible<a name="ZH-CN_TOPIC_0000001892919369"></a>

<!-- md-trans-meta sourceCommit=6077752645c837632b476e9dd86a739eeb461024 translatedAt=2026-11-09T10:35:52.022Z pushedAt=2026-11-19T06:32:19.773Z nl=2 -->

[Priority] High

[Description] Due to the internal design constraints of AI processors, when copying data from GM to Local Memory, ensuring 512-byte alignment of GM addresses can maximize bandwidth efficiency. The following figures show the single-core bandwidth efficiency under 512-byte alignment and 32-byte alignment: when copying the same amount of data, in the case of the largest bandwidth difference, the 32-byte alignment scenario can only achieve 70% of the 512-byte alignment scenario.

> [!NOTE] Note 
> This performance optimization method applies to the following product models:
> <!-- npu="950" id1 -->
> - Ascend 950PR&950DT products
> <!-- end id1 -->
> <!-- npu="910b" id2 -->
> - Atlas A2 products
> <!-- end id2 -->
> - The test data is related to the processor model, and slight fluctuations may occur during actual testing. The specific bandwidth values are not necessarily strictly consistent with the test data below.

**Figure 1**  GM-\>UB comparison of the measured bandwidth difference between 512-byte alignment and 32-byte alignment in the direction<a name="fig5376636154310"></a><br>
![](../../../figures/gm_ub_align.png "Comparison of the measured bandwidth difference between 512-byte alignment and 32-byte alignment in the GM--UB direction")

**Figure 2**  UB-\>GM comparison of the measured bandwidth difference between 512-byte alignment and 32-byte alignment in the direction<a name="fig13744224715"></a>  
![](../../../figures/ub_gm_align.png "Comparison of the measured bandwidth difference between 512-byte alignment and 32-byte alignment in the UB--GM direction")
