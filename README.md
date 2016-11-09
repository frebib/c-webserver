# Webserver in C

This small multi-client webserver is written as an assignment submission.
It can handle many concurrent connections and implements a basic subset of the [HTTP/1.1 standard according to RFC 2616](https://www.ietf.org/rfc/rfc2616.txt)

### Features (Roadmap)
* [x] Basic HTTP GET/HEAD commands
* [x] 200, 403 & 404 responses for files
* [x] File MIME type detection via [libmagic](https://github.com/file/file)
* [x] SSL/TLS support via [OpenSSL](https://github.com/openssl/openssl)
* [x] Configurable servers via config file
* [x] Default error pages
* [ ] Index pages

### How to run the webserver
Start by editing the configuration file, modifying servers as required.
For SSL, ensure the certificate paths are correct. All paths are relative to the webserver binary.

Ensure the config is readable from the working directory when the server starts. The certificates will be read upon the first connection to the server, so make sure to test it!

Specify the config on the command line with the `-c` flag: `webserver -d -c <config-file>` (Adding `-d` for optional daemon mode, if not specified in the config)

### Dependencies
The following libraries are required for this program to build and run:

 * [libmagic (file)](https://github.com/file/file) - Used for MIME type detection of files.  
    _Packaged as `file` or `file-dev` typically._
 * [pcre (Perl Compatible Regular Expressions)](http://www.pcre.org/) - Used for.. regex support.  
    _Packaged as `pcre`, `libpcre` or `pcre-dev` typically._
 * [OpenSSL](https://github.com/openssl/openssl) - Used for HTTPS SSL/TLS connections.  
    _Packaged as `openssl` or `openssl-dev` typically._

All of the above should be available in your package manager as `<package>` or `<package>-dev`.


### Building & Running
It can be built and run with the following command:

```
make all run
```

---

The simplest way to run this program is via [Docker](https://www.docker.com/). You can download run the pre-built image from [DockerHub (frebib/c-webserver)](https://hub.docker.com/r/frebib/c-webserver/) by omitting the `docker build` step; Docker will pull the image from the repository instead.  

Build the container and run it with the following commands:

```
docker build -t frebib/c-webserver .
docker run -d --name webserver -p 8088:8088 frebib/c-webserver
```

To view the output, view the logs:
```
docker logs -f webserver
```
