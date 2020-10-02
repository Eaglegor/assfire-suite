FROM amd64/alpine:3.12.0

ARG APPLICATION_VERSION
ARG APPLICATION_NAME

RUN apk add --no-cache bash libstdc++ rpm

COPY ./assfire-${APPLICATION_VERSION}-Linux-${APPLICATION_NAME}.rpm /root/
RUN rpm -i --nodeps /root/assfire-${APPLICATION_VERSION}-Linux-${APPLICATION_NAME}.rpm

COPY ./assfire-${APPLICATION_VERSION}-Linux-${APPLICATION_NAME}-reverse-proxy.rpm /root/
RUN rpm -i --nodeps /root/assfire-${APPLICATION_VERSION}-Linux-${APPLICATION_NAME}-reverse-proxy.rpm
