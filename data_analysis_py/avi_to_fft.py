import cv2
import numpy as np
import matplotlib.pyplot as plt

def process_video(video_path):
    cap = cv2.VideoCapture(video_path)

    fps = int(cap.get(cv2.CAP_PROP_FPS))
    if fps != 20:
        print("Warning: The video does not have 20fps.")

    average_values = []

    while cap.isOpened():
        ret, frame = cap.read()

        if not ret:
            break

        # Convert frame to grayscale
        gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # Calculate FFT on the average pixel value of the frame
        average_values.append(np.mean(gray_frame))

    # Step 2: Compute FFT
    fft_result = np.fft.fft(average_values)
    fft_result_magnitude = np.abs(fft_result)

    # Step 3: Frequency Bins
    n = len(fft_result)
    freq = np.fft.fftfreq(n, 1/fps)

    # Step 4: Plotting
    plt.plot(freq, fft_result_magnitude)
    plt.title('Frequency Spectrum')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Amplitude')
    plt.show()

    cap.release()

if __name__ == "__main__":
    video_path = "C:\\Users\\Tobias\\Videos\\" + "onlyor_5hz.avi"
    process_video(video_path)
