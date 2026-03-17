Usecase: CES 2025 Multi VECU Platform Test Bench 

Pre-requesite: Gcc compiler to execute the cpp code (available readily in x_86 Instance)




Path - /home/ubuntu/sil-kit/build/Release "path where you have silkit install folder"
./start-Registry.sh  "will start the registry as proxy having ip address of instance"


--note "for all the components, config files need to update with the silkit proxy ip address in .yml file "  

ZonalA.cpp
Path - Domain_Body_TestCase/Zonal_A
CLI Command - ./scripts/first_build.sh               // to build the component for first time
CLI Command - ./scripts/start.sh


HPC.cpp within QEMU 
--qemu boot up for HPC
./home/ubuntu/sil-kit-adapters-qemu/tools/run-silkit-qemu-demos-guest.sh
Path - HPC_QEMU/
CLI Command - ./scripts/first_build.sh               // to build the component for first time
CLI Command - ./scripts/start_HPC.sh

Telematics_QEMU.cpp within QEMU
--qemu boot up for HPC
./home/ubuntu/sil-kit-adapters-qemu/tools_1/run-silkit-qemu-demos-guest.sh
Path - Telematics_QEMU/
CLI Command - ./scripts/first_build.sh               // to build the component for first time
CLI Command - ./scripts/start_telematic.sh          // ip address is of mqtt broker (one time step) change accordingly 

ZonalB.cpp
Path - Domain_Body_TestCase/Zonal_B/
CLI Command - ./scripts/first_build.sh              // to build the component for first time
CLI Command - ./scripts/start.sh


RestBus.cpp:   The Rest Bus Code to send the input to Zonal A and receive the output from Telematic ECU , Zonal_B                  
Path - Domain_Body_TestCase/PlantModel_RestBus/
CLI Command - ./scripts/first_build.sh            // to build the component for first time
CLI Command - ./scripts/start.sh                  // ip address is of mqtt broker change accordingly also need to chnage the test case id



To run the Android App:

ssh to the ec2 instance - ces-2025-android-test-instance
ssh -i catalog-default-key.pem ubuntu@<public-ip addr>

To run Automated script ---->
To run the script, go to "home/ubuntu/android-cuttlefish/cf/" directory
python3 /home/ubuntu/automatedCuttlefish.py
 
To Run android application :
open browser
Access Andriod app from URL: https://<public_ip_of_instance>:8443/
    
To remember while running 'automatedCuttlefish.py':
1) When "Waiting for emulator to fully boot..." gets printed on the terminal, 
     refresh the browser and toggle cvd-1 button to 'ON' state. Then, wait for the display to appear.
 
2) Make sure to press 'ENTER' to close the cuttlefish emulator, before ending the test


-- NOTE make sure the MQTT broker instance is in running state

Test Cases with ID:

Test Case ID - 1                
Input -                                                   
Switch off the Ignition                                   
Set the vehicle speed <=0 
Set the trunk open request from Android APP

Expected Output -
Trunk door should be opened 
Observe the Trunk status on Andorid app, trunk status should be open 

Actual Output - 
Test Case 1 Passed


Test Case ID - 2   
Input -                                                            
Switch On the Ignition                            
Set the vehicle speed <=0                         
Set trunk door request as close from Android app 

Expected Output -
Trunk door should be closed 
Observe the Trunk statsu on Andorid app, trunk status should be closed 


Actual Output -
Test Case 2 Passed

Test Case ID - 3   
Input -                                                            
Switch On the Ignition                            
Set the vehicle speed <=0                         
Set trunk door request as open from Android app 

Expected Output -
Trunk door should be opened 
Observe the Trunk statsu on Andorid app, trunk status should be open 


Actual Output -
Test Case 3 Failed
											





 