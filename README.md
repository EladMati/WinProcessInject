# WinProcessInject
Inject dll to notepad processes. Also kill all calculator processes - Do it continuisly for new relevat processes

How to run the project:
1) Create folder "C:\Temp" and put inside it the dll "TotallyNotAVirus.dll"
2) Make sure that the antivirus is off!!
3) a. Download WinProcessInject.exe and run it as administrator
2) b. Or Download the whole solution zip. Unzip it. Run Visual studio (2022) as administrator! load the solution (sln file) .Run it a s Release x64 (with c++ 17 defenitions). 
   
How it was tested:
1) Several notepad processess were opened before runing the program. Each time diffrent number of them. Several notepad files were opened while program is running.
2) Meanwhile several calculaor procceses were opened before runing the program. Each time diffrent number of them. Several notepad files were opened while program is running.
3) It was tested on windows 11 x64 bit only
   
Considerations and limitations:
1) It was taested for only few running processes of calculator and of notepad.
2) It was tested for short periods (several minutes)- Can be run for much longer.
3) It was not tested in system with a lot of overall open processes
4) It was note tested for stressed systems with high load of cpu
   
Known issues:
1) The time for checking new calculator processes and for new notepad processes id hardcodeded to 300 ms.
2) For the calculator processes the process name to compare was "**CalculatorApp.exe**" even though it can be diff is some os( for example "calc.exe") - Not enough robust . May not work on all ermutations of OS of desired windows.
It (CalculatorApp.exe) was cosn because, that was the only way the tested os windows 11 x64 was albe to detect open calculator processes

Future Improvments:
1) Ajust more precisely the interaction time (the rest time) of each thread form 300 ms - to be more balanced: between CPU consumption to faster detection time of new calculator and notepad processes.
2) "**CalculatorApp.exe**" - Make the detaction of calculator processes more robust and suitable to all permutations of OS of desired windows. Also need to check equivalnt of of "notepad.exe".
3) Make the theads listen to the creation of new processes ( As equivalent to netlink syscall in linux) instead of pulling all possibles runnig pids in the systems each time.
4) Path of the dll, the time for checking new processes, both can be accepted as argument to the program.
