FROM alpine:3.4
MAINTAINER Joe Groocock <frebib@gmail.com>

RUN apk add --no-cache git make gcc libc-dev pcre-dev file-dev openssl-dev jansson-dev && \
    git clone https://github.com/frebib/c-webserver.git /webserver && \
    cd /webserver && \
    make all && \
    apk del --no-cache git make gcc

RUN mkdir -p /srv/http /etc/webserver && \
    adduser -h /srv/http -u 105 -s /bin/false -D http

USER http
WORKDIR /srv/http

VOLUME /srv/http

CMD ["/webserver/bin/webserver"]
