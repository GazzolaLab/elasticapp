import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm
from joblib import Parallel, delayed
import multiprocessing
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.colors import to_rgb
import matplotlib.patches as patches
import os
from datetime import datetime

import glob

base_paths = glob.glob("data/rod_coordinates_*.csv")
base_paths.sort()
num_files = len(base_paths)

# Get number of rods
data = np.genfromtxt(base_paths[0], delimiter=",", )
if len(data.shape) == 1:
    n_rods = 1
    N_nodes = data.shape[0]
else:
    N_nodes = 8 + 1
    n_rods = data.shape[0] // N_nodes
print(f"{n_rods=}")
    
# Plotting geometry
r_cyl = 75
Hi = 400
Hf = 50
rect_time = 3.0
move_duration = 4.0
m = (Hi - Hf) / (-move_duration);
current_top_height = lambda time: Hi #* (time < rect_time) + (m * (time - rect_time) + Hi) * (time >= rect_time and time < rect_time+move_duration) + Hf * (time >= rect_time+move_duration);

# For scaling
g = -981.0
length = 75.74
rho = 8.94e-3
dimension = 1.21
E = 12e9
I = np.pi * (dimension**4) / 4
total_mass = g * rho * n_rods * (np.pi * dimension ** 2) * length 
energy_scale = E * I / length

# RMSE windows
rmse_windows = [10, 100, 1000]

def get_force_history():
    paths = glob.glob("data/imposed_plane_*.csv")
    paths.sort()
    num_files = len(paths)
    data_col = []
    for path in paths:
        data = np.genfromtxt(path, delimiter=",", )
        data_col.append(data)
    data_col = np.array(data_col)
    assert data_col.shape[1] == 4, f"{data_col.shape=}"
    assert data_col.shape[0] == num_files, f"{data_col.shape=}"
    return data_col

def get_rmse(x, target, window):
    assert x.shape[0] >= window
    rmses = []
    for i in range(x.shape[0] - window):
        rmses.append(np.sqrt(np.mean((x[i:i+window]-target)**2)))
    ret = np.ones(x.shape[0])
    ret[-len(rmses):] = np.array(rmses)
    return ret

