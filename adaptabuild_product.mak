# ----------------------------------------------------------------------------
# adaptabuild_product.mak - product specific include file
#
# Here is where you specify your product options
# ----------------------------------------------------------------------------

# The umm_malloc library can be build with a number of different options, and
# we control the build by setting the UMM_MALLOC_CFGFILE on the command line
#
# The following configurations can be built as separate products, each one has
# a corresponding .h file
#
# default
# enable_critical_depth_check
# enable_info
# enable_inline_metrics
# enable_integrity_checks
# enable_poison_check
# enable_first_fit
#
PRODUCT_LIST := default \
                enable_info \
                enable_critical_depth_check \
                enable_inline_metrics \
                enable_integrity_check \
                enable_poison_check \
                enable_first_fit  

ifneq ($(filter $(PRODUCT),$(PRODUCT_LIST)),)
    # Set PRODUCT_MAIN here based on $(PRODUCT)
    # The unittest product typically does not need PRODUCT_MAIN
    ifeq (default,$(PRODUCT))
        UMM_MALLOC_CFGFILE := \"options/default.h\"
    else ifeq (enable_info,$(PRODUCT))
        UMM_MALLOC_CFGFILE := \"options/enable_umm_info.h\"
    else ifeq (enable_critical_depth_check,$(PRODUCT))
        UMM_MALLOC_CFGFILE := \"options/enable_critical_depth_check.h\"
    else ifeq (enable_inline_metrics,$(PRODUCT))
        UMM_MALLOC_CFGFILE := \"options/enable_inline_metrics.h\"
    else ifeq (enable_integrity_check,$(PRODUCT))
        UMM_MALLOC_CFGFILE := \"options/enable_integrity_check.h\"
    else ifeq (enable_poison_check,$(PRODUCT))
        UMM_MALLOC_CFGFILE := \"options/enable_poison_check.h\"
    else ifeq (enable_first_fit,$(PRODUCT))
        UMM_MALLOC_CFGFILE := \"options/enable_first_fit.h\"
    endif
else
  $(error PRODUCT must be one of $(PRODUCT_LIST))
endif


