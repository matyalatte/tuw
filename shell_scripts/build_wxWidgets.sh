#!/bin/bash
# Builds wxWidgets in ~/wxWidgets-${wx_version}/${build_type}

# You can specify build type as an argument like "bash build_wxWidgets.sh Release"
if [ "$1" = "Debug" ]; then
    build_type="Debug"
else
    build_type="Release"
fi

# wxWidgets version is defined in ./Simple-Command-Runner/WX_VERSION.txt
wx_version="$(cat $(dirname "$0")/../WX_VERSION.txt)"

# Options are defined in wxWidgets/configure
options="--disable-shared
 --disable-compat30
 --disable-tests
 --without-regex
 --without-subdirs
 --without-zlib
 --without-expat
 --without-libjpeg
 --without-libpng
 --without-libtiff
 --without-nanosvg
 --without-liblzma
 --without-libnotify
 --without-opengl
 --without-sdl
 --without-libmspack
 --without-gtkprint
 --without-gnomevfs
 --without-libxpm
 --without-libjbig
 --disable-glcanvasegl
 --disable-config
 --disable-arcstream
 --disable-backtrace
 --disable-cmdline
 --disable-debugreport
 --disable-dialupman
 --disable-filehistory
 --disable-fontenum
 --disable-fontmap
 --disable-fs_archive
 --disable-fs_inet
 --disable-fs_zip
 --disable-fswatcher
 --disable-mimetype
 --disable-printfposparam
 --disable-secretstore
 --disable-snglinst
 --disable-sound
 --disable-spellcheck
 --disable-sysoptions
 --disable-tarstream
 --disable-webrequest
 --disable-zipstream
 --disable-dbghelp
 --disable-docview
 --disable-help
 --disable-mshtmlhelp
 --disable-html
 --disable-htmlhelp
 --disable-xrc
 --disable-aui
 --disable-propgrid
 --disable-ribbon
 --disable-stc
 --disable-loggui
 --disable-logwin
 --disable-logdialog
 --disable-mdi
 --disable-mdidoc
 --disable-mediactrl
 --disable-richtext
 --disable-postscript
 --disable-printarch
 --disable-svg
 --disable-webview
 --disable-actindicator
 --disable-addremovectrl
 --disable-animatectrl
 --disable-bannerwindow
 --disable-artstd
 --disable-arttango
 --disable-bmpcombobox
 --disable-calendar
 --disable-choicebook
 --disable-colourpicker
 --disable-commandlinkbutton
 --disable-dataviewctrl
 --disable-datepick
 --disable-editablebox
 --disable-fontpicker
 --disable-gauge
 --disable-grid
 --disable-headerctrl
 --disable-hyperlink
 --disable-infobar
 --disable-listbook
 --disable-notebook
 --disable-odcombobox
 --disable-prefseditor
 --disable-radiobox
 --disable-richmsgdlg
 --disable-richtooltip
 --disable-rearrangectrl
 --disable-searchctrl
 --disable-taskbarbutton
 --disable-taskbaricon
 --disable-tbarnative
 --disable-timepick
 --disable-togglebtn
 --disable-toolbar
 --disable-toolbook
 --disable-treebook
 --disable-treelist
 --disable-splash
 --disable-coldlg
 --disable-creddlg
 --disable-finddlg
 --disable-fontdlg
 --disable-numberdlg
 --disable-tipdlg
 --disable-progressdlg
 --disable-wizarddlg
 --disable-busyinfo
 --disable-hotkey
 --disable-joystick
 --disable-metafile
 --disable-dragimage
 --disable-dctransform
 --disable-webviewwebkit
 --disable-privatefonts
 --disable-gif
 --disable-pcx
 --disable-tga
 --disable-iff
 --disable-pnm
 --disable-xpm
 --disable-ico_cur
 --prefix=$(pwd)"

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # no-rtti option doesn't support OSX
    options="${options} --enable-no_rtti --disable-intl --disable-xlocale"
fi

if [ ${build_type} = "Debug" ]; then
    options="${options} --enable-debug"
else
    # Optimize for size
    export CXXFLAGS="-Os"
    export CFLAGS="-Os"
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        export CXXFLAGS="${CXXFLAGS} -ffunction-sections -fdata-sections"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        export CXXFLAGS="${CXXFLAGS} -ffunction-sections -fdata-sections"
    fi
fi
echo "CMake arguments: ${options}"

# Get nproc
if nproc; then
    num_proc=$(nproc) # for linux
elif sysctl -n hw.logicalcpu; then
    num_proc=$(sysctl -n hw.logicalcpu) # for osx
else
    num_proc=2
fi

# Build
pushd ~/wxWidgets-"$wx_version"
mkdir ${build_type}
cd ${build_type}
../configure ${options}
make -j"${num_proc}"
popd
