# Szkolenie - C++20 #

## Ankieta

* https://forms.gle/2oNo6KFwBvg4EkZd7

## Docs

* https://infotraining.github.io/docs-cpp-20

## Konfiguracja środowiska

Proszę wybrać jedną z poniższych opcji:

### Lokalna

Przed szkoleniem należy zainstalować jeden z kompilatorów wspierających C++20:

#### Visual Studio 2022
* przy instalacji należy zaznaczyć opcje:
  * Desktop development with C++
  * C++ CMake tools for Windows
  * vcpkg package manager
  * C++ Modules

#### gcc-13
* [CMake > 3.25](https://cmake.org/)
* proszę sprawdzić wersję w linii poleceń        
  
  ```
  cmake --version
  ```
      
* IDE: Visual Studio Code
  * [Visual Studio Code](https://code.visualstudio.com/)
    * Zainstalować wtyczki
    * C/C++ Extension Pack
    * Live Share

### Docker + Visual Studio Code

Jeśli uczestnicy szkolenia korzystają w pracy z Docker'a, to należy zainstalować:

#### Visual Studio Code

* [Visual Studio Code](https://code.visualstudio.com/)
* Zainstalować wtyczki
  * Live Share
  * Dev Containers ([wymagania](https://code.visualstudio.com/docs/devcontainers/containers#_system-requirements))
    * po instalacji wtyczki - należy otworzyć w VS Code folder zawierający sklonowane repozytorium i
      z palety poleceń (Ctrl+Shift+P) wybrać opcję **Dev Containers: Rebuild and Reopen in Container**
