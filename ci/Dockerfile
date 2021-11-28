FROM amd64/alpine:3.14.3

RUN apk update && apk add --no-cache bash g++ git curl make linux-headers perl npm openssl openssl-dev rpm go git zip

# Installing cmake (using one of the latest versions that may be not available in official alpine repo)
RUN git clone --depth 1 --branch v3.21.4 https://github.com/Kitware/CMake.git  /opt/cmake && \
       cd /opt/cmake && \
       ./configure && \
       make install && \
       cd / && \
       rm -rf /opt/cmake

# Installing ninja (used by vcpkg buildsystem)
RUN git clone --depth 1 --branch v1.10.2 https://github.com/ninja-build/ninja.git /opt/ninja && \
       cmake -B/opt/ninja/build-cmake -DCMAKE_INSTALL_PREFIX=/usr/ /opt/ninja && \
       cmake --build /opt/ninja/build-cmake && \
       cmake --build /opt/ninja/build-cmake --target install && \
       rm -rf /opt/ninja

# Installing vcpkg (used to manage dependencies)
RUN git clone --branch master https://github.com/Microsoft/vcpkg.git /opt/vcpkg &&  \
    cd /opt/vcpkg && \
#     git cherry-pick -n 557ecbe253f87d75c6914b9a522ea92fc15eb20f && \
    VCPKG_FORCE_SYSTEM_BINARIES=1 ./bootstrap-vcpkg.sh && \
    ./vcpkg integrate install

# Installing dependencies
RUN /opt/vcpkg/vcpkg install grpc --triplet x64-linux --host-triplet x64-linux --no-binarycaching && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install cpp-redis --triplet x64-linux --host-triplet x64-linux --no-binarycaching && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install spdlog --triplet x64-linux --host-triplet x64-linux --no-binarycaching && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install catch2 --triplet x64-linux --host-triplet x64-linux --no-binarycaching && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install prometheus-cpp --triplet x64-linux --host-triplet x64-linux --no-binarycaching && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install cpprestsdk --triplet x64-linux --host-triplet x64-linux --no-binarycaching && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install gtest --triplet x64-linux --host-triplet x64-linux --no-binarycaching && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install librabbitmq --triplet x64-linux --host-triplet x64-linux --no-binarycaching && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install crossguid --triplet x64-linux --host-triplet x64-linux --no-binarycaching && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install cxxopts --triplet x64-linux --host-triplet x64-linux --no-binarycaching && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees

ENV VCPKG_ROOT /opt/vcpkg

ENV GOPATH /root/go
ENV PATH $PATH:/usr/local/go/bin:$GOPATH/bin

RUN go get \
    github.com/grpc-ecosystem/grpc-gateway/v2/protoc-gen-grpc-gateway \
    github.com/grpc-ecosystem/grpc-gateway/v2/protoc-gen-openapiv2 \
    google.golang.org/protobuf/cmd/protoc-gen-go \
    google.golang.org/grpc/cmd/protoc-gen-go-grpc

RUN npm install -g react-scripts

RUN curl -sSL https://raw.githubusercontent.com/helm/helm/master/scripts/get-helm-3 | bash

RUN apk add --no-cache docker