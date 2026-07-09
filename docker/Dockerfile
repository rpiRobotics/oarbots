FROM ros:jazzy-ros-base-noble

LABEL org.opencontainers.image.source=https://github.com/rpiRobotics/oarbots
LABEL org.opencontainers.image.description="Image for building and running the rpiRobotics/oarbots project"
LABEL org.opencontainers.image.licenses=Apache-2.0


RUN apt update

# Install Azure SDK

# Install dependencies for libsoundio1
RUN apt -y install libgl1 libxrandr2 libxinerama1 libxcursor1 libjack-jackd2-0 libasound2t64 libpulse0

RUN mkdir /tmp/kinect

# Download the packages
RUN curl -sSL https://packages.microsoft.com/ubuntu/18.04/prod/pool/main/libk/libk4a1.4/libk4a1.4_1.4.1_amd64.deb > /tmp/kinect/libk4a1.4_1.4.1_amd64.deb
RUN curl -sSL https://packages.microsoft.com/ubuntu/18.04/prod/pool/main/libk/libk4a1.4-dev/libk4a1.4-dev_1.4.1_amd64.deb > /tmp/kinect/libk4a1.4-dev_1.4.1_amd64.deb
RUN curl -sSL https://packages.microsoft.com/ubuntu/18.04/prod/pool/main/libk/libk4abt1.1/libk4abt1.1_1.1.2_amd64.deb > /tmp/kinect/libk4abt1.1_1.1.2_amd64.deb
RUN curl -sSL https://packages.microsoft.com/ubuntu/18.04/prod/pool/main/libk/libk4abt1.1-dev/libk4abt1.1-dev_1.1.2_amd64.deb > /tmp/kinect/libk4abt1.1-dev_1.1.2_amd64.deb
RUN curl -sSL http://ftp.de.debian.org/debian/pool/main/libs/libsoundio/libsoundio1_1.1.0-1_amd64.deb > /tmp/kinect/libsoundio1_1.1.0-1_amd64.deb
RUN curl -sSL https://packages.microsoft.com/ubuntu/18.04/prod/pool/main/k/k4a-tools/k4a-tools_1.4.1_amd64.deb > /tmp/kinect/k4a-tools_1.4.1_amd64.deb

# Automatically accept EULA; used for automated installs in Docker containers and GitHub Workflows
RUN echo 'libk4a1.4 libk4a1.4/accepted-eula-hash string 0f5d5c5de396e4fee4c0753a21fee0c1ed726cf0316204edda484f08cb266d76' | debconf-set-selections
RUN echo 'libk4abt1.0	libk4abt1.1/accepted-eula-hash	string	03a13b63730639eeb6626d24fd45cf25131ee8e8e0df3f1b63f552269b176e38' | debconf-set-selections

# Install packages
RUN dpkg -i /tmp/kinect/libk4a1.4_1.4.1_amd64.deb
RUN dpkg -i /tmp/kinect/libk4a1.4-dev_1.4.1_amd64.deb
RUN dpkg -i /tmp/kinect/libk4abt1.1_1.1.2_amd64.deb
RUN dpkg -i /tmp/kinect/libk4abt1.1-dev_1.1.2_amd64.deb

# libsoundio1 is a dependency of k4a-tools
RUN dpkg -i /tmp/kinect/libsoundio1_1.1.0-1_amd64.deb

RUN dpkg -i /tmp/kinect/k4a-tools_1.4.1_amd64.deb

# Remove temporary folder
RUN rm -rf /tmp/kinect