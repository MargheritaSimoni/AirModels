#Neutron Imaging simulation
Thermal neutrons are used to simulate Argon activation in air. Thermal neutron cross section of air components in the free gas model can be compared to cross sections in the Young and Koppel model with the help of NCrystal. S(ab) was computed for N2 molecule and O2 molecule using the Young and Koppel model for biatomic molecules, in addition the paramagnetic contribution to O2 cross section was considered.

##Geometry
From left to right (negative to positive Z) the geometry is composed of a neutron source, an air room and a neutron transmission detector. The world is made of galactic material, in order to only observe the contribution of the room, and measure transmission in the correct geometry.

The **room** is a 3mx3mx3m room, filled with air defined by me.

The **Imaging detector** is composed of two box solids, placed one in front of the other, the simulation detects neutrons that pass from one to the other, saving their energy and XY position. The detector is ideal, so it is made of the same material as the world (defined as worldMaterial).

the **neutron beam** has a squared section of 2x2 cm and impinges on the sample travelling from negative to positive Z values.

##Materials
In this version air is defined in four different ways, depending on the XS model and the level of hydration (AirDry_G4,AirHdr40_G4,AirDry_NC,AirHydr40_NC) The type of air can be chosen by changing the definition of the variable air in the line "auto air = G4Material::GetMaterial("AIR_NC");" to compare free gas model to NCrystal models. 

**nb.:** Geant defines material composition with number of atoms or mass fraction, NCrystal wants mole fraction of the atom (not the molecule!) currently all materials are defined assuming dry air composition to be 78%N2 21%O2 and 1%Ar, which are in both case defined considering only the most abundant isotope of each element


##Physics list: Ncrystal
It is possible to use a physics list manually defined, (in the case of "physicsList.cc", it was taken from a geant4 example), or to use a pre-defined G4 physicslist that correctly treats neutrons in the thermal range, using free gas cross sections (for example QGSP_BIC_HP). Either way,when using NCrystal it is important that the physics list used has 1 active process derived from G4HadronElasticProcess for neutrons, because NCrystal takes over the pHadElastic process, redefining it according to its cross sections.

**nb.:** standard parameters for NCrystal materials are defined inside MatHelper.cc and they are: density=1.0gcm3, kStateSolid, temperature=293.15 kelvin, pressure= 1.0 atmosphere. It is important to note that they may not correspond to default geant parameters, (for example default temperature in geant is 273.15K) so it is always good practice to specify these parameters.

**nb.:** NCrystal is not thread-safe, so in the main all lines to run in multi-thread mode were commented.

**nb.:** Ncrystal takes over pHadElastic up to 5eV, as defined in the class G4NCProcWrapper.cc (even though inside the class G4NVInstall.hh is reported as 2eV). The 5eV threshold may be revisited in future versions as materials like tungsten have a resonance below 5eV.

**nb.:** NCrystal installation needs to be done after initialization of runManager, so the latter can not be done inside the macro file

##Primary generator
In this version neutrons are generated with a constant distribution, using G4UniformRand(), the beam has a squared section of 5x5 mm.

**nb.:** G4UniformRand does not vary its seed automatically


