import socket
import pytest
import threading

# Define the server address and port
SERVER_ADDRESS = "localhost"
SERVER_PORT = 7500

# Function to test if the connection to the server is successful
def test_connection_single():
    try:
        # Create a socket
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # Attempt to connect to the server
        client_socket.connect((SERVER_ADDRESS, SERVER_PORT))
        print("[SUCCESS] Connection to the server was successful.")
        client_socket.close()
    except Exception as e:
        pytest.fail(f"[FAILURE] Failed to connect to the server: {e}")

def threaded_connection_test():
    def connect():
        try:
            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.connect((SERVER_ADDRESS, SERVER_PORT))
            print("[SUCCESS] Connection to the server was successful.")
        
            client_socket.close()
        except Exception as e:
            pytest.fail(f"[FAILURE] Threaded connection failed: {e}")

    threads = []
    for _ in range(100):  # Create 1000 threads
        thread = threading.Thread(target=connect)
        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()

# Test function for pytest
def test_server_connection():
    test_connection_single()

def test_threaded_connections():
    threaded_connection_test()