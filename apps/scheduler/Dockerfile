FROM amd64/alpine:3.12.0

RUN apk add --no-cache bash libstdc++ rpm

COPY ./assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-scheduler-server.rpm /root/
RUN rpm -i --nodeps /root/assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-scheduler-server.rpm

COPY ./assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-scheduler-grpc-gateway.rpm /root/
RUN rpm -i --nodeps /root/assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-scheduler-grpc-gateway.rpm
