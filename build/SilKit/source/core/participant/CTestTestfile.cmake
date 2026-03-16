# CMake generated Testfile for 
# Source directory: /home/kj/CES_2025_artifacts/sil-kit/SilKit/source/core/participant
# Build directory: /home/kj/CES_2025_artifacts/sil-kit/build/SilKit/source/core/participant
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Test_Participant "/home/kj/CES_2025_artifacts/sil-kit/build/Release/SilKitUnitTests" "--gtest_output=xml:Test_Participant_gtestresults.xml" "--gtest_filter=Test_Participant.*")
set_tests_properties(Test_Participant PROPERTIES  WORKING_DIRECTORY "/home/kj/CES_2025_artifacts/sil-kit/build/Release" _BACKTRACE_TRIPLES "/home/kj/CES_2025_artifacts/sil-kit/SilKit/cmake/SilKitTest.cmake;111;add_test;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/core/participant/CMakeLists.txt;65;add_silkit_test_to_executable;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/core/participant/CMakeLists.txt;0;")
add_test(Test_ValidateAndSanitizeConfig "/home/kj/CES_2025_artifacts/sil-kit/build/Release/SilKitUnitTests" "--gtest_output=xml:Test_ValidateAndSanitizeConfig_gtestresults.xml" "--gtest_filter=Test_ValidateAndSanitizeConfig.*")
set_tests_properties(Test_ValidateAndSanitizeConfig PROPERTIES  WORKING_DIRECTORY "/home/kj/CES_2025_artifacts/sil-kit/build/Release" _BACKTRACE_TRIPLES "/home/kj/CES_2025_artifacts/sil-kit/SilKit/cmake/SilKitTest.cmake;111;add_test;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/core/participant/CMakeLists.txt;70;add_silkit_test_to_executable;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/core/participant/CMakeLists.txt;0;")
