# Makefile to build all subdirs

all: basic_t char_dev_t blk_dev_t proc_t time_t \
		ds_t mm_t fs_t

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

blk_dev_t:
	@echo
	@echo ==================== Build block device directory ====================
	@echo

	make -C blk_dev

	@echo
	@echo ==================== Build Complete ====================
	@echo

proc_t:
	@echo
	@echo ==================== Build procfs directory ====================
	@echo

	make -C proc

	@echo
	@echo ==================== Build Complete ====================
	@echo

time_t:
	@echo
	@echo ==================== Build timer device directory ====================
	@echo

	make -C time

	@echo
	@echo ==================== Build Complete ====================
	@echo

mm_t:
	@echo
	@echo ==================== Build memory management directory ====================
	@echo

	make -C mm

	@echo
	@echo ==================== Build Complete ====================
	@echo

ds_t:
	@echo
	@echo ==================== Build data structure directory ====================
	@echo

	make -C ds

	@echo
	@echo ==================== Build Complete ====================
	@echo

fs_t:
	@echo
	@echo ==================== Build filesystem directory ====================
	@echo

	make -C fs

	@echo
	@echo ==================== Build Complete ====================
	@echo




clean:
	make -C basic clean
	make -C char_dev clean
	make -C blk_dev clean
	make -C proc clean
	make -C time clean
	make -C mm clean
	make -C ds clean
	make -C fs clean
