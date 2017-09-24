# Boot animation
TARGET_SCREEN_HEIGHT := 2560
TARGET_SCREEN_WIDTH := 1440

# Inherit some common LineageOS stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)

# Inherit device configuration
$(call inherit-product, device/moto/shamu_t/aosp_shamu.mk)

DEVICE_PACKAGE_OVERLAYS += device/moto/shamu_t/overlay-cm

## Device identifier. This must come after all inclusions
PRODUCT_NAME := lineage_shamu_t
PRODUCT_BRAND := motorola
PRODUCT_MODEL := Moto X Pro

TARGET_VENDOR := motorola

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_NAME=shamu_t \
    BUILD_FINGERPRINT=motorola/shamu_t/shamu_t:7.1.1/NGI55D/4298276:user/release-keys \
    PRIVATE_BUILD_DESC="shamu_t-user 7.1.1 NGI55D 4298276 release-keys"
