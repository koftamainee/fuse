# Fuse Interpreter

## Overview

The **Fuse** interpreter is a customizable application designed to execute programs written in a user-defined syntax. It supports debugging features and enables the interpretation of instructions defined in an external configuration file. The interpreter handles 32-bit unsigned integer variables with customizable names and allows operations such as arithmetic, logical, and input/output processing.

# Installation
maybe soon i will make AUR and nixpkgs packages....
# Building from source
## Dependencies
- **C compiler** (gcc, clang, etc.)
- **make** build system
- **pdflatex** for docs compilation
```bash
sudo apt install gcc make texlive-latex-base      # Debian-based

sudo dnf install gcc make texlive-scheme-basic    # Fedora-based

sudo pacman -S gcc make texlive-latex             # Arch-based

sudo zypper install gcc make texlive              # OpenSUSE
```

## Build guide
Clone repo and run 
```bash
make && make install
```
This will configure, compile and install Fuse.
## ⚠️ **CAUTION** ⚠️
Do not recommended installing fuse as root user. Run this command as root at your own risk. Directories and files, that will be created in installation process will require root access

## Docs
For Installation instruction you need to compile docs. Docs automaticly compiled during installation, but if tou want to compile only them, run
```bash
make docs
```
Compiled docs will be located in *build/docs* directory
- **user_man.pdf** - Main Interpriter manual
- **in_instruct.pdf** - Detailed Installation instruction
- **an_instruct.pdf** - Manual for logs analysis systems
- **LICENSE.pdf** - Copy of *LICENSE* file, but in pdf
If you need to compile only one document, run one of this commands
```bash
make user_man

make in_instruct

make an_instruct

make LICENSE
```
