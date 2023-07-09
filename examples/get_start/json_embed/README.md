# JSON Embedding

## Embed JSON into Executables

Simple Command Runner has a command-line util to embed JSON into exe.  
Type `SimpleCommandRunner merge` on the command prompt (or the terminal).  
It'll merge `SimpleCommandRunner` and `gui_definition.json` into `SimpleCommandRunner.new`.  
The merged executable can make a GUI without `gui_definition.json`!  

```bash
SimpleCommandRunner merge
```

You can also specify the file paths with `-j` and `-e` options.  

```bash
SimpleCommandRunner merge -j gui_definition.json -e SimpleCommandRunner.new
```

And you can use `-f` to skip the overwrite confirmation.  

```bash
SimpleCommandRunner merge -j gui_definition.json -e SimpleCommandRunner.new -f
```

## Extract JSON from Executables

You can use the `split` command if you want to extract a JSON file from the merged executable.  

```bash
SimpleCommandRunner.new split -j extracted.json -e SimpleCommandRunner -f
```

## GUI wrapper for JSON Embedding

[The JSON file](./gui_definition.json) in this folder is for the JSON embedding.  
It means, Simple Command Runner will be the GUI wrapper for itself.  

![embed json](https://github.com/matyalatte/Simple-Command-Runner/assets/69258547/5862595c-bf66-4506-9b6a-9ad3b85fcc28)  
