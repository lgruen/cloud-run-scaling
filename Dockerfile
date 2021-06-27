FROM debian:bullseye-slim AS builder

# Don't ask for time zone during apt install.
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && \
    apt install -y curl build-essential clang pkg-config autoconf libtool && \
    # Compile libmicrohttpd without TLS, to allow static linkage.
    curl -L curl https://mirror.downloadvn.com/gnu/libmicrohttpd/libmicrohttpd-0.9.73.tar.gz | tar xz && \
    cd libmicrohttpd-0.9.73 && \
    ./configure --disable-https && \
    make install && \
    cd / && \
    curl -L https://github.com/etr/libhttpserver/archive/refs/tags/0.18.2.tar.gz | tar xz && \
    cd libhttpserver-0.18.2 && \
    ./bootstrap && \
    mkdir build && \
    cd build && \
    ../configure && \
    make install

COPY main.cc /build/

RUN cd /build && \
    clang++ -std=c++20 -Wall -O3 -static $(pkg-config --cflags libhttpserver) -o main main.cc $(pkg-config --libs --static libhttpserver)

FROM scratch

COPY --from=builder /build/main .

CMD ["./main"] 