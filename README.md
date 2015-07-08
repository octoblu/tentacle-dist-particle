# tentacle-build


## Local Build

Using docker with vagrant:

```bash
docker build -t octoblu/tentacle-build:latest .

docker run \
  --volume=/home/core/Projects/Octoblu/tentacle-build:/tentacle-zip \
  --rm octoblu/tentacle-build:latest
```
