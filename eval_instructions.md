# webserv evaluation instrucions

***

###### Install Siege on Fedora
 - dnf install siege --downloadonly
 - dnf download siege --destdir=/tmp/siege_rpm
 - cd /tmp/siege_rpm
 - rpm2cpio siege*.rpm | cpio -idmv
 - ./usr/bin/siege.config
 - cd webserv
 - /tmp/siege_rpm/usr/bin/siege -t30S -b http://127.0.0.1:8080/empty.html


###### Before starting remember to set the config root(s) to the absolute path of the content.
ex: `/path/to/downloaded/repo/rootfolder`

## Mandatory Part

#### The select() (or equivalent) should be in the main loop and should check file descriptors for read and write AT THE SAME TIME. If not, the grade is 0 and the evaluation process ends now.

- check ==THERE 1==

#### There should be only one read or one write per client per select() (or equivalent). Ask the group to show you the code from the select() (or equivalent) to the read and write of a client.

- check ==THERE 2==

#### Search for all read/recv/write/send on a socket and check that, if an error is returned, the client is removed.

- check ==THERE 3==

#### Search for all read/recv/write/send and check if the returned value is correctly checked (checking only -1 or 0 values is not enough, both should be checked).

- check ==THERE 4==

#### If errno is checked after read/recv/write/send, the grade is 0 and the evaluation process ends now.

- check ==THERE 5==

#### Writing or reading ANY file descriptor without going through the select() (or equivalent) is strictly FORBIDDEN.

- check ==THERE 6==

***

## Configuration

#### Search for the HTTP response status codes list on the internet. During this evaluation, if any status codes is wrong, don't give any related points.

- check ==THERE 7==

#### Setup multiple servers with different ports.

- run `./webserv configuration_files/eval01.conf`
- browser to `http://localhost:8080`
- browser to `http://localhost:9090`

#### Setup multiple servers with different hostnames (use something like: curl --resolve example.com:80:127.0.0.1 http://example.com/). 

- run `./webserv configuration_files/eval01.conf`
- new terminal window
- `curl --resolve second.com:9090:127.0.0.1 http://second.com:9090/` 
- `curl --resolve first.com:8080:127.0.0.1 http://first.com:8080/`

#### Setup default error page (try to change the error 404).

- browser to `http://localhost:8080/ciao.html`
- check ==THERE 8==

#### Limit the client body (use: curl -X POST -H "Content-Type: plain/text" --data "BODY IS THERE write something shorter or longer than body limit").

- run `./webserv configuration_files/eval01.conf`
- change global and /uploads/ max_body_size
- `curl -X POST -F "file3=@cat.png" http://localhost:8080/uploads/`

#### Setup routes in a server to different directories.

- run `./webserv configuration_files/eval01.conf`
- try and access other folders like `/uploads/`

#### Setup a default file to search for if you ask for a directory.

- see index index.html in conf file

#### Setup a list of methods accepted for a certain route (e.g., try to delete something with and without permission).

- run `./webserv configuration_files/eval01.conf`
- `curl -X POST -F "file3=@cat.png" http://localhost:8080/uploads/`
- `curl -X POST -F "file3=@cat.png" http://localhost:8080/static/`
- `curl -i -X DELETE http://localhost:8080/uploads/cat.png`

***

## Basic checks

#### UNKNOWN requests should not result in a crash.

- `printf "CIAO /uploads/text.txt HTTP/1.1\r\nHost: 127.0.0.1 \r\nUser-Agent: curl/7.68.0\r\n\r\n" | nc 127.0.0.1 8080`

#### Upload some file to the server and get it back.

- `curl -X POST -F "file3=@cat.png" http://localhost:8080/uploads/`
- `curl -o downloaded_cat.png http://localhost:8080/uploads/cat.png`

***

## Check CGI

#### The server is working fine using a CGI.

- http://localhost:8080/cgi-bin/add.py?a=5&b=10

#### The CGI should be run in the correct directory for relative path file access.

- check ==THERE 9==
- add the following after chdir() if you want to see it
``` c++
char cwd[PATH_MAX];
if (getcwd(cwd, sizeof(cwd)) != NULL)
std::cout << RED << "[CWD]> " << cwd << RESET << std::endl;
else
printBoxError("getcwd failed");
```

### With the help of the students you should check that everything is working properly. You have to test the CGI with the "GET" and "POST" methods.

- browser to `localhost:8080/cgi-bin/pet_gallery.py`
- browser to `http://localhost:8080/cgi-bin/simple_post.py`

#### You need to test with files containing errors to see if the error handling works properly. You can use a script containing an infinite loop or an error; you are free to do whatever tests you want within the limits of acceptability that remain at your discretion. The group being evaluated should help you with this. 

- browser to `http://localhost:8080/cgi-bin/error.py`
- browser to `http://localhost:8080/cgi-bin/loop.py`

***

## Check with a browser

#### Look at the request header and response header.

- on browser -> inspect -> network -> raw request/response

#### Try a wrong URL on the server.

- browser to `http://localhost:8080/ciao.html`

#### Try to list a directory.

- visit a folder with autoindex on that does not have an index.html
- run `./webserv configuration_files/eval01.conf`
- browser to `http://localhost:8080/uploads/`

#### Try a redirected URL.

- run `./webserv configuration_files/eval01.conf`
- browser to `http:localhost:8080/redirect/`
- check redirection in terminal ==PRINT Response==

***

## Port issues

#### In the configuration file setup multiple ports and use different websites. Use the browser to ensure that the configuration works as expected and shows the right website.

- see question above

#### In the configuration, try to setup the same port multiple times. It should not work.

- run `./webserv configuration_files/eval02.conf`

#### Launch multiple servers at the same time with different configurations but with common ports. Does it work? If it does, ask why the server should work if one of the configurations isn't functional. Keep going.

- run `./webserv configuration_files/eval03.conf`

***

## Siege and stress test

#### Use Siege to run some stress tests.

- run `./webserv configuration_files/eval01.conf`
- in a new terminal window run `siege -t30S -b http://localhost:8080/empty.html`

#### Verify there is no memory leak (Monitor the process memory usage. It should not go up indefinitely).

- run commands above
- in a third terminal window run `ps aux | grep webserv`
- copy the ./webserv pid and run `top -p <PID>` while siege runs
- check that RES value doesn't increase

#### Check if there is no hanging connection.

- run `siege -t30S -b http://localhost:8080/empty.html`
- wait approx 1 min
- run `netstat -an | grep ":8080"`

***

## Bonus part

#### Cookies and session

- check ==THERE 10==
- browser to `http:localhost:8080/cookie_test.html`
- on browser: inspect -> storage

#### CGI

- browser to `http://localhost:8080/cgi-bin/mult.php?a=15&b=10`