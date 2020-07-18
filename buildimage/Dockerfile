FROM amd64/alpine:3.12.0

RUN apk add --no-cache bash g++ cmake git curl make linux-headers perl

# Installing ninja
RUN git clone --depth 1 https://github.com/ninja-build/ninja.git /opt/ninja && \
       cmake -B/opt/ninja/build-cmake -DCMAKE_INSTALL_PREFIX=/usr/ /opt/ninja && \
       cmake --build /opt/ninja/build-cmake && \
       cmake --build /opt/ninja/build-cmake --target install && \
       rm -rf /opt/ninja

# Installing vcpkg (used to manage dependencies)
RUN cd /opt && \
    git clone https://github.com/Microsoft/vcpkg.git &&  \
    cd vcpkg && \
    ./bootstrap-vcpkg.sh --useSystemBinaries && \
    ./vcpkg integrate install

# Installing dependencies
RUN /opt/vcpkg/vcpkg install grpc --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install cpp-redis --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install cxxopts --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install spdlog --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install boost-uuid --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install catch2 --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install prometheus-cpp --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees

ENV VCPKG_ROOT /opt/vcpkg
