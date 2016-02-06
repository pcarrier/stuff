FROM alpine

ADD eatmemory.c /

RUN \
apk update && \
apk add gcc musl-dev && \
gcc -o eatmemory eatmemory.c && \
apk del gcc musl-dev && \
rm -rf /var/cache/apk/*
CMD /eatmemory
