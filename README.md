# CMMC — The C-- descendant that escaped containment. Requires Windows. Demands Zig.
---

Oh. You actually opened this.
I’m so sorry.

Welcome to CMMC, the new C-- compiler for a language that died decades ago, has no standard library, and will absolutely not protect you from memory corruption. This entire project is held together with hope, caffeine, and whatever the AI spat out while I was losing my sanity.

Does it resemble the original C--?
No.
Does it work?
Sometimes.
Is it your problem now?
Absolutely.

Below is a syntax table and a how‑to guide for the brave souls who decided to continue reading. You are about to experience joy, pain, and possibly enlightenment.

| Syntax | Explain | WHY?! |
| --- | --- | --- |
| bits8/16/32/64 | weird number types. | because int was too normal!? |
| word1/2/4/8 | Stupid strings that do nothing | Because who wants working strings |
| float4/8 | Weird floats | Because who doesnt want two things that do exactly the same thing! |
| align1/2/4/8 | Dont even ask! | I cant tell you why |
| if / else | Conditional screw ups | Because every language needs a way to regret its life choices |
| switch | Does nothing but regret | I was too lazy to finish this off |
| goto / jump | Stuff, have fun figuring them out | Because every language has them!! |
| return | The escape | When you just want to leave |
| import / export | Fake module system | I wanted to pretend this language is modern |
| foreign | Call things you shouldn’t | Because safety is for cowards |
| data / stack | Memory section hints | I miss assembly and bad decisions |
| noalias | “Trust me bro” pointer qualifier | I saw LLVM do it once |
| section | Custom binary section | I don’t know why this exists |
| pragma | Compiler directives | Future me will cry |
| :: | Namespace separator | I stole this from C++ |
| == != <= >= | Comparisons | Sometimes they even work |
| + - * / % | Math | You’ll break something |
| ( ) { } [ ] | Structure | Barely contains the chaos |
| ; , : | Punctuation | I didn’t reinvent these |
| anything else | Lexer panic | You deserve it |

---

# 🧪 Example Code (If This Works, You Win)

```cmm
bits32 main() {
    return 0;
}
```
If this compiles, congratulations — you’ve achieved enlightenment.
If it doesn’t, that’s a feature.

---

# ⚙ How To Use This Mistake

1. write a `cmm` file:
something like:

```cmm
bits32 main() {
    return 42;
}
````

2. Run the compiler:

`cmmc yourfile.cmm -o yourfile.exe`

3. Accept your fate

 - If it crashes → that’s a feature

 - If it runs → that’s a miracle

 - If it segfaults → that’s your fault

---

# ⚠ Known Issues (aka: Everything)

- Segfaults are considered “runtime feedback.”
- The optimizer is powered by wishful thinking.
- Strings are lies.
- Memory safety is a myth.
- The language specification changes depending on my mood.
- The compiler sometimes works better when you don’t look at it.
- Windows + Zig is required because I made choices and now you must live with them.

---

# 🛠 Roadmap (If I Don’t Give Up)

 - A real library.
 - Make the VM not a hello, world script.
 - a optimiser that is not nothing.
 - a backend that not libtcc.
 - a reason for this insanity.
 - better docs that arent me crying for help!!

 ---

 # ❓ FAQ

 ### Q: Why??
 A: I don't really know!?

 ### Q: No seriously, why? 
 A: seriously, I don't really know!?

 ### Q:  Should I use this in production?  
 A: Only if you hate your coworkers.

 ### Q: Will this ever be stable?
 A: Define "stable"?

 ---

 # 🐀 Mascot (CMMC the Cursed Possum)
 He lives in your stack.
 He eats your pointers.
 He judges your code.
 He knows what you did in main().

 ---

# 🏗️ Building

CMMC requires Windows and Zig because I made choices and now you must live with them.

To see all build options, run:

```bash
.\build help
```

This will show you the full help menu that also includes whatever other questionable features I’ve duct‑taped into the build system.

Once you find the option you want, run it.
If it works, great.
If it doesn’t, that’s a feature.

---

## Third-Party Software

This repository includes `libtcc.dll` from the **Tiny C Compiler (TCC)** project by Fabrice Bellard and contributors.

* **License:** GNU Lesser General Public License v2.1 (see `LICENSE.LGPL` or `NOTICE` for details)
* **Source Code:** You can obtain the original TCC source code at [repo.or.cz/tinycc.git](https://repo.or.cz/tinycc.git).
