# Copyright 2023, IVEX NV
# All rights reserved.
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential

FROM ubuntu:22.04 AS cpp_builder

SHELL ["/bin/bash", "-c"]

RUN apt update && apt install -y g++ g++-10 git wget make build-essential && mkdir -p /app

RUN mkdir dependencies && \
    echo "Updating CMAKE..." && \
    wget https://github.com/Kitware/CMake/releases/download/v3.29.3/cmake-3.29.3-linux-x86_64.sh && \
    apt remove cmake && \
    apt purge --auto-remove cmake && \
    bash cmake-3.29.3-linux-x86_64.sh --skip-license --exclude-subdir --prefix=/usr/local && \
    echo "Installing Qt..." && \
    apt update && apt install -y \ 
    qtbase5-dev \
    libqt5xmlpatterns5-dev \
    libxerces-c-dev \
    pkg-config && \
    echo "Dependencies installed." 

RUN echo "Installing gtest..." && \
    git clone https://github.com/google/googletest.git -b release-1.10.0 && \
    cd googletest && \
    mkdir build && \
    cd build && \
    CXX=g++-10 cmake .. && \
    make && \
    make install && \
    echo "Gtest installed."

COPY framework /app/framework


RUN echo "Building framework..." && \
    cd /app/framework && \
    cmake -G "Unix Makefiles" -B./build -S . \
    -DCMAKE_INSTALL_PREFIX="/home/$(whoami)/qc-build" \
    -DENABLE_FUNCTIONAL_TESTS=$TEST_ENABLED  -DXERCES_ROOT="/usr" \
    -DQt5_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt5/" \
    -DQt5XmlPatterns_DIR="/usr/lib/x86_64-linux-gnu/cmake/Qt5XmlPatterns/" && \
    cmake --build ./build --target install --config Release -j4 && \
    cmake --install ./build && \
    # @TODO this is just because DemoCheckerBundle is not installed by default
    cp /app/framework/build/examples/checker_bundle_example/DemoCheckerBundle /home/root/qc-build/bin/ && \
    echo "Done."


# Runtime stage
FROM python:3.11.9-slim-bookworm  as runtime

RUN echo "Installing Qt..." && \
    apt update && apt install -y \ 
    qtbase5-dev \
    libqt5xmlpatterns5-dev \
    libxerces-c-dev \
    pkg-config && \
    echo "Dependencies installed." 

RUN  mkdir -p /app
WORKDIR /app

COPY requirements.txt /app/
RUN pip install -r /app/requirements.txt

COPY src/ /app/src/
COPY --from=cpp_builder /app/framework/doc/ /app/doc
# Copy install directory in runtime image
COPY --from=cpp_builder /home/root/qc-build /app/build

ENV PYTHONUNBUFFERED=1

ENTRYPOINT ["python", "/app/src/runtime/runtime.py", "-config"]
