FROM amd64/alpine:3.12.0

RUN apk add --no-cache bash libstdc++ rpm

COPY ./assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-sample-server.rpm /root/
RUN rpm -i --nodeps /root/assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-sample-server.rpm

COPY ./assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-sample-reverse-proxy.rpm /root/
RUN rpm -i --nodeps /root/assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-sample-grpc-gateway.rpm
