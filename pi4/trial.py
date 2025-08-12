import serial
import time

# Initialize serial connection
ser = serial.Serial(
    port='/dev/serial0',
    baudrate=2400,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
)

ser.flushInput()  # Clear any residual data
print("Connected to /dev/serial0 at 9600 baud")
print("UART receiver started. Press Ctrl+C to exit.")
print("--------------------------------------------------")

try:
    buffer = ""
    while True:
        # Read available data
        data = ser.read(ser.in_waiting or 1)
        
        if data:
            # Print raw bytes for debugging
            print(f"[{time.strftime('%H:%M:%S')}] Raw: {data}")
            
            # Try to decode as ASCII
            try:
                decoded = data.decode('ascii')
                buffer += decoded
                print(f"[{time.strftime('%H:%M:%S')}] ASCII: '{decoded}'")
                
                # Check for complete message (ending with newline)
                if '\n' in buffer:
                    lines = buffer.split('\n')
                    for line in lines[:-1]:  # Process complete lines
                        print(f"[{time.strftime('%H:%M:%S')}] MESSAGE: '{line}'")
                    buffer = lines[-1]  # Keep incomplete line in buffer
                    
            except UnicodeDecodeError:
                print(f"[{time.strftime('%H:%M:%S')}] Non-ASCII data")
                
        time.sleep(0.01)  # Small delay to prevent busy waiting
        
except KeyboardInterrupt:
    print("\nExiting...")
    ser.close()