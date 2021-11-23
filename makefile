CC = clang
FLAG = -g -Wall -std=c11
DEBUGFLAG=-DDEBUG


pstree:pstree_testing.c
	gcc ${FLAG} -o pstree pstree_testing.c


.PHONY: test
test:
	$(CC) $(FLAG) $(DEBUGFLAG) -o test_pstree pstree_testing.c
	./test_pstree

.PHONY: clean
clean:
	rm -rf *.o  test_pstree pstree

