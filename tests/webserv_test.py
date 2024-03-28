import subprocess
import requests
import pytest
import signal
from pathlib import Path
from typing import Generator, Any


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