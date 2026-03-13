# Running BusTub via Docker on Windows
This guide walks you through setting up and running the BusTub database system using Docker on Windows 11, fully aligned with CMU's official Ubuntu 24.04 development environment.

## Prerequisites
### 1. Enable Windows Features
First, enable required Windows components for WSL2 and Docker:
1. Press `Win+X` → select **Windows Terminal (Admin)**
2. Run the following commands to enable features:
   ```powershell
   dism /online /enable-feature /featurename:Microsoft-Hyper-V /All /NoRestart
   dism /online /enable-feature /featurename:Containers /All /NoRestart
   wsl --install --no-distribution  # Install WSL2 kernel (no distro needed)
   ```
3. **Restart your computer** to apply changes

### 2. Install Docker Desktop
1. Download Docker Desktop for Windows: [Docker Official Download](https://www.docker.com/products/docker-desktop/)
2. During installation, check **Use the WSL 2 based engine**
3. Launch Docker Desktop and verify it's running (✅ icon in system tray)

## Step 1: Create Local Directory
Create a dedicated folder for BusTub code (no Chinese/special characters):
1. Open **This PC** → D: drive → New Folder → Name it `bustub` (Path: `D:\bustub`)

## Step 2: Set Up Docker Container
### 1. Pull Ubuntu 24.04 Image
Open Windows Terminal (PowerShell/CMD, no admin required) and run:
```bash
# Pull Ubuntu 24.04 base image (skip if already downloaded)
docker pull ubuntu:24.04

# Create and enter BusTub container (map D:\bustub to /root/bustub in container)
docker run -it --name bustub-dev -v D:\bustub:/root/bustub ubuntu:24.04 /bin/bash
```
You'll see a container prompt (`root@<container-id>:/#`) when successful.

### 2. Install Git (Minimum Dependency)
```bash
# Update apt index and install git
apt-get -y update && apt-get -y install git
```

### 3. Clone BusTub Repository
```bash
# Navigate to mapped directory (syncs with D:\bustub on Windows)
cd /root/bustub

# Clone BusTub repository (code auto-syncs to Windows D:\bustub)
git clone https://github.com/cmu-db/bustub.git .
```

### 4. Install Official Dependencies
```bash
# Enter the official script directory
cd build_support

# Run official package installation script (skip interactive prompt with -y)
./packages.sh -y
```

### 5. Verify Dependencies
```bash
# Verify clang-15 installation
clang-15 --version

# Verify cmake installation
cmake --version
```
- Expected output for clang: `Ubuntu clang version 15.0.x`
- Expected output for cmake: Version ≥ 3.16

## Step 3: Build BusTub with Clang-15
```bash
# Return to BusTub root directory
cd /root/bustub

# Create build directory
mkdir build && cd build

# Configure CMake with clang-15 (Debug mode with AddressSanitizer)
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_C_COMPILER=clang-15 \
      -DCMAKE_CXX_COMPILER=clang++-15 \
      ..

# Build with multi-core compilation (faster)
make -j$(nproc)
```
The build completes successfully when you see `[100%] Built target <module-name>`.

## Step 4: VS Code Integration (Optional)
For a better development experience, connect VS Code to the container:

### 1. Install VS Code Extensions
- [Docker Extension](https://marketplace.visualstudio.com/items?itemName=ms-azuretools.vscode-docker)
- [Remote - Containers Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

### 2. Connect to Container
1. Open VS Code → Click the **Docker icon** in the left sidebar
2. Under **Containers**, find `bustub-dev` (ubuntu:24.04)
3. Right-click → Select **Attach Visual Studio Code**
4. Once connected, open the `/root/bustub` directory in VS Code

### 3. Develop in VS Code
- Edit code directly in VS Code (syncs with `D:\bustub` on Windows)
- Use the VS Code terminal to rebuild:
  ```bash
  cd /root/bustub/build
  make -j$(nproc)
  ```

## Step 5: Verify BusTub Installation
```bash
# In the build directory, run BusTub shell
./bin/bustub-nc-shell

# Test basic SQL commands
CREATE TABLE student (id INT PRIMARY KEY, name VARCHAR, score INT);
INSERT INTO student VALUES (1, 'Alice', 95), (2, 'Bob', 88);
SELECT name, score FROM student WHERE score >= 90;

# Exit shell with Ctrl+D
```

## Step 6: Manage the Container
### Stop the Container (Preserve Data)
```bash
# From Windows Terminal (outside container)
docker stop bustub-dev
```

### Restart the Container
```bash
docker start bustub-dev
docker exec -it bustub-dev /bin/bash -c "cd /root/bustub/build && /bin/bash"
```

### Remove the Container (Permanent Deletion)
```bash
docker stop bustub-dev && docker rm bustub-dev
```

## Notes
1. **Path Synchronization**: Code edited in `D:\bustub` (Windows) auto-syncs to `/root/bustub` (container)
2. **TODO Errors**: `terminate called after throwing an instance of 'std::logic_error'` is expected for unimplemented features (complete BusTub labs to resolve)
3. **Performance**: Allocate ≥4 CPU cores and 8GB RAM in Docker Desktop → Settings → Resources for faster builds
4. **Official Compatibility**: This setup matches CMU's official Ubuntu 24.04 grading environment

## Troubleshooting
- **Clang Warning**: Ensure you specify clang-15 in the CMake command (eliminates GCC warnings)
- **Build Errors**: Delete the build directory and re-run CMake if cached configurations cause issues:
  ```bash
  rm -rf /root/bustub/build
  mkdir /root/bustub/build && cd /root/bustub/build
  cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang-15 -DCMAKE_CXX_COMPILER=clang++-15 ..
  make -j$(nproc)
  ```
- **Directory Not Found**: Always run BusTub binaries from the `/root/bustub/build` directory (binaries are not in the root directory)

---

## License
This guide is provided as-is, aligned with the [BusTub MIT License](https://github.com/cmu-db/bustub/blob/master/LICENSE).