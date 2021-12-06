FROM amd64/alpine:3.14.3

RUN apk add --no-cache bash libstdc++ rpm

COPY ./assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-locations-server.rpm /root/
RUN rpm -i --nodeps /root/assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-locations-server.rpm

COPY ./assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-locations-grpc-gateway.rpm /root/
RUN rpm -i --nodeps /root/assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-locations-grpc-gateway.rpm
