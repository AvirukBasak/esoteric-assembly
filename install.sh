#!/bin/bash
checkAndInst() {
    if ! which "$1" > /dev/null
    then
        echo "Package $2 is not installed!"
        read -p "Install? (y/n) "
        if [ "$REPLY" = "y" ]
        then
            if which "apt" > /dev/null
            then
                apt install "$2"
            elif which "apt-get" > /dev/null
            then
                apt-get install "$2"
            elif which "pacman" > /dev/null
            then
                pacman -S "$1"
            elif which "yum" > /dev/null
            then
                yum install "$1"
            else
                echo "No supported package manager found"
                echo "Installation failed! :("
                exit 1
            fi
            echo
        else
            echo "Installation failed! :("
            exit 1
        fi
    fi
    if ! which "$1" > /dev/null
    then
        echo "Installation failed! :("
        exit 1
    fi
}

if ! [ "$(id -u)" = 0 ] && [[ "${PREFIX:0:27}" != *"com.termux"* ]]
then
    echo "Please execute install.sh as root"
    echo "Installation failed! :("
    exit 1
fi

VER="2022.5.14"
SIZE="30"

checkAndInst "gcc" "clang"
checkAndInst "make" "make"

echo "Compiling to '${PREFIX:0:27}/usr/bin/asm'..."
make
cp bin/asm "${PREFIX:0:27}/usr/bin/asm"
echo "Setting permission 755..."
chmod 755 "${PREFIX:0:27}/usr/bin/asm"

# If termux, then tec the binary to prevent 
# linker warnings on android 5 or 7 devices
if [[ "${PREFIX:0:27}" = *"com.termux"* ]]
then
    echo 
    checkAndInst "termux-elf-cleaner" "termux-elf-cleaner"
    echo "Running 'termux-elf-cleaner'..."
    termux-elf-cleaner "${PREFIX:0:27}/usr/bin/asm"
fi
echo 
echo "Success! Installed v$VER, $SIZE KB"
echo "Run 'asm' to execute"
echo "Done"
