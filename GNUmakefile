ifndef MCU_SDK_TOP
$(error MCU_SDK_TOP must be set)
endif

# Build configuration (debug or release)
BUILD := debug

# Library files
LIBFREERTOS := libfreertos_$(BUILD).a
LIBLWIP := liblwip_$(BUILD).a
LIBDRIVER := libdriver_$(BUILD).a
LIBBOARD := libboard_$(BUILD).a
LIBSOLO5_FRT := libsolo5_frt_$(BUILD).a
ALL_LIBS := $(LIBFREERTOS) $(LIBLWIP) $(LIBDRIVER) $(LIBBOARD) \
            $(LIBSOLO5_FRT) 

all: $(ALL_LIBS)
	@echo Building: library files
	make -f Makefile.freertos lib
	make -f Makefile.lwip lib
	make -f Makefile.driver lib
	make -f Makefile.board lib
	make -f Makefile.solo5_frt lib

.PHONY: all $(ALL_LIBS) clean

.NOTPARALLEL:

# Commands
clean:
	@echo  Cleaning: library files
	make -f Makefile.freertos lib_clean
	make -f Makefile.lwip lib_clean
	make -f Makefile.driver lib_clean
	make -f Makefile.board lib_clean
	make -f Makefile.solo5_frt lib_clean
	rm -rf ./solo5-frt-common

