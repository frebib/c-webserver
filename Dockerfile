FROM alpine:3.4
MAINTAINER Joe Groocock <frebib@gmail.com>

RUN apk add --no-cache git make gcc libc-dev pcre-dev file-dev openssl-dev && \
    git clone https://github.com/frebib/c-webserver.git /webserver && \
    cd /webserver && \
    make all && \
    apk del --no-cache git make gcc

WORKDIR /webserver

CMD ["bin/webserver"]
