FROM amd64/alpine:3.14.3

RUN apk update && apk add --no-cache bash g++ git curl make linux-headers perl npm openssl openssl-dev rpm go git

# Installing cmake (using one of the latest versions that may be not available in official alpine repo)
RUN git clone --depth 1 --branch v3.21.4 https://github.com/Kitware/CMake.git  /opt/cmake && \
       cd /opt/cmake && \
       ./configure && \
       make install && \
       cd / && \
       rm -rf /opt/cmake

# Installing ninja (used to vcpkg buildsystem)
RUN git clone --depth 1 --branch v1.10.2 https://github.com/ninja-build/ninja.git /opt/ninja && \
       cmake -B/opt/ninja/build-cmake -DCMAKE_INSTALL_PREFIX=/usr/ /opt/ninja && \
       cmake --build /opt/ninja/build-cmake && \
       cmake --build /opt/ninja/build-cmake --target install && \
       rm -rf /opt/ninja

# Installing vcpkg (used to manage dependencies)
RUN git clone --branch 2021.05.12 https://github.com/Microsoft/vcpkg.git /opt/vcpkg &&  \
    cd /opt/vcpkg && \
#     git cherry-pick -n 557ecbe253f87d75c6914b9a522ea92fc15eb20f && \
    ./bootstrap-vcpkg.sh --useSystemBinaries && \
    ./vcpkg integrate install

# Installing dependencies
RUN /opt/vcpkg/vcpkg install \
    --triplet x64-linux --host-triplet x64-linux \
    grpc \
    cpp-redis \
    cxxopts \
    spdlog \
    catch2 \
    prometheus-cpp \
    cpprestsdk \
    gtest \
    librabbitmq \
    && \
    rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees


ENV VCPKG_ROOT /opt/vcpkg

ENV GOPATH /root/go
ENV PATH $PATH:/usr/local/go/bin:$GOPATH/bin

RUN npm install -g react-scripts

RUN curl -sSL https://raw.githubusercontent.com/helm/helm/master/scripts/get-helm-3 | bash

