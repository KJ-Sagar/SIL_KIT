# CMake generated Testfile for 
# Source directory: /home/kj/CES_2025_artifacts/sil-kit/SilKit/source/util
# Build directory: /home/kj/CES_2025_artifacts/sil-kit/build/SilKit/source/util
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Test_LabelMatching "/home/kj/CES_2025_artifacts/sil-kit/build/Release/SilKitUnitTests" "--gtest_output=xml:Test_LabelMatching_gtestresults.xml" "--gtest_filter=Test_LabelMatching.*")
set_tests_properties(Test_LabelMatching PROPERTIES  WORKING_DIRECTORY "/home/kj/CES_2025_artifacts/sil-kit/build/Release" _BACKTRACE_TRIPLES "/home/kj/CES_2025_artifacts/sil-kit/SilKit/cmake/SilKitTest.cmake;111;add_test;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/util/CMakeLists.txt;120;add_silkit_test_to_executable;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/util/CMakeLists.txt;0;")
subdirs("tests")
