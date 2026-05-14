# AirModels - Comparison of neutron cross section models and their impact in transport simulations

A Geant4-based simulation framework for studying neutront transport in air and air activation and using thermal neutron cross-sections. This project compares **Geant4** free gas libraries with the model available in [1], implemented with the use of **NCrystal**.

## Overview

### What It Does
- Simulates thermal neutron beams passing through an air-filled chamber
- Compares free gas model (G4 standard) with molecular model from [1]
- Saves energy spectra and spatial distributions (XY position) of neutrons
- Models argon activation in air 
- Detects and records neutrons that pass through a transmission detector

### Key Features
- **Multiple air models**: Dry air, humid air (56.5%), with both G4 and NCrystal physics
- **Configurable geometry**: 3m³ air chamber with adjustable sample and detector positions
- **Two physics backends**: G4 standard physics or NCrystal for implementation of cross section model from [1] 
- **Visualization support**: Interactive UI with OpenGL rendering
- **ROOT analysis**: Built-in macros for histogram and ntuple analysis
- **Flexible configuration**: CMake build system with compile-time options

## Project Structure
```
ArActivation/
├── CMakeLists.txt                                  # CMake build configuration
├── README.md                                       # Project documentation
├── exampleB4c.cc                                   # Main application entry point
│
├── analysis/                                       # Analysis and post-processing scripts
│   ├── Extract1D2DIstogram_txt.py                  # Extract 1D/2D histograms to text format
│   └── ratioIsograms2files.py                      # Compute histogram ratios between two files
│
├── data/                                           # Material and configuration databases
│   ├── myAirHydr_OTHERPHASES.ncmat                 # Humid air material definition (alternative phases)
│   ├── myDryAir.ncmat                              # Dry air NCrystal material
│   ├── myDryAir_noMag.ncmat                        # Dry air material without magnetic contribution
│   └── myHydrAir_56_5pc.ncmat                      # Humid air NCrystal material (56.5% RH)
│
├── include/                                        # Header files (.hh)
│   ├── B4Analysis.hh                               # Analysis manager definitions
│   ├── B4PrimaryGeneratorAction.hh                 # Primary particle generator declarations
│   ├── B4RunAction.hh                              # Run action declarations
│   ├── B4cActionInitialization.hh                  # User action initialization declarations
│   ├── B4cCalorHit.hh                              # Calorimeter hit class declarations
│   ├── B4cCalorimeterSD.hh                         # Sensitive detector declarations
│   ├── B4cDetectorConstruction.hh                  # Detector geometry and materials declarations
│   └── B4cEventAction.hh                           # Event action declarations
│
├── macros/                                         # Geant4 macro scripts
│   ├── constGPS.mac                                # Constant-energy GPS configuration
│   ├── expGPS.mac                                  # Exponential GPS spectrum setup
│   ├── gui.mac                                     # GUI initialization macro
│   ├── init_vis.mac                                # Visualization startup macro
│   ├── inputfileGPS.mac                            # GPS configuration from input file
│   ├── linGPS.mac                                  # Linear energy spectrum GPS setup
│   ├── monoGPS.mac                                 # Monoenergetic GPS configuration
│   ├── powGPS.mac                                  # Power-law GPS spectrum setup
│   ├── run1.mac                                    # Single simulation run macro
│   ├── scriptloop.mac                              # Batch execution loop script
│   ├── spectrum_VESUVIO_200points.dat              # Input neutron spectrum data
│   ├── transmissionpowGPS.mac                      # Transmission-oriented power-law GPS setup
│   └── vis.mac                                     # Visualization settings
│
├── src/                                            # Source files (.cc)
│   ├── B4PrimaryGeneratorAction.cc                 # Primary particle generator implementation
│   ├── B4RunAction.cc                              # Run-level actions and analysis handling
│   ├── B4cActionInitialization.cc                  # User action initialization implementation
│   ├── B4cCalorHit.cc                              # Calorimeter hit class implementation
│   ├── B4cCalorimeterSD.cc                         # Sensitive detector implementation
│   ├── B4cDetectorConstruction.cc                  # Detector geometry and material implementation
│   └── B4cEventAction.cc                           # Event-level actions
```

## Physics Models

### Air Materials - Cross-Section Models
Air is implemented in **four different configurations**:

| Model | Type | Hydration | 
|-------|------|-----------|
| `AirDry_G4` | Geant4 NIST | Dry
| `AirHydr56_G4` | Geant4 NIST | 56.5% humidity |
| `AirDry_NC` | NCrystal molecular model from [1] | Dry
| `AirHydr56_NC` | NCrystal molecular model from [1] | 56.5% humidity | 


**Important Notes:**

- NCrystal assumes mole fraction of atoms (not molecules)
- Standard NCrystal parameters: density=1.0 g/cm³, T=293.15 K, P=1.0 atm 
-NCrystal Installation Order: NCrystal must be initialized after runManager creation cannot be done in macro files
- NCrystal is **not thread-safe** - multi-threaded mode disabled in main: Disable multi-threading with -DMY_MULTITHREADED=OFF if using NCrystal
- NCrystal energy threshold: 5 eV (defined in `G4NCProcWrapper.cc`) - NCrystal takes over elastic scattering from pHadElastic below 5 eV according to documentation of [2]
-Material Definition: Always verify mole fractions match your target composition
-Overlaps Checking: Enabled by default; disable in production for speed


## Geometry

1. **Neutron Source**: Produces uniform square beam (5×5 mm at origin)
2. **Air Room**: 3×3×3 m³ galactic material-filled chamber
3. **Transmission Detector**: Twin-box detector (4×4 mm XY, 1 mm Z thickness)
   - Registers transmitted neutrons
   - Records hit energy and (x,y) position


## Bibliography

[1] Simoni, M., Felix Fernandez-Alonso, Tommaso Giovannini, Matthew Krzystyniak, Jose Ignacio Mar-
quez Damian, Anna Marsicano, Marco Martellucci, Triestino Minniti, Roberto Senesi, Matteo Sorbara
& Giovanni Romanelli (2026). Molecular contributions to the thermal neutron cross sections of O2, N2,
and air.. J. Chem. Phys.. DOI: 10.1063/5.0324136

[2] X. Cai and T. Kittelmann, “Ncrystal: A library for thermal neutron transport,” Computer Physics Communications 246, 106851 (2020).
