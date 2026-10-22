# Abyssal Reverb

A starter VST3/Standalone audio effect built with JUCE and CMake. It is an original experimental reverb inspired by the *general idea* of atmospheric, characterful reverbs; it does not copy Lair's proprietary code, interface, presets, or DSP.

## Current controls

- **Size** — perceived room size.
- **Decay** — intended decay-length control (mapped to the available JUCE reverb algorithm).
- **Damping** — high-frequency damping.
- **Width** — stereo width.
- **Mix** — dry/wet balance.
- **Drive** — saturation before the reverb.
- **Tone** — low-pass tone shaping of the wet signal.
- **Character** — Space, Artifact, Mirror, Rift; each applies a different combination of size, damping, width and tone shaping.

## Build through GitHub Actions (Windows)

1. Create a new GitHub repository and upload the contents of this folder (the `.github` folder must be included).
2. Open **Actions** and enable workflows if GitHub asks.
3. Select **Build Abyssal Reverb** and click **Run workflow**, or push a commit to `main` / `master`.
4. When the job completes, open the run and download the `AbyssalReverb-Windows` artifact.
5. Extract `AbyssalReverb-Windows-VST3.zip`. Copy `Abyssal Reverb.vst3` to your DAW's VST3 folder, commonly `C:\Program Files\Common Files\VST3`, then rescan plugins in the DAW.

The first CI run downloads JUCE from its official Git repository, so it needs internet access and may take several minutes. No local compiler installation is required if you build only through GitHub Actions.

## Local build (optional)

Install Visual Studio 2022 with **Desktop development with C++**, CMake, and Git. Then run:

```powershell
cmake -S . -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target AbyssalReverb_VST3 AbyssalReverb_Standalone --parallel
```

## Important notes

- This is a functional starter project, not yet a feature-for-feature replacement for Lair. It currently uses JUCE's built-in reverb engine with additional saturation and tone shaping; it does not yet implement a custom multi-line feedback-delay network, convolution, freeze mode, or preset browser.
- Before distributing a compiled plugin commercially, review the JUCE licence terms and any applicable third-party licence obligations.
