# cpp_client to interact with ABX Mock Exchange Server for building order book

### Assumptions
* The provided code is written primarily for **Linux** machines.
* It assumes the user has g++ installed and it supports c++14 or above. Also, it assumes the user has make installed
* It assumes the sequence number for packets starts with 1.
* Also based upon the resend request packet structure. It assumes the max possible value for packet sequence number is 255. (since resendSeq field is of size 1 byte (8 bits => 2^8) as given in the assessment)
* In case of failure the reason for failure is logged and the program exits, instead of retries, for simplicity. 

### Setting up Configuration
* The program is configured using the file at location: **configs/config.txt**
* It mainly consist of the below three parameters
```
# ABX server host
HOST="127.0.0.1"

# Server application port
PORT=3000

# output file name
OUTPUT_FILE=response.json
```

### Build instructions
```
# run this in the parent folder where Makefile is present
$ make

# after make succeeds, run the below program to run the binary
$ ./bin/build_order_book

# to clean the binaries run
$ make clean

# for diagnosis, logs are present in logs folder
```

### Expected output
* Once the program succeeds it will write the **json output in the configured file name in the root folder** (in this case, **response.json**)
* In case of failure, the logs will contain the details
about what went wrong during the code run
* The logs are created for each date, for ease of readability
* A **sample output** created during program run with the provide ABX exchange server is given in **sample-output** folder