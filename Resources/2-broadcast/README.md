# README

You need an account on docker hub to deliver the homework.
It is a public registry of docker images where you can push to 
distribute your container images: https://hub.docker.com/

# The project

You can find inspiration from the project in the udp/client and udp/server 
directory (docker file included)

# Build

In order to build and run the application use the "luncher" but first 
change variables in the beginning of the file with your directory and
the number of nodes to have
```
python3 launch.py
```

The script creates a docker compose that lauch executing the following:
```
docker compose up --build
```

You can even stop the launcher and launch the script again after 
modification using the same command