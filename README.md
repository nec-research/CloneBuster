# CloneBuster

This is the artifact submission corresponding to paper No Forking Way: Detecting Cloning Attacks on Intel SGX Applications. In our paper, we introduced a clone-detection tool for Intel SGX, dubbed CloneBuster, that uses a cache-based covert channel for enclaves to detect clones (i.e., other enclaves loaded with the same binary). 

Our artifact consists of source code with the instructions to compile the code, run it, and install the software dependencies. It requires a machine that supports SGX 1.0 and the Intel SDK installed. Our artifact has been tested on Ubuntu 18.04 and 20.04, with the SDK version 2.18 (although it should work with other versions of the SDK). Details about the hardware (cache size an associativity) are hardcoded. Once the code is compiled, the application launches the enclave and provides the enclave with details about the number of monitored ways of the cache as input. Then, the enclave creates all the eviction sets and the covert channel and monitors it when triggered. As output, the enclave writes a file with the cache samples (reading times of the data in the eviction sets).

## Organization of the repository

`Warning: this is a proof-of-concept, it is mainly useful for collecting data and evaluating it as done in the CloneBuster paper. Use it under your own risk.`

This code was tested on Ubuntu 18.04 and 20.04 on different CPU models.
It requires SGX to be enabled in the BIOS of a capable machine and the Intel SDK to be installed. Further details on how to do that are included at the Installation section of this readme so the interested user can install them from scratch. 

The folders are organized as follows

* App contains the untrusted app that communicates with the enclave
* Enclave contains the code where all the relevant functions to create the eviction sets and monitor the covert channel are coded
* Include is not relevant for this example
* Evaluation includes scripts to filter and label the obtained data

It also includes a Makefile that compiles the enclave and the app.

## Architecture details

We note that in order to work properly, the hardware details referring to each machine need to be changed in 

Enclave/ThreadLibrary/cache_details.h

```

/*Cache and memory architecture details*/
#define CACHE_SET_SIZE 16 //Ways
#define CACHE_SLICES 12
#define SETS_PER_SLICE 1024
#define BITS_SET 10
#define BITS_LINE 6 //64 bytes per cache line

```

or given to the enclave as input (in a real setting should be provided during attestation).

The required information can by obtained by launching:

`$ cat /proc/cpuinfo`

Concretely, the values needed for CloneBuster can be obtained in the following way:

```
cache_size to get the slices -> $ cat /proc/cpuinfo | grep "cache size" | head -n 1
CACHE_SET_SIZE -> $ cpuid | grep -A 9 "cache 3" | grep "ways" | head -n 1
CACHE_SLICES -> CACHE_SIZE(KB)/1024 
```

Further, there are other parameters that have to be adjusted depending on the experiment that one needs to execute, 
those are all given as #defines, and are split among different files including ThreadTest.h, ThreadTest.cpp and Enclave.cpp

## Installation

The folder includes a Makefile that will get the code compiled and generate an enclave and the executable app. In order to do that, go to the source folder and execute make.

```bash
cd source
make
```

### Pre-requisite

```
This application requires SGX to be enabled in the BIOS of a capable machine, and the Intel SGX driver and SDK to be installed.
```

### How to install SGX and related packages

As per the Intel manual, first of install the following packages:

```bash
sudo apt-get install libssl-dev libcurl4-openssl-dev libprotobuf-dev dkms build-essential python
```

Then, download and install the Intel SGX driver, SDK, and PSW (Version 2.18). Note that this requires sudo privileges. Further, the example is given for Ubuntu 20.04, for another distro the corresponding values need to be adjusted. For the correct ones check on

*https://download.01.org/intel-sgx/sgx-linux/2.18/distro/*

```bash
wget https://download.01.org/intel-sgx/sgx-linux/2.18/distro/ubuntu20.04-server/sgx_linux_x64_driver_2.11.054c9c4c.bin
chmod +x sgx_linux_x64_driver_2.11.054c9c4c.bin
sudo ./sgx_linux_x64_driver_2.11.054c9c4c.bin
wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -
echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu focal main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
sudo apt-get update
sudo apt-get install libsgx-launch libsgx-urts libsgx-launch-dbgsym libsgx-urts-dbgsym
sudo apt-get install libsgx-epid libsgx-epid-dbgsym
sudo apt-get install libsgx-quote-ex libsgx-quote-ex-dev libsgx-quote-ex-dbgsym
wget -O sgx_installer.bin https://download.01.org/intel-sgx/sgx-linux/2.18/distro/ubuntu20.04-server/sgx_linux_x64_sdk_2.18.100.3.bin
chmod +x sgx_installer.bin
echo -e "no\n/opt/intel" | sudo ./sgx_installer.bin
source /opt/intel/sgxsdk/environment
```
After installing the SGX driver, the machine has to be rebooted.

# How to use CloneBuster

In this version of the code, app should be called with an output_file as an argument. All the measurements and data 
will be then written to that file. 

The amount of data that is written for each of the experiments is hardcoded in `#define REPS 120`, each repetition simulates the execution of a protected application. This value is located and can be edited at:

Enclave/Enclave.cpp

