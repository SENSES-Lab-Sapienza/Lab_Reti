On Connection

*4
$6
CLIENT
$7
SETINFO
$8
LIB-NAME
$8
redis-py

+OK

*4
$6
CLIENT
$7
SETINFO
$7
LIB-VER
$5
5.0.3

+OK

On set(key, value)

*3
$3
SET
$3
key
$5
value

+OK

On get(key)

*2
$3
GET
$3
key

$5
value

On get(unexisting key)

*2
$3
GET
$16
non-existing-key

$-1

On set(key, duration in seconds e.g. 10)

*5
$3
SET
$3
key
$5
value
$2
EX
$2
10

+OK

