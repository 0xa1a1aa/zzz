.PHONY: clean

binload_demo: binload_demo.c src/binload.c src/llist.c
	gcc -I./include src/binload.c src/llist.c -lbfd $< -o $@

clean:
	rm -fv binload_demo
