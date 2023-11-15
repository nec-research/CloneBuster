FROM ubuntu:20.04 as sgxbase

USER root
RUN  apt-get update \
  && apt-get install -y wget \
  git \
  make \
  gnupg \
  libssl-dev \
  libcurl4-openssl-dev \
  libprotobuf-dev \
  dkms \
  build-essential \
  python3 \
  && rm -rf /var/lib/apt/lists/*

RUN echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu focal main' > /etc/apt/sources.list.d/intel-sgx.list
RUN wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | apt-key add -

WORKDIR /tmp
RUN apt update && apt upgrade && apt autoremove && apt clean && apt autoclean &&  apt --fix-broken install
RUN wget -qO - https://download.01.org/intel-sgx/sgx_repo/ubuntu/intel-sgx-deb.key | sudo apt-key add -
RUN echo 'deb [arch=amd64] https://download.01.org/intel-sgx/sgx_repo/ubuntu focal main' | sudo tee /etc/apt/sources.list.d/intel-sgx.list
RUN apt-get update
RUN apt-get install -y libsgx-launch libsgx-urts libsgx-launch-dbgsym libsgx-urts-dbgsym
RUN apt-get install -y libsgx-epid libsgx-epid-dbgsym
RUN apt-get install -y libsgx-quote-ex libsgx-quote-ex-dev libsgx-quote-ex-dbgsym
RUN wget -O sgx_installer.bin https://download.01.org/intel-sgx/sgx-linux/2.18/distro/ubuntu20.04-server/sgx_linux_x64_sdk_2.18.100.3.bin
RUN chmod +x sgx_installer.bin
RUN echo -e "no\n/opt/intel" | ./sgx_installer.bin
RUN export SGX_SDK=/opt/intel/sgxsdk
RUN export PATH=$PATH:$SGX_SDK/bin:$SGX_SDK/bin/x64
RUN export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$SGX_SDK/pkgconfig
#if [ -z "$LD_LIBRARY_PATH" ]; then
RUN export LD_LIBRARY_PATH=$SGX_SDK/sdk_libs
WORKDIR /project
RUN git clone https://github.com/SamiraBriongos/Clonebuster.git
WORKDIR ./Clonebuster/source
RUN make
