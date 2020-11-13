all: prog2

prog2: prog2.c prog2.h
	gcc -Wall -Wextra -g prog2.c -o prog2
	gcc -Wall -Wextra -g prog2_rdt.c -o prog2_rdt
	gcc -Wall -Wextra -g prog2_sr.c -o prog2_sr
# 	gcc -Wall -Wextra -g test.c -o test

clean:
	rm -f prog2 *~ *.swp
	rm -f prog2_rdt *~ *.swp
	rm -f prog2_sr *~ *.swp
# 	rm -f test *~ *.swp
