import re
import numpy as np

GAS_PATH = r"GasTable_Xe_173K.gas"

FLOAT_RE = re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?")

def load_vd_from_gas(gas_path: str):
    """
    Returns:
      E_Vcm:           ndarray, shape (N,)
      v_mm_per_us:     ndarray, shape (N,)
    Assumptions (true for your file):
      - 'E fields' block stores reduced field E/P in V/(cm*Torr)
      - Each field point has a 33-value record under 'The gas tables follow:'
      - record[:,0] is electron drift velocity in cm/us
      - footer contains 'PGAS = <torr>'
    """
    lines = open(gas_path, "r", errors="ignore").read().splitlines()

    # --- Find PGAS (Torr) from footer ---
    pgas_torr = None
    for line in lines:
        if "PGAS" in line:
            # Example: "..., PGAS  = 1.35000000E+03, TGAS  = ..."
            nums = [float(x) for x in FLOAT_RE.findall(line)]
            # Heuristic: PGAS is usually the second-to-last float in that line
            # Safer: scan for the substring and parse around it is harder; this works on your file.
            # We'll just take the first float that looks like ~1350.
            for x in nums:
                if 1000 < x < 2000:
                    pgas_torr = x
                    break
    if pgas_torr is None:
        raise RuntimeError("Could not find PGAS in the .gas file footer.")

    # --- Extract E/P grid from the 'E fields' block ---
    e_start = None
    for i, line in enumerate(lines):
        if line.strip().startswith("E fields"):
            e_start = i + 1
            break
    if e_start is None:
        raise RuntimeError("Could not find 'E fields' block.")

    eoverp = []
    i = e_start
    while i < len(lines):
        nums = [float(x) for x in FLOAT_RE.findall(lines[i])]
        if len(nums) == 0:
            break
        eoverp.extend(nums)
        i += 1
    eoverp = np.array(eoverp, dtype=float)

    # Convert reduced field (V/(cm*Torr)) -> V/cm
    E_Vcm = eoverp * pgas_torr

    # --- Extract the big gas table after 'The gas tables follow:' ---
    t_start = None
    for i, line in enumerate(lines):
        if "The gas tables follow" in line:
            t_start = i + 1
            break
    if t_start is None:
        raise RuntimeError("Could not find 'The gas tables follow:' block.")

    # Read floats until the footer starts (your file footer begins around 'H Extr:' / 'Thresholds:' etc.)
    table_floats = []
    for line in lines[t_start:]:
        if "H Extr" in line or "Thresholds" in line or "Interp:" in line:
            break
        table_floats.extend([float(x) for x in FLOAT_RE.findall(line)])

    table_floats = np.array(table_floats, dtype=float)

    # Your file: 200 field points, 33 values each
    n_fields = len(E_Vcm)
    ncols = 33
    if table_floats.size != n_fields * ncols:
        raise RuntimeError(
            f"Unexpected table size: got {table_floats.size} floats, "
            f"expected {n_fields*ncols} (= {n_fields}*{ncols})."
        )

    table = table_floats.reshape(n_fields, ncols)

    # Column 0 is drift velocity in cm/us for your file
    v_cm_per_us = table[:, 0]
    v_mm_per_us = v_cm_per_us * 10.0

    return E_Vcm, v_mm_per_us

E_Vcm, v_mm_per_us = load_vd_from_gas(GAS_PATH)

print("E range (V/cm):", E_Vcm.min(), "to", E_Vcm.max())
print("v_drift range (mm/us):", v_mm_per_us.min(), "to", v_mm_per_us.max())

def v_drift_mm_per_us(E):
    return float(np.interp(E, E_Vcm, v_mm_per_us))

def drift_time_us(z_mm, z_anode_mm, E_Vcm_local):
    return (z_anode_mm - z_mm) / v_drift_mm_per_us(E_Vcm_local)

print("Example v_drift at 100 V/cm:", v_drift_mm_per_us(100.0), "mm/us")