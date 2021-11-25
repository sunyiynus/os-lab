CC = clang
FLAG = -g -Wall -o2 -std=c11
DEBUGFLAG=-DDEBUG


pstree:pstree.c
	gcc ${FLAG} -o pstree pstree.c


.PHONY: test
test:
	$(CC) $(FLAG) $(DEBUGFLAG) -o test_pstree pstree.c
	./test_pstree

.PHONY: clean
clean:
	rm -rf *.o  test_pstree pstree

