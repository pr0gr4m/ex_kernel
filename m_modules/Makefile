# Makefile to build all subdirs

all: basic char_dev

basic:
	@echo
	@echo ==================== Build basic directory ====================
	@echo

	make -C basic

	@echo
	@echo ==================== Build Complete ====================
	@echo

kernel32:
	@echo
	@echo ==================== Build character device directory ====================
	@echo

	make -C char_dev

	@echo
	@echo ==================== Build Complete ====================
	@echo

clean:
	make -C basic clean
	make -C char_dev clean
