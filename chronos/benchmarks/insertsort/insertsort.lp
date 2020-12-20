Maximize

52 dSta_0 + 8 d0_1 + 2 d1_2 + 5 d1_4 + 16 d2_3 + 5 d3_4 + 16 d3_3 + 2 d4_5 + 7 d4_1


Subject to


\ === tcfg constraints ===
dSta_0 = 1
b0 - d0_1 = 0
b0 - dSta_0 = 0
b1 - d1_2 - d1_4 = 0
b1 - d0_1 - d4_1 = 0
b2 - d2_3 = 0
b2 - d1_2 = 0
b3 - d3_4 - d3_3 = 0
b3 - d2_3 - d3_3 = 0
b4 - d4_5 - d4_1 = 0
b4 - d1_4 - d3_4 = 0
b5 - d4_5 = 0


\ === infeasible path constraints ===

b0 = 1
b1 = 9
b4 = 9
b5 = 1
b3- 10 b2 =  0

Generals

Sta
b0
d0_1
b1
d1_2
d1_4
b2
d2_3
b3
d3_4
d3_3
b4
d4_5
d4_1
b5

End
\ total_cons: 17 	total_vars: 15
