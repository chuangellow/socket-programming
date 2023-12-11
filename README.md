# Socket Programming

Some exercises for socket programming

# Build the Docker Image

1. Place your ```server.c``` and ```client.c``` in the same directory as the ```Dockerfile```.
2. Build the Docker image by running the following command in the directory containing your Dockerfile:

```
docker build -t socket-programming .
```

# Running the Containers

1. Run the Server Container:

- Start the server container:
```
docker run -it --rm --name server-container -p 8080:8080 socket-programming
```
This command starts the server and exposes port 8080 for communication.

2. Run the Client Container:

- In a new terminal, start the client container:

```
docker run -it --rm --name client-container --link server-container socket-programming ./client
```