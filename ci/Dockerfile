FROM amd64/alpine:edge

RUN apk update && apk add --no-cache bash g++ git curl make linux-headers perl npm openssl openssl-dev rpm go git

# Installing cmake
RUN git clone --depth 1 --branch v3.19.5 https://github.com/Kitware/CMake.git  /opt/cmake && \
       cd /opt/cmake && \
       ./configure && \
       make install && \
       cd / && \
       rm -rf /opt/cmake

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
RUN /opt/vcpkg/vcpkg install catch2 --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install prometheus-cpp --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install cpprestsdk --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees
RUN /opt/vcpkg/vcpkg install gtest --triplet x64-linux && rm -rf /opt/vcpkg/downloads /opt/vcpkg/buildtrees

ENV VCPKG_ROOT /opt/vcpkg

RUN go get -v \
    github.com/grpc-ecosystem/grpc-gateway/protoc-gen-grpc-gateway \
    github.com/grpc-ecosystem/grpc-gateway/protoc-gen-swagger \
    github.com/golang/protobuf/protoc-gen-go

ENV GOPATH /root/go
ENV PATH $PATH:/usr/local/go/bin:$GOPATH/bin

RUN npm install -g react-scripts

RUN curl -sSL https://raw.githubusercontent.com/helm/helm/master/scripts/get-helm-3 | bash

