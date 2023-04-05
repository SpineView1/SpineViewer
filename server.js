const express = require('express');
const app = express();
const port = 3000; // Set the port number for your server

// Serve static files from the current directory and the js directory
app.use(express.static(__dirname));
app.use('/js', express.static(path.join(__dirname, 'js')));

app.get('/', (req, res) => {
    const filename = req.query.filename || '111.stl';
    const filePath = `${__dirname}/index.html`;
    const queryParams = `?filename=${filename}`;
    const fullPath = filePath + queryParams;
    res.sendFile(fullPath);
});


app.listen(port, () => {
    console.log(`Server listening on port ${port}.`);
});
