import serial
import time
import keyboard
import threading

# Set COM Port parameters
port_name = "COM3"
baud_rate = 10417
data_bits = serial.EIGHTBITS
parity = serial.PARITY_NONE
stop_bits = serial.STOPBITS_ONE

# Flag to indicate if the script should quit
quit_flag = False

def check_for_quit():
    global quit_flag
    while not quit_flag:
        if keyboard.is_pressed('q'):
            quit_flag = True
            print("Quit key 'q' pressed. Exiting...")

try:
    # Create a new SerialPort object
    serial_port = serial.Serial(
        port=port_name,
        baudrate=baud_rate,
        bytesize=data_bits,
        parity=parity,
        stopbits=stop_bits,
        timeout=1  # shorter timeout in seconds
    )
    print(f"Connected to {port_name} at {baud_rate} baud.")
except serial.SerialException as e:
    print(f"Failed to open port: {e}")
    exit()

# Start a thread to monitor the 'q' key press
quit_thread = threading.Thread(target=check_for_quit)
quit_thread.start()

# Generate a filename with a timestamp
timestamp = time.strftime("%Y%m%d_%H%M%S")
file_path = f"output_{timestamp}.bin"
file_stream = open(file_path, "wb")

# Record data for 5 seconds or until 'q' is pressed
start_time = time.time()
end_time = start_time + 5
print("Starting data recording for 5 seconds... (Press 'q' to quit)")

try:
    while time.time() < end_time and not quit_flag:
        current_time = time.time()
        print(f"Current time: {current_time}, End time: {end_time}")

        try:
            # Read a chunk of data from the serial port
            print("Attempting to read from serial port...")
            raw_data = serial_port.read(1024)  # Read up to 1024 bytes
            if raw_data:
                print(f"Raw data: {raw_data}")

                # Write the raw binary data to the file
                file_stream.write(raw_data)
            else:
                print("No data received.")
        except serial.SerialTimeoutException:
            print("Read timeout occurred, continuing...")
        except Exception as e:
            print(f"Error reading from COM port: {e}")
            break
except Exception as e:
    print(f"Unexpected error: {e}")
finally:
    # Close the file and the port when done
    quit_flag = True  # Ensure the quit thread exits
    quit_thread.join()  # Wait for the quit thread to finish
    file_stream.close()
    serial_port.close()
    print(f"Port {port_name} closed and file {file_path} saved.")
