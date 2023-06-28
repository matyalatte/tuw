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
lib_options="--without-regex
 --without-zlib
 --without-expat
 --without-libjpeg
 --without-libpng
 --without-libtiff
 --without-nanosvg
 --without-liblzma
 --without-opengl
 --without-sdl
 --without-libnotify
 --without-libmspack
 --without-gtkprint
 --without-gnomevfs
 --without-libxpm
 --without-libjbig
 --without-libiconv
 --disable-glcanvasegl
 --disable-std_iostreams
 --disable-std_string_conv_in_wxstring"

non_gui_options="--disable-plugins
 --disable-intl
 --disable-xlocale
 --disable-largefile
 --disable-config
 --disable-ipv6
 --disable-ipc
 --disable-any
 --disable-apple_ieee
 --disable-arcstream
 --disable-base64
 --disable-backtrace
 --disable-cmdline
 --disable-debugreport
 --disable-dialupman
 --disable-dynamicloader
 --disable-ffile
 --disable-filehistory
 --disable-filesystem
 --disable-fontenum
 --disable-fontmap
 --disable-fs_archive
 --disable-fs_inet
 --disable-fs_zip
 --disable-fswatcher
 --disable-log
 --disable-mimetype
 --disable-printfposparam
 --disable-secretstore
 --disable-snglinst
 --disable-sound
 --disable-spellcheck
 --disable-stdpaths
 --disable-stopwatch
 --disable-sysoptions
 --disable-tarstream
 --disable-textbuf
 --disable-textfile
 --disable-webrequest
 --disable-zipstream
 --disable-variant
 --disable-ftp
 --disable-http
 --disable-fileproto
 --disable-sockets
 --disable-url
 --disable-protocol
 --disable-protocol-ftp
 --disable-protocol-http
 --disable-protocol-file
 --disable-threads
 --disable-dbghelp"

big_gui_options="--disable-docview
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
 --disable-clipboard"

ctrl_options="--disable-markup
 --disable-accel
 --disable-actindicator
 --disable-addremovectrl
 --disable-animatectrl
 --disable-bannerwindow
 --disable-artstd
 --disable-arttango
 --disable-bmpcombobox
 --disable-calendar
 --disable-caret
 --disable-checklst
 --disable-choicebook
 --disable-collpane
 --disable-colourpicker
 --disable-comboctrl
 --disable-commandlinkbutton
 --disable-dataviewctrl
 --disable-nativedvc
 --disable-datepick
 --disable-display
 --disable-editablebox
 --disable-fontpicker
 --disable-gauge
 --disable-grid
 --disable-headerctrl
 --disable-hyperlink
 --disable-imaglist
 --disable-infobar
 --disable-listbook
 --disable-notebook
 --disable-notifmsg
 --disable-odcombobox
 --disable-prefseditor
 --disable-radiobox
 --disable-richmsgdlg
 --disable-richtooltip
 --disable-rearrangectrl
 --disable-sash
 --disable-scrollbar
 --disable-searchctrl
 --disable-slider
 --disable-splitter
 --disable-statbmp
 --disable-statbox
 --disable-statusbar
 --disable-taskbarbutton
 --disable-taskbaricon
 --disable-tbarnative
 --disable-timepick
 --disable-togglebtn
 --disable-toolbar
 --disable-toolbook
 --disable-treebook
 --disable-treelist"

dlg_options="--disable-aboutdlg
 --disable-choicedlg
 --disable-coldlg
 --disable-creddlg
 --disable-finddlg
 --disable-fontdlg
 --disable-numberdlg
 --disable-splash
 --disable-textdlg
 --disable-tipdlg
 --disable-progressdlg
 --disable-wizarddlg"

misc_gui_options="--disable-miniframe
 --disable-splines
 --disable-mousewheel
 --disable-busyinfo
 --disable-hotkey
 --disable-joystick
 --disable-metafile
 --disable-dragimage
 --disable-dctransform
 --disable-webviewwebkit
 --disable-privatefonts"

# You can remove a test tool from the release build with "bash build_wxWidgets.sh NoTest"
if [ "$1" = "NoTest" ]; then
    misc_gui_options="${misc_gui_options} --disable-uiactionsim"
fi

img_options="--disable-palette
 --disable-image
 --disable-gif
 --disable-pcx
 --disable-tga
 --disable-iff
 --disable-pnm
 --disable-xpm
 --disable-ico_cur"

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    non_gui_options="${non_gui_options} --enable-no_rtti"
    ctrl_options="${ctrl_options} --disable-bmpbutton --disable-radiobtn --disable-combobox"
else
    # Somehow OSX will use gnu++11 without the 'with-cxx' option
    non_gui_options="${non_gui_options} --disable-datetime"
    ctrl_options="${ctrl_options} --disable-filectrl"
    lib_options="${lib_options} --without-cairo --with-cxx=17"
fi

options="--disable-shared
 --disable-compat30
 --disable-tests
 --without-subdirs
 ${lib_options}
 ${non_gui_options}
 ${big_gui_options}
 ${ctrl_options}
 ${dlg_options}
 ${misc_gui_options}
 ${img_options}
 --prefix=$(pwd)"

if [ ${build_type} = "Debug" ]; then
    options="${options} --enable-debug"
else
    options="${options} --disable-debug --disable-debug_flag --disable-debug_info"
    # Optimize for size
    export CXXFLAGS="-Os -ffunction-sections -fdata-sections -flto"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # for .mm files
        export OBJCXXFLAGS="-Os -ffunction-sections -fdata-sections -flto"
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
