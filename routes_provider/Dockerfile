FROM amd64/alpine:3.12.0

COPY . /opt/assfire-router/
RUN apk add --no-cache bash libstdc++

ENTRYPOINT /opt/assfire-router/bin/assfire-router