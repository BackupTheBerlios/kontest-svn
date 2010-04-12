find_package(MSGFMT)

if(MSGFMT_FOUND)
 file(GLOB PO_FILES *.po)
 add_translations(kontest ${PO_FILES})
endif(MSGFMT_FOUND)
