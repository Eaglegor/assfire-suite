FROM amd64/alpine:3.12.0

RUN apk add --no-cache bash libstdc++ rpm

COPY ./assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-tsp-server.rpm /root/
RUN rpm -i --nodeps /root/assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-tsp-server.rpm

COPY ./assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-tsp-grpc-gateway.rpm /root/
RUN rpm -i --nodeps /root/assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-tsp-grpc-gateway.rpm
