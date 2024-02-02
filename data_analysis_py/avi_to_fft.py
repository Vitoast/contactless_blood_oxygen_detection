import cv2
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import os

def process_video(video_path, save_path=None):
    cap = cv2.VideoCapture(video_path)

    fps = int(cap.get(cv2.CAP_PROP_FPS))
    if fps != 20:
        print("Warning: The video does not have 20fps.")

    average_values = []

    # Step 1: collect frame data (averages)
    while cap.isOpened():
        ret, frame = cap.read()

        if not ret:
            break

        # Convert frame to grayscale
        gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # Calculate FFT on the average pixel value of the frame
        average_values.append(np.mean(gray_frame))

    interpolate = True
    padding_factor = 1

    # if requested insert interpolated values
    if interpolate:
        interpolated_time = np.arange(0, 2 * len(average_values) - 1)
        interpolator = interp1d(interpolated_time[::2], average_values, kind='quadratic')
        average_values = interpolator(interpolated_time)
        fps = fps * 2

    # Step 2: Compute FFT with padded 0s
    fft_result = np.fft.fft(average_values, n=padding_factor * len(average_values))
    fft_result_magnitude = np.abs(fft_result)

    # Step 3: Frequency Bins
    n = len(fft_result)
    freq = np.fft.fftfreq(n, 1 / (fps))

    # Step 4: Plotting
    plt.clf()
    plt.plot(freq[:n // 2], fft_result_magnitude[:n // 2])  # Adjust the range for frequencies up to 40 Hz
    plt.title('Frequency Spectrum')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Amplitude')

    # Save plot if save_path is provided
    if save_path:
        file_name = os.path.splitext(os.path.basename(video_path))[0]
        save_file_path = os.path.join(save_path, f'{file_name}_spectrum.png')
        plt.savefig(save_file_path)
    else:
        plt.show()

    cap.release()

if __name__ == "__main__":
    folder_path = "C:\\Users\\Tobias\\Videos\\"
    save_folder_path = "C:\\Users\\Tobias\\Videos\\plots\\" 

    for file_name in os.listdir(folder_path):
        if file_name.endswith(".avi"):
            video_path = os.path.join(folder_path, file_name)
            process_video(video_path, save_folder_path)
