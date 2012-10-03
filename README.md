# Keepup
Automatically monitor, restart, daemonize and notify about process failures without any complicated DSL.

Keepup was written with simplicity in mind, so it just monitors and automatic restart a single process. It has a very simple codebase (less than 150 lines of ANSI C) and no dependencies.

## Usage

```
keepup [options] [command]

-d --daemonize                 Daemonize the process
-p --pidfile <path>            Writes a pidfile for the monitored process
-k --keepup-pidfile <path>     Writes a pidfile for the keepup process
-r --max-retries <number>      Inform the maximum number of allowed retries. When it is set to 0, the process will retry infinitely. Default is 0.
-e --error-command <command>   Execute <command> when the process fail
```

## Example

Automatic restart and daemonize the `node build/server.js` process.
```
$ keepup -d "node build/server.js"
```

Monitor a list of process (by using a simple shell script)

```bash
#!/usr/bin/env bash

keepup -d "mongod run --config /etc/mongodb/mongod.conf"
keepup -d "redis-server /etc/redis/6379.conf"
keepup -d "node /home/deployer/app/server.js"
```
