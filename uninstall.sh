#!/bin/bash
if ! [ "$(id -u)" = 0 ] && [[ "${PREFIX:0:27}" != *"com.termux"* ]]
then
    echo "Please execute uninstall.sh as root"
    echo "Uninstallation failed! :)"
    exit 1
fi
echo "Removing '${PREFIX:0:27}/usr/bin/asm'..."
rm "${PREFIX:0:27}/usr/bin/asm"
echo "Done"
