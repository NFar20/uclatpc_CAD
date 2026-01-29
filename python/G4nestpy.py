import uproot
import pandas as pd
import numpy as np
import nestpy

f = uproot.open("../build/output.root") # importing geant4 output
tree = f["data"]
df = tree.arrays(library="pd")

print(df.columns)
print(df.head())

#default detector settings (used to determine which attributes I can modify)

# det = nestpy.VDetector()
# nc  = nestpy.NESTcalc(det)

# print("VDetector attrs (filtered):")
# det = nestpy.VDetector()
# for name in dir(det):
#     if any(k in name.lower() for k in ["field", "temp", "density", "pressure", "gas", "liquid", "g1", "g2", "s2", "extraction", "drift", "diff", "lifetime"]):
#         print(" ", name)

# print(det)

def make_custom_detector( # method to create custom detector
    g1: float,
    # g1_gas: float,
    E_gas_Vcm: float,
    top_drift_cm: float,
    in_gas: bool = False,
    s2_fano: float = 1.0,
    s2_thr: float = 0.0
) -> nestpy.VDetector:
    det = nestpy.VDetector()

    det.set_g1(float(g1))
    # det.set_g1_gas(float(g1_gas))
    det.set_E_gas(float(E_gas_Vcm))
    det.set_TopDrift(float(top_drift_cm))
    det.set_inGas(bool(in_gas))
    det.set_s2Fano(float(s2_fano))
    det.set_s2_thr(float(s2_thr))

    return det

# Setting detector parameters
det = make_custom_detector(
    g1=0.12,          # PDE * light collection
    # g1_gas=0.10,      # if your model uses a separate gas light collection
    E_gas_Vcm=250.0,
    top_drift_cm=45.0,# drift length in cm (example)
    in_gas=False,
    s2_fano=1.0,
    s2_thr=0.0
)

nc = nestpy.NESTcalc(det)

def nest_quanta_row(edep_keV: float, efield_Vcm: float, itype: int,
                    density: float = 2.9, A: float = 131.293, Z: float = 54):
    """
    itype: 0 = ER, 1 = NR (your convention)
    Returns: (ne, ng)
    Notes:
      - density in g/cm^3 (LXe ~2.9 depending on T)
      - drift_field in V/cm
      - A,Z for xenon defaults
    """
    interaction = nestpy.INTERACTION_TYPE.ER if itype == 0 else nestpy.INTERACTION_TYPE.NR

    # Use keyword args to match your nestpy signature
    y = nc.GetYields(
        interaction=interaction,
        energy=float(edep_keV),
        density=float(density),
        drift_field=float(efield_Vcm),
        A=float(A),
        Z=float(Z),
    )

    # Turn yields into actual quanta (with fluctuations)
    q = nc.GetQuanta(y, float(edep_keV))

    # Attribute names can vary slightly; these are common in this API
    ne = q.electrons
    ng = q.photons
    return ne, ng

ne_list = []
ng_list = []

for edep, ef, itype in zip(df["edep_keV"].values, df["Efield_Vcm"].values, df["type"].values):
    ne, ng = nest_quanta_row(edep, ef, int(itype))
    ne_list.append(ne)
    ng_list.append(ng)

df["Ne"] = np.array(ne_list, dtype=float)
df["Ng"] = np.array(ng_list, dtype=float)

voxel_mm = 1.0

df["vx"] = np.floor(df["x_mm"]/voxel_mm).astype(int)
df["vy"] = np.floor(df["y_mm"]/voxel_mm).astype(int)
df["vz"] = np.floor(df["z_mm"]/voxel_mm).astype(int)

sites = (df.groupby(["event_id","vx","vy","vz","type"], as_index=False)
           .agg(edep_keV=("edep_keV","sum"),
                x_mm=("x_mm","mean"),
                y_mm=("y_mm","mean"),
                z_mm=("z_mm","mean"),
                t_ns=("t_ns","min"),
                Efield_Vcm=("Efield_Vcm","mean")))

z_gate_mm = 25.5
z0 = sites["z_mm"].values
dz_mm = z_gate_mm - z0

# Convert mm->cm
dz_cm = dz_mm / 10.0

def drift_time_us(z_mm: float, z_gate_mm: float, E_Vcm: float) -> float:
    dz_mm = z_gate_mm - float(z_mm)
    v = v_drift_mm_per_us(E_Vcm)
    return dz_mm / v if v > 0 else float("nan")

v_cm_us = np.array([nc.GetDriftVelocity(ef) for ef in sites["Efield_Vcm"].values])
t_us = dz_cm / v_cm_us
sites["t_drift_us"] = t_us

print(v_cm_us)
