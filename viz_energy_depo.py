#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import sys


def show_energy_depo_heatmap(energy_depo):
    data = np.loadtxt(energy_depo)
    plt.figure(figsize=(8, 6))
    plt.imshow(data, cmap='hot', origin='lower')
    plt.colorbar(label='Energy Deposited (MeV)')
    plt.xlabel('X Position')
    plt.ylabel('Y Position')
    plt.title('2D Energy Deposition Heatmap')
    plt.show()


if __name__ == "__main__":
    assert len(sys.argv) == 2, "Please provide the energy deposition file"
    show_energy_depo_heatmap(sys.argv[1])
