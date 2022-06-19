How to run and install project:

This project is built using platform io on the VSCode platform https://docs.platformio.org/en/latest/integration/ide/vscode.html#ide-vscode

Once you have platformio install you can use the PIO Home tab to open this project.

To build for a root go to the platformio.ini file and make sure the buildflags have the -DROOT uncommented and the -UROOT commented

To build for a node go to the platformio.ini file and make sure the buildflags have the -UROOT uncommented and the -DROOT commented

You may need to check what port the ESP is on and change it in the platformio.ini file

Server ip will also need to be changed in http_requests.hpp

Now if you click build and install the program should upload to your ESP.