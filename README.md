# Sample Ping & Traceroute 

Follow me on TWITCH - https://www.twitch.tv/romheat LIVE CODE

Live session code

## tcp-forward

forward tcp port

```
.\portforward -p <port> -s <remote_server> -d <remote_port> [-v]
```

## Playground

There is a mini http server that require python3 in the httpserver folder

httpserver/server.sh init a http server on 8080

- exec the server
- portforward -p 9999 -s localhost -d 8080
- navigate with your browser @ http://localhost:9999

```js
./portforward -p 9999 -s localhost -d 8080 -v

> Forward to localhost > 127.0.0.1 @ 8080
> Listen connections on 0.0.0.0:9999
> Connection accepted from 127.0.0.1

Client 127.0.0.1 -> Remote 127.0.0.1
RECVC: GET / HTTP/1.1
Host: 127.0.0.1:9999
User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:91.0) Gecko/20100101 Firefox/91.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 1
Sec-Fetch-Dest: document
Sec-Fetch-Mode: navigate
Sec-Fetch-Site: none
Sec-Fetch-User: ?1
Cache-Control: max-age=0


Remote 127.0.0.1 -> Client 127.0.0.1
RECVC: HTTP/1.0 200 OK
Server: SimpleHTTP/0.6 Python/3.9.2
Date: Sun, 09 Jan 2022 10:04:04 GMT
Content-type: text/html
Content-Length: 402
Last-Modified: Sat, 08 Jan 2022 23:16:20 GMT

ge/webp,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 1
Sec-Fetch-Dest: document
Sec-Fetch-Mode: navigate
Sec-Fetch-Site: none
Sec-Fetch-User: ?1
Cache-Control: max-age=0


Remote 127.0.0.1 -> Client 127.0.0.1
RECVC: <!doctype html>

<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>TEST</title>
  <meta name="description" content="">
  <meta name="author" content="">
</head>

<body>
  HI!!!!
  <p id="new"></p>
  <img src='https://www.kernel.org/theme/images/logos/tux.png'></img>
  <script src="js/script.js"></script>
 </body>
</html>
etch-User: ?1
Cache-Control: max-age=0
```

### Maintainers

[romheat](https://github.com/romheat).

You can follow me on twitter: [![Twitter](http://i.imgur.com/wWzX9uB.png)@romheat](https://www.twitter.com/romheat)

and Twitch -> https://www.twitch.tv/romheats