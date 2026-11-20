# Overview of Data Move-Out for Cube Computation

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-10-27T03:00:41.979Z pushedAt=2026-10-27T13:03:35.340Z nl=2 -->

The data move-out APIs for cube computation mainly implement efficient movement between the L0C Buffer and Global Memory or the Unified Buffer (UB).

For Ascend 950PR&950DT products:

**Figure 1** Overall flowchart of data move-out for cube computation

![Overall flowchart of data move-out for cube computation](../../../../figures/fixpipe-20.png)

**Table 1** Movement paths and storage hierarchy

|Source Location|Source Address Alignment Requirement|Destination Location|Destination Address Alignment Requirement|Format Conversion|Typical Application Scenario|
|--------|--------|--------|--------|--------|--------|
|L0C Buffer|64 bytes|Global Memory|1 byte/32 bytes|NZ2ND, NZ2DN, NZ2NZ|Move data out of the L0C Buffer to Global Memory.|
|L0C Buffer|64 bytes|UB|32 bytes|NZ2ND, NZ2DN, NZ2NZ|Move data out of the L0C Buffer to the UB.|
