# JSON Embedding

## Embed JSON

You can merge Simple Command Runner and `gui_definition.json` into an executable via command-line.  

```bat
SimpleCommandRunner.exe merge -j gui_definition.json -e SimpleCommandRunner.new.exe -f
```

The merged exe will work without json files!  

## Extract JSON

You can also split the merged executable into json and the original exe.  

```bat
SimpleCommandRunner.new.exe split -j new.json -e SimpleCommandRunner.orig.exe -f
```

## GUI wrapper for Simple Command Runner

[The json file](./gui_definition.json) in this folder is for the JSON embedding.  
It means, Simple Command Runner will be the GUI wrapper for itself.  

![embed json](https://github.com/matyalatte/Simple-Command-Runner/assets/69258547/5862595c-bf66-4506-9b6a-9ad3b85fcc28)  
