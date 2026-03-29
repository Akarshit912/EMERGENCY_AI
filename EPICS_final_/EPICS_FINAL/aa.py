import requests

# Replace with your deployed API URL
API_URL = "https://esp32-ml-api-1.onrender.com/predict"

# Example input
data = {
    "temperature": 24,
    "humidity": 96,
    "lat": 28.07,
    "lon": 73.85
}

try:
    response = requests.post(API_URL, json=data, timeout=10)
    response.raise_for_status()  # Raises error if status != 200
    print("API is working!")
    print("Response JSON:", response.json())

except requests.exceptions.RequestException as e:
    print("API request failed:", e)
