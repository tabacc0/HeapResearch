V1-PoC : v1.c ../house-of-smoke-legacy-builder.tar
	docker load -i ../house-of-smoke-legacy-builder.tar
	trap 'docker image rm house-of-smoke-legacy-builder:latest >/dev/null \
		2>&1 || true' EXIT; \
	docker run --rm \
		--user "$$(id -u):$$(id -g)" \
		-v .:/work \
		-w /work \
		house-of-smoke-legacy-builder:latest\
		gcc -g ./v1.c -o V1-PoC
	patchelf V1-PoC --set-interpreter ./ld-linux-x86-64.so.2 --set-rpath .;

V2-PoC : v2.c ../house-of-smoke-legacy-builder.tar
	docker load -i ../house-of-smoke-legacy-builder.tar
	trap 'docker image rm house-of-smoke-legacy-builder:latest >/dev/null \
		2>&1 || true' EXIT; \
	docker run --rm \
		--user "$$(id -u):$$(id -g)" \
		-v .:/work \
		-w /work \
		house-of-smoke-legacy-builder:latest\
		gcc -g ./v2.c -o V2-PoC
	patchelf V2-PoC --set-interpreter ./ld-linux-x86-64.so.2 --set-rpath .;
Leak-Poc : leak.c ../house-of-smoke-legacy-builder.tar
	docker load -i ../house-of-smoke-legacy-builder.tar
	trap 'docker image rm house-of-smoke-legacy-builder:latest >/dev/null \
		2>&1 || true' EXIT; \
	docker run --rm \
		--user "$$(id -u):$$(id -g)" \
		-v .:/work \
		-w /work \
		house-of-smoke-legacy-builder:latest\
		gcc -g ./leak.c -o Leak-PoC
	patchelf Leak-PoC --set-interpreter ./ld-linux-x86-64.so.2 --set-rpath .;
Expand-PoC : expand.c ../house-of-smoke-legacy-builder.tar
	docker load -i ../house-of-smoke-legacy-builder.tar
	trap 'docker image rm house-of-smoke-legacy-builder:latest >/dev/null \
		2>&1 || true' EXIT; \
	docker run --rm \
		--user "$$(id -u):$$(id -g)" \
		-v .:/work \
		-w /work \
		house-of-smoke-legacy-builder:latest\
		gcc -g ./expand.c -o Expand-PoC
	patchelf Expand-PoC --set-interpreter ./ld-linux-x86-64.so.2\
		--set-rpath .;
clean :
	rm -f V1-PoC V2-PoC Expand-PoC Leak-PoC 2> /dev/null
