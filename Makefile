
# Main Makefile

# Detect the platform and set the PLATFORM variable
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM := linux
    endif
    ifeq ($(UNAME_S),Darwin)
        PLATFORM := darwin
    endif
endif

# Include the platform-specific Makefile
include Makefile.$(PLATFORM)

