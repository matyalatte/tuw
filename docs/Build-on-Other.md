# Building Workflow for Other Platforms

You can build tuw on other Unix-like systems, not only Linux.  
The workflow is the same as Linux.  

## Requirements

-   C++ compiler
-   GTK+ 3.10 or later
-   Ninja
-   [Meson](https://github.com/mesonbuild/meson) (**0.58** or later)
-   bash
-   Shell scripts in [`./Tuw/shell_scripts`](../shell_scripts)

## FreeBSD

On FreeBSD, You can get all the required tools with `pkg`.

```shell
pkg install git bash pkgconf meson ninja
git clone https://github.com/matyalatte/tuw.git
cd tuw
bash shell_scripts/build.sh
./build/Release/Tuw
```

## OpenBSD

On OpenBSD, You can get all the required tools with `pkg_add`.

```shell
pkg_add bash meson ninja
git clone https://github.com/matyalatte/tuw.git
cd tuw
bash shell_scripts/build.sh
./build/Release/Tuw
```

## NetBSD

On NetBSD, You can get all the required tools with `pkgin`.

```shell
pkgin in git mozilla-rootcerts-openssl bash pkgconf meson ninja
git clone https://github.com/matyalatte/tuw.git
cd tuw
bash shell_scripts/build.sh
./build/Release/Tuw
```

If you get linker errors, try to disalble LTO with `b_lto = false` in `./presets/release.ini`.

## Haiku

On Haiku, You can get all the required tools with `pkgman`.

```shell
pkgman install meson ninja gtk3_devel
git clone https://github.com/matyalatte/tuw.git
cd tuw
bash shell_scripts/build.sh
./build/Release/Tuw
```

> [!WARNING]  
> Note that GTK on Haiku is still a WIP project.
> Some features might not work properly.
> As far as I know, drag-drop events are not supported yet.

You can also use [the Haiku theme for GTK](https://github.com/B00merang-Project/Haiku) if you don't like the default theme.  

```
mkdir -p /boot/home/config/non-packaged/data/themes
cd /boot/home/config/non-packaged/data/themes
git clone https://github.com/B00merang-Project/Haiku.git

# replace `gtk-theme='Adwaita'` with `gtk-theme='Haiku'`
nano /boot/system/non-packaged/data/glib-2.0/schemas/00_org.gnome.desktop.interface.gschema.override

glib-compile-schemas /boot/system/non-packaged/data/glib-2.0/schemas
```
