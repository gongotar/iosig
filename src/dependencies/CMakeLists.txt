#add_subdirectory(qcustomplot)

include(FetchContent)

FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v2.13.4)
FetchContent_MakeAvailable(Catch2)

