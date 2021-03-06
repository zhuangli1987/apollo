
set(FASTRTPS_ROOT "${CMAKE_SOURCE_DIR}/third_party/fast-rtps")
if(EXISTS ${FASTRTPS_ROOT})
    set(FASTRTPS_INCLUDE_DIR "${FASTRTPS_ROOT}/include")
    set(FASTRTPS_LINK_DIR "${FASTRTPS_ROOT}/lib")
    file(GLOB FASTRTPS_LIBRARIES "${FASTRTPS_LINK_DIR}/libfastrtps.so")
    include_directories(${FASTRTPS_INCLUDE_DIR})
    link_directories(${FASTRTPS_LINK_DIR})
else()
    message("fast-rtps dir not exist")
endif()
