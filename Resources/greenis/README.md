# README

Install redis client:
```
pip3 install -r requirement.txt
```

In a shell run:
```
docker run -p 7379:6379 redis/redis-stack-server:latest
```

Sniff traffic on port 7379
```
sudo tcpdump -i any -v port 7379 -w redis.pcap
```

Wireshark reverse-engineer the protocol:
```
wireshark redis.pcap
```

# Resources

Run redis with Docker:
https://redis.io/docs/install/install-stack/docker/#redisredis-stack-server

Redis serialization protocol specification
https://redis.io/docs/reference/protocol-spec/
