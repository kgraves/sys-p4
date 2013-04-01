# A distributed, multi-threaded, key-value store  written in C++
## This project is an academic project from the Fall 2012 semester

### Compiling
```bash
make
```

### Usage
There are three main parts to the project:
- master_server
- worker_server
- client

#### master_server
This executable is run as the master server and divys up the incoming requests to 
all available worker servers.

to run master_server:
```bash
$ master_server num_servers
```

- num_servers = the number of worker servers that will be run
Note: being that this is an academic project, run on limited hardware, there is 
a limit of 4 servers one can register.


#### worker_server
This executable is run as a worker and is given requests, by the master server to
respond to.

to run worker_server:
```bash
$ worker_server
```

#### client
This executable is run on the client of the two above servers. If the client is 
run on a separate machine from the server executables, the machine must have 
network and/or internet connectivity

to run master_server:
```bash
$ client input_file
```

- input_file = a text file whose content is described below

