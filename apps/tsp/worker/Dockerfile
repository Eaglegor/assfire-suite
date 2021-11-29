FROM amd64/alpine:3.14.3

RUN apk add --no-cache bash libstdc++ rpm

COPY ./assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-tsp-worker.rpm /root/
RUN rpm -i --nodeps /root/assfire-${ASSFIRE_APPLICATION_RELEASE_VERSION}-Linux-tsp-worker.rpm

