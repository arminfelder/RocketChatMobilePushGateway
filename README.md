## lightweight, push gateway for RocketChat servers

### manual build instructions

- build and install proxygen libs: https://github.com/facebook/proxygen
- install libjsoncpp-dev, libcurlpp0-dev, cmake
- cmake 
- make
- place the credentials in the servers directory:
    - FCM -> google/serverKey.txt
    - APNS -> 
    
### Docker
- pass port 11000
- mount your credentials folder into the container with -v /certs:/yourCertsFolder

e.g. docker run -t gateway -v /certs:/yourCertsFolder -p 0.0.0.0:80:11000