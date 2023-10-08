# Frequently Asked Questions

## Can I redistribute Tuw with my scripts?

Yes, and you can rename it to `GUI.exe`, `'project name'-GUI.exe`, or `'project name'_GUI.exe`.  

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

## My Linux machine says `Could Not Display` when lauching the executable.

Check `Allow executing file as program.` (Properties->Permissions->Execute)  
You will be able to launch the executable by double-click.  
![CloudNotDisplay](https://github.com/matyalatte/tuw/assets/69258547/ecf995a6-cc75-4ba6-a253-ad2104f2e2c9)  

## My Linux machine says `File not found` (or nothing) when clicking the executable.

It could be due to glibc dependences.  
The released binary requires GTK+ 3.10, GLIBC 2.15, and GLIBCXX 3.4.21 (or newer versions of the libraries).  
You should get the libs, or [build the executable](./Building.md) by yourself.  
