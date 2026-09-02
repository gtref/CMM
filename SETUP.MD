## 📥 Installing Requirements

CMMC needs three external tools to build: **Bison**, **Flex**, and **Zig**.  
You can either place the binaries locally in your workspace (`.\tools\` or `.\bin\`) or install them system-wide on Windows.

---

## 🛠 Local Workspace Tooling (Recommended)

If you prefer non-system-wide installs or portable setups:
1. Create a `tools\` (or `bin\`) folder in the repository root.
2. Place `bison.exe`, `flex.exe`, and/or `zig.exe` directly inside `.\tools\` (or `.\bin\`).
3. The build system scripts (`build.bat`, `scripts\build\build_all.bat`) and `cmmc.exe` linker stage will automatically detect and prefer local binaries in `.\tools\` or `.\bin\` before falling back to system `PATH`.

---

## 🧱 Install Bison & Flex (WinFlexBison)

Windows doesn’t ship Bison/Flex, so you’ll need **WinFlexBison**, a maintained Windows port.

1. Download the latest release:  
   https://github.com/lexxmark/winflexbison/releases

2. Download the ZIP named something like:  
   `winflexbison-<version>.zip`

3. Extract it somewhere permanent, e.g.:  
   `C:\Tools\winflexbison`

4. Inside that folder, rename the executables:

win_bison.exe → bison.exe
win_flex.exe  → flex.exe


This makes them match the names used in the build script.

5. Add the folder to your PATH:

- Start Menu → Edit the system environment variables  
- Environment Variables  
- System variables → Path → Edit → New  
- Add:  
  `C:\Tools\winflexbison`

6. Verify install:

```bash
bison --version
flex --version
```


If both commands print versions, you’re good.

---

## ⚡ Install Zig

CMMC uses Zig as its build system.

1. Download Zig for Windows:  
https://ziglang.org/download/

2. Choose the latest **Windows x86_64** build.

3. Extract it somewhere permanent, e.g.:  
`C:\Tools\zig`

4. Add Zig to your PATH:

- Start Menu → Edit the system environment variables  
- Environment Variables  
- System variables → Path → Edit → New  
- Add:  
  `C:\Tools\zig`

5. Verify install:

zig version


If it prints a version, Zig is installed correctly.

---

## 🧪 After Installing Everything

Run:


.\build doctor


If the build script prints its help menu, all dependencies are installed and CMMC is ready to build.

If the doctor reported one of the tools missing, Well i cant help you no longer and this is the time when the AI comes out to play.
