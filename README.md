# 12 Gauge — guitar-triggered impact effect

A Windows-ready VST3/Standalone audio-plugin starter that pairs a compact orange stompbox-inspired interface with a synthesized, cinematic 12-gauge-style impact. Guitar audio triggers an original noise-and-low-frequency sound; no firearm recording or external sample is used. It is an audio effect for music production, not a physical weapon simulation.

The visual design is an original homage to classic orange distortion pedals and does not use Boss branding or logos.

## Build on Windows

1. Install Visual Studio 2022 with the **Desktop development with C++** workload and install CMake.
2. Clone this repository and open a Developer PowerShell in its directory.
3. Configure and build (JUCE is fetched automatically):

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

The VST3 bundle and Standalone app will be under `build/12GAUGE_DS2_artefacts/Release/`. Copy the `.vst3` bundle to your DAW's VST3 folder (commonly `C:\Program Files\Common Files\VST3`) and rescan plugins.

## Controls

- **TRIGGER**: how hard a note must hit to fire the effect.
- **TONE**: brightness of the synthesized crack.
- **DECAY**: length of the synthesized tail.
- **MIX**: blend from dry guitar to the generated effect.

Input guitar audio is passed through as the dry signal. The generated effect is synthesized in real time, with no samples. Use an audio interface and a DAW/host that supports VST3.

## Notes

This repository contains the plugin source, not a prebuilt Windows binary. The first CMake configure needs internet access to download JUCE 8.0.6. Plugin testing and signing are not included.
