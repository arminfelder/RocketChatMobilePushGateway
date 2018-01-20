## lightweight, push gateway for RocketChat servers

### usage

How do mobile devices get push notifications?  The Android app is registering itself with Google Firebase Messaging (former GCM), by using given credentials, built into the app, 
the app then sends the received push token to the Rocket.Chat Server you are connected to. 
The RC server then send notifications using the API key, which is related to the key the app is using, together with the token as recipent to Google, which is then sending it to the mobile.
If you have your own RC server and want to use the Rocket.Chat cordova app, you would need to configure your server to forward the notifications to the app developers, 
as they are the only ones, who are able to send push messages to their app. This is done by setting the push gateway.
If you develop your own app or just build it yourself, you have to set your credentials from Google Firebase to your server.
If you develop your own app, and want other servers to be able to use it, with push, then you need to setup your own gateway, 
which then has to be used by the other servers. In that case you should use this gateway server. 

### manual build instructions

- build and install proxygen libs: https://github.com/facebook/proxygen
- install libjsoncpp-dev, libcurlpp0-dev, cmake
- cmake 
- make
- place the credentials in the servers "credentials" directory:
    - FCM -> google/serverKey.txt
    - APNS -> apple/cred.pem (see https://github.com/joshuakuai/PusherCpp)
    
### Docker
- pass port 11000
- mount your credentials folder into the container with -v /certs:/yourCertsFolder
    - yourCertsFolder/google/serverKey.txt
    - yourCertsFolder/apple/cred.pem (see https://github.com/joshuakuai/PusherCpp)

e.g. docker run -t gateway -v /yourCertsFolder:/certs -p 0.0.0.0:80:11000