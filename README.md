# nginx backend delay bug

I've experienced strange behavior of nginx with slow uwsgi backend:
if the backend replies in more than around 1.2 seconds, nginx introduces
additional delay, so total reply time raises up to around 2.2 seconds.

A graph of nginx reply time vs. backend reply time demonstrates this
in a more visual way:

![graph](graph.svg)

## Repository contents

This repository contains a setup which reliably reproduces the
problem for me. It consists of a simple HTTPish backed simulator
which replies in 0.5 and 1.5 seconds interchangeably, an nginx
config which uses the backend and a script which runs everything.

## Expected behavior

Obviously, nginx should not add any additional delay over the backend,
so since backend replies in 0.5 seconds and then in 1.5 seconds, nginx
should reply in around the same time.

## Actual behavior

In practice, however, this happens:

```
===> Compiling backend simulator
===> Running backend simulator
===> Running nginx
===> Waiting 1 sec for everybody to start
===> Trying request
/!\ First request: backend replies in 0.5 seconds
        0,52 real         0,01 user         0,00 sys
/!\ Second request: backend replies in 1.5 seconds
        2,23 real         0,00 user         0,00 sys
/!\ If you're seeing more than 2 seconds here, you're experiencing nginx problem
===> Shutting everything down
```

Note that the second response comes with 2.23 seconds instead of expected 1.52 or so.

## ktrace dumps

See ktrace dumps [with bug](ktrace.11.2-bug.txt) [without bug](ktrace.12.x-ok.txt).

The relevant difference is how kevent behaves after nginx sends a reply back to client.

When there's a bug, two events on client fd come come separately:
WRITE with 100 msec delay (introducing its own latency) and then
READ with additional 630 msec.

https://github.com/AMDmi3/nginx-bug-demo/blob/master/ktrace.11.2-bug.txt#L241-L249

When there's no bug, both events come at the same time and immediately:

No bug: https://github.com/AMDmi3/nginx-bug-demo/blob/master/ktrace.12.x-ok.txt#L226-L227

## Enviroment

- OS
  - :skull: FreeBSD 11.2-RELEASE-p1 amd64
  - :skull: FreeBSD 12.0-CURRENT amd64 d71bcb11d(drm-next)
  - :+1: FreeBSD 12.0-CURRENT amd64 (r333375) - does not reproduce
  - :+1: FreeBSD 12.0-CURRENT amd64 ce438e4cdd8(master) - does not reproduce
- nginx
  - :skull: 1.14.0
  - :skull: 1.15.4
  - :skull: 1.15.5

## Contacts

- Dmitry Marakasov <amdmi3@amdmi3.ru>
