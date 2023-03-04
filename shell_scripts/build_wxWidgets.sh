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

options="--disable-shared\
    --disable-tests\
    --without-libpng\
    --without-libjpeg\
    --without-libtiff\
    --without-expat\
    --without-liblzma\
    --without-regex\
    --without-zlib\
    --without-opengl\
    --disable-glcanvasegl\
    --disable-sound\
    --disable-tarstream\
    --disable-zipstream\
    --disable-docview\
    --disable-html\
    --disable-htmlhelp\
    --disable-mshtmlhelp\
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
    --disable-addremovectrl\
    --disable-animatectrl\
    --disable-bannerwindow\
    --disable-artstd\
    --disable-arttango\
    --disable-calendar\
    --disable-colourpicker\
    --disable-datepick\
    --disable-searchctrl\
    --disable-richmsgdlg\
    --disable-richtooltip\
    --disable-rearrangectrl\
    --disable-spinbtn\
    --disable-spinctrl\
    --disable-joystick\
    --disable-privatefonts\
    --disable-dragimage\
    --disable-timepick\
    --disable-tbarnative\
    --disable-toolbar\
    --disable-toolbook\
    --disable-prefseditor\
    --disable-notebook\
    --disable-treebook\
    --disable-treelist\
    --disable-gif\
    --disable-pcx\
    --disable-tga\
    --disable-iff\
    --disable-pnm\
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
