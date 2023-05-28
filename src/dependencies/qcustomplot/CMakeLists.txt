add_library(qcustomplot SHARED
        qcustomplot.cpp
        qcustomplot.h)

target_link_libraries(qcustomplot PUBLIC ${Qt5Core_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5PrintSupport_LIBRARIES})
target_include_directories(qcustomplot PUBLIC ${Qt5Core_INCLUDE_DIRS})

target_compile_options(qcustomplot PUBLIC -Wno-all -Wno-extra -Wno-pedantic -Wno-conversion -Wno-deprecated-declarations -O3)