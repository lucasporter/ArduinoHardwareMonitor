import wmi
import serial
import time

# Connect to Arduino (adjust ttyS4 if needed!)
arduino = serial.Serial('/dev/ttyS4', 9600)
time.sleep(2)  # Give Arduino time to reset

# Connect to Windows WMI sensors
w = wmi.WMI(namespace="root\\LibreHardwareMonitor")

def get_sensor_readings():
    sensors = {"CPU": None, "GPU": None, "VRM": None}
    for sensor in w.Sensor():
        if sensor.SensorType == 'Temperature':
            if 'CPU' in sensor.Name and sensors["CPU"] is None:
                sensors["CPU"] = sensor.Value
            elif 'GPU' in sensor.Name and sensors["GPU"] is None:
                sensors["GPU"] = sensor.Value
            elif 'VRM' in sensor.Name and sensors["VRM"] is None:
                sensors["VRM"] = sensor.Value
    return sensors

while True:
    sensor_data = get_sensor_readings()
    cpu = sensor_data.get('CPU', 0)
    gpu = sensor_data.get('GPU', 0)
    vrm = sensor_data.get('VRM', 0)

    message = f"CPU:{cpu:.1f};GPU:{gpu:.1f};VRM:{vrm:.1f}\n"
    print(f"Sending: {message.strip()}")
    
    arduino.write(message.encode('utf-8'))

    time.sleep(2)

