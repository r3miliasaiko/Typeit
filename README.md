# TypeIt



A terminal-based typing game built with [FTXUI](https://github.com/ArthurSonzogni/FTXUI). Words teleport across the screen - type them before they escape!

![TypeIt Screenshot](screenshot.png)




## Build from Source

### Prerequisites

- CMake 3.15 or higher
- C++20 compatible compiler

### Windows (Visual Studio 2022)

```powershell
# Clone the repository
git clone https://github.com/r3miliasaiko/Typeit.git
cd Typeit

# Configure using VS2022 preset
cmake --preset windows-vs2022

# Build
cmake --build build/windows-vs2022 --config Release



```
### Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake ninja-build

# Clone and build
git clone https://github.com/r3miliasaiko/Typeit.git
cd Typeit

cmake --preset linux-base
cmake --build --preset linux-build
```







## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) - Terminal UI library
