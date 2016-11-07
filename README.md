# Webserver in C

This small multi-client webserver is written as an assignment submission.
It can handle many concurrent connections and implements a basic subset of the [HTTP/1.1 standard according to RFC 2616](https://www.ietf.org/rfc/rfc2616.txt)

It can be built and run with the following command:

```
make all run
```

---

The simplest way to run this program is via [Docker](https://www.docker.com/). Build the container and run it with the following commands:

```
docker build -t frebib/c-webserver .
docker run -d --name webserver -p 8088:8088 frebib/c-webserver
```

To view the output, view the logs:
```
docker logs -f webserver
```
