[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/Cajoc9es)
## CSC 435 Programming Assignment 4 (Fall 2024)
**Jarvis College of Computing and Digital Media - DePaul University**

**Student**: Reyna Royce (Email: rroyce@depaul.edu)  
**Solution programming language**: C++

### Requirements

If you are implementing your solution in C++ you will need to have GCC 14.x and CMake 3.28.x installed on your system.
On Ubuntu 24.04 LTS you can install GCC and set it as default compiler using the following commands:

```
sudo apt install build-essential cmake g++-14 gcc-14 cmake
sudo update-alternatives --remove-all gcc
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 130
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 140
sudo update-alternatives --remove-all g++
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 130
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 140
```

If you are implementing your solution in Java you will need to have Java 21.x and Maven 3.8.x installed on your systems.
On Ubuntu 24.04 LTS you can install Java and Maven using the following commands:

```
sudo apt install maven openjdk-21-jdk
```

### Setup

There are 3 datasets (dataset1_client_server, dataset2_client_server, dataset3_client_server) that you need to use to evaluate the indexing performance of your solution.
Before you can evaluate your solution you need to download the datasets. You can download the datasets from the following link:

https://depauledu-my.sharepoint.com/:f:/g/personal/aorhean_depaul_edu/Ej4obLnAKMdFh1Hidzd1t1oBHY7IvgqXoLdKRg-buoiisw?e=SWLALa

After you finished downloading the datasets copy them to the dataset directory (create the directory if it does not exist).
Here is an example on how you can copy Dataset1 to the remote machine and how to unzip the dataset:

```
remote-computer$ mkdir datasets
local-computer$ scp dataset1_client_server.zip cc@<remote-ip>:<path-to-repo>/datasets/.
remote-computer$ cd <path-to-repo>/datasets
remote-computer$ unzip dataset1_client_server.zip
```

### C++ solution
#### How to build/compile

To build the C++ solution use the following commands:
```
cd app-cpp
mkdir build
cmake -S . -B build
cmake --build build --config Release
```

#### How to run application

To run the C++ server (after you build the project) use the following command:
```
./build/file-retrieval-server <port>
> <list | quit>
```

To run the C++ client (after you build the project) use the following command:
```
./build/file-retrieval-client
> <connect | index | search | quit>
```

To run the C++ benchmark (after you build the project) use the following command:
```
./build/file-retrieval-benchmark <server IP> <server port> <number of clients> [<dataset path>]
```

#### Example (2 clients and 1 server)

**Step 1:** start the server:

Server
```
./build/file-retrieval-server 12345
>
```

**Step 2:** start the clients and connect them to the server:

Client 1
```
./build/file-retrieval-client
> connect 127.0.0.1 12345
Connection successful!
```

Client 2
```
./build/file-retrieval-client
> connect 127.0.0.1 12345
Connection successful!
```

**Step 3:** list the connected clients on the server:

Server
```
> list
client1: 127.0.0.1 5746
client2: 127.0.0.1 9677
```

**Step 4:** index files from the clients:

Client 1
```
> index ../datasets/dataset1_client_server/2_clients/client_1
Completed indexing 68383239 bytes of data
Completed indexing in 2.974 seconds
```

Client 2
```
> index ../datasets/dataset1_client_server/2_clients/client_2
Completed indexing 65864138 bytes of data
Completed indexing in 2.386 seconds
```

**Step 5:** search files from the clients:

Client 1
```
> search at
Search completed in 0.4 seconds
Search results (top 10 out of 0):
> search Worms
Search completed in 2.8 seconds
Search results (top 10 out of 12):
* client1:folder4/Document10553.txt:4
* client1:folder3/Document1043.txt:4
* client2:folder7/Document1091.txt:3
* client1:folder3/Document10383.txt:3
* client2:folder7/folderB/Document10991.txt:2
* client2:folder8/Document11116.txt:1
* client2:folder5/folderB/Document10706.txt:1
* client2:folder5/folderB/Document10705.txt:1
* client2:folder5/folderA/Document10689.txt:1
* client1:folder4/Document1051.txt:1
```

Client 2
```
> search distortion AND adaptation
Search completed in 3.27 seconds
Search results (top 10 out of 4):
* client2:folder7/folderC/Document10998.txt:6
* client1:folder4/Document10516.txt:3
* client2:folder8/Document11159.txt:2
* client2:folder8/Document11157.txt:2
```

**Step 6:** close and disconnect the clients:

Client 1
```
> quit
```

Client 2
```
> quit
```

**Step 7:** close the server:

Server
```
> quit
```

#### Example (benchmark with 2 clients and 1 server)

**Step 1:** start the server:

Server
```
./build/file-retrieval-server 12345
>
```

**Step 2:** start the benchmark:

Benchmark
```
./build/file-retrieval-benchmark 127.0.0.1 12345 2 ../datasets/dataset1_client_server/2_clients/client_1 ../datasets/dataset1_client_server/2_clients/client_2
Completed indexing 134247377 bytes of data
Completed indexing in 6.015 seconds
Searching at
Search completed in 0.4 seconds
Search results (top 10 out of 0):
Searching Worms
Search completed in 2.8 seconds
Search results (top 10 out of 12):
* client1:folder4/Document10553.txt:4
* client1:folder3/Document1043.txt:4
* client2:folder7/Document1091.txt:3
* client1:folder3/Document10383.txt:3
* client2:folder7/folderB/Document10991.txt:2
* client2:folder8/Document11116.txt:1
* client2:folder5/folderB/Document10706.txt:1
* client2:folder5/folderB/Document10705.txt:1
* client2:folder5/folderA/Document10689.txt:1
* client1:folder4/Document1051.txt:1
Searching distortion AND adaptation
Search completed in 3.27 seconds
Search results (top 10 out of 4):
* client2:folder7/folderC/Document10998.txt:6
* client1:folder4/Document10516.txt:3
* client2:folder8/Document11159.txt:2
* client2:folder8/Document11157.txt:2
```

**Step 3:** close the server:

Server
```
> quit
```