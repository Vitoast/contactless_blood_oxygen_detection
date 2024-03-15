import cv2
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import os
import re

# Function to analyse a video input
def process_video(video_path, save_path=None, file_name=None):
    # Enable the average FFT plot
    show_filtered_pictures = True
    # Enable the filtered picture output for IR and OR
    show_plotted_frequencies = True
    # Adjust frequencies for IR and OR light and get information from file_name
    target_frequency_IR, target_frequency_OR = parse_filename(file_name)
    target_frequency_IR = 6
    print(target_frequency_IR, target_frequency_OR)
    # Get video input
    cap = cv2.VideoCapture(video_path)
    fps = int(cap.get(cv2.CAP_PROP_FPS))
    if fps != 20:
        print("Warning: The video does not have 20fps.")
    # Adjust interpolation and padding of data in case of undersampling
    interpolate = (target_frequency_OR > fps/2) | (target_frequency_IR > fps/2)
    undersampled = interpolate
    padding_factor = 1
    
    average_values = []
    gray_frames = []
    frame_count = 0
    
    # Collect frame data (plus averages)
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break
        # Convert frame to grayscale
        gray_frames.append(cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY))        
        # Calculate average pixel value of the frame
        average_values.append(np.mean(gray_frames[frame_count]))
        frame_count += 1

    # Ensure data is provided
    if len(gray_frames) <= 0 :
        if len(gray_frames[0]) <= 0:
            print("No valid input data provided.")
            return -1        
    frame_width = len(gray_frames[0])
    frame_depth = len(gray_frames[0][0])

    # This part calculates and plots the FFT of the average pixel value
    if show_plotted_frequencies:
        
        # if requested insert interpolated values
        if interpolate:
            interpolated_time = np.arange(0, 2 * len(average_values) - 1)
            interpolator = interp1d(interpolated_time[::2], average_values, kind='quadratic')
            average_values = interpolator(interpolated_time)
            fps = fps * 2      

        # Compute FFT with padded zeros if requested
        fft_result = np.fft.fft(average_values, n=padding_factor * len(average_values))
        fft_result_magnitude = np.abs(fft_result)

        # Frequency Bins
        n = len(fft_result)
        freq = np.fft.fftfreq(n, 1 / (fps))

        # Plotting
        plt.clf()
        plt.plot(freq[:n // 2], fft_result_magnitude[:n // 2])  # Adjust the range for frequencies
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
    
    # This part caclulates the FFT values for each pixel, filters the frequency and shows the resulting picture
    if show_filtered_pictures & (not undersampled): 
        
        fft_result = []
        fft_result_magnitude = []

        # Iterate over every pixel to get its FFT
        for pixel_x in range(frame_width):
            fft_result.append([])
            fft_result_magnitude.append([])
            for pixel_y in range(frame_depth):
                time_series = []
                # Iterate over every frame for every pixel
                for frame_number in range(frame_count):
                    time_series.append(gray_frames[frame_number][pixel_x][pixel_y])
                # Get FFT for current pixel
                fft_result[pixel_x].append(np.fft.fft(time_series, n=padding_factor * len(time_series)))
                fft_result_magnitude[pixel_x].append(np.abs(fft_result[pixel_x][pixel_y]))

        # Compute index in FFT result that corresponds to frequency
        target_index_IR = int(target_frequency_IR * len(fft_result_magnitude[0][0]) / fps)
        target_index_OR = int(target_frequency_OR * len(fft_result_magnitude[0][0]) / fps)
       
        # Compute the image values for the filtered data
        filtered_fft_image_IR = []
        filtered_fft_image_OR = []
        for pixel_x in range(frame_width):
            filtered_fft_image_IR.append([])
            filtered_fft_image_OR.append([])
            for pixel_y in range(frame_depth):
                filtered_fft_image_IR[pixel_x].append(fft_result_magnitude[pixel_x][pixel_y][target_index_IR])
                filtered_fft_image_OR[pixel_x].append(fft_result_magnitude[pixel_x][pixel_y][target_index_OR])

        # Plot the filtered IR image
        if target_frequency_IR > 0:
            plt.clf()        
            plt.imshow(filtered_fft_image_IR, cmap='gray')
            plt.title(f'Infrared filtered: {target_frequency_IR} Hz')
            plt.colorbar()
            # Save plot if save_path is provided
            if save_path:
                file_name = os.path.splitext(os.path.basename(video_path))[0]
                save_file_path = os.path.join(save_path, f'{file_name}_IR-filtered.png')
                plt.savefig(save_file_path)
            else:
                plt.show()
        
        # Plot the filtered OR image
        if target_frequency_OR > 0:
            plt.clf()        
            plt.imshow(filtered_fft_image_OR, cmap='gray')
            plt.title(f'Orange light filtered: {target_frequency_OR} Hz')
            plt.colorbar()
            # Save plot if save_path is provided
            if save_path:
                file_name = os.path.splitext(os.path.basename(video_path))[0]
                save_file_path = os.path.join(save_path, f'{file_name}_OR-filtered.png')
                plt.savefig(save_file_path)
            else:
                plt.show()

    # Free video input
    cap.release()

# Use this to get information about used modulation frequencies from video file names
def parse_filename(name):
    # Define the regular expression pattern
    pattern = r'IR-(\d+)_OR-(\d+)'

    # Ffind all matches of the pattern in the string
    matches = re.findall(pattern, name)

    if matches:
        # Extract the numbers from the matches and convert them to numbers
        ir_number = int(matches[0][0])
        or_number = int(matches[0][1])

        return ir_number, or_number
    else:
        print("Input file does not have correct naming.")
        return None, None

# Main Function that handles paths
if __name__ == "__main__":
    folder_path = "C:\\Users\\Tobias\\Videos\\"
    save_folder_path = "C:\\Users\\Tobias\\Videos\\plots\\" 

    for file_name in os.listdir(folder_path):
        if file_name.endswith(".avi"):
            video_path = os.path.join(folder_path, file_name)
            process_video(video_path, save_folder_path, file_name)
