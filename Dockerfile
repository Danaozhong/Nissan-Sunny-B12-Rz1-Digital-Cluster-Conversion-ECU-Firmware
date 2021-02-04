FROM cortesja/stm32-cmake:latest

LABEL maintainer="Clemens Zangl" \
      description="Dockerfile for the Nissan Sunny Rz1 digital cluster conversion project" \
      version="0.1"


# Install GCC, Git (to checkout google test), and other helpful tools
RUN apt-get install -y --no-install-recommends \
    git \
    gcc

RUN apt-get autoclean 
RUN apt-get autoremove 
RUN apt-get clean 
RUN rm -rf /var/lib/apt/lists/*

# Clone GoogleTest
RUN git clone https://github.com/google/googletest.git -b release-1.10.0 /home/dependencies/googletest

# Make sure the file can be executed

# Start from a Bash prompt
CMD /bin/bash -c "chmod +x ./buildall.sh && ./buildall.sh"