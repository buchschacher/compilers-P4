LOAD 0
STORE x
LOAD 5
STORE y
L0: NOOP
LOAD y
STORE T0
LOAD x
SUB T0
BRZPOS L1
LOAD x
STORE OUT
WRITE OUT
LOAD 1
STORE T1
LOAD x
ADD T1
STORE x
BR L0
L1: NOOP
STOP
OUT 0
T0 0
T1 0
