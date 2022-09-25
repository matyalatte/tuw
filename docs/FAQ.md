# Frequently Asked Questions

## Can I distribute the exe with my scripts?

Yes, and you can rename it to `GUI.exe`, `'project name'-GUI.exe`, or `'project name'_GUI.exe`.  

## What's `gui_config.json` for?

It saves the previously executed arguments.  
The executable will use them as default values when launching.  

## Linux says `Could Not Display` when lauching the executable.

Check `Allow executing file as program.` (Properties->Permissions->Execute)  
You will be able to launch the executable by double-click.  
![CloudNotDisplay](https://user-images.githubusercontent.com/69258547/189526464-cd62887b-62b1-4071-ae38-a7ab73600bbf.png)  

## Linux says nothing when clicking the executable.

Launch it from the terminal to see the error messages.  
It could be due to glibc dependences.  
![GLIBC](https://user-images.githubusercontent.com/69258547/190676422-ae1af3a0-e9c1-4afd-a6ec-62803dc303c0.png)  
The Linux build only works on Ubuntu 20.04 or later due to the dependences.  
Or you need to get the libs.  
Or need to build the executable by your self. (But Idk if it's possible on your environment.)  
