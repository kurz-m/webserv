import subprocess
import requests
import pytest
import signal
import time
from threading import Thread
from pathlib import Path
from typing import Generator, Any, List


@pytest.fixture(scope="module")
def get_webserv_backend() -> Generator[subprocess.Popen[bytes], Any, Any]:
  path = Path(__file__).parent.parent / "webserv"
  webserv_process = subprocess.Popen([str(path)])
  yield webserv_process
  webserv_process.send_signal(signal.SIGINT)
  webserv_process.wait()


def test_simple_request(get_webserv_backend) -> None:
  res = requests.get("http://localhost:8080/")
  assert res.status_code == 200
  assert res.headers["Content-Type"] == "text/html"
  assert res.headers["Content-Length"] == "159"


def test_large_body(get_webserv_backend) -> None:
  def assert_response(res: requests.Response):
      assert res.status_code == 413
      assert res.headers["Content-Type"] == "text/html"
      assert res.headers["Content-Length"] == "196"

  with open("tests/assets/testimg.jpg", mode="rb") as f:
    data = f.read()
    res1 = requests.post("http://localhost:8080/", data=data)
    res2 = requests.get("http://localhost:8080/", data=data)
  assert_response(res1)
  assert_response(res2)


def test_cgi(get_webserv_backend) -> None:
  res = requests.get("http://localhost:8080/cgi-bin/test.py")
  assert res.status_code == 200

def test_stress(get_webserv_backend) -> None:
  global running
  running = True
  
  def test_worker() -> None:
    global running
    while running:
       res = requests.get("http://localhost:8080/")
       assert res.status_code == 200
    
  def worker_cgi() -> None:
    global running
    while running:
      res = requests.get("http://localhost:8080/cgi-bin/test.py")
      assert res.status_code == 200
  
  threads: List[Thread] = []
  for i in range(200):
    thread_ = Thread(target=test_worker)
    thread_.start()
    threads.append(thread_)
  for i in range(200):
    thread_ = Thread(target=worker_cgi)
    thread_.start()
    threads.append(thread_)
  time.sleep(30)
  running = False
  for thread in threads:
    thread.join()
