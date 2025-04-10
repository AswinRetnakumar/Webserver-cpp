import threading


class ThreadPool:

    def __init__(self, max_workers):
        self.max_workers = max_workers
        self.tasks = []
        self.lock = threading.Lock()
        self.condition = threading.Condition(self.lock)
        self.threads = []
        self.stop_tasks = False
        self.kill = False

        for _ in range(max_workers):
            thread = threading.Thread(target=self.worker_loop)
            thread.start()
            self.threads.append(thread)
    
    def worker_loop(self):
        while True:
            task = None
            with self.condition:
                while len(self.tasks) == 0 and not self.stop_tasks:
                    self.condition.wait()
                if self.stop_tasks and len(self.tasks) == 0:
                    break
                if self.kill:
                    break
                task = self.tasks.pop(0)
                
            if task is not None:
                try:
                    task()
                except Exception as e:
                    print(f"Task failed: {e}")


    def close(self):
        print("Closing thread pool on job completion.")
        with self.condition:
            self.stop_tasks = True
            self.condition.notify_all()
        for thread in self.threads:
            thread.join()

    def submit(self, task):
        with self.condition:
            self.tasks.append(task)
            self.condition.notify()

    def kill(self):
        with self.condition:
            self.kill = True
            self.condition.notify_all()
        for thread in self.threads:
            thread.join()
        self.tasks.clear()
                
if __name__ == "__main__":
    import time
    pool = ThreadPool(6)
    for i in range(10):
        pool.submit(lambda i=i: (print(f"Task {i} started."), time.sleep(i), print(f"Task {i} finished.")))
    time.sleep(4)
    pool.submit(lambda: (print("Task 11 started."), [i for i in range(10000000)], print("Task 11 finished.")) )
    pool.close()
    print("All tasks completed.")