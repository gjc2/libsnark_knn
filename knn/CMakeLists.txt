set(CMAKE_CXX_STANDARD 11)

add_definitions(-DCURVE=BN128)



add_executable(knn main.cpp)
configure_file(dataset.txt ${CMAKE_CURRENT_BINARY_DIR}/dataset.txt COPYONLY)


include_directories(/usr/local/include)
include_directories(../libsnark)
include_directories(../libsnark/depends/libfqfft/)
include_directories(../libsnark/depends/)
link_directories(/usr/local/lib)

target_link_libraries(knn snark gtest ff gmpxx gmp procps)

