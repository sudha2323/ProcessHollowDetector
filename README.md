# 🕵️‍♂️ ProcessHollowDetector

A lightweight C++ security tool to detect **process hollowing** on Windows. Compares the PE header of a process in memory with the on-disk executable.

---

## 🔍 Features

- Enumerates all running processes
- Compares memory and disk PE headers
- Detects potential process hollowing
- Lightweight and fast

---

## 💻 Usage

```bash
Run as administrator (required for accessing process memory).

