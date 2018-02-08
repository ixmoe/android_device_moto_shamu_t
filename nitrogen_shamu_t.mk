#
# Copyright 2018 Nitrogen Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Boot animation
TARGET_SCREEN_HEIGHT := 2560
TARGET_SCREEN_WIDTH := 1440

# Inherit some common Nitrogen stuff.
$(call inherit-product, vendor/nitrogen/products/common.mk)

# Inherit device configuration
$(call inherit-product, device/moto/shamu_t/aosp_shamu.mk)

## Device identifier. This must come after all inclusions
PRODUCT_DEVICE := shamu_t
PRODUCT_NAME := nitrogen_shamu_t
PRODUCT_BRAND := motorola
PRODUCT_MODEL := Moto X Pro

TARGET_VENDOR := motorola

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_NAME=shamu_t \
    BUILD_FINGERPRINT=motorola/shamu_t/shamu_t:7.1.1/N6F27M/4299435:user/release-keys \
    PRIVATE_BUILD_DESC="shamu_t-user 7.1.1 N6F27M 4299435 release-keys"
