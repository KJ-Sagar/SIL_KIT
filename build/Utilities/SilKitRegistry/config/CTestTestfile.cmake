# CMake generated Testfile for 
# Source directory: /home/kj/CES_2025_artifacts/sil-kit/Utilities/SilKitRegistry/config
# Build directory: /home/kj/CES_2025_artifacts/sil-kit/build/Utilities/SilKitRegistry/config
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Test_RegistryConfiguration "/home/kj/CES_2025_artifacts/sil-kit/build/Release/SilKitRegistryTests" "--gtest_output=xml:Test_RegistryConfiguration_gtestresults.xml" "--gtest_filter=Test_RegistryConfiguration.*")
set_tests_properties(Test_RegistryConfiguration PROPERTIES  WORKING_DIRECTORY "/home/kj/CES_2025_artifacts/sil-kit/build/Release" _BACKTRACE_TRIPLES "/home/kj/CES_2025_artifacts/sil-kit/SilKit/cmake/SilKitTest.cmake;111;add_test;/home/kj/CES_2025_artifacts/sil-kit/Utilities/SilKitRegistry/config/CMakeLists.txt;43;add_silkit_test_to_executable;/home/kj/CES_2025_artifacts/sil-kit/Utilities/SilKitRegistry/config/CMakeLists.txt;0;")
