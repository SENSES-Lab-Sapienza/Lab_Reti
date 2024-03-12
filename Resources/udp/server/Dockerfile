FROM gcc:latest as builder
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN gcc -o b_server -static b_server.c

FROM alpine:latest as runtime
COPY --from=builder /usr/src/myapp/b_server /b_server
ENTRYPOINT ["./b_server"]