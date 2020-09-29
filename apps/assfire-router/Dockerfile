FROM amd64/alpine:3.12.0

ARG APPLICATION_VERSION

RUN apk add --no-cache bash libstdc++ rpm

COPY ./assfire-${APPLICATION_VERSION}-Linux-router.rpm /root/
RUN rpm -i --nodeps assfire-${APPLICATION_VERSION}-Linux-router.rpm

COPY ./assfire-${APPLICATION_VERSION}-Linux-router-reverse-proxy.rpm /root/
RUN rpm -i --nodeps assfire-${APPLICATION_VERSION}-Linux-router-reverse-proxy.rpm

ENTRYPOINT assfire-router