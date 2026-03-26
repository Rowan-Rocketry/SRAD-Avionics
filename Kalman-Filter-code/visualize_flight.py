from logging import DEBUG, basicConfig, getLogger
import csv
from pathlib import Path

import numpy as np
import matplotlib.pyplot as plt


class KalmanFilter:
    def __init__(
        self,
        initial_position: float = 0.0,
        initial_velocity: float = 0.0,
        process_noise: float = 1.0,
        measurement_noise: float = 5.0,
        csv_file: str = None,
        csv_column: str = "altimeter",
        logging: bool = False,
        plotting: bool = False,
    ):
        self.logging = logging
        self.plotting = plotting

        if self.logging:
            self.logger = getLogger(__name__)
            basicConfig(level=DEBUG, format="%(message)s")

        # -----------------------------
        # Load Data
        # -----------------------------
        if csv_file:
            self.times, self.sensor_values = self._load_from_csv(csv_file, csv_column)
        else:
            raise ValueError("CSV file required for this version")

        # -----------------------------
        # Kalman State (position + velocity)
        # -----------------------------
        self.x = np.array([[initial_position],
                           [initial_velocity]])

        self.P = np.array([[10.0, 0.0],
                           [0.0, 10.0]])

        dt = self._compute_dt()

        # State transition matrix
        self.F = np.array([[1, dt],
                           [0, 1]])

        # Measurement matrix (we only measure position)
        self.H = np.array([[1, 0]])

        # Process noise
        self.Q = np.array([[1, 0],
                           [0, 1]]) * process_noise

        # Measurement noise
        self.R = np.array([[measurement_noise]])

    # -----------------------------
    # CSV Loader
    # -----------------------------
    def _load_from_csv(self, csv_file: str, column: str):
        times = []
        values = []

        with open(csv_file, 'r') as file:
            reader = csv.DictReader(file)
            for row in reader:
                if 'time' in row:
                    times.append(float(row['time']))
                else:
                    times.append(len(times))

                values.append(float(row[column]))

        return times, values

    # -----------------------------
    # Estimate dt from time data
    # -----------------------------
    def _compute_dt(self):
        if len(self.times) > 1:
            return self.times[1] - self.times[0]
        return 1.0

    # -----------------------------
    # Run Filter
    # -----------------------------
    def run(self):
        estimates = []

        for z in self.sensor_values:

            # 🔮 PREDICT
            self.x = self.F @ self.x
            self.P = self.F @ self.P @ self.F.T + self.Q

            # 📏 UPDATE
            z = np.array([[z]])
            y = z - (self.H @ self.x)
            S = self.H @ self.P @ self.H.T + self.R
            K = self.P @ self.H.T @ np.linalg.inv(S)

            self.x = self.x + K @ y
            self.P = (np.eye(2) - K @ self.H) @ self.P

            estimates.append(self.x[0, 0])  # position only

            if self.logging:
                self.logger.info(f"Estimate: {self.x.flatten()}")

        if self.plotting:
            self._plot(estimates)

        return estimates

    # -----------------------------
    # Plot Results
    # -----------------------------
    def _plot(self, estimates):
        x_axis = self.times

        plt.figure(figsize=(12, 6))

        plt.plot(x_axis, self.sensor_values, "x", label="Sensor Data")
        plt.plot(x_axis, estimates, label="Kalman Estimate")

        # Peak detection
        max_altitude = max(self.sensor_values)
        max_index = self.sensor_values.index(max_altitude)
        max_time = x_axis[max_index]

        plt.axvline(x=max_time, linestyle='--', linewidth=2, label="Peak Time")

        plt.xlabel("Time (seconds)")
        plt.ylabel("Altitude (feet)")
        plt.title("Kalman Filter (Position + Velocity)")
        plt.legend()
        plt.grid()

        plt.tight_layout()
        plt.show()


# -----------------------------
# Main
# -----------------------------
if __name__ == "__main__":
    csv_path = Path(__file__).parent / "mock_flight_data.csv"

    kf = KalmanFilter(
        initial_position=0.0,
        initial_velocity=0.0,
        process_noise = 0.5,    
        measurement_noise = 10.0,
        csv_file=str(csv_path),
        csv_column="altimeter",
        logging=False,
        plotting=True,
    )

    kf.run()