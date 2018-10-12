#!/bin/sh

if ! [ -e backend -a backend -nt backend.c ]; then
	echo "===> Compiling backend simulator"
	cc -o backend backend.c
fi

echo "===> Running backend simulator"

./backend &
backend_pid="$!"

echo "===> Running nginx"

nginx -p . -c nginx.conf >/dev/null 2>&1 &
nginx_pid="$!"

echo "===> Waiting 1 sec for everybody to start"

sleep 1

kill -0 "$backend_pid" >/dev/null 2>&1 || { echo "backend seem to have failed to start"; kill "$nginx_pid" >/dev/null 2>&1; exit 1; }
kill -0 "$nginx_pid" >/dev/null 2>&1 || { echo "nginx seem to have failed to start"; kill "$backend_pid" >/dev/null 2>&1; exit 1; }

echo "===> Trying request"

echo "/!\\ First request: backend replies in 0.5 seconds"

time curl --silent http://localhost:8080 >/dev/null

echo "/!\\ Second request: backend replies in 1.5 seconds"

time curl --silent http://localhost:8080 >/dev/null

echo "/!\\ If you're seeing more than 2 seconds here, you're experiencing nginx problem"

echo "===> Shutting everything down"

kill "$backend_pid"
kill "$nginx_pid"

wait
