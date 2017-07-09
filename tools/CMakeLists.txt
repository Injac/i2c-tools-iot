cmake_minimum_required(VERSION 2.8)

project(24C16_staticlib C)


add_library(24c16 SHARED i2cbusses.c  util.c lib24c16.c)

target_include_directories(24c16 PUBLIC ${PROJECT_SOURCE_DIR} ../include ../include/linux)

#target_include_directories(24c16
 # PUBLIC
  #  # Headers used from source/build location:
   # "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    # "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/linux>"
    #"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    # Headers used from installed location:
    #"$<INSTALL_INTERFACE:include>"     
#)

target_link_libraries(24c16 stdc++)

install(TARGETS 24c16 DESTINATION lib)
install(FILES ../include/lib24c16.h i2cbusses.h ../include/linux/i2c-dev.h util.h ../version.h DESTINATION include)