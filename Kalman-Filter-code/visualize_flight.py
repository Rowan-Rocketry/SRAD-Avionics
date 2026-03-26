from logging import DEBUG, basicConfig, getLogger
from random import randint, random
import csv
from pathlib import Path

import matplotlib.pyplot as plt


class KalmanFilter(object):
    def __init__(
        self,
        initial_estimate: float = random(),
        initial_est_error: float = random(),
        initial_measure_error: float = random(),
        process_noise: float = 0.1,
        sensor_values: list = [],
        csv_file: str = None,
        csv_column: str = "altimeter",
        logging: bool = False,
        plotting: bool = False,
    ):
        self.estimate = initial_estimate
        self.gain = random()
        self.est_error = initial_est_error
        self.measure_error = initial_measure_error
        self.process_noise = process_noise
        self.logging = logging
        self.plotting = plotting

        # Load data from CSV file if provided, otherwise use synthetic data
        if csv_file:
            # _load_from_csv now returns a tuple (times, values)
            self.times, self.sensor_values = self._load_from_csv(csv_file, csv_column)
        else:
            # synthetic data: build a list of readings at the specified rate
            # sensor_values indicates number of samples in total
            self.sensor_values = [70 + randint(-20, 20) for _ in range(sensor_values)]
            # create a corresponding time axis (assume one second between samples)
            self.times = list(range(len(self.sensor_values)))

        if self.logging:
            self.logger = getLogger(__name__)
            basicConfig(level=DEBUG, format="%(message)s")

    def _load_from_csv(self, csv_file: str, column: str) -> tuple[list, list]:
        """Load sensor values and corresponding timestamps from a CSV file
        
        Args:
            csv_file: Path to the CSV file
            column: Name of the column to extract for the sensor values
        
        Returns:
            A tuple ``(times, values)`` where ``times`` is a list of floats
            representing the time for each measurement (if a ``time`` column is
            present in the CSV) or simply ``range(len(values))`` otherwise.

        The returned lists are guaranteed to be the same length.  If the CSV
        contains only one-second increments and the user wants higher precision
        they can up‑sample the data using :meth:`upsample` or regenerate the
        CSV with finer time steps.
        """
        times = []
        sensor_data = []
        csv_path = Path(csv_file)
        
        if not csv_path.exists():
            raise FileNotFoundError(f"CSV file not found: {csv_file}")
        
        try:
            with open(csv_path, 'r') as file:
                reader = csv.DictReader(file)
                for row in reader:
                    # try to collect a time value if the file has one
                    if 'time' in row:
                        try:
                            times.append(float(row['time']))
                        except ValueError:
                            raise ValueError(f"Non-numeric value in 'time' column: {row['time']}")
                    else:
                        # placeholder; we'll replace later with simple indices
                        times.append(None)
                    try:
                        value = float(row[column])
                        sensor_data.append(value)
                    except (ValueError, KeyError) as e:
                        if isinstance(e, KeyError):
                            raise KeyError(f"Column '{column}' not found in CSV. Available columns: {list(row.keys())}")
                        else:
                            raise ValueError(f"Non-numeric value in '{column}' column: {row[column]}")
        except Exception as e:
            raise Exception(f"Error loading CSV file: {str(e)}")
        
        if not sensor_data:
            raise ValueError(f"No data found in CSV file or column '{column}'")

        # if the CSV didn't actually have time values, fall back to indices
        if all(t is None for t in times):
            times = list(range(len(sensor_data)))
        
        return times, sensor_data

    def calculate_kalman_gain(self) -> None:
        """calculates Kalman gain given error values"""
        self.gain = self.est_error / (self.est_error + self.measure_error)

    def update_estimate(self, sensor_value: int = 0.0) -> None:
        """updates estimate based on Kalman gain"""
        new_estimate = self.estimate + self.gain * (sensor_value - self.estimate)
        self.estimate = new_estimate

    def calculate_estimate_error(self) -> None:
        """calculates error of the updated estimate"""
        self.est_error = (1 - self.gain) * self.est_error

    def iterative_updates(self) -> None:
        e = []
        for sensor_value in self.sensor_values:
            self.calculate_kalman_gain()
            self.update_estimate(sensor_value=sensor_value)
            self.calculate_estimate_error()
            if self.logging:
                self.logger.info(f"estimate: {self.estimate}")
            e.append(self.estimate)
        if self.plotting:
            fig = plt.figure(figsize=(12, 6))

            # choose x-axis values; use timestamps if we have them
            x_axis = self.times if hasattr(self, 'times') else list(range(len(self.sensor_values)))

            plt.plot(
                x_axis,
                self.sensor_values,
                "x",
                color="gray",
                label="sensor values",
            )
            plt.plot(
                x_axis, e, "-k", color="green", label="Kalman estimate"
            )
            
            # Find and mark the highest altitude
            max_altitude = max(self.sensor_values)
            max_index = self.sensor_values.index(max_altitude)
            max_time = x_axis[max_index]
            
            # Mark the time when highest altitude occurs with a vertical red line
            plt.axvline(x=max_time, color='red', linestyle='--', linewidth=2, label=f"Max Altitude Time: {max_time}")
            
            # Add annotation at the right edge of the graph
            plt.annotate(f'Max: {max_altitude:.1f} ft', 
                        xy=(x_axis[-1], max_altitude),
                        xytext=(x_axis[-1] - (x_axis[-1]-x_axis[0])*0.1, max_altitude - 1000),
                        bbox=dict(boxstyle="round,pad=0.3", facecolor="yellow", alpha=0.8),
                        arrowprops=dict(arrowstyle="->", color="red"))
            
            plt.legend(loc="upper left")
            # Dynamically set y-axis limits based on data range
            min_val = min(min(self.sensor_values), min(e))
            max_val = max(max(self.sensor_values), max(e))
            margin = (max_val - min_val) * 0.1  # 10% margin
            plt.ylim(min_val - margin, max_val + margin)
            plt.xlabel("Time (seconds)")
            plt.ylabel("Altitude (feet)")
            plt.title("Flight Altitude - Kalman Filter Estimation")
            plt.tight_layout()
            plt.show()


if __name__ == "__main__":
    # Example 1: Using synthetic data (original behavior)
    # kf = KalmanFilter(
    #     initial_estimate=68.0,
    #     initial_est_error=2.0,
    #     initial_measure_error=4.0,
    #     sensor_values=500,
    #     logging=True,
    #     plotting=True,
    # )
    
    # Example 2: Loading flight data from CSV file
    csv_path = Path(__file__).parent / "mock_flight_data.csv"
    
    kf = KalmanFilter(
        initial_estimate=0.0,
        # Tune for more smoothing: trust the model more than noisy measurements
        initial_est_error=1.0,      # Moderate confidence in initial estimate
        initial_measure_error=0.01, # Lower confidence in measurements (more noise)
        csv_file=str(csv_path),
        csv_column="altimeter",  # Change to other columns like 'kalman_velocity' as needed
        logging=True,
        plotting=True,
    )
    
    kf.iterative_updates()
