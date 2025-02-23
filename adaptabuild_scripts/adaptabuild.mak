# ----------------------------------------------------------------------------
# Do NOT move these functions - they must live in the top level makefile
#
ABS_PATH := $(patsubst %/,%,$(dir $(realpath $(firstword $(MAKEFILE_LIST)))))
$(call log_info,ABS_PATH is $(ABS_PATH))

ROOT_PATH := $(patsubst %/,%,$(dir $(firstword $(MAKEFILE_LIST))))
$(call log_info,ROOT_PATH is $(ROOT_PATH))

# The adaptabuild path MUST be at the root level
#
ADAPTABUILD_PATH := $(ROOT_PATH)/adaptabuild

LOG_ERROR ?=
LOG_WARNING ?=
LOG_NOTICE ?= x
LOG_INFO ?=
LOG_DEBUG ?= x

# Add any top level definitions for adaptabuild here:
#
GCC_VERSION := 10.3.1

include $(ADAPTABUILD_PATH)/make/adaptabuild.mak
