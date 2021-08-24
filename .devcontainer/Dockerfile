ARG VARIANT="buster"
FROM mcr.microsoft.com/vscode/devcontainers/base:0-${VARIANT}

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    python3 \
    python3-pip \
    python3-setuptools \
    python3-wheel  \
    ninja-build \
    clang-format \
    xz-utils \
    make \
    curl \
    ca-certificates 
    
# RUN curl https://sh.rustup.rs -sSf | sh -s -- -y && source $HOME/.cargo/env

# RUN pip3 install --upgrade pip && pip3 install esptool

RUN mkdir -p /home/vscode/waf && \
    curl -o /home/vscode/waf/waf https://waf.io/waf-2.0.22 && \
    chmod 755 /home/vscode/waf/waf && \
    chown -R vscode /home/vscode/waf && \
    echo 'PATH=$PATH:/home/vscode/waf' >> /home/vscode/.profile

RUN cd / && \
    git clone git://github.com/icamgo/xtensa-toolchain.git xtensa-toolchain && \
    cd xtensa-toolchain/release && \
    make linux64 && \
    cd /xtensa-toolchain/release/xtensa-toolchain/xtensa-lx106-elf/xtensa-lx106-elf/bin && \   
    echo 'PATH=$PATH:/xtensa-toolchain/release/xtensa-toolchain/xtensa-lx106-elf/xtensa-lx106-elf/bin' >> /home/vscode/.profile && \
    echo 'PATH=$PATH:/xtensa-toolchain/release/xtensa-toolchain/xtensa-lx106-elf/libexec/gcc/xtensa-lx106-elf/4.8.2' >> /home/vscode/.profile

