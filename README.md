## Rocket.Chat Mobile Push Gateway Documentation

### Overview

This lightweight push gateway is designed for Rocket.Chat servers. It enables push notifications for mobile devices.

### How Push Notifications Work

1. Mobile apps register with **Google Firebase Messaging** (formerly GCM) or Apple APNS using built-in credentials.
2. The app sends the received push token to the connected Rocket.Chat server.
3. The Rocket.Chat server uses the API credentials and push token to transmit notifications via Google Firebase or Apple APNS.
4. Google or Apple forwards the push notifications to the mobile device.

#### Notes:

- If deploying your own Rocket.Chat server with the Rocket.Chat Cordova app, configure your server to forward
  notifications to app developers who can send push messages.
- For custom-built apps, configure your own credentials (Google Firebase/Apple APNS) on the server. Other servers can
  use this gateway for push notifications.

---

### Dependencies

Make sure the following are installed before building:

- `libjsoncpp-dev`
- `cmake`
- `git`
- `zlib1g-dev`
- `openssl`, `libssl-dev`
- `uuid-dev`
- `libgflags-dev`

---

### Manual Build Instructions

1. **CMake Build**:
    - Clone the repository recursively:
      ```
      git clone --recurse-submodules <repository_url>
      ```
    - Run:
      ```
      cmake .
      make
      ```

2. **Set Credentials**:
    - **Environment Variables**:
        - `FCM_SERVICE_ACCOUNT_JSON`
        - `APNS_PRIVATE_KEY` (PEM format):
            1. Create a key
               following [Apple's Guide](https://developer.apple.com/documentation/usernotifications/setting_up_a_remote_notification_server/establishing_a_token_based_connection_to_apns).
            2. Convert the `.p8` file to PEM format and encode base64:
               e.g.
               ```
               openssl pkcs8 -nocrypt -in key.p8 -out key.pem
               cat key.pem | base64 -w0
               ```
        - Additional:
            - `APNS_TEAM_ID`
            - `APNS_KEY`
            - `APNS_APPID`

    - **Credentials Directory**:
      ```
      credentials/google/serviceAccount.json
      credentials/apple/key.pem
      credentials/apple/setting.json
      ```
      Example `setting.json`:
      ```json
      {
          "teamId": "YOUR_APPLE_DEVELOPER_TEAM_ID",
          "key": "AUTHKEY_IDENTIFIER",
          "appId": "YOUR_APP_BUNDLE_ID"
      }
      ```

3. **Optional - Forward Gateway**:
    - Enable chaining (supports custom + official apps):
      ```
      FORWARD_GATEWAY_ENABLE=TRUE
      FORWARD_GATEWAY_URL=https://gateway.rocket.chat # Optional
      ```

---

### Docker Build Instructions

1. Build the Docker container:
   ```
   docker build .
   ```
2. Mount the credentials' folder:
   ```
   docker run -t gateway -v /yourCertsFolder:/certs -p 80:11000 <image_id>
   ```
   Folder structure:
   ```
   /yourCertsFolder/google/serverKey.txt
   /yourCertsFolder/apple/key.pem
   ```

---

### Kubernetes

Adapt the example files in `./k8s` to suit your deployment requirements.