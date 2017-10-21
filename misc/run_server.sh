#! /bin/sh
ADDRESS=0.0.0.0
PORT=9000
DIRECTORY=./

if hash gnome-terminal 2>/dev/null; then
    if [ $PORT -lt 1024 ]; then
        gnome-terminal --tab -e "/bin/bash -c 'sudo ./ogmp $ADDRESS $PORT $DIRECTORY; exec /bin/bash -i'"
    else
        gnome-terminal --tab -e "/bin/bash -c './ogmp $ADDRESS $PORT $DIRECTORY; exec /bin/bash -i'"
    fi
elif hash xterm 2>/dev/null; then
    if [ $PORT -lt 1024 ]; then
        sudo xterm -hold -e ./ogmp $ADDRESS $PORT $DIRECTORY
    else
        xterm -hold -e ./ogmp $ADDRESS $PORT $DIRECTORY
    fi
elif hash konsole 2>/dev/null; then
    if [ $PORT -lt 1024 ]; then
        sudo konsole --noclose -e ./ogmp $ADDRESS $PORT $DIRECTORY
    else
        konsole --noclose -e ./ogmp $ADDRESS $PORT $DIRECTORY
    fi
else
    echo "No terminal found"
fi
