# CMake generated Testfile for 
# Source directory: /home/kj/CES_2025_artifacts/sil-kit/SilKit/source/experimental
# Build directory: /home/kj/CES_2025_artifacts/sil-kit/build/SilKit/source/experimental
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Test_ParticipantExtensionsImpl "/home/kj/CES_2025_artifacts/sil-kit/build/Release/SilKitUnitTests" "--gtest_output=xml:Test_ParticipantExtensionsImpl_gtestresults.xml" "--gtest_filter=Test_ParticipantExtensionsImpl.*")
set_tests_properties(Test_ParticipantExtensionsImpl PROPERTIES  WORKING_DIRECTORY "/home/kj/CES_2025_artifacts/sil-kit/build/Release" _BACKTRACE_TRIPLES "/home/kj/CES_2025_artifacts/sil-kit/SilKit/cmake/SilKitTest.cmake;111;add_test;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/experimental/CMakeLists.txt;34;add_silkit_test_to_executable;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/experimental/CMakeLists.txt;0;")
subdirs("netsim")
