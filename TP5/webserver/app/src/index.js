import http from 'http';
import * as fs from "fs/promises";
import pg from "pg";

const port = 8080;

const { Client } = pg;
const config = { user: "postgres", password: "postgres", database: "postgres", port: "5432" }

const client = new Client(config);

await client.connect();

http.createServer(async (req, res) => {
    console.log(req.url);

    if (req.url == "/") {
        res.writeHead(200, { "Content-Type": "text/html" });
        const data = await fs.readFile("public/index.html");
        res.write(data);
        res.end();
    }

    else if (req.url == "/index.js") {
        res.writeHead(200, { "Content-Type": "text/javascript" });
        const data = await fs.readFile("public/index.js");
        res.write(data);
        res.end();
    }

    else if (req.url == "/login" && req.method == "POST") {
        let body = [];
        req.on('data', chunk => {
            body.push(chunk);
        })
            .on('end', async () => {
                body = Buffer.concat(body).toString();
                const jsonBody = JSON.parse(body);
                const response = await client.query(`SELECT * from utilisateur WHERE username='${jsonBody.username}' AND password='${jsonBody.password}'`);
                console.log(response.rows);
                if (response.rows.length == 1) {
                    res.writeHead(200, { "Content-Type": "text/plain" });
                    res.write(process.env.HOSTNAME);
                    res.end();
                }
                else {
                    res.writeHead(403, { "Content-Type": "text/plain" });
                    res.write("Error");
                    res.end();
                }
            });
    }

    console.log("ok");
}).listen(port, () => {
    console.log(`App is running on port ${port}`);
});
