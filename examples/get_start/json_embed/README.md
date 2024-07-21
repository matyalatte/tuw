# JSON Embedding

## Embed JSON into Executables

Tuw has a command-line utility to embed JSON into an executable.  
Type `Tuw merge` in the command prompt (or terminal).  
It'll merge `Tuw` and `gui_definition.json` into `Tuw.new`.  
The merged executable can create a GUI without needing `gui_definition.json`!  

```bash
Tuw merge
```

You can also specify the file paths with the `-j` and `-e` options.  

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

[The JSON file](./gui_definition.json) in this folder is for JSON embedding.  
This means Tuw will act as the GUI wrapper for itself.  

![embed json](https://github.com/matyalatte/tuw/assets/69258547/d28e9bfd-4905-4697-9523-49af8c294e6b)  
