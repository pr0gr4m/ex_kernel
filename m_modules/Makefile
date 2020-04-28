# Makefile to build all subdirs

all: basic_t char_dev_t

basic_t:
	@echo
	@echo ==================== Build basic directory ====================
	@echo

	make -C basic

	@echo
	@echo ==================== Build Complete ====================
	@echo

char_dev_t:
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
