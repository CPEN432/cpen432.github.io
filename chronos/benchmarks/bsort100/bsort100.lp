Maximize

9 dSta_0 + 7 d0_1 + 5 d1_2 + 7 d1_1 + 5 d2_3 + 2 d3_4 + 6 d4_5 + 1 d4_8 + 4 d5_6 + 5 d5_7 + 6 d6_7 + 1 d7_8 + 2 d7_4 + 3 d8_9 + 2 d8_10 + 2 d9_10 + 5 d9_3


Subject to


\ === tcfg constraints ===
dSta_0 = 1
b0 - d0_1 = 0
b0 - dSta_0 = 0
b1 - d1_2 - d1_1 = 0
b1 - d0_1 - d1_1 = 0
b2 - d2_3 = 0
b2 - d1_2 = 0
b3 - d3_4 = 0
b3 - d2_3 - d9_3 = 0
b4 - d4_5 - d4_8 = 0
b4 - d3_4 - d7_4 = 0
b5 - d5_6 - d5_7 = 0
b5 - d4_5 = 0
b6 - d6_7 = 0
b6 - d5_6 = 0
b7 - d7_8 - d7_4 = 0
b7 - d5_7 - d6_7 = 0
b8 - d8_9 - d8_10 = 0
b8 - d4_8 - d7_8 = 0
b9 - d9_10 - d9_3 = 0
b9 - d8_9 = 0
b10 - d8_10 - d9_10 = 0


\ === infeasible path constraints ===

b0 = 1
b1 = 1024
b2 = 1
b3 = 1023
b4 = 524798
b5 = 523776
b7 = 523776
b8 = 1023
b9 = 1023
b10 = 1

Generals

Sta
b0
d0_1
b1
d1_2
d1_1
b2
d2_3
b3
d3_4
b4
d4_5
d4_8
b5
d5_6
d5_7
b6
d6_7
b7
d7_8
d7_4
b8
d8_9
d8_10
b9
d9_10
d9_3
b10

End
\ total_cons: 32 	total_vars: 28
