docker create -it --name arm_test linaro/wine-arm64
docker start arm_test
docker exec -it arm_test apt-get install -y --no-install-recommends xvfb at-spi2-core
docker cp ./tests arm_test:/
docker exec -it arm_test wine-arm64 cmd.exe /c '/tests/json_check/json_check.exe /tests/json_check/broken.json /tests/json_check/gui_definition.json'
docker exec -it arm_test wine-arm64 cmd.exe /c '/tests/exe_container/exe_container.exe /tests/json_check/gui_definition.json'
docker exec -it arm_test wine-arm64 cmd.exe /c '/tests/main_frame/main_frame.exe /tests/json_check/gui_definition.json /tests/main_frame/config_ascii.json /tests/main_frame/config_utf.json'
docker stop arm_test
docker container prune


docker pull scottyhardy/docker-wine:arm64
docker create -i --platform=linux/arm64 --name wine_test --mount type=bind,source="$(pwd)"/tests,target=/tests --entrypoint=sh scottyhardy/docker-wine:arm64
docker start wine_test
docker cp ./tests wine_test:/
docker exec -i wine_test xvfb-run wine cmd.exe /c '/tests/json_check/json_check.exe /tests/json_check/broken.json /tests/json_check/gui_definition.json'
/usr/bin/qemu-aarch64-static wine cmd.exe /c '/tests/json_check/json_check.exe /tests/json_check/broken.json /tests/json_check/gui_definition.json'
/usr/bin/qemu-aarch64-static /opt/wine-arm64/wine cmd.exe /c 'echo hello'


docker build -t wine_test ./
docker create -it --name wine_test --mount type=bind,source="$(pwd)"/tests,target=/tests --entrypoint=sh wine_test
docker exec -i wine_test qemu-aarch64-static /opt/wine-arm64/wine cmd.exe /c 'echo hello'


docker run -it --name wine_test --mount type=bind,source="$(pwd)"/tests,target=/tests -v /usr/bin/qemu-aarch64-static:/usr/bin/qemu-aarch64-static --entrypoint=sh scottyhardy/docker-wine:arm64

docker run -it --name wine_test --platform linux/arm64 --mount type=bind,source="$(pwd)"/tests,target=/tests --entrypoint=sh scottyhardy/docker-wine:arm64
docker run --rm --privileged multiarch/qemu-user-satic:register

qemu-aarch64-static /usr/bin/xvfb-run /usr/local/bin/wine cmd.exe /c 'echo hello'
qemu-aarch64-tatic /usr/local/bin/wine cmd /c 'echo hello'


echo deb http://http.kali.org/kali​ kali-rolling main contrib non-free >> /etc/apt/sources.list
ecbo deb http://old.kali.org/kali​ sana main non-free contrib >> /etc/apt/sources.list
echo deb http://old.kali.org/kali​ moto main non-free contrib >> /etc/apt/sources.list

## Test Status

-   :heavy_check_mark: : Tested
-   :warning: : Tested once, but not with the latest build
-   :x: : Not tested

| OS | Arch | libc | Build | Manual Test | CI Test |
| :--: | :--: | :--: | :--: | :--: | :--: |
| Windows10 | x64 |  | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Windows10 | arm64 |  | :heavy_check_mark: | :x: | :heavy_check_mark: |
| Windows8 | x64 |  | :x: | :warning: | :x: |
| macOS11 | x64 |  | :heavy_check_mark: | :x: | :heavy_check_mark: |
| macOS11 | arm64 |  | :heavy_check_mark: | :x: | :x: |
| macOS10.15 | x64 |  | :heavy_check_mark: | :heavy_check_mark: | :x: |
| macOS10.12 | x64 |  | :x: | :warning: | :x: |
| Ubuntu20.04 | x64 | glibc | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| Ubuntu20.04 | arm64 | glibc | :heavy_check_mark: | :x: | :heavy_check_mark: |
| Ubuntu18.04 | x64 | glibc | :x: | :warning: | :x: |
| Fedora32 | x64 | glibc | :x: | :warning: | :x: |
| Alpine3.16 | x64 | musl | :heavy_check_mark: | :x: | :heavy_check_mark: |
| Alpine3.16 | arm64 | musl | :heavy_check_mark: | :x: | :heavy_check_mark: |
| Alpine3.18 | x64 | musl | :x: | :warning: | :x: |
