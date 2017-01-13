#! /bin/sh
ADDRESS=0.0.0.0
PORT=80
DIRECTORY=htdocs

if hash gnome-terminal 2>/dev/null; then
    if (( $PORT < 1024 )); then
        gnome-terminal --tab -e "/bin/bash -c 'sudo ./ogmp $ADDRESS $PORT $DIRECTORY; exec /bin/bash -i'" $ADDRESS $PORT $DIRECTORY
    else
        gnome-terminal --tab -e "/bin/bash -c './ogmp $ADDRESS $PORT $DIRECTORY; exec /bin/bash -i'" $ADDRESS $PORT $DIRECTORY
    fi
elif hash xterm 2>/dev/null; then
    if (( $PORT < 1024 )); then
        sudo xterm -hold -e ./ogmp $ADDRESS $PORT $DIRECTORY
    else
        xterm -hold -e ./ogmp $ADDRESS $PORT $DIRECTORY
    fi
elif hash konsole 2>/dev/null; then
    if (( $PORT < 1024 )); then
        sudo konsole --noclose -e ./ogmp $ADDRESS $PORT $DIRECTORY
    else
        konsole --noclose -e ./ogmp $ADDRESS $PORT $DIRECTORY
    fi
else
    echo "No terminal found"
fi
