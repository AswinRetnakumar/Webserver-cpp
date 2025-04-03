import requests
import pytest
import threading

# Define the server address and port
BASE_URL = "http://localhost:7500"


def threaded_connection_test():
    route = '/'
    def connect(route):
        try:
            response = requests.get(BASE_URL + route)
            assert response.status_code == 200, f"GET {route} - Failed with status code {response.status_code}"
        except Exception as e:
            pytest.fail(f"[FAILURE] Threaded connection failed: {e}")

    threads = []
    for _ in range(5000):  # Create 1000 threads
        thread = threading.Thread(target=connect, args=route)
        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()

def test_threaded_connections():
    threaded_connection_test()
