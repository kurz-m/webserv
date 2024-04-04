async function displayFileContentsAsync(response) {
  try {
    const response = await fetch('https://api.github.com/repos/github/gitignore/contents/Node.gitignore');
    const text = await response.text();
    console.log(text);
  } catch (error) {
    console.error(error);
  }
}

function init() {
  const button = document.getElementById('sendButton');
  const uploadButton = document.getElementById('selectFile');
  
  uploadButton.addEventListener('click', () => {
    fileInput.click();
  });
  
  button.addEventListener('click', () => {
    const file = fileInput.files[0];
    if (file) {
      const reader = new FileReader();
      
      reader.onload = (e) => {
        const rawData = e.target.result;
        const serverUrl = origin + '/upload/' + file.name;
        
        fetch(serverUrl, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/octet-stream'
          },
          body: rawData
        })
        .then(response => {
          document.body.innerHTML = response.text();
        //   const link = document.createElement('a');
        //   if (response.status == 201) {
        //     document.getElementById('status').innerText = response.statusText;
        //     link.href = serverUrl;
        //   } else if (response.status == 200) {
        //     link.href = response.url;
        //   } else 
        //   link.text = "click me!";
        //   document.body.appendChild(link);
        //   console.log(response);
        //   // location.replace(response.url);
        //   document.getElementById('status').innerText = response.statusText;
        // }).then(htmlContent => {
        //   document.body.innerHTML = htmlContent;
        })
        .catch(error => console.error('Error: ', error));
      };
      reader.readAsArrayBuffer(file);
    }
  });
}

const fileInput = document.createElement('input'); // Create a hidden file input
fileInput.type = 'file';
fileInput.id = 'file';
fileInput.name = 'file';
window.onload = init;