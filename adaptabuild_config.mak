# ----------------------------------------------------------------------------
# Do NOT move these functions - they must live in the top level makefile
#
# ROOT_PATH is the path to project relative to the directory that you called
# this makefile from. The adaptabuild system need to know this so that it can
# include the files it needs.
#
ROOT_PATH := $(dir $(firstword $(MAKEFILE_LIST)))

#ROOT_PATH := $(patsubst %/,%,$(dir $(firstword $(MAKEFILE_LIST))))

# The adaptabuild path MUST be at the root level - unfortunately there is
# currently no obvious (to me) way to move this boilerplate into an include file.
#
ADAPTABUILD_PATH := $(ROOT_PATH)/adaptabuild

# ----------------------------------------------------------------------------
# Now that we have ADAPTABUILD_PATH set we can import the log utilities
# 
LOG_WARNING ?= x
LOG_NOTICE ?= x
LOG_INFO ?= x
LOG_DEBUG ?= x

include $(ADAPTABUILD_PATH)/make/log.mak

# There can be only one top level source directory - all of the artifacts to
# be built with adaptabuild must live under this directory! 
#
# SRC_PATH is always specified relative to the ROOT_PATH. It doesn't
# really matter how complex the path is, the adaptabuild system will
# normalize it relative to the ROOT_PATH internally. 
#
# For example, if you have organized all the artifact source code under a
# directory called "src" you would write:
#
# SRC_PATH := $(ROOT_PATH)/src
#
# If the artifacts at the same level as the ROOT_PATH, the following values
# can be used and are equivalent:
#
# SRC_PATH := $(ROOT_PATH)
# SRC_PATH := $(ROOT_PATH)/
# SRC_PATH := $(ROOT_PATH)/.
#
SRC_PATH := $(ROOT_PATH)/.

# ---------------------------------------------------------------------------
# Do NOT move this include - it MUST be before the definition of MCU_MAK
#                            and after the definition of SRC_PATH
#
include $(ADAPTABUILD_PATH)/make/adaptabuild.mak
