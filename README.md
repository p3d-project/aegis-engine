# Aegis Engine

Aegis Engine is a Nintendo DS game engine, specifically developed for the Persona 3 Dual (P3D) project.

The Aegis engine was named as such for two reasons:
- It is a play on the character *Aigis'* name in P3D
- It means *protection, backing, or sponsorship by a particularly powerful person, organization, or system* which plays into the idea that a game engine *backs* a game

> Want to help develop the engine? Join the [Discord!](https://discord.gg/CQnkc5gS6a) Any help, big or small, would be greatly appreciated!

![Stars](https://img.shields.io/github/stars/p3d-project/aegis-engine?style=flat-square&color=gold)
![Forks](https://img.shields.io/github/forks/p3d-project/aegis-engine?style=flat-square&color=blue)
![Last Commit](https://img.shields.io/github/last-commit/p3d-project/aegis-engine?style=flat-square&color=green)
![License](https://img.shields.io/github/license/p3d-project/aegis-engine?style=flat-square)

[![C++](https://img.shields.io/badge/C++-%2300599C.svg?logo=c%2B%2B&logoColor=white)](#)

[![Discord](https://img.shields.io/discord/1498850477545357482?label=Discord&logo=discord&style=flat-square&color=5865F2)](https://discord.gg/CQnkc5gS6a)

## Architecture

Refer to [P3D Aegis Engine v2 HLD](https://docs.google.com/document/d/1earODRhL6BscfMGfGM2qVblGMRSetTuKtAmm_4TKdcw/edit?usp=sharing)

## Building & Running
This repo has the `debug`, `release` and `default` (alias of `debug`) CMake presets.

```bash
cmake --preset <preset>
cmake --build --preset <preset>
./out/build/<preset>/aegis_engine
```

For development, it is recommended to use the debug preset.
```bash
cmake --preset debug
cmake --build --preset debug
./out/build/debug/aegis_engine
```

### Flatbuffers
See https://flatbuffers.dev/quick_start/

## README TODO
- [ ] Add library mentions in README

---

![Alt](https://repobeats.axiom.co/api/embed/688e10f06f887746a68124a2d64b2da43f99ff19.svg "Repobeats analytics image")

---

## Legal
### License
The codebase is licensed under the Creative Commons **Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0).**

For the codebase, the license means:

- **Attribution (BY)**: You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
- **NonCommercial (NC)**: You may not utilize this codebase for commercial purposes.
- **ShareAlike (SA)**: If you remix, transform, or build upon the engine code, you must distribute your contributions under the same license as the original.

### Libraries
> N/A

If you want to use the codebase in a commercial application, contact thep3dproject@gmail.com for solutions.
