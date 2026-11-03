## 編譯指令

- 需編譯檔案：`main.c`

##### 環境(Docker)

- 創建Image：`docker build -t c-env .`
- 創建Container：`docker run --rm -it -v "${PWD}/src:/app" c-env`

##### 編譯(擇一)

- 編譯指令(Makefile, recommended)：`make run`
- 編譯指令(gcc)：`gcc -Wall -Wextra -O2 main.c -o main -lev -g -fsanitize=address`

## 執行方式

`./main`

## 操作範例

![Demo](/demo.png "操作範例")

## 離開方式

輸入`EXIT`或`exit`
