const http = require('http');
const port = 8080;

http.createServer((req, res) => {
    res.writeHead(200, { "Content-Type": "text/html" });
    res.write(`<p>${process.env.HOSTNAME}</p>`);
    res.end();
}).listen(port, () => {
    console.log(`App is running on port ${port}`);
});