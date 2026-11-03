FROM gcc:latest

WORKDIR /app

RUN apt-get update && \
    apt-get install -y libev-dev make gdb && \
    rm -rf /var/lib/apt/lists/*

CMD ["bash"]