Once the app is running, it will permanently run (while (1)) and request for user input M that refers to the number of ways to be monitored out of the total number of ways for each set of experiments. If no input is given, no output will be generated. 
As a result, the app has to be manually killed once the desired experiments have been executed. 
**Warning** The same instance can be used for multiple experiments. This avoids generating and validating the eviction sets for every single experiment as they take time, and this step is expected to be executed just once in a realistic environment, i.e. the enclave does not need to be finished between consecutive calls.

```
**NOTE** Building the Spoiler sets takes around 4 minutes, however building the eviction sets takes different amounts of
time depending on the architecture (processors with a number of cores that is a power of 2 take less time to build them), 
the noise on the system and how fragmented is the SGX memory (this is related with the elapsed time since the last reboot 
and the number of executions of any SGX application) and the tests performed to make sure eviction sets are correct. The time 
might range between 10 to 60 minutes
```
**NOTE** In order to generate and analyze all the data for all the experiments described in the paper, the amount of time is significantly high (days). Therefore, in order to get some basic results we recommend to just get the data for the most basic scenarios (the ones right after this paragraph) for just one configuration option (i.e. one value of m), and then go to the How to evaluate the results section.

## No clones and no other applications(baseline)

The enclave is called by executing

```bash
./app output_file.txt
```

Where `output_file.txt` could be any name. Note that this file will include all the measurements. If different experiments are executed one after the other, the file has to be manually partitioned. A script that helps with that task is provided in the Evaluation folder, but it requires the first and the last line of the experiment in the `output_file.txt`. 

**NOTE** If many experiments (different values of m) are going to be tested and data is going to be analyzed, please execute

```bash
wc output_file.txt
```
and write down the value for each of them. The value of the initial and last lines of the data corresponding to the experiment
will be needed later for preparing the files for the evaluation.

Once the app starts, it asks for some input. That input refers to the number of monitored ways (m in the paper). For the initial case we could use for example a value of 14 (could be any between 9 and 16). If it is the first time the app is executed (initialization) it will generate and validate the eviction sets and it will output the following message once is is ready.

```
Eviction sets have been created and the app is ready for input
```
From this moment, once any input is given the results will be written in the file.

## Clones (1 to n) and no other applications

Since this version of the repository does not synchronize the execution of the 2 clones, (it would ease the execution of clone attacks) we use an approximation, and launch one initial instance in normal mode (as described before and could be that one) 
and then a second one in attack mode. The second instance should be launched once the first one is already running and waiting 
for new input, otherwise both instances will generate conflicts when trying to create and validate the eviction sets, and 
as a result they might not succeed (i.e. the application will not run).

**NOTE** If one wishes to use the same regular instance that was used in the previous case, and in order to ease the labelling,
please execute

```bash
wc output_file.txt
```
and write the value of the output. It will be needed later for preparing the files for the evaluation

For the second instance go to `Enclave/Enclave.cpp` and change the hardcoded value `#define ATTACK 0`, the 0 has to be changed 
to 1, and then needs to be compiled again. Therefore one should execute:

```bash
make
./app output_file_attack.txt
```

Then provide the same input as in the regular instance (same value of m)

As in the previous case, the instance in attack mode will run permanently and has to be killed manually


## Clones and other applications

As in the previous case with clones, we need a second instance with `#define ATTACK 1`, that has to be compiled and executed.

Once the attack instance is running and has finished with the eviction sets, open a different terminal and execute:

```bash
cd Noise
./launch_noise_instance.sh
```

# How to evaluate the results

In order to analyze and evaluate the results using the collected data, the `Evaluation` folder includes some scripts that 
first will generate a file including normal and attack samples (with clones) and another one that will use that file as input
to train and test the different detection algorithms evaluated in the paper.

## Prepare the data

The provided scripts will filter out the data from `output_file.txt` obtained when executing app, and get a labeled csv file 
for its evaluation. In order to do so execute:

```bash
cd Evaluation
./prepare_normal_samples.sh raw_data clean_data.csv ini end
./prepare_attack_samples.sh raw_data clean_data.csv ini end
```
Where `raw_data` is the aforementioned `output_file.txt`, `clean_data.csv` is the name of the file where the data ready to be analyzed
will be written, `ini` and `end` refer respectively to the first and last line corresponding to the experiment to be evaluated. **NOTE** that this means same conditions, same value of m, with and without a clone.

For example, if one executes 

```bash
./app output_file.txt
```
with m=14 and then gets the number of lines in output_file.txt and is equal to 1000, for the normal set of samples:

```bash
cd Evaluation
./prepare_normal_samples.sh output_file.txt clean_data.csv 0 1000
```

Then with the attack running, the regular app is executed with m=14 again, so the lines in output_file.txt is now equal to 2000,
then, the final version of the clean file is obtained after executing 

```bash
./prepare_attack_samples.sh output_file.txt clean_data.csv 1000 2000
```

## Visual inspection

If everything has been executed correctly, and the generated csv file is opened, it should be possible to visually see that 
if the last column is equal to '1'(attack) there is a great number of misses or high values on the initial columns of the same row.
On the contrary, when there is no attack the number of misses should be low.


## Tested systems

This code has been successfully tested on:

* Intel(R) Xeon(R) E-2176G CPU @ 3.70GHz
	* L3: 12 MB
	* Associativity: 12
	* Cache sets: 12288
	* Slices: 12
* Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz
	* L3: 8MB
	* Associativity: 16
	* Cache sets: 8192
	* Slices: 8

