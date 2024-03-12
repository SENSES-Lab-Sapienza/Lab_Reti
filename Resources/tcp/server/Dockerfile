FROM gcc:latest as builder
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN gcc -o server -static server.c

FROM alpine:latest as runtime
COPY --from=builder /usr/src/myapp/server /server
ENTRYPOINT ["./server"]