## For generate new schema
odb -I/usr/local/include/ -d mysql --generate-query --generate-schema user.hxx

## Build project
```bash
mkdir build
cd build
cmake ..
make
```
## Requirements
boost==1.65  
log4cxx  
odb==2.5.0 (look into Dockerfile-builder)  
openssl  
libcrypto  
