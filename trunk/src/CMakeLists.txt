
#include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ../build )

set(kontest_SRCS
    main.cpp
    mainwindow.cpp
    logwindow.cpp
    partialswindow.cpp
    dxclusterwindow.cpp
    qso.cpp
    logbook.cpp

)

#kde4_add_ui_files(kontest_SRCS kontest.ui )
#qt4_add_resources (kontest_SRCS ../kontest-resources.qrc )
kde4_add_executable(kontest ${kontest_SRCS})

target_link_libraries(kontest ${KDE4_KDEUI_LIBS} ${KDE4_KDECORE_LIBS} hamlib)

install(TARGETS kontest DESTINATION ${BIN_INSTALL_DIR})
#install( FILES kontest.rc
#         DESTINATION  ${DATA_INSTALL_DIR}/kontest )

install( FILES kontest.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR} )
install( FILES kontest.rc  DESTINATION  ${DATA_INSTALL_DIR}/kontest )