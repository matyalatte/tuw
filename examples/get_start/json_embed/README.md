# JSON Embedding

## Embed JSON into Executables

Tuw has a command-line util to embed JSON into exe.  
Type `Tuw merge` on the command prompt (or the terminal).  
It'll merge `Tuw` and `gui_definition.json` into `Tuw.new`.  
The merged executable can make a GUI without `gui_definition.json`!  

```bash
Tuw merge
```

You can also specify the file paths with `-j` and `-e` options.  

```bash
Tuw merge -j gui_definition.json -e Tuw.new
```

And you can use `-f` to skip the overwrite confirmation.  

```bash
Tuw merge -j gui_definition.json -e Tuw.new -f
```

## Extract JSON from Executables

You can use the `split` command if you want to extract a JSON file from the merged executable.  

```bash
Tuw.new split -j extracted.json -e Tuw -f
```

## GUI wrapper for JSON Embedding

[The JSON file](./gui_definition.json) in this folder is for the JSON embedding.  
It means, Tuw will be the GUI wrapper for itself.  

![embed json](https://github.com/matyalatte/Tuw/assets/69258547/5862595c-bf66-4506-9b6a-9ad3b85fcc28)  
