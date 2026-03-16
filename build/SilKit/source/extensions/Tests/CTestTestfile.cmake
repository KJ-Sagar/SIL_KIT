# CMake generated Testfile for 
# Source directory: /home/kj/CES_2025_artifacts/sil-kit/SilKit/source/extensions/Tests
# Build directory: /home/kj/CES_2025_artifacts/sil-kit/build/SilKit/source/extensions/Tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Test_SilKitExtensions "/home/kj/CES_2025_artifacts/sil-kit/build/Release/SilKitUnitTests" "--gtest_output=xml:Test_SilKitExtensions_gtestresults.xml" "--gtest_filter=Test_SilKitExtensions.*")
set_tests_properties(Test_SilKitExtensions PROPERTIES  WORKING_DIRECTORY "/home/kj/CES_2025_artifacts/sil-kit/build/Release" _BACKTRACE_TRIPLES "/home/kj/CES_2025_artifacts/sil-kit/SilKit/cmake/SilKitTest.cmake;111;add_test;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/extensions/Tests/CMakeLists.txt;38;add_silkit_test_to_executable;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/extensions/Tests/CMakeLists.txt;0;")
