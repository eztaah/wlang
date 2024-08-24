# le script attend en argument la version 


# create release folder for each releases (linux and windows)
mkdir wlang + version  # wlang-1.0.0_linux_amd64
mkdir wlang + version  # wlang-1.0.0_win64


# compile wlangc and libw for linux
cd compiler 
make release -DLINUX_BUILD
cd ../stdlib
make            # build stdlib
# move the generated files into the release folder
mv ./wlangc ./wlang-1.0.0_linux_amd64/wlangc
mv ./libw.a ./wlang-1.0.0_linux_amd64/libw.a

# compile wlangc for windows
cd compiler 
make release -DWINDOW_BUILD
# move the generated files into the release folder
mv ./wlangc ./wlang-1.0.0_win64/wlangc

# for all the created folders
# add changelelog, add readme
cp ./CHANGELOG.txt ./wlang_linux/CHANGELOG.txt
cp ./CHANGELOG.txt ./wlang_win64/CHANGELOG.txt
cp ./README-linux.md ./wlang_linux/README.md
cp ./README-win64.md ./wlang_win64/README.md

# add hello-world example
cp hello-world-linux/ ./wlang-linux/
cp hello-world-windows/ ./wlang-windows/

# compress release folders
