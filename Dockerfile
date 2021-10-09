# syntax=docker/dockerfile:1


FROM archlinux:latest
WORKDIR /test
COPY build/ninja/src/Release/libAlpackage.so /lib/libAlpackage.so
COPY build/ninja/ext/modules/textdb/Release/libAlpackageTextDB.so /lib/alpackage/libAlpackageTextDB.so
COPY build/ninja/ ./cmake

RUN pacman-key --init
RUN pacman --noconfirm -Syu
RUN pacman --noconfirm -S gperftools boost boost-libs cmake gtest ripgrep

RUN mkdir /home/usr1 -p

ENV HOME="/home/usr1/" XDG_CONFIG_HOME="/home/usr1/.config"


RUN mkdir -p "$XDG_CONFIG_HOME/alpackage"
COPY textdb.scsv "$XDG_CONFIG_HOME/alpackage/textdb.scsv"


RUN rg "/home/kalelzar/Code/cpp/Alpackage/build/ninja" * --files-with-matches | xargs --max-args=1 sed -i -E 's$/home/kalelzar/Code/cpp/Alpackage/build/ninja/$/test/cmake/$g'

CMD ["/bin/ctest", "--test-dir", "/test/cmake/", "--output-on-failure"]
