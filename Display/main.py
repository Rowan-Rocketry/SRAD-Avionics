import requests
import json
import time
import matplotlib.pyplot as plt
from collections import deque

# --- Configuration ---
SENDER_URL = "http://192.168.1.50/data"  # URL of your sender ESP
REFRESH_INTERVAL = 1.0  # seconds between fetches
MAX_POINTS = 50  # how many data points to keep in the chart

# --- Data storage ---
times = deque(maxlen=MAX_POINTS)
altitude_data = deque(maxlen=MAX_POINTS)
accel_x = deque(maxlen=MAX_POINTS)
accel_y = deque(maxlen=MAX_POINTS)
accel_z = deque(maxlen=MAX_POINTS)
gyro_x = deque(maxlen=MAX_POINTS)
gyro_y = deque(maxlen=MAX_POINTS)
gyro_z = deque(maxlen=MAX_POINTS)

# --- Setup Matplotlib ---
plt.ion()
fig, axs = plt.subplots(3, 1, figsize=(8, 8))
fig.suptitle("Live Sensor Data")

# Altitude plot
alt_line, = axs[0].plot([], [], 'b-', label="Altitude")
axs[0].set_title("Altitude")
axs[0].set_ylabel("m")
axs[0].grid(True)
axs[0].legend()

# Acceleration plot
accel_lines = {
    'x': axs[1].plot([], [], 'r-', label="Accel X")[0],
    'y': axs[1].plot([], [], 'g-', label="Accel Y")[0],
    'z': axs[1].plot([], [], 'b-', label="Accel Z")[0]
}
axs[1].set_title("Acceleration")
axs[1].set_ylabel("m/s²")
axs[1].grid(True)
axs[1].legend()

# Gyro plot
gyro_lines = {
    'x': axs[2].plot([], [], 'r-', label="Gyro X")[0],
    'y': axs[2].plot([], [], 'g-', label="Gyro Y")[0],
    'z': axs[2].plot([], [], 'b-', label="Gyro Z")[0]
}
axs[2].set_title("Gyroscope")
axs[2].set_ylabel("°/s")
axs[2].set_xlabel("Time (s)")
axs[2].grid(True)
axs[2].legend()


def fetch_data():
    """Fetch JSON data from ESP sender."""
    try:
      ui_utils.add_separator()
        response = requests.get(SENDER_URL, timeout=2)
        response.raise_for_status()
        data = response.json()
        return {
            "altitude": data.get("altitude", 0),
            "accel": data.get("accel", {"x": 0, "y": 0, "z": 0}),
            "gyro": data.get("gyro", {"x": 0, "y": 0, "z": 0}),
            "altOs": data.get("altOs", 0)
        }
    except Exception as e:
        print("Error fetching data:", e)
        return None


def update_plot(t, d):
    """Update live matplotlib plots."""
    times.append(t)
    altitude_data.append(d["altitude"])
    accel_x.append(d["accel"]["x"])
    accel_y.append(d["accel"]["y"])
    accel_z.append(d["accel"]["z"])
    gyro_x.append(d["gyro"]["x"])
    gyro_y.append(d["gyro"]["y"])
    gyro_z.append(d["gyro"]["z"])

    # Update altitude
    alt_line.set_data(times, altitude_data)
    axs[0].set_xlim(min(times), max(times) if times else 1)
    axs[0].set_ylim(min(altitude_data, default=0)-1, max(altitude_data, default=1)+1)

    # Update accel
    accel_lines['x'].set_data(times, accel_x)
    accel_lines['y'].set_data(times, accel_y)
    accel_lines['z'].set_data(times, accel_z)
    axs[1].set_xlim(min(times), max(times) if times else 1)
    axs[1].set_ylim(-10, 10)

    # Update gyro
    gyro_lines['x'].set_data(times, gyro_x)
    gyro_lines['y'].set_data(times, gyro_y)
    gyro_lines['z'].set_data(times, gyro_z)
    axs[2].set_xlim(min(times), max(times) if times else 1)
    axs[2].set_ylim(-200, 200)

    plt.pause(0.01)


# --- Main loop ---
print("Starting live data plot...")
start_time = time.time()
while True:
    data = fetch_data()
    if data:
        elapsed = time.time() - start_time
        update_plot(elapsed, data)
    time.sleep(REFRESH_INTERVAL)
