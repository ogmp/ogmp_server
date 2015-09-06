![Logo](http://ogmp.net/logo.png)

# OGMP
OGMP is the **O**ver**G**rowth **M**ulti**P**layer mod, an unoffical addition to the upcoming game [Overgrowth](http://www.wolfire.com/overgrowth) by Wolfire Games.

## Installation
At the moment you will need a Linux machine to host your own server. To use this project you have to compile it first:

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

If you just want to play with friends you do not need your own server though, you can use the public server [ogmp.net](http://ogmp.net/).

## Usage
To start a server you have to run the ogmp binary:

    ./ogmp [bind address] [port] [htdocs]
    e.g. ./ogmp 0.0.0.0 9000 htdocs

## Bugs
This mod is still early alpha and buggy. If you encounter a bug it would be nice to send us information about it via the issue tracker of Github. Please try to avoid duplicates though.
