set(FILES
        public/*
        src/*
        Dockerfile
        nginx.conf
        package.json
        package-lock.json)
set(TARGET_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/react)

file(GLOB_RECURSE PROJECT_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${FILES})

foreach (file ${PROJECT_FILES})
    configure_file(${file} ${TARGET_DIRECTORY}/${file} @ONLY)
endforeach ()
