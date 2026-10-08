KP_DIR	?= $(CURDIR)/KernelPatch
NDK_VER	?= 30.0.16248370

ifeq ($(OS),Windows_NT)
	LLVM_ARCH = windows
	NDK_HOME ?= D:\android_sdk\ndk\$(NDK_VER)
else
	LLVM_ARCH = linux
	NDK_HOME ?= $(HOME)/android_sdk/ndk/$(NDK_VER)
endif

# CC := $(NDK_HOME)/toolchains/llvm/prebuilt/$(LLVM_ARCH)-x86_64/bin/clang --target=aarch64-linux-android34
CC := $(NDK_HOME)/toolchains/llvm/prebuilt/$(LLVM_ARCH)-x86_64/bin/clang --target=aarch64-linux-gnu
LD := $(NDK_HOME)/toolchains/llvm/prebuilt/$(LLVM_ARCH)-x86_64/bin/ld.lld

CFLAGS := -O2 -Wall -nostdinc -ffreestanding -fno-stack-protector -fno-pic -fno-pie -fno-common -mgeneral-regs-only
CFLAGS += -nostdlib -nostdlib++ -fno-strict-aliasing -fvisibility-inlines-hidden -fdata-sections -ffunction-sections

INC := -I$(KP_DIR)/kernel/include -I$(KP_DIR)/kernel/patch/include -I$(KP_DIR)/kernel/linux/include \
	-I$(KP_DIR)/kernel/linux/arch/arm64/include -I$(KP_DIR)/kernel/linux/tools/arch/arm64/include
	

############################################################################


TARGET	:= bye_neighbor.kpm
OBJ		:= bye_neighbor.o


all: $(TARGET)
	rm -f *.o
	# adb push bye_neighbor.kpm /sdcard/

$(TARGET): $(OBJ)
	$(LD) -r -nostdlib -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	rm -f *.o *.kpm

.PHONY: all clean