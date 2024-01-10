# 本地构建

### 0. 克隆仓库

``` powershell
git clone <git-repository> roc-master
```

### 1. 安装Chocolatey

```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

**需要管理员权限**

### 2. 安装依赖包

``` powershell
choco install packages.config -y
# 安装cmake并添加环境变量
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
```

### 3. 编译打包

``` powershell
cd roc-master
cmake --preset x64-release 
cmake --build build/x64-release --target PackInstaller
```

# Docker构建

```powershell
# 1. 克隆仓库
git clone <git-repository> roc-master
# 2. 进入目录
cd roc-master
# 3. 构建镜像 (耗时较长)
docker build -t buildtools:latest -m 2GB .
# 4. 进入容器
docker run -it --rm  -v "$(pwd):C:\workdir" buildtools:latest 
# 5. cmake config
cmake --preset x64-release 
# 6. 编译并打包
cmake --build build/x64-release --target PackInstaller
```

