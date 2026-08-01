# 🛠 Contributing to CMMC
### (aka: Helping Maintain This Beautiful Disaster)
First off — thank you for even considering contributing to CMMC.
This project is held together with caffeine, regret, and a possum that lives in the stack, so any help is deeply appreciated.

This document explains how to contribute without accidentally summoning undefined behaviour or the possum (unless you want to).

---

# 📦 What You Need
CMMC currently requires:

- `Windows`
- `Zig` installed and on your `PATH`

- A willingness to suffer

- A sense of humour

- Optional: snacks for the possum


---

# 🔧 How to Build CMMC
Run:
```bash
.\build help
```
This shows all build options, including debug builds, release builds, and whatever cursed scripts I duct‑taped together at 3 AM.

---


## 🧪 How to Add a Feature

Every feature in CMMC touches multiple parts of the compiler. Here’s the workflow:

### 1. Add a Token to the Lexer
Update the lexer so your new keyword or operator is recognized.

### 2. Add a Rule to the Parser
Define the syntax. If the parser cries, that’s normal.

### 3. Add an AST Node
Create a new AST node type. This is where the possum usually appears.

### 4. Add VM Behaviour
If the VM should run it, implement the logic. The VM is currently a hello‑world script, so anything you add is an upgrade.

### 5. Add Backend Behaviour
If the feature should compile to machine code, update the backend. Yes, it still uses libtcc.

### 6. Add Tests
Optional but appreciated. If your test crashes the compiler, that’s a feature.

---

## 🐛 Reporting Bugs (We Call Them “Features”)

Use the **Feature Report** template.  
In CMMC, bugs are features, and features are bugs.

If you see the possum, use the **Possum Sighting Report** template.

---

## 🌟 Requesting Real Features

Use the **Real Feature Request** template.  
This is for actual improvements, not “the compiler exploded when I typed `float8` wrong.”

---

## 🧩 Good First Issues

Look for issues tagged:

- good first issue  
- easy  
- cursed  
- possum-approved  

Examples include:

- adding new keywords  
- improving error messages  
- making the VM slightly less useless  
- adding new possum behaviours  

---

## 🧼 Code Style

CMMC does not have a strict code style, but:

- Keep code readable  
- Keep comments funny  
- Keep functions small  
- Keep the possum fed  
- Avoid reinventing C unless it’s funny

---

## 🤝 Pull Requests

When submitting a PR:

- Keep it small if possible  
- Explain what you changed  
- Mention if the possum appeared  
- Screenshots of crashes are welcome  
- PRs are merged quickly — chaos must grow

---

## 🐀 Possum Interaction Rules

- Do not anger him  
- Do not feed him pointers  
- Do not let him near the optimizer  
- If he judges your code, accept it

---

## ❤️ Thank You

CMMC is a weird, cursed, experimental compiler project — and contributors like you make it better, stranger, and more fun.

Welcome to the chaos.

