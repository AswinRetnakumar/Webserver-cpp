import requests
import threading
import pytest

# Define the base URL of the webserver
BASE_URL = "http://localhost:7500"

# Define the routes to test
ROUTES = ["/"]

# Function to test a single route
def test_route(route):
    try:
        response = requests.get(BASE_URL + route)
        assert response.status_code == 200, f"GET {route} - Failed with status code {response.status_code}"
    except requests.exceptions.RequestException as e:
        pytest.fail(f"GET {route} - Failed with exception {e}")

# Function to perform threaded tests
def threaded_test():
    threads = []
    for route in ROUTES:
        thread = threading.Thread(target=test_route, args=(route,))
        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()

@pytest.mark.parametrize("route", ROUTES)
def test_routes(route):
    test_route(route)

def test_threaded_routes():
    threaded_test()