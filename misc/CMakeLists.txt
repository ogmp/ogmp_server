configure_file(config.ini ../config.ini COPYONLY)

IF(WIN32)
    configure_file(start_server.bat ../start_server.bat COPYONLY)
ELSEIF(LINUX)
    configure_file(run_server.sh ../run_server.sh COPYONLY)
ENDIF()
