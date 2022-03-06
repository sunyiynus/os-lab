#### 南大 OS设计与实现课后作业完成
-------
1. pstree
Memory leak check: No leak.
```shell
==2188675== HEAP SUMMARY:
==2188675==     in use at exit: 0 bytes in 0 blocks
==2188675==   total heap usage: 4,273 allocs, 4,273 frees, 314,960 bytes allocated
==2188675== 
==2188675== All heap blocks were freed -- no leaks are possible
```
Perfermance measure:

Improvement Place:
Using assert() to keep away from uinitial vairable
Using oop (is shooting small with big gun)
Adding error, warning handle and signal handle
Print like real pstree




