# ----------------------------------------------------------------------------
# adaptabuild_product.mak - product specific include file
#
# Here is where you specify your product options
# ----------------------------------------------------------------------------

PRODUCT_LIST := unittest  

ifneq ($(filter $(PRODUCT),$(PRODUCT_LIST)),)
  # Set PRODUCT_MAIN here based on $(PRODUCT)
  # The unittest product typically does not need PRODUCT_MAIN
else
  $(error PRODUCT must be one of $(PRODUCT_LIST))
endif


