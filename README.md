# WinProcessInject
Inject dll to notepad processes. Also kill all calc.exe processes - Do it continuisly for new processes
How to run the project:
1) Create folder "C:\Temp" and put inside it the dll "TotallyNotAVirus.dll"
2) a. Download WinProcessInject.exe and run it as administrator
2) b. Or Download the whole solution zip. Unzip it and open it in Visual studio (2022). Run it a s Release x64 (with c++ 17 defenitions)
How it was tested:
1)  Several notepad processess were opened before runing the program. Each time diffrent number of them. Several notepad files were opned while program is running
2) Meanwhile several calculaor procceses were opened before runing the program. Each time diffrent number of them. Several notepad files were opned while program is running
Considerations and limitations:
1) The log file is appending each time
2) The time for checking new new processes is hardcodeded to 300 ms
3) for the calculator processes the process name to compare was "CalculatorApp.exe" even though it can be diff is some os( for example "calc.exe") - Not eanouth robust
