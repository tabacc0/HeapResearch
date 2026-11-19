V1-PoC : v1.c ld-linux-x86-64.so.2 libc.so.6 
	gcc -g ./v1.c -o V1-PoC
	patchelf V1-PoC --set-interpreter ./ld-linux-x86-64.so.2 --set-rpath .;

V2-PoC : v2.c ld-linux-x86-64.so.2 libc.so.6
	gcc -g ./v2.c -o V2-PoC
	patchelf V2-PoC --set-interpreter ./ld-linux-x86-64.so.2 --set-rpath .;
Leak-Poc : leak.c ld-linux-x86-64.so.2 libc.so.6
	gcc -g ./leak.c -o Leak-PoC
	patchelf Leak-PoC --set-interpreter ./ld-linux-x86-64.so.2 --set-rpath .;
Expand-PoC : expand.c ld-linux-x86-64.so.2 libc.so.6
	gcc -g ./expand.c -o Expand-PoC
	patchelf Expand-PoC --set-interpreter ./ld-linux-x86-64.so.2\
		--set-rpath .;
all : expand.c leak.c v1.c v2.c ld-linux-x86-64.so.2 libc.so.6
	gcc -g ./v1.c -o V1-PoC ;
	gcc -g ./expand.c -o Expand-PoC;
	gcc -g ./v2.c -o V2-PoC;
	gcc -g ./leak.c -o Leak-PoC;
	patchelf V1-PoC --set-interpreter ./ld-linux-x86-64.so.2 --set-rpath .;
	patchelf V2-PoC --set-interpreter ./ld-linux-x86-64.so.2 --set-rpath .;
	patchelf Leak-PoC --set-interpreter ./ld-linux-x86-64.so.2 \
		--set-rpath .;
	patchelf Expand-PoC --set-interpreter ./ld-linux-x86-64.so.2\
		--set-rpath .;
clean :
	rm -f V1-PoC V2-PoC Expand-PoC Leak-PoC 2> /dev/null
