KP_DIR	?= $(CURDIR)/KernelPatch
NDK_VER	?= 30.0.16248370

ifeq ($(OS),Windows_NT)
	LLVM_ARCH = windows
	NDK_HOME ?= D:\android_sdk\ndk\$(NDK_VER)
else
	LLVM_ARCH = linux
	NDK_HOME ?= $(HOME)/android_sdk/ndk/$(NDK_VER)
endif

LLVM_PATH := $(NDK_HOME)/toolchains/llvm/prebuilt/$(LLVM_ARCH)-x86_64
CXX := $(LLVM_PATH)/bin/clang++ --target=aarch64-linux-android34
LD := $(LLVM_PATH)/bin/ld.lld

CXXFLAGS := -O2 -Wall -fno-strict-aliasing -fvisibility-inlines-hidden -fdata-sections -ffunction-sections -fno-stack-protector -fPIC -static-libstdc++


############################################################################


DUCK_DETECTOR_URL	:= https://raw.githubusercontent.com/eltavine/Duck-Detector-Refactoring/main/feature/nativeroot/data/src/main/cpp/nativeroot
DUCK_DETECTOR_FILES	:= common/types.h probes/permission_boundary_netlink.h probes/permission_boundary_probe.h \
	probes/permission_boundary_link.cpp probes/permission_boundary_neigh.cpp probes/permission_boundary_netlink.cpp
DUCK_SRC_FILES 		:= $(addprefix nativeroot/, $(filter %.c %.cpp, $(DUCK_DETECTOR_FILES)))


all: clean download_duck make_neighbor check_neighbor
no_check: clean download_duck make_neighbor

download_duck:
	@echo "-------- Downloading Duck Detector (eltavine) sources"
ifeq ($(OS),Windows_NT)
	@mkdir nativeroot nativeroot\common nativeroot\probes
else
	@mkdir -p nativeroot nativeroot/common nativeroot/probes
endif
	@for file in $(DUCK_DETECTOR_FILES); do \
		echo "---> downloading: $$file"; curl -sSL "$(DUCK_DETECTOR_URL)/$$file" -o "nativeroot/$$file"; \
	done
	@echo -e "\n"

make_neighbor:
	@echo "-------- Building AF_NETLINK Neighbor Leak checker"
	$(CXX) $(CXXFLAGS) $(DUCK_SRC_FILES) \
	-I. -I$(LLVM_PATH)/sysroot/usr/include -I$(LLVM_PATH)/sysroot/usr/include/c++/v1 -I$(LLVM_PATH)/sysroot/usr/include/aarch64-linux-android \
	test_neighbor.cpp -o test_neighbor
	@echo -e "\n"

check_neighbor:
	@echo "-------- Pushing 'test_neighbor' to device & running it"
	@MSYS_NO_PATHCONV=1 adb push test_neighbor /data/local/tmp/
	@MSYS_NO_PATHCONV=1 adb shell "chmod +x /data/local/tmp/test_neighbor"
	@echo -e "\n\n\n"
	@echo "-------- /data/local/tmp/test_neighbor output"
	MSYS_NO_PATHCONV=1 adb shell "/data/local/tmp/test_neighbor"

clean:
	@echo "-------- Clean up"
	rm -rf test_neighbor nativeroot
	MSYS_NO_PATHCONV=1 adb shell "rm /data/local/tmp/test_neighbor"
	@echo -e "\n"

.PHONY: clean all no_check download_duck make_neighbor check_neighbor