# syntax=docker/dockerfile:1


FROM greyltc/archlinux-aur:yay
WORKDIR /test

RUN mkdir /home/usr1 -p

ENV HOME="/home/usr1/" XDG_CONFIG_HOME="/home/usr1/.config"

RUN mkdir -p "$XDG_CONFIG_HOME/alpackage"
COPY textdb.scsv "$XDG_CONFIG_HOME/alpackage/textdb.scsv"

RUN pacman-key --init
RUN sudo -u ab -D~ bash -c 'yay -Syu --needed --noprogressbar --noconfirm rapidcheck hayai-git'
RUN pacman --noconfirm -S gperftools boost boost-libs cmake gtest libgit2 clang ninja gcovr

CMD make clean install testAll clean
