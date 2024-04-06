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
    time.sleep(1)
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


def test_autoindex(get_webserv_backend) -> None:
    res = requests.get("http://localhost:3490/")
    assert res.status_code == 200
    assert "Directory Listing" in res.text


def test_index_redirect(get_webserv_backend) -> None:
    res = requests.get("http://localhost:8080/")
    assert res.status_code == 200
    assert "Hello World" in res.text

def test_autoindex_off(get_webserv_backend) -> None:
    res = requests.get("http://localhost:8081/")
    assert res.status_code == 404

def test_forbidden_function(get_webserv_backend) -> None:
    res = requests.delete("http://localhost:8080/")
    assert res.status_code == 403
  

def test_post(get_webserv_backend) -> None:
    with open("tests/assets/testimage.png", mode="rb") as f:
        data = f.read()
    res = requests.post("http://localhost:3490/testimage.png", data)
    assert res.status_code == 201
    res = requests.get(res.url)
    assert res.status_code == 200
    assert res.headers['Content-Type'] == 'image/png'
    assert int(res.headers['Content-Length']) == 749879
    res = requests.delete(res.url)
    assert res.status_code == 200

