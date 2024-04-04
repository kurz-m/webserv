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
          if (response.status == 201) {
            location.replace(serverUrl);
          }
          console.log(response);
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