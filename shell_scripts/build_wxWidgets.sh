#!/bin/bash
# Builds and installs wxWidgets with cmake.

# you need build-essential and libgtk-3-dev for linux
# sudo apt -y install build-essential libgtk-3-dev

wx_version="$(cat $(dirname "$0")/../WX_VERSION.txt)"

if [ "$1" = "Debug" ];
    then build_type="Debug";
    else build_type="Release";
fi

pushd ~/wxWidgets-"$wx_version"
mkdir ${build_type}
cd ${build_type}

# Options are defined in wxWidgets/configure
options="--disable-shared\
    --disable-compat30\
    --disable-tests\
    --without-regex\
    --without-zlib\
    --without-expat\
    --without-libjpeg\
    --without-libpng\
    --without-libtiff\
    --without-nanosvg\
    --without-liblzma\
    --without-opengl\
    --without-sdl\
    --without-libmspack\
    --without-gtkprint\
    --without-gnomevfs\
    --without-libxpm\
    --without-libjbig\
    --disable-glcanvasegl\
    --disable-cmdline\
    --disable-filehistory\
    --disable-fontenum\
    --disable-fontmap\
    --disable-fs_inet\
    --disable-fswatcher\
    --disable-mimetype\
    --disable-printfposparam\
    --disable-secretstore\
    --disable-sound\
    --disable-spellcheck\
    --disable-sysoptions\
    --disable-tarstream\
    --disable-webrequest\
    --disable-zipstream\
    --disable-docview\
    --disable-help\
    --disable-mshtmlhelp\
    --disable-html\
    --disable-htmlhelp\
    --disable-xrc\
    --disable-aui\
    --disable-propgrid\
    --disable-ribbon\
    --disable-stc\
    --disable-loggui\
    --disable-logwin\
    --disable-logdialog\
    --disable-mdi\
    --disable-mdidoc\
    --disable-mediactrl\
    --disable-richtext\
    --disable-postscript\
    --disable-printarch\
    --disable-svg\
    --disable-webview\
    --disable-actindicator\
    --disable-addremovectrl\
    --disable-animatectrl\
    --disable-bannerwindow\
    --disable-artstd\
    --disable-arttango\
    --disable-bmpcombobox\
    --disable-calendar\
    --disable-choicebook\
    --disable-colourpicker\
    --disable-commandlinkbutton\
    --disable-dataviewctrl\
    --disable-datepick\
    --disable-editablebox\
    --disable-fontpicker\
    --disable-gauge\
    --disable-grid\
    --disable-headerctrl\
    --disable-hyperlink\
    --disable-infobar\
    --disable-listbook\
    --disable-notebook\
    --disable-odcombobox\
    --disable-prefseditor\
    --disable-radiobox\
    --disable-richmsgdlg\
    --disable-richtooltip\
    --disable-rearrangectrl\
    --disable-searchctrl\
    --disable-spinbtn\
    --disable-spinctrl\
    --disable-taskbaricon\
    --disable-tbarnative\
    --disable-timepick\
    --disable-togglebtn\
    --disable-toolbar\
    --disable-toolbook\
    --disable-treebook\
    --disable-treelist\
    --disable-splash\
    --disable-coldlg\
    --disable-creddlg\
    --disable-finddlg\
    --disable-fontdlg\
    --disable-numberdlg\
    --disable-tipdlg\
    --disable-progressdlg\
    --disable-wizarddlg\
    --disable-busyinfo\
    --disable-hotkey\
    --disable-joystick\
    --disable-metafile\
    --disable-dragimage\
    --disable-dctransform\
    --disable-webviewwebkit\
    --disable-privatefonts\
    --disable-gif\
    --disable-pcx\
    --disable-tga\
    --disable-iff\
    --disable-pnm\
    --disable-xpm\
    --disable-ico_cur\
    --prefix=$(pwd)"

if [ ${build_type} = "Debug" ];
    then options="${options} --enable-debug" ;
fi

# configure
../configure ${options};

function nproc_for_mac(){
    if sysctl -n hw.logicalcpu;
        then num_proc=$(sysctl -n hw.logicalcpu); # use hw.logicalcpu if exists
        else num_proc=2; echo ${num_proc}; # when sysctl won't work
    fi
}

if nproc;
    then num_proc=$(nproc); # use nproc if exists
    else num_proc=$(nproc_for_mac); # when nproc doesn't exist
fi

# build
make -j"${num_proc}"
popd
