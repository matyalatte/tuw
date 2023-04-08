# Building workflow for ubuntu 20.04 or later.
#
# 1. Use this docker file to build the executable.
#    docker build -t simple-command-runner ./
# 2. Run the built image. 
#    docker run simple-command-runner
# 3. Use "docker cp" to get the built executable.
#    docker cp <CONTAINER ID>:/Simple-Command-Runner/build/SimpleCommandRunner ./

# Base image
ARG ubuntu_version="20.04"
FROM ubuntu:${ubuntu_version}

# Install packages
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       wget ca-certificates build-essential libgtk-3-dev git cmake \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Install wxWidgets
ARG wx_version="3.2.2"
RUN wget https://github.com/wxWidgets/wxWidgets/releases/download/v${wx_version}/wxWidgets-${wx_version}.tar.bz2;\
    tar -xvjof wxWidgets-${wx_version}.tar.bz2;\
    rm -f wxWidgets-${wx_version}.tar.bz2

WORKDIR /wxWidgets-${wx_version}/release
RUN ../configure --disable-shared\
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
    --disable-ico_cur

RUN make -j"$(nproc)"; make install

# Build Simple Command Runner
WORKDIR /
RUN git clone https://github.com/matyalatte/Simple-Command-Runner.git

WORKDIR /Simple-Command-Runner/build
RUN cmake -DCMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=OFF ../; \
    cmake --build .
