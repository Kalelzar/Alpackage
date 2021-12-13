# syntax=docker/dockerfile:1


FROM greyltc/archlinux-aur:yay

RUN mkdir /home/usr1 -p

ENV HOME="/home/usr1/" XDG_CONFIG_HOME="/home/usr1/.config"

RUN mkdir -p "$XDG_CONFIG_HOME/alpackage"
COPY textdb.scsv "$XDG_CONFIG_HOME/alpackage/textdb.scsv"

RUN pacman-key --init
RUN sudo -u ab -D~ bash -c 'yay -Syu --needed --noprogressbar --noconfirm hayai-git'
RUN sudo -u ab -D~ bash -c 'yay -Syu --needed --noprogressbar --noconfirm rapidcheck'
RUN pacman --noconfirm -S openssh
RUN pacman --noconfirm -S gperftools
RUN pacman --noconfirm -S boost
RUN pacman --noconfirm -S boost-libs
RUN pacman --noconfirm -S cmake
RUN pacman --noconfirm -S gtest
RUN pacman --noconfirm -S libgit2
RUN pacman --noconfirm -S clang
RUN pacman --noconfirm -S ninja
RUN pacman --noconfirm -S gcovr

RUN git clone "https://github.com/Kalelzar/Alpackage.git" /test --recurse-submodules
WORKDIR /test

CMD make clean install testAll clean
