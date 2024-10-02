import serial
import time

# Open the serial port (replace 'COM7' with your correct port)
ser = serial.Serial('COM7', 115200)  # Adjust port and baud rate
time.sleep(2)  # Wait for the serial connection to establish

print("Connected to Arduino. Displaying LCD output in real-time:")
tag_id=0
pass_wrong=0
while True:
    # Read any incoming message from Arduino
    if ser.in_waiting > 0:
        message = ser.readline().decode('utf-8').strip()
        len_message=len(message)
        print(f"LCD Message: {message}")  # Print all messages sent from Arduino

        if message == "tagId is : 131.107.229.39":
            print("Tag of 1")
            tag_id=1
            pass_wrong=0
            ser.write("tags\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()
            ser.write(f"{tag_id}\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()

        if message == "tagId is : 227.73.25.42":
            print("Tag of 2")
            tag_id=2
            pass_wrong=0
            ser.write("tags\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()
            ser.write(f"{tag_id}\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()

        if message == "tagId is : 42.170.46.2":
            print("tTag of 3")
            tag_id=3
            pass_wrong=0
            ser.write("tags\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()
            ser.write(f"{tag_id}\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()
                
        if tag_id == 1:
            if message == "1111#":    
                print("Password accepted.")
                pass_wrong=1
                ser.write("passwords_correct\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()

        if tag_id == 2:
            if message == "2222#":
                print("Password accepted.")
                pass_wrong=1
                ser.write("passwords_correct\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()

        if tag_id == 3:
            if message == "3333#":
                print("Password accepted.")
                pass_wrong=1
                ser.write("passwords_correct\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()

        if len_message != 0:
            if message[-1] == "#":
                if pass_wrong==0:
                    print("Password Wrong.")
                    ser.write("passwords_wrong\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()

        if len_message != 0:
            if message[-1] == "*":
                print("Password Clear.")
                ser.write("password_clear\n".encode('utf-8'))  # Send the command to Arduino to call RECOGNITION()
    # Optional: add a small delay to avoid high CPU usage
    time.sleep(0.1)
