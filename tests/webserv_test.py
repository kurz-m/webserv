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
	print("opened webserv!")
	yield webserv_process
	print("closing webserv!")
	webserv_process.send_signal(signal.SIGINT)
	webserv_process.wait()

def test_simple_request(get_webserv_backend) -> None:
	print("testing webserv!")
	res = requests.get("http://localhost:8080/")
	assert res.status_code == 200
	assert res.headers['Content-Type'] == 'text/html'
	assert res.headers['Content-Length'] == '159'
	assert res.text == """<html>
<head><title>Hello World!</title></head>
<body>
<center><h1>Hello World!</h1></center>
<hr><center>webserv by makurz and flauer</center>
</body>
</html>"""



