import os
import glob

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns


RESULT_DIR: str = "/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/data"
PLOTS_DIR: str = "/Users/mishashkarubski/CLionProjects/fourier-parallelization/public/results/plots"

sns.set_theme()


def plot_results():
    result_paths: list = glob.glob(os.path.join(RESULT_DIR, "*.csv"))
    conv_result_paths: list = list(filter(lambda x: 'conv' in x, result_paths))
    fft_result_paths: list = list(filter(lambda x: 'fft' in x, result_paths))

    # Convolution results
    fig, ax = plt.subplots(len(conv_result_paths) // 2, 2, figsize=(12, 8))
    fig.tight_layout(pad=4)

    plot_name: str = "convolution"

    for i, path in zip(range(len(conv_result_paths)), conv_result_paths):
        index = np.unravel_index(i, (2, 2))
        df = pd.read_csv(path, index_col='image_size')
        kernel_size = list(df['kernel_size'])[0]

        sns.lineplot(df.drop('kernel_size', axis=1, inplace=False), dashes=True, markers=True, ax=ax[index])

        ax[index].set_xlabel("Size (bins)")
        ax[index].set_ylabel("Time (ms)")
        ax[index].set_title(fr"Convolution with {kernel_size}$\times${kernel_size} kernel")

    plt.suptitle("Time comparison between 2D convolution with differently-sized gaussian kernels")
    plt.subplots_adjust(wspace=0.3, hspace=0.3)
    plt.savefig(os.path.join(PLOTS_DIR, plot_name + ".png"))

    # Fast Fourier Transform results
    fig, ax = plt.subplots(1, len(fft_result_paths), figsize=(12, 4.5))
    fig.tight_layout(pad=4)

    plot_name: str = "fft"

    for i, path in zip(range(len(fft_result_paths)), fft_result_paths):
        df = pd.read_csv(path, index_col='size')

        sns.lineplot(df, dashes=True, markers=True, ax=ax[i])

        ax[i].set_xlabel("Size (bins)")
        ax[i].set_ylabel("Time (ms)")
        ax[i].set_title(fr"{2 - i}D FFT")

    plt.suptitle("Cooley-Tukey Fast Fourier Transform (radix-2 decimation-in-time)")
    plt.subplots_adjust(wspace=0.3, hspace=0.3)
    plt.savefig(os.path.join(PLOTS_DIR, plot_name + ".png"))


if __name__ == '__main__':
    plot_results()
