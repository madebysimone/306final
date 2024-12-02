# Project Name
TARGET = MIDIController

# Sources
CPP_SOURCES = MIDIController.cpp
# CPP_SOURCES = buttonTest.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy
DAISYSP_DIR = ../../DaisySP

# Core location, and generic makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

