# Open a JSON file with Tuw

Tuw can take the first argument as a JSON path if the path includes a file extension. By renaming your JSON files to `*.tuw` and setting Tuw as a default application for `*.tuw` files, you can launch GUIs just by clicking on the files.  

![default app](https://github.com/user-attachments/assets/73e42775-d609-4f0d-b2a1-43cb8f4455cf)

On Windows, setting Tuw as the default application is straightforward. However, for other operating systems, the process is a bit more complex. Below are examples of how to set it up on Ubuntu 20.04 and macOS 10.15.

## Ubuntu 20.04

### 1. Add `application/x-tuw` to `~/.local/share/mime/packages/Overrides.xml`

(Make `Overrides.xml` if it does not exist.)

```xml
<?xml version="1.0" encoding="UTF-8"?>
<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
  <mime-type type="application/x-tuw">
    <comment>TUW custom file</comment>
    <glob pattern="*.tuw"/>
  </mime-type>
</mime-info>
```

### 2. Make `~/.local/share/applications/tuw.desktop`

Replace `/full/path/to/Tuw` with your path to Tuw. And save it as `tuw.desktop`.

```ini
[Desktop Entry]
Version=1.0
Type=Application
Name=Tuw
Comment=Tiny GUI wrapper for CLI tools
Exec=/full/path/to/Tuw %f
Terminal=false
MimeType=application/x-tuw;
Categories=Utility;Application;
```

### 3. Update database

You can apply the changes with the following commands.

```console
update-mime-database ~/.local/share/mime
update-desktop-database ~/.local/share/applications/
xdg-mime default tuw.desktop application/x-tuw
```

## macOS 10.15

### 1. Make a wrapper with AppleScript

Open the script editor and write the following AppleScript. And replace `/full/path/to/Tuw` with your path to Tuw.

```scpt
on open fileList
    -- Modify this variable for your environment
    set exePath to "/full/path/to/Tuw"

    set filePath to item 1 of fileList
    set posixFilePath to POSIX path of filePath

    tell application "Terminal"
        do script exePath & " " & posixFilePath & "; exit;"
    end tell
end open
```

### 2. Export the script as an application (File > Export...)

![export_applescript](https://github.com/user-attachments/assets/29a9f20f-804c-46ac-b134-a1a127b84153)

### 3. Right click a .tuw file and set Tuw as a default application
