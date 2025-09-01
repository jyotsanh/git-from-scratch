# git-from-scratch
A lightweight **C implementation of a Git-like version control system** for text files.  
This project is a learning-oriented reimplementation of core Git features to understand how Git works under the hood.

---

## 🚀 Current Progress
You can compile the project and run your own `mygit` command:

```bash
# Compile
gcc main.c -o mygit

# Use it like git
./mygit <command>
```

## ✅ Supported Commands
1. `mygit init`

Initializes a new repository in the current directory.
It creates the following structure:
```bash
.git/
 ├── objects/
 ├── refs/
 └── HEAD
```

2. `mygit cat-file` 

Used to inspect the content of a commit object.
You can view details such as:

**Author**

**Parent commit**

**Commit message**

# 🛠 Purpose
This project is not a full Git replacement, but rather an educational implementation to:

- Explore how Git manages objects (blobs, trees, commits).

- Understand hashing, object storage, and repository internals.

- Gain deeper insights into version control concepts.

## 📂 Repository Structure
```bash
.
├── src
|    ├──main.c     # Entry point
├── README.md      # Documentation
└── (more files to come...)

```

## 🔧 Requirements
- GCC (tested with **GCC 12.3.0** on Ubuntu 24.04 LTS)  
- Any C compiler supporting C11 or later should also work  
- Linux/macOS environment (Windows may work under WSL)  
