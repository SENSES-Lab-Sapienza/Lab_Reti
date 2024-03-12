FROM gcc:latest as builder
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN gcc -o client -static client.c

FROM alpine:latest as runtime
COPY --from=builder /usr/src/myapp/client /client
ENTRYPOINT ["./client"]