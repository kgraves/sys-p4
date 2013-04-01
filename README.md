# A distributed, multi-threaded, key-value store  written in C++
## This project is an academic project from the Fall 2012 semester

This store provides three basic operations: get, put, delete, and also 
only works with string and integer data types

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

### Input files
The input files have a very simple structure. They can contain any number of lines 
that adhere to the following structures:

If the operation is a put, it will adhere to the following syntax:
```text
op_num key_type key num_values list_of_values
```
- op_num = 0 => get, 1 => put, 2 => delete
- key_type = string or integer
- key = a string which represents the key to insert the values under
- num_values = the number of values following, and that should be inserted
- a list of space-separated values to be inserted

an example of a put operation is:
```text
1	integer	925	1	verses
```

If the operation is either a get or delete, it will adhere to the following syntax:
```text
op_num key_type key
```
- op_num = 0 => get, 1 => put, 2 => delete
- key_type = string or integer
- key = a string which represents the key to get or delete, depending on the op_num

an example of a get operation is:
```text
0	string	instruction
```

an example of a delete operation is:
```text
2	integer	724
```

