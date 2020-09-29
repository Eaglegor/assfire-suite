include(react_utils)
configure_react_project(
        PROJECT_ROOT maptest
        FILES
            maptest/public/*
            maptest/src/*
            maptest/Dockerfile
            maptest/nginx.conf
            maptest/package.json
)
