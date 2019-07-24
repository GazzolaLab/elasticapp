shfmt:
	@shfmt --version
	@find . -maxdepth 3 -name '*.sh'\
		| while read -r src; do shfmt -w -p "$$src"; done

shcheck:
	@shellcheck --version
	@find . -maxdepth 3 -name '*.sh'\
		| while read -r src; do shellcheck "$$src"; done

cmakefmt:
	@cmake-format --version
	@find . \( -name "CMakeLists.txt" -o -name "*.cmake" \) -type f\
		| while read -r src; do cmake-format -i "$$src"; done

ci: shcheck

fmt:shfmt cmakefmt

all:fmt ci
