FROM cxx-builder as builder

COPY . /test

RUN rm -rf /test/build
RUN mkdir /test/build

WORKDIR /test/build

RUN cmake ..
RUN make

# FROM alpine

# RUN mkdir -p /service/

# WORKDIR /service

# COPY --from=builder /test/build/restserver /service/
# COPY --from=builder /test/log4cxx.properties /service/
# COPY --from=builder /test/test.conf /service/

# ENTRYPOINT ['/service/restserver']