def plot_X(i, forces, rmses):
    if i == 0:
        time = 0
    else:
        time = forces[-1,0]
    base_path = base_paths[i]
    data = np.genfromtxt(base_path, delimiter=",", )
    X, Y, Z = np.transpose(data)

    num_rod = (~np.isnan(data)).sum(axis=1).astype(np.bool_).sum() // N_nodes
    #print(f"{num_rod=}")

    fig = plt.figure(figsize=(16,8))

    # Energy
    energy_path = base_path
    energy_path = energy_path.replace('rod_coordinates', 'rod_energies')
    energies = np.genfromtxt(energy_path, delimiter=",", )
    energies /= (energy_scale) # Rescale by EI
    energies = np.nan_to_num(energies, 0.0)
    bins = 20
    ax = fig.add_subplot(4,8,23)
    ax.hist(energies[:,0], bins=bins)
    ax.legend(["e.trans"])
    hist, _ = np.histogram(energies[:,0], bins=bins)
    ylim = np.median(hist) * 1.5 + 5
    ax.set_ylim([0,ylim])

    ax = fig.add_subplot(4,8,24)
    ax.hist(energies[:,1], bins=bins)
    ax.legend(["e.rot"])
    hist, _ = np.histogram(energies[:,1], bins=bins)
    ylim = np.median(hist) * 1.5 + 5
    ax.set_ylim([0,ylim])

    ax = fig.add_subplot(4,8,31)
    ax.hist(energies[:,2], bins=bins)
    ax.legend(["e.bend"])
    hist, _ = np.histogram(energies[:,2], bins=bins)
    ylim = np.median(hist) * 1.5 + 5

    ax.set_ylim([0,ylim])
    ax = fig.add_subplot(4,8,32)
    ax.hist(energies[:,3], bins=bins)
    ax.legend(["e.shear"])
    hist, _ = np.histogram(energies[:,3], bins=bins)
    ylim = np.median(hist) * 1.5 + 5
    ax.set_ylim([0,ylim])

    # Get index of abnormal rod
    std_m = 2.5
    thresh = energies.std(axis=0, keepdims=True) * std_m
    mask = np.any(energies > thresh, axis=1)
    
    #DEBUG: get index of rod escaped
    x_mask_ind1 = np.where(np.any((data[:,0].reshape([450,9]) < -76), axis=1))[0]
    x_mask_ind2 = np.where(np.any((data[:,0].reshape([450,9]) >  76), axis=1))[0]
    y_mask_ind1 = np.where(np.any((data[:,1].reshape([450,9]) < -76), axis=1))[0]
    y_mask_ind2 = np.where(np.any((data[:,1].reshape([450,9]) >  76), axis=1))[0]
    z_mask_ind1 = np.where(np.any((data[:,2].reshape([450,9]) <   0), axis=1))[0]

    mask[:] = False
    mask[x_mask_ind1] = True
    mask[x_mask_ind2] = True
    mask[y_mask_ind1] = True
    mask[y_mask_ind2] = True
    mask[z_mask_ind1] = True

    # Plot 3D
    ax = fig.add_subplot(2,4,5,projection="3d")
    for j in range(0, n_rods):
        ax.plot(
            X[j * N_nodes: (j + 1) * N_nodes],
            Y[j * N_nodes: (j + 1) * N_nodes],
            Z[j * N_nodes: (j + 1) * N_nodes],
            c=to_rgb("xkcd:reddish"),
            solid_capstyle="round",
            alpha=0.8,
            linewidth=2.5,
        )
        if not mask[j]:
            continue
        ax.plot(
            X[j * N_nodes: (j + 1) * N_nodes],
            Y[j * N_nodes: (j + 1) * N_nodes],
            Z[j * N_nodes: (j + 1) * N_nodes],
            c='k',
            solid_capstyle="round",
            alpha=0.8,
            linewidth=2.5,
        )
    #ax.set_xlim(-r_cyl * 1.1, r_cyl * 1.1)
    #ax.set_ylim(-r_cyl * 1.1, r_cyl * 1.1)
    #ax.set_zlim(0, 140)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.set_zlabel("z")
    ax.set_title(f"N: {num_rod}")

    # Plot Front
    ax = fig.add_subplot(2,4,6)
    for j in range(0, n_rods):
        ax.axhline(y=0.0)
        ax.axhline(y=current_top_height(time))
        ax.plot(
            X[j * N_nodes: (j + 1) * N_nodes],
            Z[j * N_nodes: (j + 1) * N_nodes],
            c=to_rgb("xkcd:reddish"),
            solid_capstyle="round",
            alpha=0.8,
            linewidth=2.5,
        )
        if not mask[j]:
            continue
        ax.plot(
            X[j * N_nodes: (j + 1) * N_nodes],
            Z[j * N_nodes: (j + 1) * N_nodes],
            c='k',
            solid_capstyle="round",
            alpha=0.8,
            linewidth=2.5,
        )
    ax.axvline(x=-r_cyl, color='r')
    ax.axvline(x=r_cyl, color='r')
    ax.set_title("(red line: wall)")
    #ax.set_xlim(-r_cyl * 1.1, r_cyl * 1.1)
    ax.set_ylim(0, 140)
    ax.set_xlabel("x")
    ax.set_ylabel("z")

    # Plot Top
    ax = fig.add_subplot(2,4,1)
    for j in range(0, n_rods):
        ax.plot(
            X[j * N_nodes: (j + 1) * N_nodes],
            Y[j * N_nodes: (j + 1) * N_nodes],
            c=to_rgb("xkcd:reddish"),
            solid_capstyle="round",
            alpha=0.8,
            linewidth=2.5,
        )
        if not mask[j]:
            continue
        ax.plot(
            X[j * N_nodes: (j + 1) * N_nodes],
            Y[j * N_nodes: (j + 1) * N_nodes],
            c='k',
            solid_capstyle="round",
            alpha=0.8,
            linewidth=2.5,
        )
    ax.add_patch(
        patches.Rectangle((-r_cyl, -r_cyl), r_cyl*2, r_cyl*2, linewidth=1, edgecolor='b', facecolor='none')
    )
    ax.set_title("(blue line: wall)")
    #ax.set_xlim(-r_cyl * 1.1, r_cyl * 1.1)
    #ax.set_ylim(-r_cyl * 1.1, r_cyl * 1.1)
    ax.set_xlabel("x")
    ax.set_ylabel("y")

    # Plot Radial Hist
    ax = fig.add_subplot(4,4,2)
    ax.hist(np.hypot(X, Y), bins=30)
    ax.axvline(x=0, color='r')
    ax.axvline(x=r_cyl, color='r')
    ax.axvline(x=r_cyl*np.sqrt(2), color='m')
    ax.set_xlabel("radial (hypot(x,y)) (red like: wall)")
    ax.set_ylabel("count")

    # Plot z Hist
    ax = fig.add_subplot(4,4,6)
    ax.hist(Z, bins=30)
    ax.axvline(x=0, color='r')
    ax.set_xlabel("z (red line: bround)")
    ax.set_ylabel("count")

    # Plot z-force plot
    l = min(forces.shape[0], 30)
    ax = fig.add_subplot(2,4,3)
    ax.plot(forces[-l:,0], forces[-l:,3] / total_mass, label='z')
    ax.legend()
    ax.set_xlabel("time")
    ax.set_ylabel("z-force (bottom plane)")
    ax.set_ylim(0,1.5)

    # Plot z-force RMSE convergence
    ax = fig.add_subplot(2,4,4)
    for window, rmse in zip(rmse_windows, rmses):
        ax.plot(forces[:,0], rmse, label=f'w-{window}')
    ax.set_yscale('log')
    ax.legend()
    ax.set_xlabel("time")
    ax.set_ylabel("RMSE for z-force")

    # Plot Interpenetration
    base_path = base_paths[i]
    penetration_path = base_path
    penetration_path = penetration_path.replace('rod_coordinates', 'penetrations')
    data = np.genfromtxt(penetration_path)
    data = data.ravel()
    ax = fig.add_subplot(2,4,7)
    ax.hist(data, bins=30)
    ax.set_xlabel("depth")
    ax.set_ylabel("Count")
    ax.set_title(f"Penetration ({len(data)=})")

    plt.savefig(
        "snap" + str(f"%.{7}d" % i) + ".png",
        format="png",
        dpi=250,
        #bbox_inches="tight",
        #pad_inches=0.2,
    )
    plt.clf()
    plt.close('all')

if __name__ == "__main__":
    now = datetime.now()
    datetimestr = now.strftime("%m-%d-%Y_%H-%M-%S")
    # Precompute
    forces = get_force_history()
    rmses = []
    for window in rmse_windows:
        rmses.append(get_rmse(forces[:,3]/total_mass, 1.0, window))
    rmses = np.array(rmses)

    # Run
    num_cores = multiprocessing.cpu_count()
    Parallel(n_jobs=num_cores)(
        delayed(plot_X)(i, forces[:i], rmses[:,:i]) for i in tqdm(range(num_files))
    )
    #for i in tqdm(range(num_files)):
    #    plot_X(i)
    os.system(
            f"ffmpeg -r 60 -y -s 3840x2160 -f image2 -pattern_type glob -i 'snap*.png' -vcodec libx264 -crf 15 -pix_fmt yuv420p -vf 'crop=trunc(iw/2)*2:trunc(ih/2)*2' nest_{datetimestr}.mp4"
    )
    os.system("rm -f snap*.png")
