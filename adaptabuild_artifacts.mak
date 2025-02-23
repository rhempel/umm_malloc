# ----------------------------------------------------------------------------
# adaptabuild_artifacts.mak - product specific libraries file
#
# Here is where you specify the libraries or other artifacts your product
# needs to have built.
# ----------------------------------------------------------------------------

ifeq (host,$(MCU))
    # Do nothing - we want the standard library for host builds
else
    # Do nothing
endif

include $(SRC_PATH)/adaptabuild_module.mak
