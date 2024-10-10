

# Check and find source repository
include(FetchContent)
 if(IS_DIRECTORY ${TARGET_GIT_LOCAL_PATH})
     message(STATUS "Source code support is local folder ${TARGET_GIT_LOCAL_PATH}")
     FetchContent_Declare(
         Repository
         SOURCE_DIR ${TARGET_GIT_LOCAL_PATH}
     )
 else()
       message(STATUS "Source code from GitHub repo version ${TARGET_GIT_REPOSITORY}")
     FetchContent_Declare(
         Repository
         GIT_REPOSITORY  ${TARGET_GIT_REPOSITORY}
         GIT_TAG ${TARGET_GIT_TAG}
     )
 endif()
 FetchContent_Populate(Repository)

