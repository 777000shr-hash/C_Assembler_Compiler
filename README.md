# C_Assembler_Compiler

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

---

## 💻 Project Description

This is a final project (Lab C) demonstrating a deep understanding of computer architecture by building a complete **Two-Pass Translator/Assembler** written in **C**.

The system accepts input in **symbolic assembly language** and produces **binary output** (Machine Language).

The assembler is designed with a **two-pass model** to correctly handle forward references and global definitions.

---

## ✨ Key Features

* **Full Symbolic Assembly Support:** Supports instructions, labels/tags, data definitions, and string definitions.
* **Macro Processing:** The system performs full macro expansion during the pre-processing stage.
* **Binary Output Generation:** Creates standard output files: binary object files (`.ob`), entry table files (`.ent`), and external reference files (`.ext`).
* **C Modularity:** The code is divided into separate modules for clean structure and maintainability.
* **Comprehensive Error Handling:** Clear reporting and output of compilation errors.

---

## 🛠️ Build & Run Instructions

The assembler can be built and run using the included **`makefile`**.

### 1. Build the Project (Compilation)

Running the following command in the project root directory will compile the source code and create the executable (`assembler`):

```bash
make
