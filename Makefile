.PHONY: clean

zzz: src/zzz.c src/binload.c src/llist.c
	gcc -I./include src/binload.c src/llist.c -lbfd -lcapstone $< -o $@

clean:
	rm -fv zzz
