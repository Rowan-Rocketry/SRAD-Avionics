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
        self.logging = logging
        self.plotting = plotting

        # Load data from CSV file if provided, otherwise use synthetic data
        if csv_file:
            self.sensor_values = self._load_from_csv(csv_file, csv_column)
        else:
            self.sensor_values = [70 + randint(-20, 20) for _ in range(sensor_values)]

        if self.logging:
            self.logger = getLogger(__name__)
            basicConfig(level=DEBUG, format="%(message)s")

    def _load_from_csv(self, csv_file: str, column: str) -> list:
        """Load sensor values from a CSV file
        
        Args:
            csv_file: Path to the CSV file
            column: Name of the column to extract
            
        Returns:
            List of numeric values from the specified column
        """
        sensor_data = []
        csv_path = Path(csv_file)
        
        if not csv_path.exists():
            raise FileNotFoundError(f"CSV file not found: {csv_file}")
        
        try:
            with open(csv_path, 'r') as file:
                reader = csv.DictReader(file)
                for row in reader:
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
        
        return sensor_data

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
            plt.plot(
                range(len(self.sensor_values)),
                self.sensor_values,
                "x",
                color="gray",
                label="sensor values",
            )
            plt.plot(
                range(len(self.sensor_values)), e, "-k", color="blue", label="Kalman estimate"
            )
            plt.legend(loc="upper left")
            # Dynamically set y-axis limits based on data range
            min_val = min(min(self.sensor_values), min(e))
            max_val = max(max(self.sensor_values), max(e))
            margin = (max_val - min_val) * 0.1  # 10% margin
            plt.ylim(min_val - margin, max_val + margin)
            plt.xlabel("Time Index")
            plt.ylabel("Value")
            plt.title("Kalman Filter Estimation")
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
        # Aggressive (more responsive)
        initial_est_error=2.0,
        initial_measure_error=4.0,  
        csv_file=str(csv_path),
        csv_column="altimeter",  # Change to other columns like 'kalman_velocity' as needed
        logging=True,
        plotting=True,
    )
    
    kf.iterative_updates()
