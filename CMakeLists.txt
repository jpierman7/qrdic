cmake_minimum_required(VERSION 3.0) 

PROJECT (genqr)

find_package(OpenCV REQUIRED)
#find_package (zbar REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS}  zbar)


SET(OpenCV_LIBRARIES opencv_core opencv_highgui opencv_imgproc opencv_xfeatures2d)

set (CMAKE_CXX_STANDARD 11)
add_executable(genqr genqr.cpp)
add_executable(decode detectqr.cpp)

option(BUILD_SHARED_LIBS "Set ON to build shared library" ON)
set (CMAKE_CXX_STANDARD 11)
set (QR_LIB ~/Desktop/DICID/QR-Code-generator/ )
set(CPPSOURCES ${QR_LIB}/cpp/BitBuffer.cpp ${QR_LIB}/cpp/QrCode.cpp ${QR_LIB}/cpp/QrSegment.cpp)
add_library(qrcodegen-cpp ${CPPSOURCES})

target_include_directories(qrcodegen-cpp PRIVATE ${QR_LIB}/cpp)
set_target_properties(qrcodegen-cpp PROPERTIES OUTPUT_NAME qrcodegen PREFIX "")

#install(TARGETS qrcodegen-cpp RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)


target_link_libraries( genqr ${OpenCV_LIBS} ${OpenCV_LIBRARIES} -lzbar qrcodegen-cpp)
target_link_libraries( decode ${OpenCV_LIBS} ${OpenCV_LIBRARIES} -lzbar qrcodegen-cpp)