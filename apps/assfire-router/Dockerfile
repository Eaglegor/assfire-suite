FROM amd64/alpine:3.12.0

RUN apk add --no-cache bash libstdc++ rpm

COPY ./assfire-${CI_COMMIT_REF_NAME}-Linux-router.rpm /root/
RUN rpm -i --nodeps assfire-${CI_COMMIT_REF_NAME}-Linux-router.rpm

COPY ./assfire-${CI_COMMIT_REF_NAME}-Linux-router-reverse-proxy.rpm /root/
RUN rpm -i --nodeps assfire-${CI_COMMIT_REF_NAME}-Linux-router-reverse-proxy.rpm

ENTRYPOINT assfire-router