# Frequently Asked Questions

## Can I redistribute the exe with my scripts?

Yes, and you can rename it to `GUI.exe`, `'project name'-GUI.exe`, or `'project name'_GUI.exe`.  
But note that it's licensed under GPL.  
You should inform users of the license and the link to the source code.  

## How can I insert an input into multiple places in commands?

Use the [`id`](../examples/comp_options/id/) option for components.  
You can use the defined IDs as variable names in commands.  

## How can I run multiple commands at the same time?

Simple Command Runner will execute a single line command on the command prompt (or the terminal.)  
So, it's the same question as "How can I run multiple commands as a single line on the command prompt (or the terminal?)"  
It's a little complicated task, but you can find tons of websites that explain about it.  
Also, you can see [some examples](../examples/tips/multi_lines/) about it.

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
Or need to build the executable by your self.  
