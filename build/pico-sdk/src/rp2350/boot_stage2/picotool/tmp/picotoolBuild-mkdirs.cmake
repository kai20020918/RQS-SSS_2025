# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/pico2/RQS-SSS_2025/build/_deps/picotool-src"
  "/home/pico2/RQS-SSS_2025/build/_deps/picotool-build"
  "/home/pico2/RQS-SSS_2025/build/_deps"
  "/home/pico2/RQS-SSS_2025/build/pico-sdk/src/rp2350/boot_stage2/picotool/tmp"
  "/home/pico2/RQS-SSS_2025/build/pico-sdk/src/rp2350/boot_stage2/picotool/src/picotoolBuild-stamp"
  "/home/pico2/RQS-SSS_2025/build/pico-sdk/src/rp2350/boot_stage2/picotool/src"
  "/home/pico2/RQS-SSS_2025/build/pico-sdk/src/rp2350/boot_stage2/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/pico2/RQS-SSS_2025/build/pico-sdk/src/rp2350/boot_stage2/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/pico2/RQS-SSS_2025/build/pico-sdk/src/rp2350/boot_stage2/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
