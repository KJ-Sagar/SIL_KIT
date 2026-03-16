# CMake generated Testfile for 
# Source directory: /home/kj/CES_2025_artifacts/sil-kit/SilKit/source/services/rpc
# Build directory: /home/kj/CES_2025_artifacts/sil-kit/build/SilKit/source/services/rpc
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Test_RpcClient "/home/kj/CES_2025_artifacts/sil-kit/build/Release/SilKitUnitTests" "--gtest_output=xml:Test_RpcClient_gtestresults.xml" "--gtest_filter=Test_RpcClient.*")
set_tests_properties(Test_RpcClient PROPERTIES  WORKING_DIRECTORY "/home/kj/CES_2025_artifacts/sil-kit/build/Release" _BACKTRACE_TRIPLES "/home/kj/CES_2025_artifacts/sil-kit/SilKit/cmake/SilKitTest.cmake;111;add_test;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/services/rpc/CMakeLists.txt;40;add_silkit_test_to_executable;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/services/rpc/CMakeLists.txt;0;")
add_test(Test_RpcServer "/home/kj/CES_2025_artifacts/sil-kit/build/Release/SilKitUnitTests" "--gtest_output=xml:Test_RpcServer_gtestresults.xml" "--gtest_filter=Test_RpcServer.*")
set_tests_properties(Test_RpcServer PROPERTIES  WORKING_DIRECTORY "/home/kj/CES_2025_artifacts/sil-kit/build/Release" _BACKTRACE_TRIPLES "/home/kj/CES_2025_artifacts/sil-kit/SilKit/cmake/SilKitTest.cmake;111;add_test;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/services/rpc/CMakeLists.txt;40;add_silkit_test_to_executable;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/services/rpc/CMakeLists.txt;0;")
add_test(Test_RpcMatching "/home/kj/CES_2025_artifacts/sil-kit/build/Release/SilKitUnitTests" "--gtest_output=xml:Test_RpcMatching_gtestresults.xml" "--gtest_filter=Test_RpcMatching.*")
set_tests_properties(Test_RpcMatching PROPERTIES  WORKING_DIRECTORY "/home/kj/CES_2025_artifacts/sil-kit/build/Release" _BACKTRACE_TRIPLES "/home/kj/CES_2025_artifacts/sil-kit/SilKit/cmake/SilKitTest.cmake;111;add_test;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/services/rpc/CMakeLists.txt;40;add_silkit_test_to_executable;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/services/rpc/CMakeLists.txt;0;")
add_test(Test_RpcSerdes "/home/kj/CES_2025_artifacts/sil-kit/build/Release/SilKitUnitTests" "--gtest_output=xml:Test_RpcSerdes_gtestresults.xml" "--gtest_filter=Test_RpcSerdes.*")
set_tests_properties(Test_RpcSerdes PROPERTIES  WORKING_DIRECTORY "/home/kj/CES_2025_artifacts/sil-kit/build/Release" _BACKTRACE_TRIPLES "/home/kj/CES_2025_artifacts/sil-kit/SilKit/cmake/SilKitTest.cmake;111;add_test;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/services/rpc/CMakeLists.txt;52;add_silkit_test_to_executable;/home/kj/CES_2025_artifacts/sil-kit/SilKit/source/services/rpc/CMakeLists.txt;0;")
